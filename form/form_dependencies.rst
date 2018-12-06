如何从表单内访问服务或配置
===================================================

有时，你可能需要从表单类内部访问 :doc:`服务 </service_container>` 或其他配置。
为此，你有两个选择：

1) 传递选项给表单
----------------------------

将服务或配置传递到表单的最简单方法是通过表单 *选项*。假设你需要访问Doctrine实体管理器以便进行一个查询。
首先，允许（实际上，是要求）将一个新的 ``entity_manager`` 选项传递给你的表单::

    // src/Form/TaskType.php
    // ...

    class TaskType extends AbstractType
    {
        // ...

        public function configureOptions(OptionsResolver $resolver)
        {
            // ...

            $resolver->setRequired('entity_manager');
        }
    }

既然你已经完成了这个，你 *必须* 在创建表单时传递一个 ``entity_manager`` 选项::

    // src/Controller/DefaultController.php
    use App\Form\TaskType;

    // ...
    public function new()
    {
        $entityManager = $this->getDoctrine()->getManager();

        $task = ...;
        $form = $this->createForm(TaskType::class, $task, array(
            'entity_manager' => $entityManager,
        ));

        // ...
    }

最后，可以在 ``buildForm()`` 方法的 ``$options`` 参数中访问 ``entity_manager`` 选项::

    // src/Form/TaskType.php
    // ...

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            /** @var \Doctrine\ORM\EntityManager $entityManager */
            $entityManager = $options['entity_manager'];
            // ...
        }

        // ...
    }

可以使用此方法将 *任何* 内容传递给表单。

2) 定义表单为服务
--------------------------------

或者，你可以将表单类定义为一个服务。在多个地方复用表单是一个好想法 - 将其注册为服务可以很容易实现。

假设你需要访问 :ref:`实体管理器 <doctrine-entity-manager>` 对象以便创建一个查询。
首先，将此作为一个参数添加到表单类::

    // src/Form/TaskType.php

    use Doctrine\ORM\EntityManagerInterface;
    // ...

    class TaskType extends AbstractType
    {
        private $entityManager;

        public function __construct(EntityManagerInterface $entityManager)
        {
            $this->entityManager = $entityManager;
        }

        // ...
    }

如果你正在使用 :ref:`自动装配 <services-autowire>` 和
:ref:`自动配置 <services-autoconfigure>`，那么你不需要做 *任何* 其他事情：
Symfony会自动将正确的 ``EntityManager`` 对象传递给你的 ``__construct()`` 方法。

如果你 **不使用自动装配和自动配置**，请手动将表单注册为服务并使用 ``form.type`` 标签进行标记：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Form\TaskType:
                arguments: ['@doctrine.orm.entity_manager']
                tags: [form.type]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Form\TaskType">
                    <argument type="service" id="doctrine.orm.entity_manager"/>
                    <tag name="form.type" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Form\TaskType;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register(TaskType::class)
            ->addArgument(new Reference('doctrine.orm.entity_manager'))
            ->addTag('form.type')
        ;

仅此而已！你的用于创建表单的控制器根本不需要修改：
Symfony足够聪明，可以从容器中加载 ``TaskType``。

阅读 :ref:`form-field-service` 以获取更多信息。
