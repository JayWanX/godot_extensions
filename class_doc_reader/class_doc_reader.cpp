#include "class_doc_reader.h"

#include "core/object/class_db.h"

#ifdef TOOLS_ENABLED
#include "core/object/script_language.h" // Script
#include "core/string/ustring.h" // DTR
#include "editor/doc/doc_tools.h"
#include "editor/doc/editor_help.h"
#endif

void ClassDocReader::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_class_docs", "class_name"), &ClassDocReader::get_class_docs);
	ClassDB::bind_method(D_METHOD("get_script_docs", "script"), &ClassDocReader::get_script_docs);
	ClassDB::bind_method(D_METHOD("has_class_doc", "class_name"), &ClassDocReader::has_class_doc);
}

#ifdef TOOLS_ENABLED
/// 把单个成员写入组字典；无描述时跳过。
static void _put_member(Dictionary &p_map, const String &p_name, const String &p_desc, bool p_use_l10n) {
	if (p_desc.is_empty()) {
		return;
	}
	p_map[p_name] = p_use_l10n ? DTR(p_desc) : p_desc;
}

/// 把一份类文档转成「methods / signals / properties / constants → 成员名 → 文本」。
static Dictionary _to_member_map(const DocData::ClassDoc &p_cd, bool p_use_l10n) {
	Dictionary methods;
	Dictionary signals;
	Dictionary properties;
	Dictionary constants;
	for (const DocData::MethodDoc &m : p_cd.methods) {
		_put_member(methods, m.name, m.description, p_use_l10n);
	}
	for (const DocData::MethodDoc &s : p_cd.signals) {
		_put_member(signals, s.name, s.description, p_use_l10n);
	}
	for (const DocData::PropertyDoc &p : p_cd.properties) {
		_put_member(properties, p.name, p.description, p_use_l10n);
	}
	for (const DocData::ConstantDoc &k : p_cd.constants) {
		_put_member(constants, k.name, k.description, p_use_l10n);
	}
	Dictionary result;
	result["methods"] = methods;
	result["signals"] = signals;
	result["properties"] = properties;
	result["constants"] = constants;
	return result;
}
#endif

Dictionary ClassDocReader::get_class_docs(const StringName &p_class_name) const {
	Dictionary result;
#ifdef TOOLS_ENABLED
	const DocData::ClassDoc *cd = EditorHelp::get_doc(String(p_class_name));
	if (cd == nullptr) {
		return result;
	}
	// 脚本类（is_script_doc）的文档是作者原文，不做翻译；原生类才经 DTR 跟随界面语言。
	return _to_member_map(*cd, !cd->is_script_doc);
#else
	return result;
#endif
}

Dictionary ClassDocReader::get_script_docs(const Ref<Script> &p_script) const {
	Dictionary result;
	ERR_FAIL_COND_V(p_script.is_null(), result);
#ifdef TOOLS_ENABLED
	// 引擎解析脚本时会把文档写进 GDScript::docs，这里直接读（不依赖 class_name）。
	Vector<DocData::ClassDoc> class_docs = p_script->get_documentation();
	if (class_docs.is_empty()) {
		return result;
	}
	return _to_member_map(class_docs[0], false); // 脚本文档为作者原文，不做翻译。
#else
	return result;
#endif
}

bool ClassDocReader::has_class_doc(const StringName &p_class_name) const {
#ifdef TOOLS_ENABLED
	return EditorHelp::get_doc(String(p_class_name)) != nullptr;
#else
	return false;
#endif
}