# ClassDocReader

继承自：[Object](https://docs.godotengine.org/en/stable/classes/class_object.html)

读取引擎解析好的类参考 / 脚本文档，供 GDScript 查询。

设计为可实例化的普通类：addon 先用 `ClassDB.class_exists()` 探测、再

`ClassDB.instantiate()` 惰性构建，这样「未编译该模块时回退本地缓存」就不会在

解析期引用到不存在的类名。

## 方法：


返回值                                                                                 | 函数签名                                                                                                                                        
----------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------
[Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)  | [get_class_docs](#i_get_class_docs) ( [StringName](https://docs.godotengine.org/en/stable/classes/class_stringname.html) class_name ) const 
[Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)  | [get_script_docs](#i_get_script_docs) ( [Script](https://docs.godotengine.org/en/stable/classes/class_script.html) script ) const           
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)              | [has_class_doc](#i_has_class_doc) ( [StringName](https://docs.godotengine.org/en/stable/classes/class_stringname.html) class_name ) const   
<p></p>

## 方法描述

### [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)<span id="i_get_class_docs"></span> **get_class_docs**( [StringName](https://docs.godotengine.org/en/stable/classes/class_stringname.html) class_name ) 

取某个类各成员（方法 / 信号 / 属性 / 常量）的文档。

**class_name：** 类名（原生类，或带 `class_name` 的脚本类）

返回字典含四组键：`methods` / `signals` / `properties` / `constants`，

值为「成员名 → 文档文本」。原生类经 DTR 随编辑器语言翻译；脚本类返回作者原文。

### [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)<span id="i_get_script_docs"></span> **get_script_docs**( [Script](https://docs.godotengine.org/en/stable/classes/class_script.html) script ) 

取某个脚本（含不带 `class_name` 的脚本）各成员的文档。

**script：** 脚本资源；需已被编辑器分析过，否则返回空字典

返回体结构与 `get_class_docs` 一致，文本为作者原文（不做翻译）。

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_has_class_doc"></span> **has_class_doc**( [StringName](https://docs.godotengine.org/en/stable/classes/class_stringname.html) class_name ) 

该类是否存在且文档已就绪。

_生成于 2026-09-23_
