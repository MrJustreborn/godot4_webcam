/*************************************************************************/
/*  xr_pose.h                                                            */
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

#ifndef XR_POSE_H
#define XR_POSE_H

#include "core/object/ref_counted.h"

class XRPose : public RefCounted {
	GDCLASS(XRPose, RefCounted);

public:
private:
	bool has_tracking_data = false;
	StringName name;
	Transform3D transform;
	Vector3 linear_velocity;
	Vector3 angular_velocity;

protected:
	static void _bind_methods();

public:
	void set_has_tracking_data(const bool p_has_tracking_data);
	bool get_has_tracking_data() const;

	void set_name(const StringName &p_name);
	StringName get_name() const;

	void set_transform(const Transform3D p_transform);
	Transform3D get_transform() const;
	Transform3D get_adjusted_transform() const;

	void set_linear_velocity(const Vector3 p_velocity);
	Vector3 get_linear_velocity() const;

	void set_angular_velocity(const Vector3 p_velocity);
	Vector3 get_angular_velocity() const;
};

#endif
