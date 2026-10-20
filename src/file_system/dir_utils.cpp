#include "dir_utils.h"

#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/object/class_db.h"
#include "core/string/ustring.h"

void DirUtils::_bind_methods() {
	ClassDB::bind_static_method("DirUtils",
			D_METHOD("compute_dir_hash", "path", "extensions"),
			&DirUtils::compute_dir_hash, DEFVAL(PackedStringArray()));
	ClassDB::bind_static_method("DirUtils",
			D_METHOD("compute_dir_hash_recursive", "path", "extensions"),
			&DirUtils::compute_dir_hash_recursive, DEFVAL(PackedStringArray()));
	ClassDB::bind_static_method("DirUtils",
			D_METHOD("copy_recursive", "src", "dst"),
			&DirUtils::copy_recursive);
	ClassDB::bind_static_method("DirUtils",
			D_METHOD("delete_directory_recursive", "path"),
			&DirUtils::delete_directory_recursive);
	ClassDB::bind_static_method("DirUtils",
			D_METHOD("get_directory_tree", "path"),
			&DirUtils::get_directory_tree);
	ClassDB::bind_static_method("DirUtils",
			D_METHOD("walk_directory_tree", "path", "file_handler", "dir_handler", "recursive"),
			&DirUtils::walk_directory_tree, DEFVAL(Callable()), DEFVAL(true));
	ClassDB::bind_static_method("DirUtils",
			D_METHOD("delete_files_if", "root_path", "should_delete"),
			&DirUtils::delete_files_if);
}

String DirUtils::_hash_joined_lines(Vector<String> &r_file_list) {
	r_file_list.sort();
	String joined;
	for (int i = 0; i < r_file_list.size(); ++i) {
		if (i > 0) {
			joined += "\n";
		}
		joined += r_file_list[i];
	}
	return joined.md5_text();
}

static Vector<String> build_lowered_exts(const PackedStringArray &p_extensions) {
	Vector<String> lowered_exts;
	for (int i = 0; i < p_extensions.size(); ++i) {
		lowered_exts.append(p_extensions[i].to_lower());
	}
	return lowered_exts;
}

String DirUtils::compute_dir_hash(const String &p_path, const PackedStringArray &p_extensions) {
	const Ref<DirAccess> dir = DirAccess::open(p_path);
	if (dir.is_null()) {
		ERR_PRINT("Cannot open directory: " + p_path);
		return String();
	}

	const Vector<String> lowered_exts = build_lowered_exts(p_extensions);
	const bool filter_enabled = !lowered_exts.is_empty();

	Vector<String> file_list;
	dir->list_dir_begin();
	String file_name = dir->get_next();
	while (!file_name.is_empty()) {
		if (!dir->current_is_dir()) {
			bool include = true;
			if (filter_enabled) {
				const String file_ext = file_name.get_extension().to_lower();
				if (!lowered_exts.has(file_ext)) {
					include = false;
				}
			}
			if (include) {
				const String file_path = p_path.path_join(file_name);
				const uint64_t modified_time = FileAccess::get_modified_time(file_path);
				file_list.append(file_name + ":" + uitos(modified_time));
			}
		}
		file_name = dir->get_next();
	}

	return _hash_joined_lines(file_list);
}

String DirUtils::compute_dir_hash_recursive(const String &p_path,
		const PackedStringArray &p_extensions) {
	const Ref<DirAccess> dir = DirAccess::open(p_path);
	if (dir.is_null()) {
		ERR_PRINT("Cannot open directory: " + p_path);
		return String();
	}

	const Vector<String> lowered_exts = build_lowered_exts(p_extensions);
	const bool filter_enabled = !lowered_exts.is_empty();

	Vector<String> file_list;
	_collect_files_recursive(p_path, String(), lowered_exts, filter_enabled, file_list);

	return _hash_joined_lines(file_list);
}

void DirUtils::_collect_files_recursive(const String &p_base_path, const String &p_relative_prefix,
		const Vector<String> &p_lowered_exts, bool p_filter_enabled, Vector<String> &r_file_list) {
	const String current_path = p_relative_prefix.is_empty()
			? p_base_path
			: p_base_path.path_join(p_relative_prefix);
	const Ref<DirAccess> dir = DirAccess::open(current_path);
	if (dir.is_null()) {
		return;
	}

	dir->list_dir_begin();
	String entry = dir->get_next();
	while (!entry.is_empty()) {
		// `.` 与 `..` 必须跳过：它们是当前目录与父目录的别名，`current_is_dir()`
		// 对二者均为真，被当成子目录递归进去即无限递归（实测在仅含一个文件的
		// 目录上就会卡死）。本文件其余四个遍历函数都有这道过滤，此处曾遗漏。
		if (entry == "." || entry == "..") {
			entry = dir->get_next();
			continue;
		}
		if (dir->current_is_dir()) {
			const String entry_rel = p_relative_prefix.is_empty()
					? entry
					: p_relative_prefix.path_join(entry);
			_collect_files_recursive(p_base_path, entry_rel, p_lowered_exts, p_filter_enabled,
					r_file_list);
		} else if (!p_filter_enabled || p_lowered_exts.has(entry.get_extension().to_lower())) {
			const String full_path = current_path.path_join(entry);
			const uint64_t modified_time = FileAccess::get_modified_time(full_path);
			// 相对根路径的记录，保证跨目录展开时哈希顺序稳定。
			const String entry_rel = p_relative_prefix.is_empty()
					? entry
					: p_relative_prefix.path_join(entry);
			r_file_list.append(entry_rel + ":" + uitos(modified_time));
		}
		entry = dir->get_next();
	}
	dir->list_dir_end();
}

Error DirUtils::copy_recursive(const String &p_src, const String &p_dst) {
	const Ref<DirAccess> dir = DirAccess::open(p_src);
	if (dir.is_null()) {
		return ERR_CANT_OPEN;
	}

	const Error mkdir_err = DirAccess::make_dir_recursive_absolute(p_dst);
	if (mkdir_err != OK) {
		return mkdir_err;
	}

	dir->list_dir_begin();
	String file_name = dir->get_next();
	while (!file_name.is_empty()) {
		if (file_name == "." || file_name == "..") {
			file_name = dir->get_next();
			continue;
		}

		const String src_path = p_src.path_join(file_name);
		const String dst_path = p_dst.path_join(file_name);

		if (dir->current_is_dir()) {
			const Error result = copy_recursive(src_path, dst_path);
			if (result != OK) {
				return result;
			}
		} else {
			const Error result = DirAccess::copy_absolute(src_path, dst_path);
			if (result != OK) {
				return result;
			}
		}

		file_name = dir->get_next();
	}
	dir->list_dir_end();
	return OK;
}

bool DirUtils::delete_directory_recursive(const String &p_path) {
	const Ref<DirAccess> dir = DirAccess::open(p_path);
	if (dir.is_null()) {
		return false;
	}

	bool all_removed = true;
	dir->list_dir_begin();
	String item = dir->get_next();
	while (!item.is_empty()) {
		if (item != "." && item != "..") {
			const String full_path = p_path.path_join(item);
			if (dir->current_is_dir()) {
				// 子目录的失败必须累计：忽略它会让「目录树只删掉了一半」也报成功。
				if (!delete_directory_recursive(full_path)) {
					all_removed = false;
				}
			} else if (DirAccess::remove_absolute(full_path) != OK) {
				// 与 delete_files_if 同口径：删不掉就报出来，不静默跳过。
				ERR_PRINT("删除失败: " + full_path);
				all_removed = false;
			}
		}
		item = dir->get_next();
	}
	dir->list_dir_end();
	// 父目录本身删不掉同样是失败；与子项的失败合并成一个结论。
	return DirAccess::remove_absolute(p_path) == OK && all_removed;
}

Dictionary DirUtils::get_directory_tree(const String &p_path) {
	const Ref<DirAccess> dir = DirAccess::open(p_path);
	if (dir.is_null()) {
		return Dictionary();
	}

	Dictionary result;
	result["name"] = p_path.get_file();
	result["path"] = p_path;
	result["directories"] = Array();
	result["files"] = Array();

	if (String(result["name"]).is_empty()) {
		result["name"] = p_path;
	}

	dir->list_dir_begin();
	String item = dir->get_next();
	while (!item.is_empty()) {
		if (item == "." || item == "..") {
			item = dir->get_next();
			continue;
		}

		const String full_path = p_path.path_join(item);
		if (dir->current_is_dir()) {
			const Dictionary sub_tree = get_directory_tree(full_path);
			if (!sub_tree.is_empty()) {
				Array directories = result["directories"];
				directories.append(sub_tree);
				result["directories"] = directories;
			}
		} else {
			Array files = result["files"];
			files.append(item);
			result["files"] = files;
		}

		item = dir->get_next();
	}
	dir->list_dir_end();
	return result;
}

bool DirUtils::walk_directory_tree(const String &p_path, const Callable &p_file_handler,
		const Callable &p_dir_handler, bool p_recursive) {
	const Ref<DirAccess> dir = DirAccess::open(p_path);
	if (dir.is_null()) {
		return false;
	}

	if (p_dir_handler.is_valid() && !p_dir_handler.call(p_path, p_path.get_file()).booleanize()) {
		return false;
	}

	dir->list_dir_begin();
	String item = dir->get_next();
	while (!item.is_empty()) {
		if (item == "." || item == "..") {
			item = dir->get_next();
			continue;
		}

		const String full_path = p_path.path_join(item);
		if (dir->current_is_dir() && p_recursive) {
			if (!walk_directory_tree(full_path, p_file_handler, p_dir_handler, p_recursive)) {
				dir->list_dir_end();
				return false;
			}
		} else if (!dir->current_is_dir()) {
			if (p_file_handler.is_valid() && !p_file_handler.call(full_path, item).booleanize()) {
				dir->list_dir_end();
				return false;
			}
		}

		item = dir->get_next();
	}
	dir->list_dir_end();
	return true;
}

void DirUtils::_delete_files_if_recursive(const String &p_root_path, const Callable &p_should_delete,
		int &r_count) {
	const Ref<DirAccess> dir = DirAccess::open(p_root_path);
	if (dir.is_null()) {
		return;
	}

	dir->list_dir_begin();
	String item = dir->get_next();
	while (!item.is_empty()) {
		if (item == "." || item == "..") {
			item = dir->get_next();
			continue;
		}

		const String full_path = p_root_path.path_join(item);
		if (dir->current_is_dir()) {
			_delete_files_if_recursive(full_path, p_should_delete, r_count);
		} else if (p_should_delete.call(full_path, item).booleanize()) {
			if (DirAccess::remove_absolute(full_path) == OK) {
				r_count += 1;
			} else {
				ERR_PRINT("删除失败: " + full_path);
			}
		}

		item = dir->get_next();
	}
	dir->list_dir_end();
}

int DirUtils::delete_files_if(const String &p_root_path, const Callable &p_should_delete) {
	int count = 0;
	_delete_files_if_recursive(p_root_path, p_should_delete, count);
	return count;
}