.. index::
    single: Forms; Validation groups based on submitted data

如何根据提交的数据选择验证组
===========================================================

如果你需要一些高级逻辑来确定验证组（例如，基于提交的数据），你可以将
``validation_groups`` 选项设置为数组回调::

    use App\Entity\Client;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...
    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults([
            'validation_groups' => [
                Client::class,
                'determineValidationGroups',
            ],
        ]);
    }

这将在提交表单之后、执行验证之前，调用类 ``Client`` 上的 ``determineValidationGroups()`` 静态方法。
该表单对象将作为参数传递给该方法（参见下一个示例）。你还可以通过使用一个  ``Closure`` 内联定义整个逻辑::

    use App\Entity\Client;
    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...
    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults([
            'validation_groups' => function (FormInterface $form) {
                $data = $form->getData();

                if (Client::TYPE_PERSON == $data->getType()) {
                    return ['person'];
                }

                return ['company'];
            },
        ]);
    }

使用 ``validation_groups`` 选项会重写正在使用的默认验证组。
如果要验证实体的默认约束，则必须按如下方式调整选项::

    use App\Entity\Client;
    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...
    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults([
            'validation_groups' => function (FormInterface $form) {
                $data = $form->getData();

                if (Client::TYPE_PERSON == $data->getType()) {
                    return ['Default', 'person'];
                }

                return ['Default', 'company'];
            },
        ]);
    }

你可以在 :doc:`验证组 </validation/groups>` 的文档中找到有关验证组和默认约束如何工作的更多信息。
