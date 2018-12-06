.. index::
   single: Forms; Fields; FormType

FormType字段
==============

``FormType`` 预定义了几个选项，这些选项可适用于所有父类型是 ``FormType`` 的类型。

+-----------+--------------------------------------------------------------------+
| Options   | - `action`_                                                        |
|           | - `allow_extra_fields`_                                            |
|           | - `by_reference`_                                                  |
|           | - `compound`_                                                      |
|           | - `constraints`_                                                   |
|           | - `data`_                                                          |
|           | - `data_class`_                                                    |
|           | - `empty_data`_                                                    |
|           | - `error_bubbling`_                                                |
|           | - `error_mapping`_                                                 |
|           | - `extra_fields_message`_                                          |
|           | - `help`_                                                          |
|           | - `help_attr`_                                                     |
|           | - `inherit_data`_                                                  |
|           | - `invalid_message`_                                               |
|           | - `invalid_message_parameters`_                                    |
|           | - `label_attr`_                                                    |
|           | - `label_format`_                                                  |
|           | - `mapped`_                                                        |
|           | - `method`_                                                        |
|           | - `post_max_size_message`_                                         |
|           | - `property_path`_                                                 |
|           | - `required`_                                                      |
|           | - `trim`_                                                          |
|           | - `validation_groups`_                                             |
+-----------+--------------------------------------------------------------------+
| Inherited | - `attr`_                                                          |
| options   | - `auto_initialize`_                                               |
|           | - `block_name`_                                                    |
|           | - `disabled`_                                                      |
|           | - `label`_                                                         |
|           | - `translation_domain`_                                            |
+-----------+--------------------------------------------------------------------+
| Parent    | none                                                               |
+-----------+--------------------------------------------------------------------+
| Class     | :class:`Symfony\\Component\\Form\\Extension\\Core\\Type\\FormType` |
+-----------+--------------------------------------------------------------------+

字段选项
-------------

.. _form-option-action:

.. include:: /reference/forms/types/options/action.rst.inc

.. _form-option-allow-extra-fields:

allow_extra_fields
~~~~~~~~~~~~~~~~~~

**类型**: ``boolean`` **默认**: ``false``

通常，如果提交了未在表单中配置的额外字段，你会获得一个
"This form should not contain extra fields." 的验证错误。

你可以通过启用表单上的 ``allow_extra_fields`` 选项来消除此验证错误。

.. include:: /reference/forms/types/options/by_reference.rst.inc

.. include:: /reference/forms/types/options/compound.rst.inc

.. _reference-form-option-constraints:

.. include:: /reference/forms/types/options/constraints.rst.inc

.. include:: /reference/forms/types/options/data.rst.inc

.. include:: /reference/forms/types/options/data_class.rst.inc

.. _reference-form-option-empty-data:

.. include:: /reference/forms/types/options/empty_data.rst.inc
    :end-before: DEFAULT_PLACEHOLDER

此选项的实际默认值取决于其他字段选项：

* 如果 ``data_class`` 已设置并且 ``required`` 为 ``true``, 那么默认值为 ``new $data_class()``;
* 如果 ``data_class`` 已设置并且 ``required`` 为 ``false``, 那么默认值为 ``null``;
* 如果 ``data_class`` 未设置并且 ``compound`` 为 ``true``, 那么默认值为 ``array()``
  (空数组);
* 如果 ``data_class`` 未设置并且 ``compound`` 为 ``false``, 那么默认值为 ``''``
  (空字符串).

.. include:: /reference/forms/types/options/empty_data.rst.inc
    :start-after: DEFAULT_PLACEHOLDER

.. _reference-form-option-error-bubbling:

.. include:: /reference/forms/types/options/error_bubbling.rst.inc

.. include:: /reference/forms/types/options/error_mapping.rst.inc

.. include:: /reference/forms/types/options/extra_fields_message.rst.inc

.. include:: /reference/forms/types/options/help.rst.inc

.. include:: /reference/forms/types/options/help_attr.rst.inc

.. include:: /reference/forms/types/options/inherit_data.rst.inc

.. include:: /reference/forms/types/options/invalid_message.rst.inc

.. include:: /reference/forms/types/options/invalid_message_parameters.rst.inc

.. include:: /reference/forms/types/options/label_attr.rst.inc

.. include:: /reference/forms/types/options/label_format.rst.inc

.. _reference-form-option-mapped:

.. include:: /reference/forms/types/options/mapped.rst.inc

.. _form-option-method:

.. include:: /reference/forms/types/options/method.rst.inc

.. include:: /reference/forms/types/options/post_max_size_message.rst.inc

.. _reference-form-option-property-path:

.. include:: /reference/forms/types/options/property_path.rst.inc

.. _reference-form-option-required:

.. include:: /reference/forms/types/options/required.rst.inc

.. include:: /reference/forms/types/options/trim.rst.inc

.. include:: /reference/forms/types/options/validation_groups.rst.inc

继承选项
-----------------

在 :class:`Symfony\\Component\\Form\\Extension\\Core\\Type\\BaseType`
类中定义了下面的选项。
``BaseType`` 类是 ``form`` 和
:doc:`ButtonType </reference/forms/types/button>`
两个类型的父类，但它不是表单类型树的一部分（即，它不能被用来作为它自身的一个表单类型）。

.. include:: /reference/forms/types/options/attr.rst.inc

.. include:: /reference/forms/types/options/auto_initialize.rst.inc

.. include:: /reference/forms/types/options/block_name.rst.inc

.. include:: /reference/forms/types/options/disabled.rst.inc

.. include:: /reference/forms/types/options/label.rst.inc

.. include:: /reference/forms/types/options/translation_domain.rst.inc
