# DirUtils

继承自：[Object](https://docs.godotengine.org/en/stable/classes/class_object.html)

目录操作工具类：提供目录遍历、递归复制/删除与哈希等静态方法。

## 方法：


返回值                                                                                 | 函数签名                                                                                                                                                                                                                                                                                                                                                                                                                                
----------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[String](https://docs.godotengine.org/en/stable/classes/class_string.html)          | [compute_dir_hash](#i_compute_dir_hash) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path, [PackedStringArray](https://docs.godotengine.org/en/stable/classes/class_packedstringarray.html) extensions=PackedStringArray() ) static                                                                                                                                                                 
[String](https://docs.godotengine.org/en/stable/classes/class_string.html)          | [compute_dir_hash_recursive](#i_compute_dir_hash_recursive) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path, [PackedStringArray](https://docs.godotengine.org/en/stable/classes/class_packedstringarray.html) extensions=PackedStringArray() ) static                                                                                                                                             
[Error](https://docs.godotengine.org/en/stable/classes/class_error.html)            | [copy_recursive](#i_copy_recursive) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) src, [String](https://docs.godotengine.org/en/stable/classes/class_string.html) dst ) static                                                                                                                                                                                                                       
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)              | [delete_directory_recursive](#i_delete_directory_recursive) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path ) static                                                                                                                                                                                                                                                                              
[int](https://docs.godotengine.org/en/stable/classes/class_int.html)                | [delete_files_if](#i_delete_files_if) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) root_path, [Callable](https://docs.godotengine.org/en/stable/classes/class_callable.html) should_delete ) static                                                                                                                                                                                                 
[Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)  | [get_directory_tree](#i_get_directory_tree) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path ) static                                                                                                                                                                                                                                                                                              
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)              | [walk_directory_tree](#i_walk_directory_tree) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path, [Callable](https://docs.godotengine.org/en/stable/classes/class_callable.html) file_handler, [Callable](https://docs.godotengine.org/en/stable/classes/class_callable.html) dir_handler=Callable(), [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) recursive=true ) static 
<p></p>

## 方法描述

### [String](https://docs.godotengine.org/en/stable/classes/class_string.html)<span id="i_compute_dir_hash"></span> **compute_dir_hash**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path, [PackedStringArray](https://docs.godotengine.org/en/stable/classes/class_packedstringarray.html) extensions=PackedStringArray() ) 

计算目录哈希（仅当前层文件，不含子目录）。

**path：** 目录路径。

**extensions：** （可选）参与计算的文件扩展名列表，如 ["png", "jpg"]。空列表表示所有文件，大小写不敏感。

MD5 哈希字符串，若目录无法打开则返回空字符串。

### [String](https://docs.godotengine.org/en/stable/classes/class_string.html)<span id="i_compute_dir_hash_recursive"></span> **compute_dir_hash_recursive**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path, [PackedStringArray](https://docs.godotengine.org/en/stable/classes/class_packedstringarray.html) extensions=PackedStringArray() ) 

递归计算目录及所有子目录的哈希。

**path：** 起始目录路径。

**extensions：** （可选）参与计算的文件扩展名列表，如 ["png", "jpg"]。空列表表示所有文件，大小写不敏感。

MD5 哈希字符串，若目录无法打开则返回空字符串。

### [Error](https://docs.godotengine.org/en/stable/classes/class_error.html)<span id="i_copy_recursive"></span> **copy_recursive**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) src, [String](https://docs.godotengine.org/en/stable/classes/class_string.html) dst ) 

递归复制目录（目标目录会自动创建）。

**src：** 源目录路径

**dst：** 目标目录路径

错误码，OK 表示成功。

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_delete_directory_recursive"></span> **delete_directory_recursive**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path ) 

递归删除目录（含所有子目录与文件）。

**path：** 要删除的目录路径

删除成功则返回 true。

### [int](https://docs.godotengine.org/en/stable/classes/class_int.html)<span id="i_delete_files_if"></span> **delete_files_if**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) root_path, [Callable](https://docs.godotengine.org/en/stable/classes/class_callable.html) should_delete ) 

根据条件删除文件，返回实际删除数量。

**root_path：** 起始目录路径

**should_delete：** 判断是否删除的回调，签名为 func(file_path: String, file_name: String) -> bool

实际删除的文件数量。

### [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)<span id="i_get_directory_tree"></span> **get_directory_tree**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path ) 

获取目录树结构（字典形式）。

**path：** 要遍历的目录路径

目录信息字典，含键：name(String)、path(String)、directories(Array[Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html))、files(Array[String](https://docs.godotengine.org/en/stable/classes/class_string.html))；目录无效时返回空字典。

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_walk_directory_tree"></span> **walk_directory_tree**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) path, [Callable](https://docs.godotengine.org/en/stable/classes/class_callable.html) file_handler, [Callable](https://docs.godotengine.org/en/stable/classes/class_callable.html) dir_handler=Callable(), [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) recursive=true ) 

通用的目录树遍历，支持文件/目录回调，回调返回 false 终止遍历。

**path：** 起始目录路径

**file_handler：** 处理文件的回调，签名为 func(file_path: String, file_name: String) -> bool，返回 false 终止遍历

**dir_handler：** 处理目录的回调（可选），签名为 func(dir_path: String, dir_name: String) -> bool，返回 false 终止遍历

**recursive：** 是否递归子目录（默认为 true）

是否完整执行完毕（未被回调终止），true 表示完整遍历。

_生成于 2026-10-05_
