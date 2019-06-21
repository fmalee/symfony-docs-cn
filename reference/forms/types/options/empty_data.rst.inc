empty_data
~~~~~~~~~~

**类型**: ``mixed``

.. This file should only be included with start-after or end-before that's
   set to this placeholder value. Its purpose is to let us include only
   part of this file.

DEFAULT_PLACEHOLDER

此选项用来确定当提交的值为空（或缺失）时该字段将 *返回* 的值。
如果视图中的已渲染表单未提供任何值，则不会设置初始值。
It does not set an initial value if none is provided when the form is rendered in a view.

这意味着它可以帮助你处理带有空白字段的表单提交。
例如，如果你希望在未选中任何值时将 ``name`` 字段显式的设置为 ``John Doe``，则可以这样操作::

    $builder->add('name', null, [
        'required'   => false,
        'empty_data' => 'John Doe',
    ]);

这仍将渲染一个空文本框，但在提交时将如果该字段仍为空，则设置为 ``John Doe`` 值。
也可以使用 ``data`` 或 ``placeholder`` 选项在已渲染表单中显示此初始值。

如果一个表单是复合(compound)形式，则可以设置 ``empty_data`` 为一个数组、对象或闭包。
有关这些选项的更多详细信息，请参阅 :doc:`/form/use_empty_data`。

.. note::

    如果要为整个表单类设置 ``empty_data`` 选项，请参阅 :doc:`/form/use_empty_data`。

.. caution::

    :doc:`表单数据转换器 </form/data_transformers>` 仍将应用于 ``empty_data`` 值。
    这意味着一个空字符串将转换为 ``null``。如果你想要返回空字符串，请使用一个自定义数据转换器。
