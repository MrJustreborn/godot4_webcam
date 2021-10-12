/*************************************************************************/
/*  pluginscript_script.h                                                */
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

#ifndef PLUGINSCRIPT_SCRIPT_H
#define PLUGINSCRIPT_SCRIPT_H

// Godot imports

#include "core/doc_data.h"
#include "core/object/script_language.h"
// PluginScript imports
#include "pluginscript_language.h"
#include <pluginscript/godot_pluginscript.h>

class PluginScript : public Script {
	GDCLASS(PluginScript, Script);

	friend class PluginScriptInstance;
	friend class PluginScriptLanguage;

private:
	godot_pluginscript_script_data *_data = nullptr;
	const godot_pluginscript_script_desc *_desc = nullptr;
	PluginScriptLanguage *_language = nullptr;
	bool _tool = false;
	bool _valid = false;

	Ref<Script> _ref_base_parent;
	StringName _native_parent;
	SelfList<PluginScript> _script_list;

	Map<StringName, int> _member_lines;
	Map<StringName, Variant> _properties_default_values;
	Map<StringName, PropertyInfo> _properties_info;
	Map<StringName, MethodInfo> _signals_info;
	Map<StringName, MethodInfo> _methods_info;
	Vector<Multiplayer::RPCConfig> _rpc_methods;

	Set<Object *> _instances;
	//exported members
	String _source;
	String _path;
	StringName _name;
	String _icon_path;

protected:
	static void _bind_methods();

	bool inherits_script(const Ref<Script> &p_script) const override;

	PluginScriptInstance *_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, Callable::CallError &r_error);
	Variant _new(const Variant **p_args, int p_argcount, Callable::CallError &r_error);

#ifdef TOOLS_ENABLED
	Set<PlaceHolderScriptInstance *> placeholders;
	//void _update_placeholder(PlaceHolderScriptInstance *p_placeholder);
	virtual void _placeholder_erased(PlaceHolderScriptInstance *p_placeholder) override;
#endif
public:
	String get_script_class_name() const {
		return _name;
	}

	String get_script_class_icon_path() const {
		return _icon_path;
	}

	virtual bool can_instantiate() const override;

	virtual Ref<Script> get_base_script() const override; //for script inheritance

	virtual StringName get_instance_base_type() const override; // this may not work in all scripts, will return empty if so
	virtual ScriptInstance *instance_create(Object *p_this) override;
	virtual bool instance_has(const Object *p_this) const override;

	virtual bool has_source_code() const override;
	virtual String get_source_code() const override;
	virtual void set_source_code(const String &p_code) override;
	virtual Error reload(bool p_keep_state = false) override;
	// TODO: load_source_code only allow utf-8 file, should handle bytecode as well ?
	virtual Error load_source_code(const String &p_path);

#ifdef TOOLS_ENABLED
	virtual const Vector<DocData::ClassDoc> &get_documentation() const override {
		static Vector<DocData::ClassDoc> docs;
		return docs;
	}
#endif // TOOLS_ENABLED

	virtual bool has_method(const StringName &p_method) const override;
	virtual MethodInfo get_method_info(const StringName &p_method) const override;

	bool has_property(const StringName &p_method) const;
	PropertyInfo get_property_info(const StringName &p_property) const;

	bool is_tool() const override { return _tool; }
	bool is_valid() const override { return true; }

	virtual ScriptLanguage *get_language() const override;

	virtual bool has_script_signal(const StringName &p_signal) const override;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const override;

	virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const override;

	virtual void update_exports() override;
	virtual void get_script_method_list(List<MethodInfo> *r_methods) const override;
	virtual void get_script_property_list(List<PropertyInfo> *r_properties) const override;

	virtual int get_member_line(const StringName &p_member) const override;

	virtual const Vector<Multiplayer::RPCConfig> get_rpc_methods() const override;

	PluginScript();
	void init(PluginScriptLanguage *language);
	virtual ~PluginScript();
};

#endif // PLUGINSCRIPT_SCRIPT_H
