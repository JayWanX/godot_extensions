#pragma once

#include "core/io/resource.h"
#include "core/variant/variant.h"

/// 定义单个项目设置的元数据资源。
class ProjectSetting : public Resource {
	GDCLASS(ProjectSetting, Resource);

	static void _bind_methods();

protected:
	void _validate_property(PropertyInfo &p_property) const;

public:
	/// 设置路径（含点号分隔的层级名）
	String setting_path;
	/// 默认值（设置不存在时读取）
	Variant default_value;
	/// 值类型（Variant.Type 枚举）
	Variant::Type value_type = Variant::Type::NIL;
	/// 初始值（注册设置时写入）
	Variant initial_value;
	/// 编辑器属性提示类型
	PropertyHint type_hint = PropertyHint::PROPERTY_HINT_NONE;
	/// 编辑器属性提示字符串
	String hint_string;
	/// 是否显示在基础设置中
	bool basic = true;
	/// 是否为内部设置
	bool internal = false;
	/// 修改后是否需要重启生效
	bool restart_if_changed = false;

	/// 创建项目设置元数据资源。
	/// [param setting_path] 设置路径（含点号分隔的层级名）。
	/// [param default_value] 默认值（设置不存在时读取）。
	/// [param value_type] 值类型（Variant.Type 枚举）。
	/// [param initial_value] 初始值（注册设置时写入）。
	/// [param type_hint] 编辑器属性提示类型。
	/// [param hint_string] 编辑器属性提示字符串。
	/// [param basic] 是否显示在基础设置中。
	/// [param internal] 是否为内部设置。
	/// [param restart_if_changed] 修改后是否需要重启生效。
	/// 新创建的项目设置资源。
	static Ref<ProjectSetting> create(
			const String &p_setting_path = String(),
			const Variant &p_default_value = Variant(),
			Variant::Type p_value_type = Variant::Type::NIL,
			const Variant &p_initial_value = Variant(),
			PropertyHint p_type_hint = PropertyHint::PROPERTY_HINT_NONE,
			const String &p_hint_string = String(),
			bool p_basic = true,
			bool p_internal = false,
			bool p_restart_if_changed = false);

private:
	/// 将 Variant 按目标 Variant 类型转换后返回；无法转换时返回原值。
	static Variant convert_value_type(const Variant &p_value, Variant::Type p_type);

	String _get_setting_path() const { return setting_path; }
	void _set_setting_path(const String &p_value) { setting_path = p_value; }

	Variant _get_default_value() const { return default_value; }
	void _set_default_value(const Variant &p_value) { default_value = p_value; }

	int _get_value_type() const { return static_cast<int>(value_type); }
	void _set_value_type(int p_value) {
		value_type = static_cast<Variant::Type>(p_value);
		// 变更类型时同步转换已有默认值/初始值，保证与 value_type 一致。
		if (value_type != Variant::Type::NIL) {
			default_value = convert_value_type(default_value, value_type);
			initial_value = convert_value_type(initial_value, value_type);
		}
	}

	Variant _get_initial_value() const { return initial_value; }
	void _set_initial_value(const Variant &p_value) { initial_value = p_value; }

	int _get_type_hint() const { return static_cast<int>(type_hint); }
	void _set_type_hint(int p_value) { type_hint = static_cast<PropertyHint>(p_value); }

	String _get_hint_string() const { return hint_string; }
	void _set_hint_string(const String &p_value) { hint_string = p_value; }

	bool _get_basic() const { return basic; }
	void _set_basic(bool p_value) { basic = p_value; }

	bool _get_internal() const { return internal; }
	void _set_internal(bool p_value) { internal = p_value; }

	bool _get_restart_if_changed() const { return restart_if_changed; }
	void _set_restart_if_changed(bool p_value) { restart_if_changed = p_value; }
};