# FileUtils

继承自：[Object](https://docs.godotengine.org/en/stable/classes/class_object.html)

文件系统工具类：提供文件类型判定等纯静态方法，不可实例化。

判定基于扩展名/文件名白名单快路径，未命中时读取文件头查 NUL 字节兜底。

## 方法：


返回值                                                                     | 函数签名                                                                                                                                                                                                                 
----------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)  | [is_text_file](#i_is_text_file) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) file, [int](https://docs.godotengine.org/en/stable/classes/class_int.html) max_read_bytes=4096 ) static 
<p></p>

## 方法描述

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_is_text_file"></span> **is_text_file**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) file, [int](https://docs.godotengine.org/en/stable/classes/class_int.html) max_read_bytes=4096 ) 

判断文件是否可视为文本文件。

先按扩展名/文件名做零 IO 快路径判定，未命中再读取文件头查 NUL 字节兜底。

**file：** 文件路径，可为系统绝对路径或 res:// 路径。

**max_read_bytes：** 读取文件头的最大字节数。

判定为文本文件则返回 true，否则（含文件不存在或无法打开）返回 false。

_生成于 2026-10-05_
