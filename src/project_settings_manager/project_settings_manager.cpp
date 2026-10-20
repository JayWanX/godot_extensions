#include "project_settings_manager.h"

#include "core/config/project_settings.h"
#include "core/object/class_db.h"
#include "core/variant/variant.h"

void ProjectSettingsManager::_bind_methods() {
	ClassDB::bind_static_method("ProjectSettingsManager",
			D_METHOD("add_setting", "project_setting"), &ProjectSettingsManager::add_setting);
	ClassDB::bind_static_method("ProjectSettingsManager",
			D_METHOD("add_settings", "project_settings"), &ProjectSettingsManager::add_settings);
	ClassDB::bind_static_method("ProjectSettingsManager",
			D_METHOD("remove_setting", "project_setting"), &ProjectSettingsManager::remove_setting);
	ClassDB::bind_static_method("ProjectSettingsManager",
			D_METHOD("remove_settings", "project_settings"), &ProjectSettingsManager::remove_settings);
	ClassDB::bind_static_method("ProjectSettingsManager",
			D_METHOD("get_setting", "project_setting"), &ProjectSettingsManager::get_setting);
}

void ProjectSettingsManager::add_setting(const Ref<ProjectSetting> &p_project_setting) {
	_validate_value_type(p_project_setting);
	_apply_setting(p_project_setting);
	_save();
}

void ProjectSettingsManager::add_settings(const Array &p_project_settings) {
	if (p_project_settings.is_empty()) {
		return;
	}
	for (int i = 0; i < p_project_settings.size(); ++i) {
		const Ref<ProjectSetting> project_setting = p_project_settings[i];
		_validate_value_type(project_setting);
		_apply_setting(project_setting);
	}
	_save();
}

void ProjectSettingsManager::remove_setting(const Ref<ProjectSetting> &p_project_setting) {
	_remove_setting(p_project_setting);
	_save();
}

void ProjectSettingsManager::remove_settings(const Array &p_project_settings) {
	if (p_project_settings.is_empty()) {
		return;
	}
	for (int i = 0; i < p_project_settings.size(); ++i) {
		const Ref<ProjectSetting> project_setting = p_project_settings[i];
		_remove_setting(project_setting);
	}
	_save();
}

Variant ProjectSettingsManager::get_setting(const Ref<ProjectSetting> &p_project_setting) {
	return ProjectSettings::get_singleton()->get_setting(p_project_setting->setting_path, p_project_setting->default_value);
}

void ProjectSettingsManager::_apply_setting(const Ref<ProjectSetting> &p_project_setting) {
	ProjectSettings *settings = ProjectSettings::get_singleton();
	if (!settings->has_setting(p_project_setting->setting_path)) {
		settings->set_setting(p_project_setting->setting_path, p_project_setting->default_value);
	}
	settings->set_initial_value(p_project_setting->setting_path, p_project_setting->initial_value);
	// add_property_info 是绑定到私有 _add_property_info_bind 的方法，经 Object::call 调用。
	const Dictionary property_info = {
			{ "name", p_project_setting->setting_path },
			{ "type", static_cast<int>(p_project_setting->value_type) },
			{ "hint", static_cast<int>(p_project_setting->type_hint) },
			{ "hint_string", p_project_setting->hint_string },
	};
	settings->call("add_property_info", property_info);
	settings->set_as_basic(p_project_setting->setting_path, p_project_setting->basic);
	settings->set_as_internal(p_project_setting->setting_path, p_project_setting->internal);
	settings->set_restart_if_changed(p_project_setting->setting_path, p_project_setting->restart_if_changed);
}

void ProjectSettingsManager::_remove_setting(const Ref<ProjectSetting> &p_project_setting) {
	ProjectSettings::get_singleton()->set_setting(p_project_setting->setting_path, Variant());
}

void ProjectSettingsManager::_validate_value_type(const Ref<ProjectSetting> &p_project_setting) {
	if (p_project_setting->default_value.get_type() == Variant::NIL) {
		return;
	}
	const Variant::Type declared_type = static_cast<Variant::Type>(p_project_setting->value_type);
	const Variant::Type actual_type = p_project_setting->default_value.get_type();
	if (declared_type == Variant::STRING && actual_type == Variant::STRING_NAME) {
		return;
	}
	if (declared_type == Variant::STRING_NAME && actual_type == Variant::STRING) {
		return;
	}
	if (declared_type != actual_type) {
		ERR_PRINT(vformat("ProjectSetting '%s' default value type %d does not match declared type %d.",
				p_project_setting->setting_path, static_cast<int>(actual_type), static_cast<int>(declared_type)));
	}
}

void ProjectSettingsManager::_save() {
	const Error error = ProjectSettings::get_singleton()->save();
	if (error != OK) {
		ERR_PRINT(vformat("Encountered error %d while saving project settings.", static_cast<int>(error)));
	}
}