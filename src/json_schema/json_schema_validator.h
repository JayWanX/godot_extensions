#pragma once

#include "core/object/object.h"
#include "core/string/ustring.h"
#include "core/variant/variant.h"

/// JSON Schema 校验器：校验通过返回空字符串，失败返回人类可读的错误描述。
///
/// 只实现了 JSON Schema 的一个子集，已支持的关键字：
/// type、properties、required、additionalProperties、propertyNames、dependencies、
/// items、prefixItems、minItems、maxItems、multipleOf、minimum、exclusiveMinimum、
/// maximum、exclusiveMaximum、minProperties、maxProperties、minLength、maxLength、
/// pattern、format（仅 color）。未列出的关键字一律忽略。
///
/// **与 `mod_loader` 的 `ModJsonSchemaValidator` 的关系**：本类是那份实现的通用版本。
/// 两者语义一致，但**分属不同模块**——`mod_loader` 只能在模块内自用（模块之间不能互引
/// 头文件），本类则供工程侧与其它模块经 `ClassDB` 取用。修改其一须同步另一处。
class JsonSchemaValidator : public Object {
	GDCLASS(JsonSchemaValidator, Object);

	static void _bind_methods();

	/// 数字允许的最大小数位数
	static constexpr int MAX_DECIMAL_PLACES = 3;
	/// multipleOf 允许的最小乘数
	static constexpr double SMALL_FLOAT_THRESHOLD = 0.001;
	/// 根级校验时使用的属性名
	static constexpr const char *ROOT_PROPERTY_NAME = "schema root";

	/// JSON 数据类型名：数组
	static constexpr const char *JSON_TYPE_ARRAY = "array";
	/// JSON 数据类型名：布尔
	static constexpr const char *JSON_TYPE_BOOLEAN = "boolean";
	/// JSON 数据类型名：整数
	static constexpr const char *JSON_TYPE_INTEGER = "integer";
	/// JSON 数据类型名：null
	static constexpr const char *JSON_TYPE_NULL = "null";
	/// JSON 数据类型名：数字
	static constexpr const char *JSON_TYPE_NUMBER = "number";
	/// JSON 数据类型名：对象
	static constexpr const char *JSON_TYPE_OBJECT = "object";
	/// JSON 数据类型名：字符串
	static constexpr const char *JSON_TYPE_STRING = "string";

	/// 关键字名：type
	static constexpr const char *KEYWORD_TYPE = "type";
	/// 关键字名：properties
	static constexpr const char *KEYWORD_PROPERTIES = "properties";
	/// 关键字名：required
	static constexpr const char *KEYWORD_REQUIRED = "required";
	/// 关键字名：items
	static constexpr const char *KEYWORD_ITEMS = "items";
	/// 关键字名：prefixItems
	static constexpr const char *KEYWORD_PREFIX_ITEMS = "prefixItems";
	/// 关键字名：minItems
	static constexpr const char *KEYWORD_MIN_ITEMS = "minItems";
	/// 关键字名：maxItems
	static constexpr const char *KEYWORD_MAX_ITEMS = "maxItems";
	/// 关键字名：multipleOf
	static constexpr const char *KEYWORD_MULTIPLE_OF = "multipleOf";
	/// 关键字名：minimum
	static constexpr const char *KEYWORD_MINIMUM = "minimum";
	/// 关键字名：exclusiveMinimum
	static constexpr const char *KEYWORD_EXCLUSIVE_MINIMUM = "exclusiveMinimum";
	/// 关键字名：maximum
	static constexpr const char *KEYWORD_MAXIMUM = "maximum";
	/// 关键字名：exclusiveMaximum
	static constexpr const char *KEYWORD_EXCLUSIVE_MAXIMUM = "exclusiveMaximum";
	/// 关键字名：additionalProperties
	static constexpr const char *KEYWORD_ADDITIONAL_PROPERTIES = "additionalProperties";
	/// 关键字名：propertyNames
	static constexpr const char *KEYWORD_PROPERTY_NAMES = "propertyNames";
	/// 关键字名：dependencies
	static constexpr const char *KEYWORD_DEPENDENCIES = "dependencies";
	/// 关键字名：minProperties
	static constexpr const char *KEYWORD_MIN_PROPERTIES = "minProperties";
	/// 关键字名：maxProperties
	static constexpr const char *KEYWORD_MAX_PROPERTIES = "maxProperties";
	/// 关键字名：minLength
	static constexpr const char *KEYWORD_MIN_LENGTH = "minLength";
	/// 关键字名：maxLength
	static constexpr const char *KEYWORD_MAX_LENGTH = "maxLength";
	/// 关键字名：pattern
	static constexpr const char *KEYWORD_PATTERN = "pattern";
	/// 关键字名：format
	static constexpr const char *KEYWORD_FORMAT = "format";

	/// format 关键字支持的取值：颜色
	static constexpr const char *FORMAT_COLOR = "color";

	/// 比较描述：大于
	static constexpr const char *COMPARISON_GREATER = "greater";
	/// 比较描述：大于等于
	static constexpr const char *COMPARISON_GREATER_OR_EQUAL = "greater or equal";
	/// 比较描述：小于
	static constexpr const char *COMPARISON_LESS = "less";
	/// 比较描述：小于等于
	static constexpr const char *COMPARISON_LESS_OR_EQUAL = "less or equal";
	/// 类型描述：对象
	static constexpr const char *TYPE_NAME_OBJECT = "object (dictionary)";
	/// 二选一描述模板
	static constexpr const char *TEMPLATE_OR = "%s or %s";

	/// 错误消息：schema 恒为假
	static constexpr const char *ERROR_SCHEMA_DENY_ALL = "Schema declared as deny all";
	/// 错误消息前缀：schema 本身非法
	static constexpr const char *ERROR_SCHEMA_PREFIX = "Schema error: ";
	/// 错误消息：schema 结构非法
	static constexpr const char *ERROR_SCHEMA_STRUCTURE_INVALID = "Schema error: schema must be empty object or object with 'type' keyword or boolean value";
	/// 错误消息模板：schema 数组项类型非法（属性路径、关键字名、期望类型）
	static constexpr const char *ERROR_SCHEMA_ITEM_TYPE_INVALID = "Schema error: expected that all elements of '%s.%s' must be '%s'";
	/// 错误消息：schema 项缺少 type 关键字
	static constexpr const char *ERROR_SCHEMA_ITEM_WITHOUT_TYPE = "Schema error: any schema item must be object with 'type' keyword";
	/// 错误消息模板：required 关键字非法（属性路径）
	static constexpr const char *ERROR_SCHEMA_REQUIRED_INVALID = "Schema error: expected array of required properties for '%s'";
	/// 错误消息：minItems 必须是数字
	static constexpr const char *ERROR_SCHEMA_MIN_ITEMS_INVALID = "Schema error: minItems must be a non-negative number.";
	/// 错误消息：maxItems 必须是数字
	static constexpr const char *ERROR_SCHEMA_MAX_ITEMS_INVALID = "Schema error: maxItems must be a non-negative number.";
	/// 错误消息模板：prefixItems 已用尽但 items 禁止追加（数组、数组长度）
	static constexpr const char *ERROR_SCHEMA_PREFIX_ITEMS_NO_ADDITIONAL = "Schema error: array '%s' is of size %s but no addition items allowed.";
	/// 错误消息模板：缺少必填属性（属性名、属性路径）
	static constexpr const char *ERROR_REQUIRED_PROPERTY_MISSING = "Missing required property: '%s' for '%s'";
	/// 错误消息模板：出现未声明的属性（属性名）
	static constexpr const char *ERROR_ADDITIONAL_PROPERTY_FOUND = "Additional properties are not required: found '%s'";
	/// 错误消息模板：属性数量不足（实际数量、下限）
	static constexpr const char *ERROR_TOO_FEW_PROPERTIES = "%d propertie(s) are not enough properties, at least %d are required";
	/// 错误消息模板：属性数量过多（实际数量、上限）
	static constexpr const char *ERROR_TOO_MANY_PROPERTIES = "%d propertie(s) are too many properties, at most %d are allowed";
	/// 错误消息模板：数组项数量不足（实际数量、下限）
	static constexpr const char *ERROR_TOO_FEW_ITEMS = "%s item(s) are not enough items, at least %s are required";
	/// 错误消息模板：数组项数量过多（实际数量、上限）
	static constexpr const char *ERROR_TOO_MANY_ITEMS = "%s item(s) are too many items, at most %s are allowed";
	/// 错误消息前缀：校验失败
	static constexpr const char *ERROR_MESSAGE_PREFIX = "Validation fails with message: %s";
	/// 错误消息模板：类型不匹配（期望类型、属性路径）
	static constexpr const char *ERROR_TYPE_MISMATCH = "Type mismatch: expected %s for '%s'";
	/// 错误消息模板：小数位超限（属性路径、数值、上限）
	static constexpr const char *ERROR_DECIMAL_PLACES_EXCEEDED = "The %s key that equals %s should have a maximum of %s decimal places";
	/// 错误消息模板：乘数小于阈值（属性路径、乘数、阈值）
	static constexpr const char *ERROR_MULTIPLIER_TOO_SMALL = "Multiplier in key %s that equals %s must be greater or equal to %s";
	/// 错误消息模板：整数不是乘数倍数（属性路径、数值、乘数）
	static constexpr const char *ERROR_NOT_MULTIPLE_INTEGER = "Key %s that equal %d must be multiple of %d";
	/// 错误消息模板：数字不是乘数倍数（属性路径、数值、乘数）
	static constexpr const char *ERROR_NOT_MULTIPLE_FLOAT = "Key %s that equal %f must be multiple of %f";
	/// 错误消息模板：整数越界（属性路径、数值、比较描述、边界）
	static constexpr const char *ERROR_RANGE_INTEGER = "Key %s that equal %d must be %s than %d";
	/// 错误消息模板：数字越界（属性路径、数值、比较描述、边界）
	static constexpr const char *ERROR_RANGE_FLOAT = "Key %s that equal %f must be %s than %f";
	/// 错误消息模板：字符串长度越界（属性路径、长度、比较描述、边界）
	static constexpr const char *ERROR_LENGTH_RANGE = "Length of '%s' (%d) %s than declared (%d)";
	/// 错误消息模板：字符串不匹配正则（属性路径）
	static constexpr const char *ERROR_PATTERN_MISMATCH = "String '%s' does not match its corresponding pattern";
	/// 错误消息模板：字符串不匹配 format（属性路径、format 取值）
	static constexpr const char *ERROR_FORMAT_MISMATCH = "String '%s' does not match its corresponding format '%s'";

	static String _type_mismatch(const String &p_json_type, const String &p_property_name);
	static String _validate_by_type(const Variant &p_data, const Dictionary &p_schema, const String &p_property_name);
	static String _validate_array(const Array &p_data, const Dictionary &p_schema, const String &p_property_name);
	static String _validate_object(const Dictionary &p_data, const Dictionary &p_schema, const String &p_property_name);
	static String _validate_number(double p_data, const Dictionary &p_schema, const String &p_property_name);
	static String _validate_string(const String &p_data, const Dictionary &p_schema, const String &p_property_name);
	static String _merge_dependencies(Dictionary &p_schema, const Dictionary &p_data, const String &p_property_name);
	static void _merge_schema_keyword(Dictionary &p_schema, const String &p_key, const Variant &p_value);
	static Array _to_array(const Variant &p_value);
	static bool _is_number(const Variant &p_value);
	static bool _is_dictionary_array(const Variant &p_value);

public:
	/// 关键字名：default（属性默认值，用于生成默认配置）。
	static constexpr const char *KEYWORD_DEFAULT = "default";

	/// 校验数据是否符合给定的 schema。
	/// [param p_data] 待校验的数据，通常为 [Dictionary]，也可以是 [Array] 等任意 JSON 值。
	/// [param p_schema] 校验规则，可为 [bool] 或 [Dictionary]。
	/// 校验通过返回空字符串，失败返回错误描述。
	static String validate(const Variant &p_data, const Variant &p_schema);
};
