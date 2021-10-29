#[compute]

#version 450

#VERSION_DEFINES

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

#define SAMPLER_NEAREST_CLAMP 0
#define SAMPLER_LINEAR_CLAMP 1
#define SAMPLER_NEAREST_WITH_MIPMAPS_CLAMP 2
#define SAMPLER_LINEAR_WITH_MIPMAPS_CLAMP 3
#define SAMPLER_NEAREST_WITH_MIPMAPS_ANISOTROPIC_CLAMP 4
#define SAMPLER_LINEAR_WITH_MIPMAPS_ANISOTROPIC_CLAMP 5
#define SAMPLER_NEAREST_REPEAT 6
#define SAMPLER_LINEAR_REPEAT 7
#define SAMPLER_NEAREST_WITH_MIPMAPS_REPEAT 8
#define SAMPLER_LINEAR_WITH_MIPMAPS_REPEAT 9
#define SAMPLER_NEAREST_WITH_MIPMAPS_ANISOTROPIC_REPEAT 10
#define SAMPLER_LINEAR_WITH_MIPMAPS_ANISOTROPIC_REPEAT 11

#define DENSITY_SCALE 1024.0

#include "cluster_data_inc.glsl"
#include "light_data_inc.glsl"

#define M_PI 3.14159265359

layout(set = 0, binding = 1) uniform sampler material_samplers[12];

layout(set = 0, binding = 2, std430) restrict readonly buffer GlobalVariableData {
	vec4 data[];
}
global_variables;

layout(push_constant, binding = 0, std430) uniform Params {
	vec3 position;
	float pad;

	vec3 extents;
	float pad2;

	ivec3 corner;
	uint shape;

	mat4 transform;
}
params;

layout(r32ui, set = 1, binding = 1) uniform volatile uimage3D emissive_only_map;

layout(set = 1, binding = 2, std140) uniform SceneParams {
	vec2 fog_frustum_size_begin;
	vec2 fog_frustum_size_end;

	float fog_frustum_end;
	float z_near; //
	float z_far; //
	float time;

	ivec3 fog_volume_size;
	uint directional_light_count; //

	bool use_temporal_reprojection;
	uint temporal_frame;
	float detail_spread;
	float temporal_blend;

	mat4 to_prev_view;
	mat4 transform;
}
scene_params;

layout(r32ui, set = 1, binding = 3) uniform volatile uimage3D density_only_map;
layout(r32ui, set = 1, binding = 4) uniform volatile uimage3D light_only_map;

#ifdef MATERIAL_UNIFORMS_USED
layout(set = 2, binding = 0, std140) uniform MaterialUniforms{
#MATERIAL_UNIFORMS
} material;
#endif

#GLOBALS

float get_depth_at_pos(float cell_depth_size, int z) {
	float d = float(z) * cell_depth_size + cell_depth_size * 0.5; //center of voxels
	d = pow(d, scene_params.detail_spread);
	return scene_params.fog_frustum_end * d;
}

#define TEMPORAL_FRAMES 16

const vec3 halton_map[TEMPORAL_FRAMES] = vec3[](
		vec3(0.5, 0.33333333, 0.2),
		vec3(0.25, 0.66666667, 0.4),
		vec3(0.75, 0.11111111, 0.6),
		vec3(0.125, 0.44444444, 0.8),
		vec3(0.625, 0.77777778, 0.04),
		vec3(0.375, 0.22222222, 0.24),
		vec3(0.875, 0.55555556, 0.44),
		vec3(0.0625, 0.88888889, 0.64),
		vec3(0.5625, 0.03703704, 0.84),
		vec3(0.3125, 0.37037037, 0.08),
		vec3(0.8125, 0.7037037, 0.28),
		vec3(0.1875, 0.14814815, 0.48),
		vec3(0.6875, 0.48148148, 0.68),
		vec3(0.4375, 0.81481481, 0.88),
		vec3(0.9375, 0.25925926, 0.12),
		vec3(0.03125, 0.59259259, 0.32));

void main() {
	vec3 fog_cell_size = 1.0 / vec3(scene_params.fog_volume_size);

	ivec3 pos = ivec3(gl_GlobalInvocationID.xyz) + params.corner;
	if (any(greaterThanEqual(pos, scene_params.fog_volume_size))) {
		return; //do not compute
	}

	vec3 posf = vec3(pos);

	vec3 fog_unit_pos = posf * fog_cell_size + fog_cell_size * 0.5; //center of voxels
	fog_unit_pos.z = pow(fog_unit_pos.z, scene_params.detail_spread);

	vec3 view_pos;
	view_pos.xy = (fog_unit_pos.xy * 2.0 - 1.0) * mix(scene_params.fog_frustum_size_begin, scene_params.fog_frustum_size_end, vec2(fog_unit_pos.z));
	view_pos.z = -scene_params.fog_frustum_end * fog_unit_pos.z;
	view_pos.y = -view_pos.y;

	if (scene_params.use_temporal_reprojection) {
		vec3 prev_view = (scene_params.to_prev_view * vec4(view_pos, 1.0)).xyz;
		//undo transform into prev view
		prev_view.y = -prev_view.y;
		//z back to unit size
		prev_view.z /= -scene_params.fog_frustum_end;
		//xy back to unit size
		prev_view.xy /= mix(scene_params.fog_frustum_size_begin, scene_params.fog_frustum_size_end, vec2(prev_view.z));
		prev_view.xy = prev_view.xy * 0.5 + 0.5;
		//z back to unspread value
		prev_view.z = pow(prev_view.z, 1.0 / scene_params.detail_spread);

		if (all(greaterThan(prev_view, vec3(0.0))) && all(lessThan(prev_view, vec3(1.0)))) {
			//reprojectinon fits
			// Since we can reproject, now we must jitter the current view pos.
			// This is done here because cells that can't reproject should not jitter.

			fog_unit_pos = posf * fog_cell_size + fog_cell_size * halton_map[scene_params.temporal_frame]; //center of voxels, offset by halton table
			fog_unit_pos.z = pow(fog_unit_pos.z, scene_params.detail_spread);

			view_pos.xy = (fog_unit_pos.xy * 2.0 - 1.0) * mix(scene_params.fog_frustum_size_begin, scene_params.fog_frustum_size_end, vec2(fog_unit_pos.z));
			view_pos.z = -scene_params.fog_frustum_end * fog_unit_pos.z;
			view_pos.y = -view_pos.y;
		}
	}

	float density = 0.0;
	vec3 emission = vec3(0.0);
	vec3 albedo = vec3(0.0);

	float cell_depth_size = abs(view_pos.z - get_depth_at_pos(fog_cell_size.z, pos.z + 1));

	vec4 world = scene_params.transform * vec4(view_pos, 1.0);
	world.xyz /= world.w;

	vec3 uvw = fog_unit_pos;

	vec4 local_pos = params.transform * world;
	local_pos.xyz /= local_pos.w;

	float sdf = -1.0;
	if (params.shape == 0) {
		//Ellipsoid
		// https://www.shadertoy.com/view/tdS3DG
		float k0 = length(local_pos.xyz / params.extents);
		float k1 = length(local_pos.xyz / (params.extents * params.extents));
		sdf = k0 * (k0 - 1.0) / k1;
	} else if (params.shape == 1) {
		// Box
		// https://iquilezles.org/www/articles/distfunctions/distfunctions.htm
		vec3 q = abs(local_pos.xyz) - params.extents;
		sdf = length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
	}

	float cull_mask = 1.0; //used to cull cells that do not contribute
	if (params.shape <= 1) {
#ifndef SDF_USED
		cull_mask = 1.0 - smoothstep(-0.1, 0.0, sdf);
#endif
		uvw = clamp((local_pos.xyz + params.extents) / (2.0 * params.extents), 0.0, 1.0);
	}

	if (cull_mask > 0.0) {
		{
#CODE : FOG
		}

#ifdef DENSITY_USED
		density *= cull_mask;
		if (abs(density) > 0.001) {
			int final_density = int(density * DENSITY_SCALE);
			imageAtomicAdd(density_only_map, pos, uint(final_density));

#ifdef EMISSION_USED
			{
				emission *= clamp(density, 0.0, 1.0);
				emission = clamp(emission, vec3(0.0), vec3(4.0));
				// Scale to fit into R11G11B10 with a range of 0-4
				uvec3 emission_u = uvec3(emission.r * 511.0, emission.g * 511.0, emission.b * 255.0);
				// R and G have 11 bits each and B has 10. Then pack them into a 32 bit uint
				uint final_emission = emission_u.r << 21 | emission_u.g << 10 | emission_u.b;
				uint prev_emission = imageAtomicAdd(emissive_only_map, pos, final_emission);

				// Adding can lead to colors overflowing, so validate
				uvec3 prev_emission_u = uvec3(prev_emission >> 21, (prev_emission << 11) >> 21, prev_emission % 1024);
				uint add_emission = final_emission + prev_emission;
				uvec3 add_emission_u = uvec3(add_emission >> 21, (add_emission << 11) >> 21, add_emission % 1024);

				bvec3 overflowing = lessThan(add_emission_u, prev_emission_u + emission_u);

				if (any(overflowing)) {
					uvec3 overflow_factor = mix(uvec3(0), uvec3(2047 << 21, 2047 << 10, 1023), overflowing);
					uint force_max = overflow_factor.r | overflow_factor.g | overflow_factor.b;
					imageAtomicOr(emissive_only_map, pos, force_max);
				}
			}
#endif
#ifdef ALBEDO_USED
			{
				vec3 scattering = albedo * clamp(density, 0.0, 1.0);
				scattering = clamp(scattering, vec3(0.0), vec3(1.0));
				uvec3 scattering_u = uvec3(scattering.r * 2047.0, scattering.g * 2047.0, scattering.b * 1023.0);
				// R and G have 11 bits each and B has 10. Then pack them into a 32 bit uint
				uint final_scattering = scattering_u.r << 21 | scattering_u.g << 10 | scattering_u.b;
				uint prev_scattering = imageAtomicAdd(light_only_map, pos, final_scattering);

				// Adding can lead to colors overflowing, so validate
				uvec3 prev_scattering_u = uvec3(prev_scattering >> 21, (prev_scattering << 11) >> 21, prev_scattering % 1024);
				uint add_scattering = final_scattering + prev_scattering;
				uvec3 add_scattering_u = uvec3(add_scattering >> 21, (add_scattering << 11) >> 21, add_scattering % 1024);

				bvec3 overflowing = lessThan(add_scattering_u, prev_scattering_u + scattering_u);

				if (any(overflowing)) {
					uvec3 overflow_factor = mix(uvec3(0), uvec3(2047 << 21, 2047 << 10, 1023), overflowing);
					uint force_max = overflow_factor.r | overflow_factor.g | overflow_factor.b;
					imageAtomicOr(light_only_map, pos, force_max);
				}
			}
#endif // ALBEDO_USED
		}
#endif // DENSITY_USED
	}
}
