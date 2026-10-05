# Prefab

继承自：[Object](https://docs.godotengine.org/en/stable/classes/class_object.html)

预制体工具类：提供将任意节点树打包为 PackedScene 的静态方法，不可实例化。

自动修复子节点的 owner 关系，确保打包后保留完整层级。

## 方法：


返回值                                                                                   | 函数签名                                                                                                                                                                                     
------------------------------------------------------------------------------------- | -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[PackedScene](https://docs.godotengine.org/en/stable/classes/class_packedscene.html)  | [pack](#i_pack) ( [Node](https://docs.godotengine.org/en/stable/classes/class_node.html) node, [int](https://docs.godotengine.org/en/stable/classes/class_int.html) free_mode=0 ) static 
<p></p>

## 枚举：<span id="enumerations"></span>

枚举 **FreeMode**：

- <span id="i_FREE_MODE_INSTANT"></span>**FREE_MODE_INSTANT** = **0** --- 立即释放源节点。
- <span id="i_FREE_MODE_DEFERRED"></span>**FREE_MODE_DEFERRED** = **1** --- 延迟释放源节点（当前帧结束）。
- <span id="i_FREE_MODE_NONE"></span>**FREE_MODE_NONE** = **2** --- 不释放源节点。


## 方法描述

### [PackedScene](https://docs.godotengine.org/en/stable/classes/class_packedscene.html)<span id="i_pack"></span> **pack**( [Node](https://docs.godotengine.org/en/stable/classes/class_node.html) node, [int](https://docs.godotengine.org/en/stable/classes/class_int.html) free_mode=0 ) 

将指定节点及其子树打包为 PackedScene，并自动修复子节点的 owner 关系。

**node：** 要打包的根节点，不能为空

**free_mode：** 打包完成后对源节点的处理方式，见 FreeMode 枚举

打包好的 PackedScene 资源。

_生成于 2026-10-05_
