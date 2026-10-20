#pragma once

#include "core/object/object.h"
#include "core/variant/dictionary.h"

/// 字典工具类：提供字典结构相关的静态方法，不可实例化。
class DictUtils : public Object {
	GDCLASS(DictUtils, Object);

	static void _bind_methods();

	/// 递归提取字典的"纯键结构"：嵌套字典保留结构，其他值统一替换为 null。
	static Dictionary _extract_key_structure(const Dictionary &p_dict);

public:
	/// 检查两个字典的键结构是否完全一致（仅比较键形状，不比较值）。
	/// [param dict1] 第一个字典
	/// [param dict2] 第二个字典
	/// 键形状完全一致则返回 true。
	static bool has_same_keys_structure(const Dictionary &p_dict1, const Dictionary &p_dict2);
};