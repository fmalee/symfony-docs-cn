invalid_message
~~~~~~~~~~~~~~~

**类型**: ``string`` **默认**: ``This value is not valid``

如果输入此字段的数据没有意义（即验证失败），则使用此验证错误消息。

例如，如果用户将一个无意义（即无法转换为一个真实时间）的字符串输入到
一个 :doc:`TimeType </reference/forms/types/time>` 字段中，或者将一个字符串（例如
``apple``）输入到一个数字字段中，则可能发生这种情况。

应使用你的验证规则（:ref:`参考 <validation-constraint-configuration>`）的验证消息来设置标准（业务逻辑）的验证（例如为一个字段设置最小长度）。
