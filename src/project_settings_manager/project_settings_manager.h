#pragma once

#include "core/object/object.h"
#include "core/variant/array.h"

#include "project_setting.h"

/// 提供项目设置的批量注册、移除与读取。
class ProjectSettingsManager : public Object {
	GDCLASS(ProjectSettingsManager, Object);

	static void _bind_methods();

private:
	/// 验证默认值类型与声明类型是否一致，不一致时断言失败。
	static void _validate_value_type(const Ref<ProjectSetting> &p_project_setting);
	/// 应用设置的内存操作（注册默认值、初始值、属性提示与用途标志）。
	static void _apply_setting(const Ref<ProjectSetting> &p_project_setting);
	/// 删除设置的内存操作。
	static void _remove_setting(const Ref<ProjectSetting> &p_project_setting);
	/// 保存项目设置到 project.godot，失败时输出错误。
	static void _save();

public:
	/// 注册单个项目设置并保存到 ProjectSettings。
	/// [param project_setting] 设置定义资源
	static void add_setting(const Ref<ProjectSetting> &p_project_setting);
	/// 批量注册项目设置，合并为一次保存。
	/// [param project_settings] 设置定义列表
	static void add_settings(const Array &p_project_settings);
	/// 删除单个项目设置并保存。
	/// [param project_setting] 设置定义资源
	static void remove_setting(const Ref<ProjectSetting> &p_project_setting);
	/// 批量删除项目设置，合并为一次保存。
	/// [param project_settings] 设置定义列表
	static void remove_settings(const Array &p_project_settings);
	/// 读取项目设置的当前值。
	/// [param project_setting] 设置定义资源
	/// 项目设置的当前值，不存在时返回默认值。
	static Variant get_setting(const Ref<ProjectSetting> &p_project_setting);
};