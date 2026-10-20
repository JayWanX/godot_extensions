# JsonSchemaValidator

继承自：[Object](https://docs.godotengine.org/en/stable/classes/class_object.html)

JSON Schema 校验器：校验通过返回空字符串，失败返回人类可读的错误描述。

## 描述：

只实现了 JSON Schema 的一个子集，已支持的关键字：

type、properties、required、additionalProperties、propertyNames、dependencies、

items、prefixItems、minItems、maxItems、multipleOf、minimum、exclusiveMinimum、

maximum、exclusiveMaximum、minProperties、maxProperties、minLength、maxLength、

pattern、format（仅 color）。未列出的关键字一律忽略。

**与 `mod_loader` 的 `ModJsonSchemaValidator` 的关系**：本类是那份实现的通用版本。

两者语义一致，但**分属不同模块**——`mod_loader` 只能在模块内自用（模块之间不能互引

头文件），本类则供工程侧与其它模块经 `ClassDB` 取用。修改其一须同步另一处。

## 方法：


返回值                                                                         | 函数签名                                                                                                                                                                                                      
--------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
[String](https://docs.godotengine.org/en/stable/classes/class_string.html)  | [validate](#i_validate) ( [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) data, [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) schema ) static 
<p></p>

## 方法描述

### [String](https://docs.godotengine.org/en/stable/classes/class_string.html)<span id="i_validate"></span> **validate**( [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) data, [Variant](https://docs.godotengine.org/en/stable/classes/class_variant.html) schema ) 

校验数据是否符合给定的 schema。

**p_data：** 待校验的数据，通常为 [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)，也可以是 [Array](https://docs.godotengine.org/en/stable/classes/class_array.html) 等任意 JSON 值。

**p_schema：** 校验规则，可为 [bool](https://docs.godotengine.org/en/stable/classes/class_bool.html) 或 [Dictionary](https://docs.godotengine.org/en/stable/classes/class_dictionary.html)。

校验通过返回空字符串，失败返回错误描述。

_生成于 2026-10-20_
