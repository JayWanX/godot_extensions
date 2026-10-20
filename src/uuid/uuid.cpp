#include "uuid.h"

#include "core/math/random_number_generator.h"
#include "core/object/class_db.h"
#include "core/string/ustring.h"

void UUID::_bind_methods() {
	ClassDB::bind_method(D_METHOD("equal_with", "other"), &UUID::equal_with);
	ClassDB::bind_static_method("UUID", D_METHOD("generate"), &UUID::generate);
}

UUID::UUID() {
	_id = generate();
}

String UUID::_to_string() {
	return String(_id);
}

bool UUID::equal_with(const Ref<UUID> &p_other) const {
	return _id == p_other->_id;
}

StringName UUID::generate() {
	// 用线程安全的局部 RNG 生成 16 个随机字节。
	RandomNumberGenerator rng;
	rng.randomize();

	uint8_t bytes[16] = {};
	for (int i = 0; i < 16; ++i) {
		bytes[i] = static_cast<uint8_t>(rng.randi() & 0xff);
	}

	// 标记为 UUID v4：置位版本位（第 7 字节高 4 位=4）与变体位（第 9 字节高 2 位=10）。
	bytes[6] = static_cast<uint8_t>((bytes[6] & 0x0f) | 0x40);
	bytes[8] = static_cast<uint8_t>((bytes[8] & 0x3f) | 0x80);

	// 编码为带连字符的十六进制字符串，再转成 StringName 以节省内存。
	const String hex = String::hex_encode_buffer(bytes, 16);
	const String formatted = hex.substr(0, 8) + "-" + hex.substr(8, 4) + "-" +
			hex.substr(12, 4) + "-" + hex.substr(16, 4) + "-" + hex.substr(20, 12);
	return StringName(formatted);
}