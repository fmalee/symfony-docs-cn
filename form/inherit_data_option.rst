.. index::
   single: Form; The "inherit_data" option

如何使用“inherit_data”减少代码重复
==================================================

当你在不同实体中有一些重复字段时，``inherit_data`` 表单字段选项非常有用。
例如，假设你有两个实体，一个 ``Company`` 和一个 ``Customer``::

    // src/Entity/Company.php
    namespace App\Entity;

    class Company
    {
        private $name;
        private $website;

        private $address;
        private $zipcode;
        private $city;
        private $country;
    }

.. code-block:: php

    // src/Entity/Customer.php
    namespace App\Entity;

    class Customer
    {
        private $firstName;
        private $lastName;

        private $address;
        private $zipcode;
        private $city;
        private $country;
    }

正如你所看到的，每个实体都持有数相同的字段：``address``、``zipcode``、``city``、``country``。

首先为这些实体构建两个表单，``CompanyType`` 和 ``CustomerType``::

    // src/Form/Type/CompanyType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    class CompanyType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('name', TextType::class)
                ->add('website', TextType::class);
        }
    }

.. code-block:: php

    // src/Form/Type/CustomerType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    class CustomerType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('firstName', TextType::class)
                ->add('lastName', TextType::class);
        }
    }

不是在两个表单中同时包含重复的 ``address``、``zipcode``、``city`` 和 ``country``
字段，而是创建一个名为 ``LocationType`` 的第三个表单::

    // src/Form/Type/LocationType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Form\Extension\Core\Type\TextareaType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    class LocationType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('address', TextareaType::class)
                ->add('zipcode', TextType::class)
                ->add('city', TextType::class)
                ->add('country', TextType::class);
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'inherit_data' => true,
            ));
        }
    }

位置表单有一个有趣的选项集，名为 ``inherit_data``。该选项允许该表单从其父表单继承数据。
如果嵌入在公司表单中，则位置表单的字段将访问 ``Company`` 实例的属性。
如果嵌入在客户表单中，则该字段将访问 ``Customer`` 实例的属性。很方便对吧？

.. note::

    不同于在 ``LocationType`` 里面设置 ``inherit_data``
    选项，你也可以（就像使用其他任何选项一样）将它传递给 ``$builder->add()`` 的第三个参数。

最后，通过将位置表单添加到两个原始表单来完成此工作::

    // src/Form/Type/CompanyType.php
    use App\Entity\Company;
    // ...

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('foo', LocationType::class, array(
            'data_class' => Company::class,
        ));
    }

.. code-block:: php

    // src/Form/Type/CustomerType.php
    use App\Entity\Customer;
    // ...

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('bar', LocationType::class, array(
            'data_class' => Customer::class,
        ));
    }

仅此而已！你已将重复的字段定义提取到单独的位置表单，你可以在任何需要的位置重复使用它。

.. caution::

    带有 ``inherit_data`` 选项集的表单不能拥有 ``*_SET_DATA`` 事件监听器。
