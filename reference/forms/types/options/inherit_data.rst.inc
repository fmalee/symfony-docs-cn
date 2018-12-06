inherit_data
~~~~~~~~~~~~

**类型**: ``boolean`` **默认**: ``false``

此选项确定表单是否将从其父表单继承数据。
如果你有一组重复的跨多个表单的字段，这将非常有用。请参阅 :doc:`/form/inherit_data_option`。

.. caution::

    当一个字段具有 ``inherit_data`` 选项集时，它将按原样使用父表单的数据。
    这意味着 :doc:`数据转换器 </form/data_transformers>` 不会应用于该字段。
