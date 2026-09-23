# ProjectSettingsManager

继承自：[Object](https://docs.godotengine.org/en/stable/classes/class_object.html)

提供项目设置的批量注册、移除与读取。

## 方法：


返回值                                                                           | 函数签名                                                                                                                                       
----------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------
[void](#)                                                                     | [add_setting](#i_add_setting) ( [ProjectSetting](ProjectSetting.md) project_setting ) static                                               
[void](#)                                                                     | [add_settings](#i_add_settings) ( [Array](https://docs.godotengine.org/en/stable/classes/class_array.html) project_settings ) static       
[Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html)  | [get_setting](#i_get_setting) ( [ProjectSetting](ProjectSetting.md) project_setting ) static                                               
[void](#)                                                                     | [remove_setting](#i_remove_setting) ( [ProjectSetting](ProjectSetting.md) project_setting ) static                                         
[void](#)                                                                     | [remove_settings](#i_remove_settings) ( [Array](https://docs.godotengine.org/en/stable/classes/class_array.html) project_settings ) static 
<p></p>

## 方法描述

### [void](#)<span id="i_add_setting"></span> **add_setting**( [ProjectSetting](ProjectSetting.md) project_setting ) 

注册单个项目设置并保存到 ProjectSettings。

**project_setting：** 设置定义资源

### [void](#)<span id="i_add_settings"></span> **add_settings**( [Array](https://docs.godotengine.org/en/stable/classes/class_array.html) project_settings ) 

批量注册项目设置，合并为一次保存。

**project_settings：** 设置定义列表

### [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html)<span id="i_get_setting"></span> **get_setting**( [ProjectSetting](ProjectSetting.md) project_setting ) 

读取项目设置的当前值。

**project_setting：** 设置定义资源

项目设置的当前值，不存在时返回默认值。

### [void](#)<span id="i_remove_setting"></span> **remove_setting**( [ProjectSetting](ProjectSetting.md) project_setting ) 

删除单个项目设置并保存。

**project_setting：** 设置定义资源

### [void](#)<span id="i_remove_settings"></span> **remove_settings**( [Array](https://docs.godotengine.org/en/stable/classes/class_array.html) project_settings ) 

批量删除项目设置，合并为一次保存。

**project_settings：** 设置定义列表

_生成于 2026-09-23_
