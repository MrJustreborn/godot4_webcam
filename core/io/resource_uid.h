/*************************************************************************/
/*  resource_uid.h                                                       */
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

#ifndef RESOURCE_UUID_H
#define RESOURCE_UUID_H

#include "core/object/ref_counted.h"
#include "core/string/string_name.h"
#include "core/templates/ordered_hash_map.h"

class Crypto;
class ResourceUID : public Object {
	GDCLASS(ResourceUID, Object)
public:
	typedef int64_t ID;
	enum {
		INVALID_ID = -1
	};

	static String get_cache_file();

private:
	mutable Ref<Crypto> crypto;
	Mutex mutex;
	struct Cache {
		CharString cs;
		bool saved_to_cache = false;
	};

	OrderedHashMap<ID, Cache> unique_ids; //unique IDs and utf8 paths (less memory used)
	static ResourceUID *singleton;

	uint32_t cache_entries = 0;
	bool changed = false;

protected:
	static void _bind_methods();

public:
	String id_to_text(ID p_id) const;
	ID text_to_id(const String &p_text) const;

	ID create_id() const;
	bool has_id(ID p_id) const;
	void add_id(ID p_id, const String &p_path);
	void set_id(ID p_id, const String &p_path);
	String get_id_path(ID p_id) const;
	void remove_id(ID p_id);

	Error load_from_cache();
	Error save_to_cache();
	Error update_cache();

	void clear();

	static ResourceUID *get_singleton() { return singleton; }

	ResourceUID();
	~ResourceUID();
};

#endif // RESOURCEUUID_H
