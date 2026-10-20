extends Node
## extensions 模块冒烟测试：覆盖 UUID、字典/文件/目录工具、预制体与项目设置。

func _ready() -> void:
	var runner := TestRunner.new()
	_test_uuid(runner)
	_test_dict_utils(runner)
	_test_file_utils(runner)
	_test_dir_utils(runner)
	_test_dir_utils_regressions(runner)
	_test_json_schema_validator(runner)
	_test_prefab(runner)
	_test_project_settings(runner)
	runner.report()
	var exit_code: int = 0 if runner.is_pass() else 1
	get_tree().quit(exit_code)

## 测试 UUID 生成与比较。
func _test_uuid(runner: TestRunner) -> void:
	var first_uuid: StringName = UUID.generate()
	runner.assert_eq(first_uuid.length(), 36, "UUID 文本长度应为 36")
	var segments: PackedStringArray = String(first_uuid).split("-")
	runner.assert_eq(segments.size(), 5, "UUID 应按连字符分为 5 段")
	runner.assert_eq(segments[0].length(), 8, "第一段长度为 8")
	runner.assert_eq(segments[1].length(), 4, "第二段长度为 4")
	runner.assert_eq(segments[2].length(), 4, "第三段长度为 4")
	runner.assert_eq(segments[3].length(), 4, "第四段长度为 4")
	runner.assert_eq(segments[4].length(), 12, "第五段长度为 12")
	var second_uuid: StringName = UUID.generate()
	runner.assert_true(first_uuid != second_uuid, "两次生成结果应不同")
	var object_a := UUID.new()
	var object_b := UUID.new()
	runner.assert_true(not object_a.equal_with(object_b), "两个独立 UUID 实例应互不相等")
	runner.assert_true(object_a.equal_with(object_a), "同一 UUID 实例应与自身相等")

## 测试字典键结构比较。
func _test_dict_utils(runner: TestRunner) -> void:
	var nested_a := {"name": "a", "nested": {"key": 1}}
	var nested_b := {"name": "b", "nested": {"key": 2}}
	var flat := {"name": "a", "nested": 9}
	runner.assert_true(DictUtils.has_same_keys_structure(nested_a, nested_b), "嵌套键结构一致应返回 true")
	runner.assert_false(DictUtils.has_same_keys_structure(nested_a, flat), "键结构不同应返回 false")
	runner.assert_true(DictUtils.has_same_keys_structure({}, {}), "两个空字典键结构一致")

## 测试文本/二进制文件判定。
func _test_file_utils(runner: TestRunner) -> void:
	runner.assert_true(FileUtils.is_text_file("res://project.godot"), "project.godot 应判定为文本文件")
	var binary_path: String = "user://test_binary.bin"
	_write_file(binary_path, PackedByteArray([0, 1, 2, 0, 255]))
	runner.assert_false(FileUtils.is_text_file(binary_path), "含 NUL 字节的文件应判定为二进制")
	runner.assert_false(FileUtils.is_text_file("user://not_exists.bin"), "不存在的文件应返回 false")
	DirAccess.remove_absolute(binary_path)

## 测试目录树、哈希、复制与删除。
func _test_dir_utils(runner: TestRunner) -> void:
	var base_path: String = "user://test_dir_utils"
	var make_error: Error = DirAccess.make_dir_recursive_absolute(base_path)
	runner.assert_eq(make_error, OK, "创建测试目录应成功")
	_write_file(base_path + "/a.txt", "hello".to_utf8_buffer())
	_write_file(base_path + "/b.dat", "binary".to_utf8_buffer())
	var dir_hash: String = DirUtils.compute_dir_hash(base_path)
	runner.assert_eq(dir_hash.length(), 32, "目录 MD5 应为 32 位")
	var filtered_hash: String = DirUtils.compute_dir_hash(base_path, PackedStringArray(["dat"]))
	runner.assert_eq(filtered_hash.length(), 32, "带扩展名过滤的哈希长度应为 32")
	var tree: Dictionary = DirUtils.get_directory_tree(base_path)
	runner.assert_keys(tree, ["name", "path", "directories", "files"], "目录树字典应含固定键")
	runner.assert_eq((tree["files"] as Array).size(), 2, "目录树应包含 2 个文件")
	var copy_path: String = "user://test_dir_utils_copy"
	runner.assert_eq(DirUtils.copy_recursive(base_path, copy_path), OK, "递归复制应返回 OK")
	runner.assert_true(DirAccess.dir_exists_absolute(copy_path), "复制的目标目录应存在")
	runner.assert_true(DirUtils.delete_directory_recursive(copy_path), "递归删除目标目录应成功")
	runner.assert_false(DirAccess.dir_exists_absolute(copy_path), "删除后目标目录应不存在")
	DirUtils.delete_directory_recursive(base_path)

## 测试通用 JSON Schema 校验器。
##
## **为什么这份实现要单独有测试**：它是从 `mod_loader` 复制的通用版，两份代码并存
## 且互不引用（模块之间不能互引头文件）。没有断言守着，两份语义会悄悄分叉，
## 而分叉的表现是「同样的配置在一处通过、在另一处被拒」。
func _test_json_schema_validator(runner: TestRunner) -> void:
	var schema: Dictionary = {
		"type": "object",
		"properties": {
			"keep_alive": {"type": "string"},
			"port": {"type": "integer", "minimum": 1, "maximum": 65535},
		},
		"required": ["keep_alive"],
		"additionalProperties": false,
	}

	# 对照组：完全合法时必须通过，否则「恒拒绝」的实现也能让失败用例绿。
	runner.assert_eq(
			JsonSchemaValidator.validate({"keep_alive": "5m", "port": 11434}, schema), "",
			"合法配置应通过校验")

	# 缺必填项。
	runner.assert_contains(
			JsonSchemaValidator.validate({"port": 11434}, schema), "keep_alive",
			"缺少 required 字段时应点名该字段")

	# 类型不符。
	runner.assert_contains(
			JsonSchemaValidator.validate({"keep_alive": 5}, schema), "Type mismatch",
			"类型不符应报类型错误")

	# 数值越界：两端各测一次。
	runner.assert_contains(
			JsonSchemaValidator.validate({"keep_alive": "5m", "port": 0}, schema),
			"port", "小于 minimum 应被拒并点名该字段")
	runner.assert_contains(
			JsonSchemaValidator.validate({"keep_alive": "5m", "port": 70000}, schema),
			"port", "大于 maximum 应被拒并点名该字段")

	# 未声明属性。
	runner.assert_contains(
			JsonSchemaValidator.validate({"keep_alive": "5m", "extra": 1}, schema),
			"extra", "additionalProperties 为 false 时应点名多余字段")

	# schema 本身非法（缺 type）。
	runner.assert_contains(
			JsonSchemaValidator.validate({}, {"properties": {}}), "Schema error",
			"schema 缺 type 关键字应报 schema 错误")

	# 布尔 schema：true 全通过，false 全拒绝。
	runner.assert_eq(JsonSchemaValidator.validate({"any": 1}, true), "",
			"schema 为 true 时应全部通过")
	runner.assert_contains(JsonSchemaValidator.validate({"any": 1}, false),
			"deny all", "schema 为 false 时应全部拒绝")


## 覆盖 `DirUtils` 两处已修缺陷，防回归。
##
## 一、`compute_dir_hash_recursive()` 曾在任何目录上**永不返回**：遍历时未跳过
##     `.` / `..`，二者被 `current_is_dir()` 判为目录而递归进自身，无限递归。
##     本组不设超时断言（卡死时整轮测试不返回，本身就是失败信号）。
## 二、`delete_directory_recursive()` 曾丢弃子项删除结果，只报父目录结果，
##     于是「目录树只删掉一半」也报成功。
func _test_dir_utils_regressions(runner: TestRunner) -> void:
	_test_hash_recursive_not_hang(runner)
	_test_delete_success_paths(runner)
	_test_delete_reports_child_failure(runner)


## 删除成功路径与边界：全部可删时报 true 且目录消失；不存在的路径报 false。
func _test_delete_success_paths(runner: TestRunner) -> void:
	var base_path: String = "user://test_dir_delete_success"
	DirUtils.delete_directory_recursive(base_path)
	DirAccess.make_dir_recursive_absolute(base_path + "/sub")
	_write_file(base_path + "/a.txt", "x".to_utf8_buffer())
	_write_file(base_path + "/sub/b.txt", "y".to_utf8_buffer())

	var removed: bool = DirUtils.delete_directory_recursive(base_path)
	runner.assert_true(removed, "全部可删时应返回 true")
	runner.assert_false(DirAccess.dir_exists_absolute(base_path),
			"返回 true 后目录应确实不存在")

	# 不存在的路径：必须返回 false，不得当成「已删除」报成功。
	runner.assert_false(
			DirUtils.delete_directory_recursive("user://test_dir_no_such_path"),
			"不存在的路径应返回 false")


## 递归哈希必须返回，且能覆盖子目录内的文件（非递归版看不到它们）。
func _test_hash_recursive_not_hang(runner: TestRunner) -> void:
	var base_path: String = "user://test_dir_hash_recursive"
	DirUtils.delete_directory_recursive(base_path)
	DirAccess.make_dir_recursive_absolute(base_path)
	_write_file(base_path + "/only.txt", "hello".to_utf8_buffer())

	# 顶层只有文件：递归与非递归结果应一致。
	var flat_root: String = DirUtils.compute_dir_hash(base_path)
	var recursive_root: String = DirUtils.compute_dir_hash_recursive(base_path)
	runner.assert_eq(recursive_root.length(), 32, "递归哈希应返回 32 位 MD5")
	runner.assert_eq(recursive_root, flat_root,
			"顶层只有文件时递归与非递归哈希应一致")

	# 子目录内的文件：递归版能看到（结果不同），非递归版看不到（保持空哈希）。
	var nested_path: String = base_path + "/nested"
	DirAccess.make_dir_recursive_absolute(nested_path)
	_write_file(nested_path + "/deep.txt", "world".to_utf8_buffer())
	var flat_nested: String = DirUtils.compute_dir_hash(base_path)
	var recursive_nested: String = DirUtils.compute_dir_hash_recursive(base_path)
	runner.assert_true(recursive_nested != recursive_root,
			"新增子目录内文件后，递归哈希必须变化")
	runner.assert_eq(flat_nested, flat_root,
			"非递归版只看当前层，子目录内文件不应影响它")
	DirUtils.delete_directory_recursive(base_path)


## 子项删不掉时必须返回 false——修复前只报父目录结果。
##
## **先做前置自检**：确认「占用确实能阻止删除」。若本环境下占用不阻止删除，
## 失败路径不可达，本组会如实跳过而非假装通过。
func _test_delete_reports_child_failure(runner: TestRunner) -> void:
	var base_path: String = "user://test_dir_delete_failure"
	DirUtils.delete_directory_recursive(base_path)
	DirAccess.make_dir_recursive_absolute(base_path + "/sub")
	_write_file(base_path + "/sub/inner.txt", "locked".to_utf8_buffer())

	var holder: FileAccess = FileAccess.open(base_path + "/sub/inner.txt", FileAccess.READ)
	runner.assert_true(holder != null, "前置：应能打开占用句柄")
	var direct_error: Error = DirAccess.remove_absolute(base_path + "/sub/inner.txt")
	runner.assert_true(direct_error != OK,
			"前置：被占用的文件应删不掉，否则本组验不到失败路径")

	if direct_error == OK:
		# 本环境下占用不阻止删除：失败路径不可达，如实跳过而不假装通过。
		print("  跳过：本环境下占用不阻止删除，删除失败路径不可达")
		if holder != null:
			holder.close()
		DirUtils.delete_directory_recursive(base_path)
		return

	var result: bool = DirUtils.delete_directory_recursive(base_path)
	runner.assert_false(result,
			"子项删不掉时必须返回 false，否则「只删掉一半」被当成成功")
	runner.assert_true(DirAccess.dir_exists_absolute(base_path),
			"子项删不掉时目录不该消失")

	if holder != null:
		holder.close()
	DirUtils.delete_directory_recursive(base_path)


## 测试节点树打包为 PackedScene。
func _test_prefab(runner: TestRunner) -> void:
	var root_node := Node.new()
	root_node.name = "Root"
	var child_node := Node.new()
	child_node.name = "Child"
	root_node.add_child(child_node)
	child_node.owner = root_node
	var packed_scene: PackedScene = Prefab.pack(root_node, Prefab.FREE_MODE_NONE)
	runner.assert_true(packed_scene != null, "打包应返回非空 PackedScene")
	var instance_node: Node = packed_scene.instantiate()
	runner.assert_true(instance_node != null, "打包结果应可实例化")
	runner.assert_eq(instance_node.name, &"Root", "实例根节点名应为 Root")
	runner.assert_true(instance_node.find_child("Child") != null, "实例应保留子节点")
	instance_node.queue_free()
	root_node.free()

## 测试项目设置的注册、读取与移除。
func _test_project_settings(runner: TestRunner) -> void:
	var setting: ProjectSetting = ProjectSetting.create("test/smoke_setting", "default_val", TYPE_STRING, "default_val")
	runner.assert_eq(setting.setting_path, "test/smoke_setting", "设置路径应正确")
	ProjectSettingsManager.add_setting(setting)
	runner.assert_eq(ProjectSettingsManager.get_setting(setting), "default_val", "读取设置应返回初始值")
	ProjectSettingsManager.remove_setting(setting)
	runner.assert_eq(ProjectSettingsManager.get_setting(setting), "default_val", "移除后读取应回落默认值")

## 以字节数组写入文件，用于构造测试数据。
## [param path] 文件路径。
## [param bytes] 要写入的原始字节。
func _write_file(path: String, bytes: PackedByteArray) -> void:
	var file := FileAccess.open(path, FileAccess.WRITE)
	if file == null:
		return
	file.store_buffer(bytes)
	file.close()