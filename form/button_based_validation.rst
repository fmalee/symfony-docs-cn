.. index::
    single: Forms; Validation groups based on clicked button

如何根据按钮点击选择验证组
===========================================================

当你的表单包含多个提交按钮时，你可以根据用于提交表单的按钮来更改验证组。
例如，考虑向导中的表单，该表单允许你前进到下一步或返回上一步。
还假设在返回上一步时，应保存表单数据，但不进行验证。

首先，我们需要在表单中添加两个按钮::

    $form = $this->createFormBuilder($task)
        // ...
        ->add('nextStep', SubmitType::class)
        ->add('previousStep', SubmitType::class)
        ->getForm();

然后，我们配置按钮以返回上一步以运行特定的验证组。
在此示例中，我们希望它禁止验证，因此我们将其 ``validation_groups`` 选项设置为 ``false``::

    $form = $this->createFormBuilder($task)
        // ...
        ->add('previousStep', SubmitType::class, array(
            'validation_groups' => false,
        ))
        ->getForm();

现在表单将跳过你的验证约束。但它仍将验证基本的完整性约束，例如检查上传的文件是否太大，或者你是否尝试在数字字段中提交文本。

.. seealso::

    要查看如何使用服务动态解析 ``validation_groups`` ，请阅读
    :doc:`/form/validation_group_service_resolver` 章节。
