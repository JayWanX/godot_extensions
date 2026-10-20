#pragma once

#include "core/object/object.h"
#include "core/variant/type_info.h"
#include "core/variant/variant.h"
#include "scene/resources/packed_scene.h"

/// 预制体工具类：提供将任意节点树打包为 PackedScene 的静态方法，不可实例化。
/// 自动修复子节点的 owner 关系，确保打包后保留完整层级。
class Prefab : public Object {
	GDCLASS(Prefab, Object);

	static void _bind_methods();

public:
	/// 打包完成后源节点的释放模式。
	enum FreeMode {
		/// 立即释放源节点。
		FREE_MODE_INSTANT = 0,
		/// 延迟释放源节点（当前帧结束）。
		FREE_MODE_DEFERRED = 1,
		/// 不释放源节点。
		FREE_MODE_NONE = 2,
	};

	/// 将指定节点及其子树打包为 PackedScene，并自动修复子节点的 owner 关系。
	/// [param node] 要打包的根节点，不能为空
	/// [param free_mode] 打包完成后对源节点的处理方式，见 FreeMode 枚举
	/// 打包好的 PackedScene 资源。
	static Ref<PackedScene> pack(Node *p_node, int p_free_mode = FREE_MODE_INSTANT);
};

VARIANT_ENUM_CAST(Prefab::FreeMode);