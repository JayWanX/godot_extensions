#include "json_schema_validator.h"

#include "core/math/math_funcs.h"
#include "core/object/class_db.h"
#include "modules/regex/regex.h"

String JsonSchemaValidator::_type_mismatch(const String &p_json_type, const String &p_property_name) {
	Array expected_types;
	expected_types.push_back(p_json_type);
	return vformat(ERROR_TYPE_MISMATCH, expected_types, p_property_name);
}

String JsonSchemaValidator::validate(const Variant &p_data, const Variant &p_schema) {
	if (p_schema.get_type() == Variant::BOOL) {
		if ((bool)p_schema) {
			return String();
		}
		return vformat(ERROR_MESSAGE_PREFIX, ERROR_SCHEMA_DENY_ALL);
	}

	if (p_schema.get_type() != Variant::DICTIONARY) {
		return ERROR_SCHEMA_STRUCTURE_INVALID;
	}

	Dictionary schema_dictionary = p_schema;
	return _validate_by_type(p_data, schema_dictionary, ROOT_PROPERTY_NAME);
}

String JsonSchemaValidator::_validate_by_type(const Variant &p_data, const Dictionary &p_schema, const String &p_property_name) {
	if (p_schema.is_empty()) {
		return String();
	}

	if (!p_schema.has(KEYWORD_TYPE)) {
		return ERROR_SCHEMA_STRUCTURE_INVALID;
	}

	Array types = _to_array(p_schema[KEYWORD_TYPE]);
	String first_mismatch;

	// 任一声明类型通过即视为通过。
	for (int i = 0; i < types.size(); i++) {
		Variant json_type = types[i];
		// 匹配语义与 GDScript 的 match 一致：仅字符串类值参与类型名比较。
		String json_type_name;
		if (json_type.get_type() == Variant::STRING || json_type.get_type() == Variant::STRING_NAME) {
			json_type_name = json_type;
		}

		String mismatch;
		if (json_type_name == JSON_TYPE_ARRAY) {
			if (p_data.get_type() == Variant::ARRAY) {
				return _validate_array(p_data, p_schema, p_property_name);
			}
			mismatch = _type_mismatch(JSON_TYPE_ARRAY, p_property_name);
		} else if (json_type_name == JSON_TYPE_BOOLEAN) {
			if (p_data.get_type() == Variant::BOOL) {
				return String();
			}
			mismatch = _type_mismatch(JSON_TYPE_BOOLEAN, p_property_name);
		} else if (json_type_name == JSON_TYPE_INTEGER) {
			if (p_data.get_type() == Variant::INT) {
				return _validate_number((double)(int64_t)p_data, p_schema, p_property_name);
			}
			if (p_data.get_type() == Variant::FLOAT) {
				double data_as_float = p_data;
				if (Math::is_finite(data_as_float) && data_as_float == (double)(int64_t)data_as_float) {
					return _validate_number(data_as_float, p_schema, p_property_name);
				}
			}
			mismatch = _type_mismatch(JSON_TYPE_INTEGER, p_property_name);
		} else if (json_type_name == JSON_TYPE_NULL) {
			if (p_data.get_type() == Variant::NIL) {
				return String();
			}
			mismatch = _type_mismatch(JSON_TYPE_NULL, p_property_name);
		} else if (json_type_name == JSON_TYPE_NUMBER) {
			if (p_data.get_type() == Variant::FLOAT || p_data.get_type() == Variant::INT) {
				return _validate_number((double)p_data, p_schema, p_property_name);
			}
			mismatch = _type_mismatch(JSON_TYPE_NUMBER, p_property_name);
		} else if (json_type_name == JSON_TYPE_OBJECT) {
			if (p_data.get_type() == Variant::DICTIONARY) {
				return _validate_object(p_data, p_schema, p_property_name);
			}
			mismatch = _type_mismatch(JSON_TYPE_OBJECT, p_property_name);
		} else if (json_type_name == JSON_TYPE_STRING) {
			if (p_data.get_type() == Variant::STRING) {
				return _validate_string(p_data, p_schema, p_property_name);
			}
			mismatch = _type_mismatch(JSON_TYPE_STRING, p_property_name);
		}

		if (first_mismatch.is_empty()) {
			first_mismatch = mismatch;
		}
	}

	if (first_mismatch.is_empty()) {
		return vformat(ERROR_TYPE_MISMATCH, types, p_property_name);
	}
	return first_mismatch;
}

String JsonSchemaValidator::_validate_array(const Array &p_data, const Dictionary &p_schema, const String &p_property_name) {
	if (p_schema.has(KEYWORD_MIN_ITEMS)) {
		Variant min_items = p_schema[KEYWORD_MIN_ITEMS];
		if (!_is_number(min_items)) {
			return ERROR_SCHEMA_MIN_ITEMS_INVALID;
		}
		if ((double)p_data.size() < (double)min_items) {
			return vformat(ERROR_TOO_FEW_ITEMS, p_data.size(), min_items);
		}
	}

	if (p_schema.has(KEYWORD_MAX_ITEMS)) {
		Variant max_items = p_schema[KEYWORD_MAX_ITEMS];
		if (!_is_number(max_items)) {
			return ERROR_SCHEMA_MAX_ITEMS_INVALID;
		}
		if ((double)p_data.size() > (double)max_items) {
			return vformat(ERROR_TOO_MANY_ITEMS, p_data.size(), max_items);
		}
	}

	if (p_schema.has(KEYWORD_PREFIX_ITEMS)) {
		if (!p_schema.has(KEYWORD_ITEMS)) {
			return vformat(ERROR_REQUIRED_PROPERTY_MISSING, KEYWORD_ITEMS, KEYWORD_PREFIX_ITEMS);
		}

		Variant prefix_items_value = p_schema[KEYWORD_PREFIX_ITEMS];
		if (!_is_dictionary_array(prefix_items_value)) {
			return vformat(ERROR_SCHEMA_ITEM_TYPE_INVALID, p_property_name, KEYWORD_PREFIX_ITEMS, JSON_TYPE_OBJECT);
		}
		Array prefix_items = prefix_items_value;

		Variant items_value = p_schema[KEYWORD_ITEMS];
		Dictionary additional_items_schema;
		if (items_value.get_type() == Variant::BOOL) {
			if (!(bool)items_value && p_data.size() > prefix_items.size()) {
				return vformat(ERROR_SCHEMA_PREFIX_ITEMS_NO_ADDITIONAL, p_data, p_data.size());
			}
		} else if (items_value.get_type() == Variant::DICTIONARY) {
			additional_items_schema = items_value;
		} else {
			return ERROR_SCHEMA_STRUCTURE_INVALID;
		}

		Array sub_errors;
		for (int index = 0; index < p_data.size(); index++) {
			Dictionary current_schema;
			String key_suffix;
			if (index <= prefix_items.size() - 1) {
				current_schema = prefix_items[index];
				key_suffix = "." + String(KEYWORD_PREFIX_ITEMS);
			} else {
				current_schema = additional_items_schema;
				key_suffix = "." + String(KEYWORD_ITEMS);
			}

			String sub_error = _validate_by_type(
					p_data[index],
					current_schema,
					p_property_name + key_suffix + "[" + String::num_int64(index) + "]");
			if (!sub_error.is_empty()) {
				sub_errors.append(sub_error);
			}
		}

		if (!sub_errors.is_empty()) {
			return vformat(ERROR_MESSAGE_PREFIX, sub_errors);
		}
		return String();
	}

	if (p_schema.has(KEYWORD_ITEMS)) {
		Variant items_schema_value = p_schema[KEYWORD_ITEMS];
		if (items_schema_value.get_type() != Variant::DICTIONARY) {
			return vformat(ERROR_SCHEMA_ITEM_TYPE_INVALID, p_property_name, KEYWORD_ITEMS, JSON_TYPE_OBJECT);
		}
		Dictionary items_schema = items_schema_value;

		for (int index = 0; index < p_data.size(); index++) {
			String item_error = _validate_by_type(
					p_data[index],
					items_schema,
					p_property_name + "[" + String::num_int64(index) + "]");
			if (!item_error.is_empty()) {
				Array item_errors;
				item_errors.append(item_error);
				return vformat(ERROR_MESSAGE_PREFIX, item_errors);
			}
		}
	}

	return String();
}

String JsonSchemaValidator::_validate_object(const Dictionary &p_data, const Dictionary &p_schema, const String &p_property_name) {
	Dictionary effective_schema = p_schema;
	if (p_schema.has(KEYWORD_DEPENDENCIES)) {
		effective_schema = p_schema.duplicate(true);
		String dependency_error = _merge_dependencies(effective_schema, p_data, p_property_name);
		if (!dependency_error.is_empty()) {
			return dependency_error;
		}
	}

	// properties 缺失时视为空声明，仅影响 additionalProperties 的判定。
	Dictionary declared_properties;
	if (effective_schema.has(KEYWORD_PROPERTIES)) {
		Variant properties_value = effective_schema[KEYWORD_PROPERTIES];
		if (properties_value.get_type() != Variant::DICTIONARY) {
			return ERROR_SCHEMA_PREFIX + vformat(ERROR_TYPE_MISMATCH, TYPE_NAME_OBJECT, p_property_name);
		}
		declared_properties = properties_value;
	}

	if (effective_schema.has(KEYWORD_REQUIRED)) {
		Variant required_value = effective_schema[KEYWORD_REQUIRED];
		if (required_value.get_type() != Variant::ARRAY) {
			return vformat(ERROR_SCHEMA_REQUIRED_INVALID, p_property_name);
		}
		Array required_properties = required_value;
		for (int i = 0; i < required_properties.size(); i++) {
			String required_property = required_properties[i];
			if (!p_data.has(required_property)) {
				return vformat(ERROR_REQUIRED_PROPERTY_MISSING, required_property, p_property_name);
			}
		}
	}

	for (const KeyValue<Variant, Variant> &element : declared_properties) {
		String key = element.key;
		Variant property_schema_value = element.value;
		if (property_schema_value.get_type() != Variant::DICTIONARY) {
			return ERROR_SCHEMA_ITEM_WITHOUT_TYPE;
		}
		Dictionary property_schema = property_schema_value;
		if (!property_schema.has(KEYWORD_TYPE)) {
			return ERROR_SCHEMA_ITEM_WITHOUT_TYPE;
		}
		if (!p_data.has(key)) {
			continue;
		}
		String property_error = _validate_by_type(p_data[key], property_schema, key);
		if (!property_error.is_empty()) {
			return property_error;
		}
	}

	if (effective_schema.has(KEYWORD_ADDITIONAL_PROPERTIES)) {
		Variant additional_value = effective_schema[KEYWORD_ADDITIONAL_PROPERTIES];
		if (additional_value.get_type() == Variant::BOOL) {
			if (!(bool)additional_value) {
				for (const KeyValue<Variant, Variant> &element : p_data) {
					String key = element.key;
					if (!declared_properties.has(key)) {
						return vformat(ERROR_ADDITIONAL_PROPERTY_FOUND, key);
					}
				}
			}
		} else if (additional_value.get_type() == Variant::DICTIONARY) {
			Dictionary additional_schema = additional_value;
			for (const KeyValue<Variant, Variant> &element : p_data) {
				String key = element.key;
				if (declared_properties.has(key)) {
					continue;
				}
				String additional_error = _validate_by_type(element.value, additional_schema, key);
				if (!additional_error.is_empty()) {
					return additional_error;
				}
			}
		} else {
			String expected = vformat(TEMPLATE_OR, JSON_TYPE_BOOLEAN, TYPE_NAME_OBJECT);
			return ERROR_SCHEMA_PREFIX + vformat(ERROR_TYPE_MISMATCH, expected, p_property_name);
		}
	}

	if (effective_schema.has(KEYWORD_PROPERTY_NAMES)) {
		Variant property_names_value = effective_schema[KEYWORD_PROPERTY_NAMES];
		if (property_names_value.get_type() != Variant::DICTIONARY) {
			return ERROR_SCHEMA_PREFIX + vformat(ERROR_TYPE_MISMATCH, TYPE_NAME_OBJECT, p_property_name);
		}
		Dictionary property_names_schema = property_names_value;
		for (const KeyValue<Variant, Variant> &element : p_data) {
			String key = element.key;
			String name_error = _validate_string(key, property_names_schema, key);
			if (!name_error.is_empty()) {
				return name_error;
			}
		}
	}

	if (effective_schema.has(KEYWORD_MIN_PROPERTIES)) {
		Variant min_properties = effective_schema[KEYWORD_MIN_PROPERTIES];
		if (!_is_number(min_properties)) {
			return ERROR_SCHEMA_PREFIX + vformat(ERROR_TYPE_MISMATCH, JSON_TYPE_INTEGER, p_property_name);
		}
		if ((double)p_data.size() < (double)min_properties) {
			return vformat(ERROR_TOO_FEW_PROPERTIES, p_data.size(), min_properties);
		}
	}

	if (effective_schema.has(KEYWORD_MAX_PROPERTIES)) {
		Variant max_properties = effective_schema[KEYWORD_MAX_PROPERTIES];
		if (!_is_number(max_properties)) {
			return ERROR_SCHEMA_PREFIX + vformat(ERROR_TYPE_MISMATCH, JSON_TYPE_INTEGER, p_property_name);
		}
		if ((double)p_data.size() > (double)max_properties) {
			return vformat(ERROR_TOO_MANY_PROPERTIES, p_data.size(), max_properties);
		}
	}

	return String();
}

String JsonSchemaValidator::_validate_number(double p_data, const Dictionary &p_schema, const String &p_property_name) {
	Array types = _to_array(p_schema[KEYWORD_TYPE]);
	// 仅在只声明 integer 时才按整数语义处理。
	bool integer_mode = types.has(JSON_TYPE_INTEGER) && !types.has(JSON_TYPE_NUMBER);

	if (p_schema.has(KEYWORD_MULTIPLE_OF)) {
		double multiplier = p_schema[KEYWORD_MULTIPLE_OF];
		if (integer_mode) {
			multiplier = (double)(int64_t)multiplier;
		}

		String data_string = Variant(p_data);
		String decimal_places = data_string.get_slice(".", 1);
		if (!decimal_places.is_empty() && decimal_places.length() > MAX_DECIMAL_PLACES) {
			return vformat(ERROR_DECIMAL_PLACES_EXCEEDED, p_property_name, p_data, MAX_DECIMAL_PLACES);
		}

		if (!(multiplier >= SMALL_FLOAT_THRESHOLD)) {
			return vformat(ERROR_MULTIPLIER_TOO_SMALL, p_property_name, multiplier, SMALL_FLOAT_THRESHOLD);
		}

		// 数值或乘数小于 1 时先放大再取余，规避浮点误差。
		double remainder;
		if (p_data < 1.0 || multiplier < 1.0) {
			remainder = Math::fmod(p_data * 1000.0, multiplier * 1000.0);
		} else {
			remainder = Math::fmod(p_data, multiplier);
		}

		if (!Math::is_zero_approx(remainder)) {
			if (integer_mode) {
				return vformat(ERROR_NOT_MULTIPLE_INTEGER, p_property_name, p_data, multiplier);
			}
			return vformat(ERROR_NOT_MULTIPLE_FLOAT, p_property_name, p_data, multiplier);
		}
	}

	if (p_schema.has(KEYWORD_MINIMUM)) {
		double minimum = p_schema[KEYWORD_MINIMUM];
		if (integer_mode) {
			minimum = (double)(int64_t)minimum;
		}
		if (p_data < minimum) {
			if (integer_mode) {
				return vformat(ERROR_RANGE_INTEGER, p_property_name, p_data, COMPARISON_GREATER_OR_EQUAL, minimum);
			}
			return vformat(ERROR_RANGE_FLOAT, p_property_name, p_data, COMPARISON_GREATER_OR_EQUAL, minimum);
		}
	}

	if (p_schema.has(KEYWORD_EXCLUSIVE_MINIMUM)) {
		double exclusive_minimum = p_schema[KEYWORD_EXCLUSIVE_MINIMUM];
		if (integer_mode) {
			exclusive_minimum = (double)(int64_t)exclusive_minimum;
		}
		if (p_data <= exclusive_minimum) {
			if (integer_mode) {
				return vformat(ERROR_RANGE_INTEGER, p_property_name, p_data, COMPARISON_GREATER, exclusive_minimum);
			}
			return vformat(ERROR_RANGE_FLOAT, p_property_name, p_data, COMPARISON_GREATER, exclusive_minimum);
		}
	}

	if (p_schema.has(KEYWORD_MAXIMUM)) {
		double maximum = p_schema[KEYWORD_MAXIMUM];
		if (integer_mode) {
			maximum = (double)(int64_t)maximum;
		}
		if (p_data > maximum) {
			if (integer_mode) {
				return vformat(ERROR_RANGE_INTEGER, p_property_name, p_data, COMPARISON_LESS_OR_EQUAL, maximum);
			}
			return vformat(ERROR_RANGE_FLOAT, p_property_name, p_data, COMPARISON_LESS_OR_EQUAL, maximum);
		}
	}

	if (p_schema.has(KEYWORD_EXCLUSIVE_MAXIMUM)) {
		double exclusive_maximum = p_schema[KEYWORD_EXCLUSIVE_MAXIMUM];
		if (integer_mode) {
			exclusive_maximum = (double)(int64_t)exclusive_maximum;
		}
		if (p_data >= exclusive_maximum) {
			if (integer_mode) {
				return vformat(ERROR_RANGE_INTEGER, p_property_name, p_data, COMPARISON_LESS, exclusive_maximum);
			}
			return vformat(ERROR_RANGE_FLOAT, p_property_name, p_data, COMPARISON_LESS, exclusive_maximum);
		}
	}

	return String();
}

String JsonSchemaValidator::_validate_string(const String &p_data, const Dictionary &p_schema, const String &p_property_name) {
	if (p_schema.has(KEYWORD_MIN_LENGTH)) {
		Variant min_length = p_schema[KEYWORD_MIN_LENGTH];
		if (!_is_number(min_length)) {
			return vformat(ERROR_TYPE_MISMATCH, JSON_TYPE_INTEGER, p_property_name + "." + String(KEYWORD_MIN_LENGTH));
		}
		if ((double)p_data.length() < (double)min_length) {
			return vformat(ERROR_MESSAGE_PREFIX, vformat(ERROR_LENGTH_RANGE, p_property_name, p_data.length(), COMPARISON_LESS, min_length));
		}
	}

	if (p_schema.has(KEYWORD_MAX_LENGTH)) {
		Variant max_length = p_schema[KEYWORD_MAX_LENGTH];
		if (!_is_number(max_length)) {
			return vformat(ERROR_TYPE_MISMATCH, JSON_TYPE_INTEGER, p_property_name + "." + String(KEYWORD_MAX_LENGTH));
		}
		if ((double)p_data.length() > (double)max_length) {
			return vformat(ERROR_MESSAGE_PREFIX, vformat(ERROR_LENGTH_RANGE, p_property_name, p_data.length(), COMPARISON_GREATER, max_length));
		}
	}

	if (p_schema.has(KEYWORD_PATTERN)) {
		Variant pattern = p_schema[KEYWORD_PATTERN];
		if (pattern.get_type() != Variant::STRING) {
			return vformat(ERROR_TYPE_MISMATCH, JSON_TYPE_STRING, p_property_name + "." + String(KEYWORD_PATTERN));
		}
		Ref<RegEx> regex;
		regex.instantiate();
		regex->compile(pattern);
		if (regex->search(p_data).is_null()) {
			return vformat(ERROR_MESSAGE_PREFIX, vformat(ERROR_PATTERN_MISMATCH, p_property_name));
		}
	}

	if (p_schema.has(KEYWORD_FORMAT)) {
		Variant format = p_schema[KEYWORD_FORMAT];
		if (format.get_type() != Variant::STRING) {
			return vformat(ERROR_TYPE_MISMATCH, JSON_TYPE_STRING, p_property_name + "." + String(KEYWORD_FORMAT));
		}
		String format_string = format;
		if (format_string.to_lower() == FORMAT_COLOR && !p_data.is_valid_html_color()) {
			return vformat(ERROR_MESSAGE_PREFIX, vformat(ERROR_FORMAT_MISMATCH, p_property_name, FORMAT_COLOR));
		}
	}

	return String();
}

String JsonSchemaValidator::_merge_dependencies(Dictionary &p_schema, const Dictionary &p_data, const String &p_property_name) {
	Variant dependencies_value = p_schema[KEYWORD_DEPENDENCIES];
	if (dependencies_value.get_type() != Variant::DICTIONARY) {
		String expected_type = vformat(TEMPLATE_OR, JSON_TYPE_ARRAY, TYPE_NAME_OBJECT);
		return ERROR_SCHEMA_PREFIX + vformat(ERROR_TYPE_MISMATCH, expected_type, p_property_name);
	}

	Dictionary dependencies = dependencies_value;
	for (const KeyValue<Variant, Variant> &element : dependencies) {
		String dependency = element.key;
		if (!p_data.has(dependency)) {
			continue;
		}

		Variant dependency_value = element.value;
		if (dependency_value.get_type() == Variant::ARRAY) {
			Array dependency_required = dependency_value;
			if (p_schema.has(KEYWORD_REQUIRED)) {
				Array required = p_schema[KEYWORD_REQUIRED];
				for (int i = 0; i < dependency_required.size(); i++) {
					required.append(dependency_required[i]);
				}
			} else {
				p_schema[KEYWORD_REQUIRED] = dependency_required;
			}
		} else if (dependency_value.get_type() == Variant::DICTIONARY) {
			Dictionary dependency_schema = dependency_value;
			for (const KeyValue<Variant, Variant> &dependency_element : dependency_schema) {
				String key = dependency_element.key;
				if (p_schema.has(key)) {
					_merge_schema_keyword(p_schema, key, dependency_element.value);
				} else {
					p_schema[key] = dependency_element.value;
				}
			}
		} else {
			String expected_type = vformat(TEMPLATE_OR, JSON_TYPE_ARRAY, TYPE_NAME_OBJECT);
			return ERROR_SCHEMA_PREFIX + vformat(ERROR_TYPE_MISMATCH, expected_type, p_property_name);
		}
	}

	return String();
}

void JsonSchemaValidator::_merge_schema_keyword(Dictionary &p_schema, const String &p_key, const Variant &p_value) {
	Variant existing = p_schema[p_key];
	if (existing.get_type() == Variant::ARRAY) {
		Array existing_array = existing;
		Array appended = p_value;
		for (int i = 0; i < appended.size(); i++) {
			existing_array.append(appended[i]);
		}
	} else if (existing.get_type() == Variant::DICTIONARY) {
		Dictionary existing_dictionary = existing;
		Dictionary appended_dictionary = p_value;
		for (const KeyValue<Variant, Variant> &element : appended_dictionary) {
			existing_dictionary[element.key] = element.value;
		}
	} else {
		p_schema[p_key] = p_value;
	}
}

Array JsonSchemaValidator::_to_array(const Variant &p_value) {
	if (p_value.get_type() == Variant::ARRAY) {
		return p_value;
	}
	Array array;
	array.push_back(p_value);
	return array;
}

bool JsonSchemaValidator::_is_number(const Variant &p_value) {
	return p_value.get_type() == Variant::INT || p_value.get_type() == Variant::FLOAT;
}

bool JsonSchemaValidator::_is_dictionary_array(const Variant &p_value) {
	if (p_value.get_type() != Variant::ARRAY) {
		return false;
	}
	Array array_value = p_value;
	for (int i = 0; i < array_value.size(); i++) {
		if (array_value[i].get_type() != Variant::DICTIONARY) {
			return false;
		}
	}
	return true;
}

void JsonSchemaValidator::_bind_methods() {
	ClassDB::bind_static_method("JsonSchemaValidator", D_METHOD("validate", "data", "schema"), &JsonSchemaValidator::validate);
}