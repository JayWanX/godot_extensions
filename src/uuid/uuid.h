#pragma once

#include "core/object/ref_counted.h"

/// 表示一个符合 UUID v4 格式的唯一标识符，基于 16 个随机字节编码为十六进制并用连字符分组。
class UUID : public RefCounted {
	GDCLASS(UUID, RefCounted);

	StringName _id;

	static void _bind_methods();

protected:
	/// 重写 Object::_to_string，使 str(uuid) 返回其文本表示。
	String _to_string() override;

public:
	/// 构造一个空 UUID 实例，其文本表示为 "00000000-0000-0000-0000-000000000000"。
	UUID();

	/// 比较当前 UUID 与另一个 UUID 是否相等。
	/// [param other] 用于比较的另一个 UUID
	/// 两者文本表示相同则返回 true。
	bool equal_with(const Ref<UUID> &p_other) const;

	/// 生成一个新的随机 UUID。
	/// 格式化后的 UUID 文本，如 `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`。
	static StringName generate();
};