# DictUtils

继承自：[Object](https://docs.godotengine.org/en/stable/classes/class_object.html)

字典工具类：提供字典结构相关的静态方法，不可实例化。

## 方法：


返回值                                                                     | 函数签名                                                                                                                                                                                                                                                
----------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)  | [has_same_keys_structure](#i_has_same_keys_structure) ( [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html) dict1, [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html) dict2 ) static 
<p></p>

## 方法描述

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_has_same_keys_structure"></span> **has_same_keys_structure**( [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html) dict1, [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html) dict2 ) 

检查两个字典的键结构是否完全一致（仅比较键形状，不比较值）。

**dict1：** 第一个字典

**dict2：** 第二个字典

键形状完全一致则返回 true。

_生成于 2026-10-05_
