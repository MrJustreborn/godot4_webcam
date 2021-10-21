/*************************************************************************/
/*  godot_space_3d.h                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef GODOT_SPACE_3D_H
#define GODOT_SPACE_3D_H

#include "godot_area_3d.h"
#include "godot_area_pair_3d.h"
#include "godot_body_3d.h"
#include "godot_body_pair_3d.h"
#include "godot_broad_phase_3d.h"
#include "godot_collision_object_3d.h"
#include "godot_soft_body_3d.h"

#include "core/config/project_settings.h"
#include "core/templates/hash_map.h"
#include "core/typedefs.h"

class GodotPhysicsDirectSpaceState3D : public PhysicsDirectSpaceState3D {
	GDCLASS(GodotPhysicsDirectSpaceState3D, PhysicsDirectSpaceState3D);

public:
	GodotSpace3D *space;

	virtual int intersect_point(const Vector3 &p_point, ShapeResult *r_results, int p_result_max, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = false) override;
	virtual bool intersect_ray(const Vector3 &p_from, const Vector3 &p_to, RayResult &r_result, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = false, bool p_pick_ray = false) override;
	virtual int intersect_shape(const RID &p_shape, const Transform3D &p_xform, real_t p_margin, ShapeResult *r_results, int p_result_max, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = false) override;
	virtual bool cast_motion(const RID &p_shape, const Transform3D &p_xform, const Vector3 &p_motion, real_t p_margin, real_t &p_closest_safe, real_t &p_closest_unsafe, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = false, ShapeRestInfo *r_info = nullptr) override;
	virtual bool collide_shape(RID p_shape, const Transform3D &p_shape_xform, real_t p_margin, Vector3 *r_results, int p_result_max, int &r_result_count, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = false) override;
	virtual bool rest_info(RID p_shape, const Transform3D &p_shape_xform, real_t p_margin, ShapeRestInfo *r_info, const Set<RID> &p_exclude = Set<RID>(), uint32_t p_collision_mask = UINT32_MAX, bool p_collide_with_bodies = true, bool p_collide_with_areas = false) override;
	virtual Vector3 get_closest_point_to_object_volume(RID p_object, const Vector3 p_point) const override;

	GodotPhysicsDirectSpaceState3D();
};

class GodotSpace3D {
public:
	enum ElapsedTime {
		ELAPSED_TIME_INTEGRATE_FORCES,
		ELAPSED_TIME_GENERATE_ISLANDS,
		ELAPSED_TIME_SETUP_CONSTRAINTS,
		ELAPSED_TIME_SOLVE_CONSTRAINTS,
		ELAPSED_TIME_INTEGRATE_VELOCITIES,
		ELAPSED_TIME_MAX

	};

private:
	uint64_t elapsed_time[ELAPSED_TIME_MAX] = {};

	GodotPhysicsDirectSpaceState3D *direct_access;
	RID self;

	GodotBroadPhase3D *broadphase;
	SelfList<GodotBody3D>::List active_list;
	SelfList<GodotBody3D>::List mass_properties_update_list;
	SelfList<GodotBody3D>::List state_query_list;
	SelfList<GodotArea3D>::List monitor_query_list;
	SelfList<GodotArea3D>::List area_moved_list;
	SelfList<GodotSoftBody3D>::List active_soft_body_list;

	static void *_broadphase_pair(GodotCollisionObject3D *A, int p_subindex_A, GodotCollisionObject3D *B, int p_subindex_B, void *p_self);
	static void _broadphase_unpair(GodotCollisionObject3D *A, int p_subindex_A, GodotCollisionObject3D *B, int p_subindex_B, void *p_data, void *p_self);

	Set<GodotCollisionObject3D *> objects;

	GodotArea3D *area = nullptr;

	real_t contact_recycle_radius = 0.01;
	real_t contact_max_separation = 0.05;
	real_t contact_max_allowed_penetration = 0.01;
	real_t constraint_bias = 0.01;

	enum {
		INTERSECTION_QUERY_MAX = 2048
	};

	GodotCollisionObject3D *intersection_query_results[INTERSECTION_QUERY_MAX];
	int intersection_query_subindex_results[INTERSECTION_QUERY_MAX];

	real_t body_linear_velocity_sleep_threshold;
	real_t body_angular_velocity_sleep_threshold;
	real_t body_time_to_sleep;
	real_t body_angular_velocity_damp_ratio;

	bool locked = false;

	real_t last_step = 0.001;

	int island_count = 0;
	int active_objects = 0;
	int collision_pairs = 0;

	RID static_global_body;

	Vector<Vector3> contact_debug;
	int contact_debug_count = 0;

	friend class GodotPhysicsDirectSpaceState3D;

	int _cull_aabb_for_body(GodotBody3D *p_body, const AABB &p_aabb);

public:
	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	void set_default_area(GodotArea3D *p_area) { area = p_area; }
	GodotArea3D *get_default_area() const { return area; }

	const SelfList<GodotBody3D>::List &get_active_body_list() const;
	void body_add_to_active_list(SelfList<GodotBody3D> *p_body);
	void body_remove_from_active_list(SelfList<GodotBody3D> *p_body);
	void body_add_to_mass_properties_update_list(SelfList<GodotBody3D> *p_body);
	void body_remove_from_mass_properties_update_list(SelfList<GodotBody3D> *p_body);

	void body_add_to_state_query_list(SelfList<GodotBody3D> *p_body);
	void body_remove_from_state_query_list(SelfList<GodotBody3D> *p_body);

	void area_add_to_monitor_query_list(SelfList<GodotArea3D> *p_area);
	void area_remove_from_monitor_query_list(SelfList<GodotArea3D> *p_area);
	void area_add_to_moved_list(SelfList<GodotArea3D> *p_area);
	void area_remove_from_moved_list(SelfList<GodotArea3D> *p_area);
	const SelfList<GodotArea3D>::List &get_moved_area_list() const;

	const SelfList<GodotSoftBody3D>::List &get_active_soft_body_list() const;
	void soft_body_add_to_active_list(SelfList<GodotSoftBody3D> *p_soft_body);
	void soft_body_remove_from_active_list(SelfList<GodotSoftBody3D> *p_soft_body);

	GodotBroadPhase3D *get_broadphase();

	void add_object(GodotCollisionObject3D *p_object);
	void remove_object(GodotCollisionObject3D *p_object);
	const Set<GodotCollisionObject3D *> &get_objects() const;

	_FORCE_INLINE_ real_t get_contact_recycle_radius() const { return contact_recycle_radius; }
	_FORCE_INLINE_ real_t get_contact_max_separation() const { return contact_max_separation; }
	_FORCE_INLINE_ real_t get_contact_max_allowed_penetration() const { return contact_max_allowed_penetration; }
	_FORCE_INLINE_ real_t get_constraint_bias() const { return constraint_bias; }
	_FORCE_INLINE_ real_t get_body_linear_velocity_sleep_threshold() const { return body_linear_velocity_sleep_threshold; }
	_FORCE_INLINE_ real_t get_body_angular_velocity_sleep_threshold() const { return body_angular_velocity_sleep_threshold; }
	_FORCE_INLINE_ real_t get_body_time_to_sleep() const { return body_time_to_sleep; }
	_FORCE_INLINE_ real_t get_body_angular_velocity_damp_ratio() const { return body_angular_velocity_damp_ratio; }

	void update();
	void setup();
	void call_queries();

	bool is_locked() const;
	void lock();
	void unlock();

	real_t get_last_step() const { return last_step; }
	void set_last_step(real_t p_step) { last_step = p_step; }

	void set_param(PhysicsServer3D::SpaceParameter p_param, real_t p_value);
	real_t get_param(PhysicsServer3D::SpaceParameter p_param) const;

	void set_island_count(int p_island_count) { island_count = p_island_count; }
	int get_island_count() const { return island_count; }

	void set_active_objects(int p_active_objects) { active_objects = p_active_objects; }
	int get_active_objects() const { return active_objects; }

	int get_collision_pairs() const { return collision_pairs; }

	GodotPhysicsDirectSpaceState3D *get_direct_state();

	void set_debug_contacts(int p_amount) { contact_debug.resize(p_amount); }
	_FORCE_INLINE_ bool is_debugging_contacts() const { return !contact_debug.is_empty(); }
	_FORCE_INLINE_ void add_debug_contact(const Vector3 &p_contact) {
		if (contact_debug_count < contact_debug.size()) {
			contact_debug.write[contact_debug_count++] = p_contact;
		}
	}
	_FORCE_INLINE_ Vector<Vector3> get_debug_contacts() { return contact_debug; }
	_FORCE_INLINE_ int get_debug_contact_count() { return contact_debug_count; }

	void set_static_global_body(RID p_body) { static_global_body = p_body; }
	RID get_static_global_body() { return static_global_body; }

	void set_elapsed_time(ElapsedTime p_time, uint64_t p_msec) { elapsed_time[p_time] = p_msec; }
	uint64_t get_elapsed_time(ElapsedTime p_time) const { return elapsed_time[p_time]; }

	bool test_body_motion(GodotBody3D *p_body, const PhysicsServer3D::MotionParameters &p_parameters, PhysicsServer3D::MotionResult *r_result);

	GodotSpace3D();
	~GodotSpace3D();
};

#endif // GODOT_SPACE_3D_H
