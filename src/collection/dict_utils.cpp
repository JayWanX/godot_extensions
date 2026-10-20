#include "dict_utils.h"

#include "core/object/class_db.h"
#include "core/variant/variant.h"

void DictUtils::_bind_methods() {
	ClassDB::bind_static_method("DictUtils",
			D_METHOD("has_same_keys_structure", "dict1", "dict2"),
			&DictUtils::has_same_keys_structure);
}

bool DictUtils::has_same_keys_structure(const Dictionary &p_dict1, const Dictionary &p_dict2) {
	return _extract_key_structure(p_dict1) == _extract_key_structure(p_dict2);
}

Dictionary DictUtils::_extract_key_structure(const Dictionary &p_dict) {
	Dictionary result;

	const Array keys = p_dict.keys();
	for (int i = 0; i < keys.size(); ++i) {
		const Variant key = keys[i];
		const Variant value = p_dict[key];

		if (value.get_type() == Variant::DICTIONARY) {
			result[key] = _extract_key_structure(value);
		} else {
			result[key] = Variant();
		}
	}

	return result;
}