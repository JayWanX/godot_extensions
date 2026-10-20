#pragma once

#include "core/object/object.h"
#include "core/templates/hash_set.h"

/// 文件系统工具类：提供文件类型判定等纯静态方法，不可实例化。
/// 判定基于扩展名/文件名白名单快路径，未命中时读取文件头查 NUL 字节兜底。
class FileUtils : public Object {
	GDCLASS(FileUtils, Object);

	static void _bind_methods();

	/// 文本文件扩展名快路径白名单。
	static const HashSet<String> &_text_extensions();
	/// 二进制文件扩展名白名单。
	static const HashSet<String> &_binary_extensions();
	/// 文本类点文件或知名构建文件名白名单。
	static const HashSet<String> &_text_basenames();
	/// 二进制类知名文件名白名单。
	static const HashSet<String> &_binary_basenames();

public:
	/// 判断文件是否可视为文本文件。
	/// 先按扩展名/文件名做零 IO 快路径判定，未命中再读取文件头查 NUL 字节兜底。
	/// [param file] 文件路径，可为系统绝对路径或 res:// 路径。
	/// [param max_read_bytes] 读取文件头的最大字节数。
	/// 判定为文本文件则返回 true，否则（含文件不存在或无法打开）返回 false。
	static bool is_text_file(const String &p_file, int p_max_read_bytes = 4096);
};