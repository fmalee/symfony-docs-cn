.. index::
    single: Forms; Multiple Submit Buttons

如何使用多个按钮来提交表单
==========================================

当你的表单包含多个提交按钮时，你需要检查点击了哪个按钮以调整控制器中的程序流程。
为此，请在表单中添加标签为 "Save and add" 的第二个按钮::

    $form = $this->createFormBuilder($task)
        ->add('task', TextType::class)
        ->add('dueDate', DateType::class)
        ->add('save', SubmitType::class, array('label' => 'Create Task'))
        ->add('saveAndAdd', SubmitType::class, array('label' => 'Save and Add'))
        ->getForm();

在控制器中，使用按钮的 :method:`Symfony\\Component\\Form\\ClickableInterface::isClicked`
方法来查询是否点击了"Save and add"按钮::

    if ($form->isSubmitted() && $form->isValid()) {
        // ... 执行一些操作，例如将任务保存到数据库

        $nextAction = $form->get('saveAndAdd')->isClicked()
            ? 'task_new'
            : 'task_success';

        return $this->redirectToRoute($nextAction);
    }

或者你可以使用表单的 :method:`Symfony\\Component\\Form\\Form::getClickedButton` 方法来获取按钮的名称::

    if ($form->getClickedButton() && 'saveAndAdd' === $form->getClickedButton()->getName()) {
        // ...
    }
