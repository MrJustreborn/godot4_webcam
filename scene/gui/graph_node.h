/*************************************************************************/
/*  graph_node.h                                                         */
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

#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include "scene/gui/container.h"
#include "scene/resources/text_line.h"

class GraphNode : public Container {
	GDCLASS(GraphNode, Container);

public:
	enum Overlay {
		OVERLAY_DISABLED,
		OVERLAY_BREAKPOINT,
		OVERLAY_POSITION
	};

private:
	struct Slot {
		bool enable_left = false;
		int type_left = 0;
		Color color_left = Color(1, 1, 1, 1);
		bool enable_right = false;
		int type_right = 0;
		Color color_right = Color(1, 1, 1, 1);
		Ref<Texture2D> custom_slot_left;
		Ref<Texture2D> custom_slot_right;
	};

	String title;
	Ref<TextLine> title_buf;

	Dictionary opentype_features;
	String language;
	TextDirection text_direction = TEXT_DIRECTION_AUTO;

	bool show_close = false;
	Vector2 position_offset;
	bool comment = false;
	bool resizable = false;

	bool resizing = false;
	Vector2 resizing_from;
	Vector2 resizing_from_size;

	Rect2 close_rect;

	Vector<int> cache_y;

	struct ConnCache {
		Vector2 pos;
		int type = 0;
		Color color;
	};

	Vector<ConnCache> conn_input_cache;
	Vector<ConnCache> conn_output_cache;

	Map<int, Slot> slot_info;

	bool connpos_dirty = true;

	void _connpos_update();
	void _resort();
	void _shape();

	Vector2 drag_from;
	bool selected = false;

	Overlay overlay = OVERLAY_DISABLED;

#ifdef TOOLS_ENABLED
	void _edit_set_position(const Point2 &p_position) override;
	void _validate_property(PropertyInfo &property) const override;
#endif

protected:
	virtual void gui_input(const Ref<InputEvent> &p_ev) override;
	void _notification(int p_what);
	static void _bind_methods();

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	bool has_point(const Point2 &p_point) const override;

	void set_slot(int p_idx, bool p_enable_left, int p_type_left, const Color &p_color_left, bool p_enable_right, int p_type_right, const Color &p_color_right, const Ref<Texture2D> &p_custom_left = Ref<Texture2D>(), const Ref<Texture2D> &p_custom_right = Ref<Texture2D>());
	void clear_slot(int p_idx);
	void clear_all_slots();

	bool is_slot_enabled_left(int p_idx) const;
	void set_slot_enabled_left(int p_idx, bool p_enable_left);

	void set_slot_type_left(int p_idx, int p_type_left);
	int get_slot_type_left(int p_idx) const;

	void set_slot_color_left(int p_idx, const Color &p_color_left);
	Color get_slot_color_left(int p_idx) const;

	bool is_slot_enabled_right(int p_idx) const;
	void set_slot_enabled_right(int p_idx, bool p_enable_right);

	void set_slot_type_right(int p_idx, int p_type_right);
	int get_slot_type_right(int p_idx) const;

	void set_slot_color_right(int p_idx, const Color &p_color_right);
	Color get_slot_color_right(int p_idx) const;

	void set_title(const String &p_title);
	String get_title() const;

	void set_text_direction(TextDirection p_text_direction);
	TextDirection get_text_direction() const;

	void set_opentype_feature(const String &p_name, int p_value);
	int get_opentype_feature(const String &p_name) const;
	void clear_opentype_features();

	void set_language(const String &p_language);
	String get_language() const;

	void set_position_offset(const Vector2 &p_offset);
	Vector2 get_position_offset() const;

	void set_selected(bool p_selected);
	bool is_selected();

	void set_drag(bool p_drag);
	Vector2 get_drag_from();

	void set_show_close_button(bool p_enable);
	bool is_close_button_visible() const;

	int get_connection_input_count();
	int get_connection_output_count();
	Vector2 get_connection_input_position(int p_idx);
	int get_connection_input_type(int p_idx);
	Color get_connection_input_color(int p_idx);
	Vector2 get_connection_output_position(int p_idx);
	int get_connection_output_type(int p_idx);
	Color get_connection_output_color(int p_idx);

	void set_overlay(Overlay p_overlay);
	Overlay get_overlay() const;

	void set_comment(bool p_enable);
	bool is_comment() const;

	void set_resizable(bool p_enable);
	bool is_resizable() const;

	virtual Size2 get_minimum_size() const override;

	bool is_resizing() const { return resizing; }

	GraphNode();
};

VARIANT_ENUM_CAST(GraphNode::Overlay)

#endif // GRAPH_NODE_H
