#pragma once

#include "core/object/object.h"
#include "core/object/script_language.h"
#include "core/variant/dictionary.h"

/// 读取引擎解析好的类参考 / 脚本文档，供 GDScript 查询。
///
/// 设计为可实例化的普通类：addon 先用 `ClassDB.class_exists()` 探测、再
/// `ClassDB.instantiate()` 惰性构建，这样「未编译该模块时回退本地缓存」就不会在
/// 解析期引用到不存在的类名。
class ClassDocReader : public Object {
	GDCLASS(ClassDocReader, Object);

	static void _bind_methods();

public:
	/// 取某个类各成员（方法 / 信号 / 属性 / 常量）的文档。
	/// [param class_name] 类名（原生类，或带 `class_name` 的脚本类）[br]
	/// 返回字典含四组键：`methods` / `signals` / `properties` / `constants`，
	/// 值为「成员名 → 文档文本」。原生类经 DTR 随编辑器语言翻译；脚本类返回作者原文。
	Dictionary get_class_docs(const StringName &p_class_name) const;

	/// 取某个脚本（含不带 `class_name` 的脚本）各成员的文档。
	/// [param script] 脚本资源；需已被编辑器分析过，否则返回空字典[br]
	/// 返回体结构与 `get_class_docs` 一致，文本为作者原文（不做翻译）。
	Dictionary get_script_docs(const Ref<Script> &p_script) const;

	/// 该类是否存在且文档已就绪。
	bool has_class_doc(const StringName &p_class_name) const;
};