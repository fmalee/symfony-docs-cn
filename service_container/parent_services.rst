.. index::
    single: DependencyInjection; Parent services

如何使用父服务来管理公共依赖
======================================================

当你向应用添加更多功能时，你可能会开始拥有共享某些相同依赖的相关类。
例如，你可能有多个需要 ``doctrine.orm.entity_manager`` 服务的仓库类和一个可选的 ``logger`` 服务::

    // src/Repository/BaseDoctrineRepository.php
    namespace App\Repository;

    // ...
    abstract class BaseDoctrineRepository
    {
        protected $objectManager;
        protected $logger;

        public function __construct(ObjectManager $objectManager)
        {
            $this->objectManager = $objectManager;
        }

        public function setLogger(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }

        // ...
    }

你的子服务类可能如下所示::

    // src/Repository/DoctrineUserRepository.php
    namespace App\Repository;

    use App\Repository\BaseDoctrineRepository

    // ...
    class DoctrineUserRepository extends BaseDoctrineRepository
    {
        // ...
    }

    // src/Repository/DoctrinePostRepository.php
    namespace App\Repository;

    use App\Repository\BaseDoctrineRepository

    // ...
    class DoctrinePostRepository extends BaseDoctrineRepository
    {
        // ...
    }

正如你使用PHP继承来避免PHP代码中的重复一样，服务容器允许你继承父服务以避免重复的服务定义：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Repository\BaseDoctrineRepository:
                abstract:  true
                arguments: ['@doctrine.orm.entity_manager']
                calls:
                    - [setLogger, ['@logger']]

            App\Repository\DoctrineUserRepository:
                # 继承 App\Repository\BaseDoctrineRepository 服务
                parent: App\Repository\BaseDoctrineRepository

            App\Repository\DoctrinePostRepository:
                parent: App\Repository\BaseDoctrineRepository

            # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Repository\BaseDoctrineRepository" abstract="true">
                    <argument type="service" id="doctrine.orm.entity_manager" />

                    <call method="setLogger">
                        <argument type="service" id="logger" />
                    </call>
                </service>

                <!-- extends the App\Repository\BaseDoctrineRepository service -->
                <service id="App\Repository\DoctrineUserRepository"
                    parent="App\Repository\BaseDoctrineRepository"
                />

                <service id="App\Repository\DoctrinePostRepository"
                    parent="App\Repository\BaseDoctrineRepository"
                />

                <!-- ... -->
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Repository\DoctrineUserRepository;
        use App\Repository\DoctrinePostRepository;
        use App\Repository\BaseDoctrineRepository;
        use Symfony\Component\DependencyInjection\ChildDefinition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register(BaseDoctrineRepository::class)
            ->setAbstract(true)
            ->addArgument(new Reference('doctrine.orm.entity_manager'))
            ->addMethodCall('setLogger', array(new Reference('logger')))
        ;

        // extend the App\Repository\BaseDoctrineRepository service
        $definition = new ChildDefinition(BaseDoctrineRepository::class);
        $definition->setClass(DoctrineUserRepository::class);
        $container->setDefinition(DoctrineUserRepository::class, $definition);

        $definition = new ChildDefinition(BaseDoctrineRepository::class);
        $definition->setClass(DoctrinePostRepository::class);
        $container->setDefinition(DoctrinePostRepository::class, $definition);

        // ...

在此上下文中，拥有一个 ``parent`` 服务意味着父服务的参数和方法调用应该被应用子服务。
具体来说，``EntityManager`` 将被注入，并在 ``App\Repository\DoctrineUserRepository``
实例化时调用 ``setLogger()``。

父服务的所有属性都与子服务共享，*除了* ``shared``、``abstract`` 和 ``tags``。
这些 *不* 从父级继承。

.. note::

    如果你的文件中有一个 ``_defaults`` 区块，则所有子服务都需要显式的重写这些值以避免歧义。
    否则，你将看到一个有关于此的明确的错误消息。

.. tip::

    在上面的示例中，共享相同配置的类也继承了PHP中的同一父类。这根本不是必需的。
    你还可以将类似的服务定义的公共部分提取到一个父服务中，而无需再在PHP中继承一个父类。

重写父依赖
------------------------------

有时你可能希望重写仅为一个子服务注入的服务。你可以通过在子类中指定它来重写大多数设置：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Repository\DoctrineUserRepository:
                parent: App\Repository\BaseDoctrineRepository

                # 重写父服务的 public 设置
                public: false

                # 将 '@app.username_checker' 参数附加到父参数列表
                arguments: ['@app.username_checker']

            App\Repository\DoctrinePostRepository:
                parent: App\Repository\BaseDoctrineRepository

                # 重写第一个参数（使用特殊的index_N键）
                arguments:
                    index_0: '@doctrine.custom_entity_manager'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <!-- overrides the public setting of the parent service -->
                <service id="App\Repository\DoctrineUserRepository"
                    parent="App\Repository\BaseDoctrineRepository"
                    public="false"
                >
                    <!-- appends the '@app.username_checker' argument to the parent
                         argument list -->
                    <argument type="service" id="app.username_checker" />
                </service>

                <service id="App\Repository\DoctrinePostRepository"
                    parent="App\Repository\BaseDoctrineRepository"
                >
                    <!-- overrides the first argument (using the index attribute) -->
                    <argument index="0" type="service" id="doctrine.custom_entity_manager" />
                </service>

                <!-- ... -->
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Repository\DoctrineUserRepository;
        use App\Repository\DoctrinePostRepository;
        use App\Repository\BaseDoctrineRepository;
        use Symfony\Component\DependencyInjection\ChildDefinition;
        use Symfony\Component\DependencyInjection\Reference;
        // ...

        $definition = new ChildDefinition(BaseDoctrineRepository::class);
        $definition->setClass(DoctrineUserRepository::class);
        // overrides the public setting of the parent service
        $definition->setPublic(false);
        // appends the '@app.username_checker' argument to the parent argument list
        $definition->addArgument(new Reference('app.username_checker'));
        $container->setDefinition(DoctrineUserRepository::class, $definition);

        $definition = new ChildDefinition(BaseDoctrineRepository::class);
        $definition->setClass(DoctrinePostRepository::class);
        // overrides the first argument
        $definition->replaceArgument(0, new Reference('doctrine.custom_entity_manager'));
        $container->setDefinition(DoctrinePostRepository::class, $definition);
