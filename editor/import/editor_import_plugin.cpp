/*************************************************************************/
/*  editor_import_plugin.cpp                                             */
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

#include "editor_import_plugin.h"
#include "core/object/script_language.h"

EditorImportPlugin::EditorImportPlugin() {
}

String EditorImportPlugin::get_importer_name() const {
	String ret;
	if (GDVIRTUAL_CALL(_get_importer_name, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(String(), "Unimplemented _get_importer_name in add-on.");
}

String EditorImportPlugin::get_visible_name() const {
	String ret;
	if (GDVIRTUAL_CALL(_get_visible_name, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(String(), "Unimplemented _get_visible_name in add-on.");
}

void EditorImportPlugin::get_recognized_extensions(List<String> *p_extensions) const {
	Vector<String> extensions;

	if (GDVIRTUAL_CALL(_get_recognized_extensions, extensions)) {
		for (int i = 0; i < extensions.size(); i++) {
			p_extensions->push_back(extensions[i]);
		}
	}
	ERR_FAIL_MSG("Unimplemented _get_recognized_extensions in add-on.");
}

String EditorImportPlugin::get_preset_name(int p_idx) const {
	String ret;
	if (GDVIRTUAL_CALL(_get_preset_name, p_idx, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(String(), "Unimplemented _get_preset_name in add-on.");
}

int EditorImportPlugin::get_preset_count() const {
	int ret;
	if (GDVIRTUAL_CALL(_get_preset_count, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(-1, "Unimplemented _get_preset_count in add-on.");
}

String EditorImportPlugin::get_save_extension() const {
	String ret;
	if (GDVIRTUAL_CALL(_get_save_extension, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(String(), "Unimplemented _get_save_extension in add-on.");
}

String EditorImportPlugin::get_resource_type() const {
	String ret;
	if (GDVIRTUAL_CALL(_get_resource_type, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(String(), "Unimplemented _get_resource_type in add-on.");
}

float EditorImportPlugin::get_priority() const {
	float ret;
	if (GDVIRTUAL_CALL(_get_priority, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(-1, "Unimplemented _get_priority in add-on.");
}

int EditorImportPlugin::get_import_order() const {
	int ret;
	if (GDVIRTUAL_CALL(_get_import_order, ret)) {
		return ret;
	}
	ERR_FAIL_V_MSG(-1, "Unimplemented _get_import_order in add-on.");
}

void EditorImportPlugin::get_import_options(List<ResourceImporter::ImportOption> *r_options, int p_preset) const {
	Array needed;
	needed.push_back("name");
	needed.push_back("default_value");
	Array options;
	if (GDVIRTUAL_CALL(_get_import_options, p_preset, options)) {
		for (int i = 0; i < options.size(); i++) {
			Dictionary d = options[i];
			ERR_FAIL_COND(!d.has_all(needed));
			String name = d["name"];
			Variant default_value = d["default_value"];

			PropertyHint hint = PROPERTY_HINT_NONE;
			if (d.has("property_hint")) {
				hint = (PropertyHint)d["property_hint"].operator int64_t();
			}

			String hint_string;
			if (d.has("hint_string")) {
				hint_string = d["hint_string"];
			}

			uint32_t usage = PROPERTY_USAGE_DEFAULT;
			if (d.has("usage")) {
				usage = d["usage"];
			}

			ImportOption option(PropertyInfo(default_value.get_type(), name, hint, hint_string, usage), default_value);
			r_options->push_back(option);
		}
	}

	ERR_FAIL_MSG("Unimplemented _get_import_options in add-on.");
}

bool EditorImportPlugin::get_option_visibility(const String &p_option, const Map<StringName, Variant> &p_options) const {
	Dictionary d;
	Map<StringName, Variant>::Element *E = p_options.front();
	while (E) {
		d[E->key()] = E->get();
		E = E->next();
	}
	bool visible;
	if (GDVIRTUAL_CALL(_get_option_visibility, p_option, d, visible)) {
		return visible;
	}

	ERR_FAIL_V_MSG(false, "Unimplemented _get_option_visibility in add-on.");
}

Error EditorImportPlugin::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	Dictionary options;
	Array platform_variants, gen_files;

	Map<StringName, Variant>::Element *E = p_options.front();
	while (E) {
		options[E->key()] = E->get();
		E = E->next();
	}

	int err;
	if (GDVIRTUAL_CALL(_import, p_source_file, p_save_path, options, platform_variants, gen_files, err)) {
		Error ret_err = Error(err);

		for (int i = 0; i < platform_variants.size(); i++) {
			r_platform_variants->push_back(platform_variants[i]);
		}
		for (int i = 0; i < gen_files.size(); i++) {
			r_gen_files->push_back(gen_files[i]);
		}
		return ret_err;
	}
	ERR_FAIL_V_MSG(ERR_METHOD_NOT_FOUND, "Unimplemented _import in add-on.");
}

void EditorImportPlugin::_bind_methods() {
	GDVIRTUAL_BIND(_get_importer_name)
	GDVIRTUAL_BIND(_get_visible_name)
	GDVIRTUAL_BIND(_get_preset_count)
	GDVIRTUAL_BIND(_get_preset_name, "preset_index")
	GDVIRTUAL_BIND(_get_recognized_extensions)
	GDVIRTUAL_BIND(_get_import_options, "preset_index")
	GDVIRTUAL_BIND(_get_save_extension)
	GDVIRTUAL_BIND(_get_resource_type)
	GDVIRTUAL_BIND(_get_priority)
	GDVIRTUAL_BIND(_get_import_order)
	GDVIRTUAL_BIND(_get_option_visibility, "option_name", "options")
	GDVIRTUAL_BIND(_import, "source_file", "save_path", "options", "platform_variants", "gen_files");
}
