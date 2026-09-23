# UUID

继承自：[RefCounted](https://docs.godotengine.org/en/stable/classes/class_refcounted.html)

表示一个符合 UUID v4 格式的唯一标识符，基于 16 个随机字节编码为十六进制并用连字符分组。

## 方法：


返回值                                                                                 | 函数签名                                                        
----------------------------------------------------------------------------------- | ------------------------------------------------------------
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)              | [equal_with](#i_equal_with) ( [UUID](UUID.md) other ) const 
[StringName](https://docs.godotengine.org/en/stable/classes/class_stringname.html)  | [generate](#i_generate) ( ) static                          
<p></p>

## 方法描述

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_equal_with"></span> **equal_with**( [UUID](UUID.md) other ) 

比较当前 UUID 与另一个 UUID 是否相等。

**other：** 用于比较的另一个 UUID

两者文本表示相同则返回 true。

### [StringName](https://docs.godotengine.org/en/stable/classes/class_stringname.html)<span id="i_generate"></span> **generate**( ) 

生成一个新的随机 UUID。

格式化后的 UUID 文本，如 `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`。

_生成于 2026-09-23_
