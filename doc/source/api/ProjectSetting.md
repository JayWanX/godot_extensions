# ProjectSetting

继承自：[Resource](https://docs.godotengine.org/en/stable/classes/class_resource.html)

定义单个项目设置的元数据资源。

## 属性：


类型                                                                            | 名称                                           | 默认值   
----------------------------------------------------------------------------- | -------------------------------------------- | ------
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)        | [basic](#i_basic)                            | true  
[Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html)  | [default_value](#i_default_value)            | null  
[String](https://docs.godotengine.org/en/stable/classes/class_string.html)    | [hint_string](#i_hint_string)                | ""    
[Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html)  | [initial_value](#i_initial_value)            | null  
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)        | [internal](#i_internal)                      | false 
[bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)        | [restart_if_changed](#i_restart_if_changed)  | false 
[String](https://docs.godotengine.org/en/stable/classes/class_string.html)    | [setting_path](#i_setting_path)              | ""    
[int](https://docs.godotengine.org/en/stable/classes/class_int.html)          | [type_hint](#i_type_hint)                    | 0     
[int](https://docs.godotengine.org/en/stable/classes/class_int.html)          | [value_type](#i_value_type)                  | 0     
<p></p>

## 方法：


返回值                                  | 函数签名                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[ProjectSetting](ProjectSetting.md)  | [create](#i_create) ( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) setting_path, [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) default_value=null, [Type](https://docs.godotengine.org/en/stable/classes/class_variant.html#enum-variant-type) value_type=0, [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) initial_value=null, [PropertyHint](https://docs.godotengine.org/en/stable/classes/class_propertyhint.html) type_hint=0, [String](https://docs.godotengine.org/en/stable/classes/class_string.html) hint_string="", [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) basic=true, [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) internal=false, [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) restart_if_changed=false ) static 
<p></p>

## 属性描述

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_basic"></span> **basic** = true

是否显示在基础设置中

### [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html)<span id="i_default_value"></span> **default_value** = null

默认值（设置不存在时读取）

### [String](https://docs.godotengine.org/en/stable/classes/class_string.html)<span id="i_hint_string"></span> **hint_string** = ""

编辑器属性提示字符串

### [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html)<span id="i_initial_value"></span> **initial_value** = null

初始值（注册设置时写入）

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_internal"></span> **internal** = false

是否为内部设置

### [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html)<span id="i_restart_if_changed"></span> **restart_if_changed** = false

修改后是否需要重启生效

### [String](https://docs.godotengine.org/en/stable/classes/class_string.html)<span id="i_setting_path"></span> **setting_path** = ""

设置路径（含点号分隔的层级名）

### [int](https://docs.godotengine.org/en/stable/classes/class_int.html)<span id="i_type_hint"></span> **type_hint** = 0

编辑器属性提示类型

### [int](https://docs.godotengine.org/en/stable/classes/class_int.html)<span id="i_value_type"></span> **value_type** = 0

值类型（Variant.Type 枚举）

## 方法描述

### [ProjectSetting](ProjectSetting.md)<span id="i_create"></span> **create**( [String](https://docs.godotengine.org/en/stable/classes/class_string.html) setting_path, [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) default_value=null, [Type](https://docs.godotengine.org/en/stable/classes/class_variant.html#enum-variant-type) value_type=0, [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) initial_value=null, [PropertyHint](https://docs.godotengine.org/en/stable/classes/class_propertyhint.html) type_hint=0, [String](https://docs.godotengine.org/en/stable/classes/class_string.html) hint_string="", [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) basic=true, [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) internal=false, [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) restart_if_changed=false ) 

创建项目设置元数据资源。

**setting_path：** 设置路径（含点号分隔的层级名）。

**default_value：** 默认值（设置不存在时读取）。

**value_type：** 值类型（Variant.Type 枚举）。

**initial_value：** 初始值（注册设置时写入）。

**type_hint：** 编辑器属性提示类型。

**hint_string：** 编辑器属性提示字符串。

**basic：** 是否显示在基础设置中。

**internal：** 是否为内部设置。

**restart_if_changed：** 修改后是否需要重启生效。

新创建的项目设置资源。

_生成于 2026-09-23_
