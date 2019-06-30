.. index::
   single: DependencyInjection; Factories

使用工厂创建服务
==================================

Symfony的服务容器提供了一种控制对象创建的强大方法，它允许你指定传递给构造函数的参数(argument)以及调用方法和设置参数（parameter）。
但有时候，这不会为你提供构建对象所需的一切。
对于这种情况，你可以使用一个工厂来创建对象并告诉服务容器在工厂中调用一个方法而不是直接实例化该类。

假设你有一个工厂，该工厂通过调用其 ``createNewsletterManager()``
静态方法来配置并返回一个新的 ``NewsletterManager`` 对象::


    class NewsletterManagerStaticFactory
    {
        public static function createNewsletterManager()
        {
            $newsletterManager = new NewsletterManager();

            // ...

            return $newsletterManager;
        }
    }

要使 ``NewsletterManager`` 对象可用作服务，可以配置服务容器以使用
``NewsletterManagerStaticFactory::createNewsletterManager()`` 工厂方法：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Email\NewsletterManager:
                # 调用该静态方法
                factory: ['App\Email\NewsletterManagerStaticFactory', 'createNewsletterManager']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Email\NewsletterManager">
                    <!-- call the static method -->
                    <factory class="App\Email\NewsletterManagerStaticFactory" method="createNewsletterManager"/>

                    <!-- if the factory class is the same as the service class, you can omit
                         the 'class' attribute and define just the 'method' attribute:

                         <factory method="createNewsletterManager"/>
                    -->
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Email\NewsletterManager;
        use App\Email\NewsletterManagerStaticFactory;
        // ...

        $container->register(NewsletterManager::class)
            // call the static method
            ->setFactory([NewsletterManagerStaticFactory::class, 'createNewsletterManager']);

.. note::

    使用一个工厂来创建服务时，为类选择的值对生成的服务没有影响。实际的类名称仅取决于工厂返回的对象。
    但是，配置的类名称可能会由编译器传递使用，因此应设置为一个合理的值。

如果你的工厂不是使用一个静态函数来配置和创建服务，而是使用一个常规方法，则可以将工厂本身实例化为一个服务。
在稍后的 ":ref:`factories-passing-arguments-factory-method`" 章节中，你将学习如何在此方法中注入参数。

然后，服务容器的配置如下所示：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Email\NewsletterManagerFactory: ~

            App\Email\NewsletterManager:
                # 在特定的工厂服务上调用一个方法
                factory: ['@App\Email\NewsletterManagerFactory', 'createNewsletterManager']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Email\NewsletterManagerFactory"/>

                <service id="App\Email\NewsletterManager">
                    <!-- call a method on the specified factory service -->
                    <factory service="App\Email\NewsletterManagerFactory"
                        method="createNewsletterManager"
                    />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Email\NewsletterManager;
        use App\Email\NewsletterManagerFactory;
        use Symfony\Component\DependencyInjection\Reference;
        // ...

        $container->register(NewsletterManagerFactory::class);

        $container->register(NewsletterManager::class)
            // call a method on the specified factory service
            ->setFactory([
                new Reference(NewsletterManagerFactory::class),
                'createNewsletterManager',
            ]);

.. _factories-invokable:

假设你现在将工厂方法更改为 ``__invoke()``，这样就可以将工厂服务用作一个回调::

    class InvokableNewsletterManagerFactory
    {
        public function __invoke()
        {
            $newsletterManager = new NewsletterManager();

            // ...

            return $newsletterManager;
        }
    }

.. versionadded:: 4.3

    Symfony 4.3中引入了可调用的服务工厂。

通过省略方法名称，可以使用可调用工厂来创建和配置服务，就像路由可以引用
:ref:`可调用控制器 <controller-service-invoke>`一样。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Email\NewsletterManager:
                class:   App\Email\NewsletterManager
                factory: '@App\Email\NewsletterManagerFactory'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="App\Email\NewsletterManager"
                         class="App\Email\NewsletterManager">
                    <factory service="App\Email\NewsletterManagerFactory"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Email\NewsletterManager;
        use App\Email\NewsletterManagerFactory;
        use Symfony\Component\DependencyInjection\Reference;

        // ...
        $container->register(NewsletterManager::class, NewsletterManager::class)
            ->setFactory(new Reference(NewsletterManagerFactory::class));

.. _factories-passing-arguments-factory-method:

将参数传递给工厂方法
---------------------------------------

.. tip::

    如果你的服务启用了 :ref:`自动装配 <services-autowire>`，则该工厂方法的参数将会自动装配。

如果需要将参数传递给工厂方法，则可以使用 ``arguments`` 选项。
例如，假设前一个示例中的 ``createNewsletterManager()`` 方法将 ``templating`` 服务作为参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Email\NewsletterManager:
                factory:   ['@App\Email\NewsletterManagerFactory', createNewsletterManager]
                arguments: ['@templating']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="App\Email\NewsletterManager">
                    <factory service="App\Email\NewsletterManagerFactory" method="createNewsletterManager"/>
                    <argument type="service" id="templating"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Email\NewsletterManager;
        use App\Email\NewsletterManagerFactory;
        use Symfony\Component\DependencyInjection\Reference;

        // ...
        $container->register(NewsletterManager::class)
            ->addArgument(new Reference('templating'))
            ->setFactory([
                new Reference(NewsletterManagerFactory::class),
                'createNewsletterManager',
            ]);
