/*************************************************************************/
/*  lightmap_gi_editor_plugin.cpp                                        */
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

#include "lightmap_gi_editor_plugin.h"

void LightmapGIEditorPlugin::_bake_select_file(const String &p_file) {
	if (lightmap) {
		LightmapGI::BakeError err;
		if (get_tree()->get_edited_scene_root() && get_tree()->get_edited_scene_root() == lightmap) {
			err = lightmap->bake(lightmap, p_file, bake_func_step);
		} else {
			err = lightmap->bake(lightmap->get_parent(), p_file, bake_func_step);
		}

		bake_func_end();

		switch (err) {
			case LightmapGI::BAKE_ERROR_NO_SAVE_PATH: {
				String scene_path = lightmap->get_scene_file_path();
				if (scene_path == String()) {
					scene_path = lightmap->get_owner()->get_scene_file_path();
				}
				if (scene_path == String()) {
					EditorNode::get_singleton()->show_warning(TTR("Can't determine a save path for lightmap images.\nSave your scene and try again."));
					break;
				}
				scene_path = scene_path.get_basename() + ".lmbake";

				file_dialog->set_current_path(scene_path);
				file_dialog->popup_file_dialog();

			} break;
			case LightmapGI::BAKE_ERROR_NO_MESHES:
				EditorNode::get_singleton()->show_warning(TTR("No meshes to bake. Make sure they contain an UV2 channel and that the 'Bake Light' flag is on."));
				break;
			case LightmapGI::BAKE_ERROR_CANT_CREATE_IMAGE:
				EditorNode::get_singleton()->show_warning(TTR("Failed creating lightmap images, make sure path is writable."));
				break;
			default: {
			}
		}
	}
}

void LightmapGIEditorPlugin::_bake() {
	_bake_select_file("");
}

void LightmapGIEditorPlugin::edit(Object *p_object) {
	LightmapGI *s = Object::cast_to<LightmapGI>(p_object);
	if (!s) {
		return;
	}

	lightmap = s;
}

bool LightmapGIEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("LightmapGI");
}

void LightmapGIEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		bake->show();
	} else {
		bake->hide();
	}
}

EditorProgress *LightmapGIEditorPlugin::tmp_progress = nullptr;

bool LightmapGIEditorPlugin::bake_func_step(float p_progress, const String &p_description, void *, bool p_refresh) {
	if (!tmp_progress) {
		tmp_progress = memnew(EditorProgress("bake_lightmaps", TTR("Bake Lightmaps"), 1000, false));
		ERR_FAIL_COND_V(tmp_progress == nullptr, false);
	}
	return tmp_progress->step(p_description, p_progress * 1000, p_refresh);
}

void LightmapGIEditorPlugin::bake_func_end() {
	if (tmp_progress != nullptr) {
		memdelete(tmp_progress);
		tmp_progress = nullptr;
	}
}

void LightmapGIEditorPlugin::_bind_methods() {
	ClassDB::bind_method("_bake", &LightmapGIEditorPlugin::_bake);
}

LightmapGIEditorPlugin::LightmapGIEditorPlugin(EditorNode *p_node) {
	editor = p_node;
	bake = memnew(Button);
	bake->set_flat(true);
	bake->set_icon(editor->get_gui_base()->get_theme_icon(SNAME("Bake"), SNAME("EditorIcons")));
	bake->set_text(TTR("Bake Lightmaps"));
	bake->hide();
	bake->connect("pressed", Callable(this, "_bake"));
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, bake);
	lightmap = nullptr;

	file_dialog = memnew(EditorFileDialog);
	file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	file_dialog->add_filter("*.lmbake ; LightMap Bake");
	file_dialog->set_title(TTR("Select lightmap bake file:"));
	file_dialog->connect("file_selected", callable_mp(this, &LightmapGIEditorPlugin::_bake_select_file));
	bake->add_child(file_dialog);
}

LightmapGIEditorPlugin::~LightmapGIEditorPlugin() {
}
