如何动态配置表单验证组
===================================================

有时你需要高级逻辑来确定验证组。如果无法通过简单的回调确定它们，则可以使用服务。
创建一个实现 ``__invoke()`` 方法的服务，该方法接受一个 ``FormInterface`` 作为参数::

    // src/Validation/ValidationGroupResolver.php
    namespace App\Validation;

    use Symfony\Component\Form\FormInterface;

    class ValidationGroupResolver
    {
        private $service1;

        private $service2;

        public function __construct($service1, $service2)
        {
            $this->service1 = $service1;
            $this->service2 = $service2;
        }

        /**
         * @param FormInterface $form
         * @return array
         */
        public function __invoke(FormInterface $form)
        {
            $groups = array();

            // ... 确定要应用哪些验证组并返回一个数组

            return $groups;
        }
    }

然后在你的表单中，注入该解析器并将其设置为 ``validation_groups``::

    // src/Form/MyClassType.php;
    namespace App\Form;

    use App\Validator\ValidationGroupResolver;
    use Symfony\Component\Form\AbstractType
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class MyClassType extends AbstractType
    {
        private $groupResolver;

        public function __construct(ValidationGroupResolver $groupResolver)
        {
            $this->groupResolver = $groupResolver;
        }

        // ...
        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'validation_groups' => $this->groupResolver,
            ));
        }
    }

这将导致表单验证器调用(invoke)你的组解析器来设置验证时要返回的验证组。
