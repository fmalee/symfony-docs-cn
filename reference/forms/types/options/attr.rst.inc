attr
~~~~

**类型**: ``array`` **默认**: ``[]``

如果要向一个HTML字段表示添加额外的属性，则可以使用此选项。
它是一个以HTML属性为键的关联数组。当你需要为某个部件设置一个自定义样式类时，就非常有用::

    $builder->add('body', TextareaType::class, [
        'attr' => ['class' => 'tinymce'],
    ]);
