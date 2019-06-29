.. index::
   single: Form; Empty data

如何为表单类配置空数据
============================================

``empty_data`` 选项允许你为表单类指定空数据集。
如果你提交了表单，但没有调用 ``setData()`` 或没有在创建表单时传入数据，则将使用此空数据集。
例如，在控制器中::

    public function index()
    {
        $blog = ...;

        // $blog 作为数据传入，因此不需要 empty_data 选项
        $form = $this->createForm(BlogType::class, $blog);

        // 没有数据传入，所以 empty_data 将用于获取“起始数据”
        $form = $this->createForm(BlogType::class);
    }

默认情况下，``empty_data`` 被设置为 ``null``。
但如果你为表单类指定了一个 ``data_class`` 选项，``empty_data`` 将默认为该类的一个新实例。
该实例将通过调用不带参数的构造函数来创建。

如果要重写此默认行为，有两种可用方法：

* `选项1：实例化一个新类`_
* `选项2：提供一个闭包`_

如果未设置 ``data_class`` 选项，则可以在表单类型为复合(compound)时，将初始数据作为字符串传递或传递一个字符串数组（其中键与字段名称匹配）。

选项1：实例化一个新类
---------------------------------

你可以使用此选项的一个提前是你是否要使用带参数的构造函数。
请记住，默认的 ``data_class`` 选项调用的构造函数是不带参数的::

    // src/Form/Type/BlogType.php

    // ...
    use App\Entity\Blog;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class BlogType extends AbstractType
    {
        private $someDependency;

        public function __construct($someDependency)
        {
            $this->someDependency = $someDependency;
        }
        // ...

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'empty_data' => new Blog($this->someDependency),
            ]);
        }
    }

你可以根据需要实例化你的类。
在此示例中，你将一些依赖传递给 ``BlogType`` 然后使用它来实例化 ``Blog`` 类。
关键是，你可以设置 ``empty_data`` 来精确你要使用的“新”对象。

.. tip::

    为了能将参数传递给 ``BlogType`` 构造函数，你需要
    :ref:`将表单类注册为服务 <service-container-creating-service>`，并使用
    ``form.type`` 标签对其进行
    :doc:`标记 </service_container/tags>`。如果你正在使用
    :ref:`默认的services.yaml配置 <service-container-services-load-example>`，那么它已经为你完成工作了。

.. _forms-empty-data-closure:

选项2：提供一个闭包
---------------------------

使用闭包是首选方法，因为它只在需要时才创建对象。

闭包必须接受一个 ``FormInterface`` 实例作为第一个参数::

    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    // ...

    public function configureOptions(OptionsResolver $resolver)
    {
        $resolver->setDefaults([
            'empty_data' => function (FormInterface $form) {
                return new Blog($form->get('title')->getData());
            },
        ]);
    }
