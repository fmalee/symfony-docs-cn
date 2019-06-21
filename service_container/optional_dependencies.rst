如何让服务参数/引用变为可选
=================================================

有时，你的某个服务可能具有可选的依赖关系，这意味着你的服务无需该依赖也可以正常运行。
你可以将容器配置为在这种情况下不抛出错误。

设置缺失的依赖为null
------------------------------------

如果服务不存在，你可以使用 ``null`` 策略显式的设置参数为 ``null``：

.. configuration-block::

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="App\Newsletter\NewsletterManager">
                    <argument type="service" id="logger" on-invalid="null"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Newsletter\NewsletterManager;
        use Symfony\Component\DependencyInjection\ContainerInterface;
        use Symfony\Component\DependencyInjection\Reference;

        // ...

        $container->register(NewsletterManager::class)
            ->addArgument(new Reference(
                'logger',
                ContainerInterface::NULL_ON_INVALID_REFERENCE
            ));

.. note::

    YAML驱动目前不支持“null”策略。

忽略缺失的依赖
-----------------------------

除了是在一个方法调用中使用外，忽略缺失的依赖的行为与“null”行为相同，在此情况下，该方法调用本身将被移除。

在以下示例中，如果服务存在，容器将使用一个方法调用来注入一个服务，如果不存在，则移除该方法调用：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Newsletter\NewsletterManager:
                calls:
                    - [setLogger, ['@?logger']]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Newsletter\NewsletterManager">
                    <call method="setLogger">
                        <argument type="service" id="logger" on-invalid="ignore"/>
                    </call>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Newsletter\NewsletterManager;
        use Symfony\Component\DependencyInjection\ContainerInterface;
        use Symfony\Component\DependencyInjection\Reference;

        $container
            ->register(NewsletterManager::class)
            ->addMethodCall('setLogger', [
                new Reference(
                    'logger',
                    ContainerInterface::IGNORE_ON_INVALID_REFERENCE
                ),
            ])
        ;

.. note::

    如果方法调用的参数是参数集合并且缺少任意参数，则会移除这些元素，但仍然会使用集合的其余元素进行方法调用。

在YAML中，特殊的 ``@?`` 语法告诉服务容器该依赖是可选的。
还必须通过添加 ``setLogger()`` 方法来重写 ``NewsletterManager``::

        public function setLogger(LoggerInterface $logger)
        {
            // ...
        }
