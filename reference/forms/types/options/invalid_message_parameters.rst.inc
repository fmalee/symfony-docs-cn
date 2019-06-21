invalid_message_parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~

**类型**: ``array`` **默认**: ``[]``

设置 ``invalid_message`` 选项时，你可能需要在字符串中包含一些变量。
这可以通过向该选项添加占位符并在此选项中包含变量来完成::

    $builder->add('someField', SomeFormType::class, [
        // ...
        'invalid_message' => 'You entered an invalid value, it should include %num% letters',
        'invalid_message_parameters' => ['%num%' => 6],
    ]);
