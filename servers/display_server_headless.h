/*************************************************************************/
/*  display_server_headless.h                                            */
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

#ifndef DISPLAY_SERVER_HEADLESS_H
#define DISPLAY_SERVER_HEADLESS_H

#include "servers/display_server.h"

#include "servers/rendering/rasterizer_dummy.h"

class DisplayServerHeadless : public DisplayServer {
private:
	friend class DisplayServer;

	static Vector<String> get_rendering_drivers_func() {
		Vector<String> drivers;
		drivers.push_back("dummy");
		return drivers;
	}

	static DisplayServer *create_func(const String &p_rendering_driver, DisplayServer::WindowMode p_mode, DisplayServer::VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i &p_resolution, Error &r_error) {
		r_error = OK;
		RasterizerDummy::make_current();
		return memnew(DisplayServerHeadless());
	}

public:
	bool has_feature(Feature p_feature) const override { return false; }
	String get_name() const override { return "headless"; }

	int get_screen_count() const override { return 0; }
	Point2i screen_get_position(int p_screen = SCREEN_OF_MAIN_WINDOW) const override { return Point2i(); }
	Size2i screen_get_size(int p_screen = SCREEN_OF_MAIN_WINDOW) const override { return Size2i(); }
	Rect2i screen_get_usable_rect(int p_screen = SCREEN_OF_MAIN_WINDOW) const override { return Rect2i(); }
	int screen_get_dpi(int p_screen = SCREEN_OF_MAIN_WINDOW) const override { return 96; /* 0 might cause issues */ }
	float screen_get_scale(int p_screen = SCREEN_OF_MAIN_WINDOW) const override { return 1; }
	float screen_get_max_scale() const override { return 1; }

	Vector<DisplayServer::WindowID> get_window_list() const override { return Vector<DisplayServer::WindowID>(); }

	WindowID get_window_at_screen_position(const Point2i &p_position) const override { return -1; }

	void window_attach_instance_id(ObjectID p_instance, WindowID p_window = MAIN_WINDOW_ID) override {}
	ObjectID window_get_attached_instance_id(WindowID p_window = MAIN_WINDOW_ID) const override { return ObjectID(); }

	void window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override {}

	void window_set_window_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override {}
	void window_set_input_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override {}
	void window_set_input_text_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override {}
	void window_set_drop_files_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override {}

	void window_set_title(const String &p_title, WindowID p_window = MAIN_WINDOW_ID) override {}

	void window_set_mouse_passthrough(const Vector<Vector2> &p_region, WindowID p_window = MAIN_WINDOW_ID) override {}

	int window_get_current_screen(WindowID p_window = MAIN_WINDOW_ID) const override { return -1; }
	void window_set_current_screen(int p_screen, WindowID p_window = MAIN_WINDOW_ID) override {}

	Point2i window_get_position(WindowID p_window = MAIN_WINDOW_ID) const override { return Point2i(); }
	void window_set_position(const Point2i &p_position, WindowID p_window = MAIN_WINDOW_ID) override {}

	void window_set_transient(WindowID p_window, WindowID p_parent) override {}

	void window_set_max_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override {}
	Size2i window_get_max_size(WindowID p_window = MAIN_WINDOW_ID) const override { return Size2i(); }

	void window_set_min_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override {}
	Size2i window_get_min_size(WindowID p_window = MAIN_WINDOW_ID) const override { return Size2i(); };

	void window_set_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override {}
	Size2i window_get_size(WindowID p_window = MAIN_WINDOW_ID) const override { return Size2i(); }
	Size2i window_get_real_size(WindowID p_window = MAIN_WINDOW_ID) const override { return Size2i(); }

	void window_set_mode(WindowMode p_mode, WindowID p_window = MAIN_WINDOW_ID) override {}
	WindowMode window_get_mode(WindowID p_window = MAIN_WINDOW_ID) const override { return WINDOW_MODE_MINIMIZED; }

	bool window_is_maximize_allowed(WindowID p_window = MAIN_WINDOW_ID) const override { return false; }

	void window_set_flag(WindowFlags p_flag, bool p_enabled, WindowID p_window = MAIN_WINDOW_ID) override {}
	virtual bool window_get_flag(WindowFlags p_flag, WindowID p_window = MAIN_WINDOW_ID) const override { return false; }

	void window_request_attention(WindowID p_window = MAIN_WINDOW_ID) override {}
	void window_move_to_foreground(WindowID p_window = MAIN_WINDOW_ID) override {}

	bool window_can_draw(WindowID p_window = MAIN_WINDOW_ID) const override { return false; }

	bool can_any_window_draw() const override { return false; }

	void process_events() override {}

	void set_icon(const Ref<Image> &p_icon) override {}

	DisplayServerHeadless() {}
	~DisplayServerHeadless() {}
};

#endif // DISPLAY_SERVER_HEADLESS_H
