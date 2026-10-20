#include "prefab.h"

#include "core/object/class_db.h"
#include "core/templates/local_vector.h"

void Prefab::_bind_methods() {
	ClassDB::bind_static_method("Prefab",
			D_METHOD("pack", "node", "free_mode"),
			&Prefab::pack, DEFVAL(FREE_MODE_INSTANT));

	BIND_ENUM_CONSTANT(FREE_MODE_INSTANT);
	BIND_ENUM_CONSTANT(FREE_MODE_DEFERRED);
	BIND_ENUM_CONSTANT(FREE_MODE_NONE);
}

Ref<PackedScene> Prefab::pack(Node *p_node, int p_free_mode) {
	ERR_FAIL_NULL_V(p_node, Ref<PackedScene>());

	// 深度优先遍历子树，统一修复 owner 关系：
	// PackedScene::pack 要求所有子节点的 owner 必须指向节点树内的某个节点，
	// 否则打包后这些子节点会丢失（不包含在场景中）。
	LocalVector<Node *> to_check;
	for (int i = 0; i < p_node->get_child_count(); ++i) {
		to_check.push_back(p_node->get_child(i));
	}
	while (!to_check.is_empty()) {
		Node *sub = to_check[to_check.size() - 1];
		to_check.remove_at(to_check.size() - 1);
		if (sub->get_owner() == nullptr) {
			continue;
		}

		for (int i = 0; i < sub->get_child_count(); ++i) {
			to_check.push_back(sub->get_child(i));
		}

		// 子节点 owner 为空（外部引用）或与外层 owner 相同（整棵子树独立）时，
		// 将 owner 重新指向打包的根节点，确保打包后保留完整层级。
		if (p_node->get_owner() == nullptr || sub->get_owner() == p_node->get_owner()) {
			sub->set_owner(p_node);
		}
	}

	// 创建并填充 PackedScene。
	Ref<PackedScene> prefab = memnew(PackedScene);
	prefab->pack(p_node);

	// 按配置释放源节点。
	switch (p_free_mode) {
		case FREE_MODE_INSTANT:
			memdelete(p_node);
			break;
		case FREE_MODE_DEFERRED:
			p_node->queue_free();
			break;
		default:
			break;
	}

	return prefab;
}