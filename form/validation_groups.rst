.. index::
    single: Forms; Validation groups

如何定义要使用的验证组
==========================================

验证组
-----------------

如果你的对象添加了 :doc:`验证组 </validation/groups>`，则需要为表单指定需要使用的验证组::

    $form = $this->createFormBuilder($user, array(
        'validation_groups' => array('registration'),
    ))->add(...);

如果你正在创建 :ref:`表单类 <form-creating-form-classes>`
（一个很好的实践），那么你需要在 ``configureOptions()`` 方法中添加以下内容::

    use Symfony\Component\OptionsResolver\OptionsResolver;

    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults(array(
            'validation_groups' => array('registration'),
        ));
    }

在这两个示例中，都 *只有* ``registration`` 验证组将被用于验证底层对象。
要同时应用 ``registration`` 组 *和* 所有不在一个验证组中的约束，请使用::

    'validation_groups' => array('Default', 'registration')
