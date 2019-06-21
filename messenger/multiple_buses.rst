.. index::
    single: Messenger; Multiple buses

多个总线
==============

构建应用时的常见构架是将命令与查询分开。命令是执行某些操作以及查询获取数据的操作。
这称为CQRS（命令查询职责分离）。请参阅Martin Fowler的 `关于CQRS的文章`_
以了解更多信息。通过定义多个总线，可以将此架构与Messenger组件一起使用。

**命令总线** 与 **查询总线** 略有不同。例如，命令总线通常不提供任何结果，查询总线则很少是异步的。
你可以通过使用中间件来配置这些总线及其规则。

通过引入一个 **事件总线** 来将动作与反应分离也可能是个好主意。事件总线可以有零个或多个订阅器。

.. configuration-block::

    .. code-block:: yaml

        framework:
            messenger:
                # 注入MessageBusInterface时要注入的总线
                default_bus: messenger.bus.commands
                buses:
                    messenger.bus.commands:
                        middleware:
                            - validation
                            - doctrine_transaction
                    messenger.bus.queries:
                        middleware:
                            - validation
                    messenger.bus.events:
                        default_middleware: allow_no_handlers
                        middleware:
                            - validation

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <!-- The bus that is going to be injected when injecting MessageBusInterface -->
                <framework:messenger default-bus="messenger.bus.commands">
                    <framework:bus name="messenger.bus.commands">
                        <framework:middleware id="validation"/>
                        <framework:middleware id="doctrine_transaction"/>
                    <framework:bus>
                    <framework:bus name="messenger.bus.queries">
                        <framework:middleware id="validation"/>
                    <framework:bus>
                    <framework:bus name="messenger.bus.events" default-middleware="allow_no_handlers">
                        <framework:middleware id="validation"/>
                    <framework:bus>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                // 注入MessageBusInterface时要注入的总线
                'default_bus' => 'messenger.bus.commands',
                'buses' => [
                    'messenger.bus.commands' => [
                        'middleware' => [
                            'validation',
                            'doctrine_transaction',
                        ],
                    ],
                    'messenger.bus.queries' => [
                        'middleware' => [
                            'validation',
                        ],
                    ],
                    'messenger.bus.events' => [
                        'default_middleware' => 'allow_no_handlers',
                        'middleware' => [
                            'validation',
                        ],
                    ],
                ],
            ],
        ]);

这将生成 ``messenger.bus.commands``、``messenger.bus.queries`` 以及 ``messenger.bus.events``
服务，以让你可以在你的服务注入它们。

类型约束和自动装配
--------------------------

自动装配是一个很棒的功能，它可以减少创建服务容器时所需的配置量。通过在服务中使用
``MessageBusInterface`` 作为参数的类型约束，将会注入默认配置的总线（即上例中的
``messenger.bus.commands``）。

使用多个总线时，你可以使用 ``DependencyInjection`` 组件的绑定功能来根据参数的名称来阐明将注入哪个总线：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            _defaults:
                # ...

                bind:
                    $commandBus: '@messenger.bus.commands'
                    $queryBus: '@messenger.bus.queries'
                    $eventBus: '@messenger.bus.events'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <defaults>
                   <bind key="$commandBus" type="service" id="messenger.bus.commands"/>
                   <bind key="$queryBus" type="service" id="messenger.bus.queries"/>
                   <bind key="$eventBus" type="service" id="messenger.bus.events"/>
                </defaults>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        $container->bind('$commandBus', 'messenger.bus.commands');
        $container->bind('$queryBus', 'messenger.bus.queries');
        $container->bind('$eventBus', 'messenger.bus.events');

为每个总线限定处理器
-------------------------

默认情况下，每个处理器都可用于处理 *所有*
总线上的消息。为了防止在没有错误的情况下将一个消息分派到错误的总线，你可以使用
``messenger.message_handler`` 标签将每个处理器限定到一个特定的总线：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\MessageHandler\SomeCommandHandler:
                tags: [{ name: messenger.message_handler, bus: messenger.bus.commands }]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\MessageHandler\SomeCommandHandler">
                    <tag name="messenger.message_handler" bus="messenger.bus.commands"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        $container->services()
            ->set(App\MessageHandler\SomeCommandHandler::class)
            ->tag('messenger.message_handler', ['bus' => 'messenger.bus.commands']);

这样，``App\MessageHandler\SomeCommandHandler`` 处理器只能被 ``messenger.bus.commands`` 总线感知。

.. tip::

    如果手动限定处理器，请确保已禁用 ``autoconfigure``，或者不实现
    ``Symfony\Component\Messenger\Handler\MessageHandlerInterface``，因为这可能导致处理器被注册两次。

    有关更多信息，请参阅 :ref:`服务自动配置 <services-autoconfigure>`。

你还可以按照一个命名约定来自动添加标签到多个类中，并使用正确的标签来按名称注册所有处理器服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml

        # 把它放在注册你的所有服务的 "App\" 行之后
        command_handlers:
            namespace: App\MessageHandler\
            resource: '%kernel.project_dir%/src/MessageHandler/*CommandHandler.php'
            tags:
                - { name: messenger.message_handler, bus: messenger.bus.commands }

        query_handlers:
            namespace: App\MessageHandler\
            resource: '%kernel.project_dir%/src/MessageHandler/*QueryHandler.php'
            tags:
                - { name: messenger.message_handler, bus: messenger.bus.queries }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- command handlers -->
                <prototype namespace="App\MessageHandler\" resource="%kernel.project_dir%/src/MessageHandler/*CommandHandler.php">
                    <tag name="messenger.message_handler" bus="messenger.bus.commands"/>
                </service>
                <!-- query handlers -->
                <prototype namespace="App\MessageHandler\" resource="%kernel.project_dir%/src/MessageHandler/*QueryHandler.php">
                    <tag name="messenger.message_handler" bus="messenger.bus.queries"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php

        // Command handlers
        $container->services()
            ->load('App\MessageHandler\\', '%kernel.project_dir%/src/MessageHandler/*CommandHandler.php')
            ->tag('messenger.message_handler', ['bus' => 'messenger.bus.commands']);

        // Query handlers
        $container->services()
            ->load('App\MessageHandler\\', '%kernel.project_dir%/src/MessageHandler/*QueryHandler.php')
            ->tag('messenger.message_handler', ['bus' => 'messenger.bus.queries']);

调试总线
-------------------

``debug:messenger`` 命令列出了每条总线的可用消息和处理器。你还可以通过提供名称作为参数将列表限制为特定总线。

.. code-block:: terminal

    $ php bin/console debug:messenger

      Messenger
      =========

      messenger.bus.commands
      ----------------------

       The following messages can be dispatched:

       ---------------------------------------------------------------------------------------
        App\Message\DummyCommand
            handled by App\MessageHandler\DummyCommandHandler
        App\Message\MultipleBusesMessage
            handled by App\MessageHandler\MultipleBusesMessageHandler
       ---------------------------------------------------------------------------------------

      messenger.bus.queries
      ---------------------

       The following messages can be dispatched:

       ---------------------------------------------------------------------------------------
        App\Message\DummyQuery
            handled by App\MessageHandler\DummyQueryHandler
        App\Message\MultipleBusesMessage
            handled by App\MessageHandler\MultipleBusesMessageHandler
       ---------------------------------------------------------------------------------------

.. _关于CQRS的文章: https://martinfowler.com/bliki/CQRS.html
