/*************************************************************************/
/*  skeleton_3d_editor_plugin.h                                          */
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

#ifndef SKELETON_3D_EDITOR_PLUGIN_H
#define SKELETON_3D_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "editor/editor_properties.h"
#include "node_3d_editor_plugin.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/skeleton_3d.h"
#include "scene/resources/immediate_mesh.h"

class EditorInspectorPluginSkeleton;
class Joint;
class PhysicalBone3D;
class Skeleton3DEditorPlugin;
class Button;
class CheckBox;

class BoneTransformEditor : public VBoxContainer {
	GDCLASS(BoneTransformEditor, VBoxContainer);

	EditorInspectorSection *section = nullptr;

	EditorPropertyVector3 *translation_property = nullptr;
	EditorPropertyVector3 *rotation_property = nullptr;
	EditorPropertyVector3 *scale_property = nullptr;
	EditorInspectorSection *transform_section = nullptr;
	EditorPropertyTransform3D *transform_property = nullptr;

	Rect2 background_rects[5];

	Skeleton3D *skeleton;
	String property;

	UndoRedo *undo_redo;

	Button *key_button = nullptr;
	CheckBox *enabled_checkbox = nullptr;

	bool keyable = false;
	bool toggle_enabled = false;
	bool updating = false;

	String label;

	void create_editors();

	// Called when one of the EditorSpinSliders are changed.
	void _value_changed(const double p_value);
	// Called when the one of the EditorPropertyVector3 are updated.
	void _value_changed_vector3(const String p_property_name, const Vector3 p_vector, const StringName p_edited_property_name, const bool p_boolean);
	// Called when the transform_property is updated.
	void _value_changed_transform(const String p_property_name, const Transform3D p_transform, const StringName p_edited_property_name, const bool p_boolean);
	// Changes the transform to the given transform and updates the UI accordingly.
	void _change_transform(Transform3D p_new_transform);
	// Update it is truely keyable then.
	void _update_key_button(const bool p_keyable);
	// Creates a Transform using the EditorPropertyVector3 properties.
	Transform3D compute_transform_from_vector3s() const;

	void update_enabled_checkbox();

protected:
	void _notification(int p_what);

public:
	BoneTransformEditor(Skeleton3D *p_skeleton);

	// Which transform target to modify.
	void set_target(const String &p_prop);
	void set_label(const String &p_label) { label = p_label; }

	void _update_properties();
	void _update_custom_pose_properties();
	void _update_transform_properties(Transform3D p_transform);

	// Transform can be keyed, whether or not to show the button.
	void set_keyable(const bool p_keyable);

	// When rest mode, pose and custom_pose editor are diasbled.
	void set_properties_read_only(const bool p_readonly);
	void set_transform_read_only(const bool p_readonly);

	// Bone can be toggled enabled or disabled, whether or not to show the checkbox.
	void set_toggle_enabled(const bool p_enabled);

	// Key Transform Button pressed.
	void _key_button_pressed();

	// Bone Enabled Checkbox toggled.
	void _checkbox_pressed();
};

class Skeleton3DEditor : public VBoxContainer {
	GDCLASS(Skeleton3DEditor, VBoxContainer);

	friend class Skeleton3DEditorPlugin;

	enum SkeletonOption {
		SKELETON_OPTION_INIT_POSE,
		SKELETON_OPTION_INSERT_KEYS,
		SKELETON_OPTION_INSERT_KEYS_EXISTED,
		SKELETON_OPTION_CREATE_PHYSICAL_SKELETON
	};

	enum RestOption {
		REST_OPTION_POSE_TO_REST
	};

	struct BoneInfo {
		PhysicalBone3D *physical_bone = nullptr;
		Transform3D relative_rest; // Relative to skeleton node.
	};

	EditorNode *editor;
	EditorInspectorPluginSkeleton *editor_plugin;

	Skeleton3D *skeleton;

	Tree *joint_tree = nullptr;
	BoneTransformEditor *rest_editor = nullptr;
	BoneTransformEditor *pose_editor = nullptr;
	BoneTransformEditor *custom_pose_editor = nullptr;

	VSeparator *separator;
	MenuButton *skeleton_options = nullptr;
	MenuButton *rest_options = nullptr;
	Button *edit_mode_button;

	bool edit_mode = false;

	EditorFileDialog *file_dialog = nullptr;

	bool keyable;

	static Skeleton3DEditor *singleton;

	void _on_click_skeleton_option(int p_skeleton_option);
	void _on_click_rest_option(int p_rest_option);
	void _file_selected(const String &p_file);
	TreeItem *_find(TreeItem *p_node, const NodePath &p_path);
	void edit_mode_toggled(const bool pressed);

	EditorFileDialog *file_export_lib = nullptr;

	void update_joint_tree();
	void update_editors();

	void create_editors();

	void init_pose();
	void insert_keys(bool p_all_bones);
	void pose_to_rest();

	void create_physical_skeleton();
	PhysicalBone3D *create_physical_bone(int bone_id, int bone_child_id, const Vector<BoneInfo> &bones_infos);

	Variant get_drag_data_fw(const Point2 &p_point, Control *p_from);
	bool can_drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from) const;
	void drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from);

	void set_keyable(const bool p_keyable);
	void set_rest_options_enabled(const bool p_rest_options_enabled);

	// Handle.
	MeshInstance3D *handles_mesh_instance;
	Ref<ImmediateMesh> handles_mesh;
	Ref<ShaderMaterial> handle_material;
	Ref<Shader> handle_shader;

	Transform3D bone_original;

	void _update_pose_enabled(int p_bone = -1);
	void _update_show_rest_only();

	void _update_gizmo_transform();
	void _update_gizmo_visible();

	void _hide_handles();

	void _draw_gizmo();
	void _draw_handles();

	void _joint_tree_selection_changed();
	void _joint_tree_rmb_select(const Vector2 &p_pos);
	void _update_properties();

	void _subgizmo_selection_change();

	int selected_bone = -1;

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

public:
	static Skeleton3DEditor *get_singleton() { return singleton; }

	void select_bone(int p_idx);

	int get_selected_bone() const;

	void move_skeleton_bone(NodePath p_skeleton_path, int32_t p_selected_boneidx, int32_t p_target_boneidx);

	Skeleton3D *get_skeleton() const { return skeleton; };

	bool is_edit_mode() const { return edit_mode; }

	void update_bone_original();
	Transform3D get_bone_original() { return bone_original; };

	Skeleton3DEditor(EditorInspectorPluginSkeleton *e_plugin, EditorNode *p_editor, Skeleton3D *skeleton);
	~Skeleton3DEditor();
};

class EditorInspectorPluginSkeleton : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginSkeleton, EditorInspectorPlugin);

	friend class Skeleton3DEditorPlugin;

	Skeleton3DEditor *skel_editor;
	EditorNode *editor;

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_begin(Object *p_object) override;
};

class Skeleton3DEditorPlugin : public EditorPlugin {
	GDCLASS(Skeleton3DEditorPlugin, EditorPlugin);

	EditorInspectorPluginSkeleton *skeleton_plugin;
	EditorNode *editor;

public:
	virtual EditorPlugin::AfterGUIInput forward_spatial_gui_input(Camera3D *p_camera, const Ref<InputEvent> &p_event) override;

	bool has_main_screen() const override { return false; }
	virtual bool handles(Object *p_object) const override;

	virtual String get_name() const override { return "Skeleton3D"; }

	Skeleton3DEditorPlugin(EditorNode *p_node);
};

class Skeleton3DGizmoPlugin : public EditorNode3DGizmoPlugin {
	GDCLASS(Skeleton3DGizmoPlugin, EditorNode3DGizmoPlugin);

	Ref<StandardMaterial3D> unselected_mat;
	Ref<ShaderMaterial> selected_mat;
	Ref<Shader> selected_sh;

public:
	bool has_gizmo(Node3D *p_spatial) override;
	String get_gizmo_name() const override;
	int get_priority() const override;

	int subgizmos_intersect_ray(const EditorNode3DGizmo *p_gizmo, Camera3D *p_camera, const Vector2 &p_point) const override;
	Transform3D get_subgizmo_transform(const EditorNode3DGizmo *p_gizmo, int p_id) const override;
	void set_subgizmo_transform(const EditorNode3DGizmo *p_gizmo, int p_id, Transform3D p_transform) override;
	void commit_subgizmos(const EditorNode3DGizmo *p_gizmo, const Vector<int> &p_ids, const Vector<Transform3D> &p_restore, bool p_cancel) override;

	void redraw(EditorNode3DGizmo *p_gizmo) override;

	Skeleton3DGizmoPlugin();
};

#endif // SKELETON_3D_EDITOR_PLUGIN_H
