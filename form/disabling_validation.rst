.. index::
    single: Forms; Disabling validation

如何禁用提交数据的验证
===============================================

有时，完全禁止表单的验证是有用的。
对于这些情况，你可以将 ``validation_groups`` 选项设置为 ``false``::

    use Symfony\Component\OptionsResolver\OptionsResolver;

    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => false,
        ));
    }

请注意，执行此操作时，表单仍将运行基本的完整性检查，例如上传的文件是否过大或是否提交了不存在的字段。

可以使用 :ref:`allow_extra_fields配置选项 <form-option-allow-extra-fields>`
控制额外表单字段的提交，并且应通过PHP和Web服务器配置处理最大上传文件大小。
