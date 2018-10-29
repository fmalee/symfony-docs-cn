.. index::
   single: Messenger

消息
========================

Symfony的信使提供一个消息总线和一些路由功能，以便在你的应用内以及通过消息队列等传输方式发送消息。
在使用它之前，请阅读 :doc:`Messenger组件文档 </components/messenger>` 以熟悉其概念。

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用程序中，运行此命令以在使用之前安装信使：

.. code-block:: terminal

    $ composer require messenger

使用Messenger服务
---------------------------

启用后，``message_bus`` 服务可以在你需要的任何服务中注入，例如在控制器中::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use App\Message\SendNotification;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Messenger\MessageBusInterface;

    class DefaultController extends AbstractController
    {
        public function index(MessageBusInterface $bus)
        {
            $bus->dispatch(new SendNotification('A string to be sent...'));
        }
    }

注册处理器
--------------------

为了在派遣消息时执行某些操作，你需要创建消息处理器。它是带一个 ``__invoke`` 方法的类::

    // src/MessageHandler/MyMessageHandler.php
    namespace App\MessageHandler;

    use Symfony\Component\Messenger\Handler\MessageHandlerInterface;

    class MyMessageHandler implements MessageHandlerInterface
    {
        public function __invoke(MyMessage $message)
        {
            // 用它做一些事情
        }
    }

消息处理器必须注册为服务并使用 ``messenger.message_handler`` 标签进行 :doc:`标记 </service_container/tags>`。
如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么 :ref:`自动配置 <services-autoconfigure>` 已经为你完成服务的注册工作。

如果你没有使用服务的自动配置，那么你需要添加此配置：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\MessageHandler\MyMessageHandler:
                tags: [messenger.message_handler]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\MessageHandler\MyMessageHandler">
                   <tag name="messenger.message_handler" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\MessageHandler\MyMessageHandler;

        $container->register(MyMessageHandler::class)
            ->addTag('messenger.message_handler');

.. note::

    如果该消息无法从处理器的类型约束中猜测出来，请使用标签上的 ``handles`` 属性。

传输
----------

默认情况下，消息在派遣后会立即处理。如果你希望异步处理消息，则必须配置一个传输系统。
这些传输系统通过队列系统或第三方与你的应用通信。
内置的AMQP传输系统允许你与大多数AMQP代理（如RabbitMQ）进行通信。

.. note::

    如果需要更多的消息代理，你应该阅读 `Enqueue's transport`_，它支持Kafka，Amazon SQS或Google Pub/Sub等服务。

一个传输系统使用“DSN”注册，“DSN”是表示连接凭据和配置的一个字符串。
默认情况下，当你安装了Messenger组件时，应该已创建以下配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    amqp: "%env(MESSENGER_TRANSPORT_DSN)%"

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:transport name="amqp" dsn="%env(MESSENGER_TRANSPORT_DSN)%" />
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'transports' => array(
                    'amqp' => '%env(MESSENGER_TRANSPORT_DSN)%',
                ),
            ),
        ));

.. code-block:: bash

    # .env
    ###> symfony/messenger ###
    MESSENGER_TRANSPORT_DSN=amqp://guest:guest@localhost:5672/%2f/messages
    ###< symfony/messenger ###

这足以让你将消息路由到 ``amqp`` 传输系统。同时还为你配置如下服务：

#. 一个 ``messenger.sender.amqp`` 发件人，用来发送(routing)消息；
#. 一个 ``messenger.receiver.amqp`` 收件人，用来接收(consuming)消息。

.. note::

    为了使用Symfony的内置AMQP传输系统，你将需要Serializer组件。确保安装时使用：

    .. code-block:: terminal

        $ composer require symfony/serializer-pack

路由
-------

你可以选择将邮件路由到发件人，而不是调用一个处理器。
作为传输系统的一部分，它负责在某处发送你的消息。你可以使用以下配置定义将哪条消息路由到哪个发件人：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                routing:
                    'My\Message\Message':  amqp # 默认传输系统的名称

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:routing message-class="My\Message\Message">
                        <framework:sender service="amqp" />
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'routing' => array(
                    'My\Message\Message' => 'amqp',
                ),
            ),
        ));

此类配置仅将 ``My\Message\Message`` 消息路由为异步，其余消息仍将直接处理。

你可以使用一个星号而不是类名将所有类的消息路由到同一发件人：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                routing:
                    'My\Message\MessageAboutDoingOperationalWork': another_transport
                    '*': amqp

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:routing message-class="My\Message\Message">
                        <framework:sender service="another_transport" />
                    </framework:routing>
                    <framework:routing message-class="*">
                        <framework:sender service="amqp" />
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'routing' => array(
                    'My\Message\Message' => 'another_transport',
                    '*' => 'amqp',
                ),
            ),
        ));

通过指定列表，还可以将一个类的消息路由到多个发件人：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                routing:
                    'My\Message\ToBeSentToTwoSenders': [amqp, audit]

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:routing message-class="My\Message\ToBeSentToTwoSenders">
                        <framework:sender service="amqp" />
                        <framework:sender service="audit" />
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'routing' => array(
                    'My\Message\ToBeSentToTwoSenders' => array('amqp', 'audit'),
                ),
            ),
        ));

通过指定 ``send_and_handle`` 选项，你还可以将一个类的消息路由到一个发件人，同时仍将它们传递到各自的处理器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                routing:
                    'My\Message\ThatIsGoingToBeSentAndHandledLocally':
                         senders: [amqp]
                         send_and_handle: true

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:routing message-class="My\Message\ThatIsGoingToBeSentAndHandledLocally" send-and-handle="true">
                        <framework:sender service="amqp" />
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'routing' => array(
                    'My\Message\ThatIsGoingToBeSentAndHandledLocally' => array(
                        'senders' => array('amqp'),
                        'send_and_handle' => true,
                    ),
                ),
            ),
        ));

消费消息
------------------

一旦消息路由后，在大多数情况下你会消费你的消息。为此，你可以使用 ``messenger:consume-messages`` 命令：

.. code-block:: terminal

    $ bin/console messenger:consume-messages amqp

第一个参数是收件人的服务名称。它可能是由你的 ``transports`` 配置创建的，也可能是你自己的收件人。

多个总线
--------------

如果你对CQRS等架构感兴趣，可能需要在应用中安装多个总线。

你可以创建多个总线（在此示例中的命令总线和事件总线），如下所示：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                # 注入 MessageBusInterface 时要注入的总线：
                default_bus: messenger.bus.commands

                # 创建总线
                buses:
                    messenger.bus.commands: ~
                    messenger.bus.events: ~

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger default-bus="messenger.bus.commands">
                    <framework:bus name="messenger.bus.commands" />
                    <framework:bus name="messenger.bus.events" />
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'default_bus' => 'messenger.bus.commands',
                'buses' => array(
                    'messenger.bus.commands' => null,
                    'messenger.bus.events' => null,
                ),
            ),
        ));

这将生成 ``messenger.bus.commands`` 和 ``messenger.bus.events`` 服务，你可以在你的服务注入它们。

.. note::

    要仅为特定总线注册一个处理器，请将一个 ``bus`` 属性添加到处理器的服务标签（``messenger.message_handler``）中，并使用该总线名称作为它的值。

类型约束和自动装配
~~~~~~~~~~~~~~~~~~~~~~~~~~

自动装配是一项很棒的功能，可以减少创建服务容器所需的配置量。
使用多个总线时，默认情况下自动装配不起作用，因为它不知道要在你自己的服务中注入哪个总线。

为了解决这一点，你可以使用依赖注入的绑定功能，该功能根据参数的名称来阐明哪个总线将被注入：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            _defaults:
                # ...

                bind:
                    $commandBus: '@messenger.bus.commands'
                    $eventBus: '@messenger.bus.events'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <defaults>
                   <bind key="$commandBus" type="service" id="messenger.bus.commands" />
                   <bind key="$commandBus" type="service" id="messenger.bus.events" />
                </defaults>
            </services>
        </container>

中间件
----------

将消息发送到消息总线时会发生什么取决于它的中间件集合（及其顺序）。
默认情况下，为每个总线配置的中间件如下所示：

#. ``logging`` 中间件，负责在总线内记录消息的开头和结尾;

#. 你自己的 中间件_ 集合；

#. ``route_messages`` 中间件，将你配置的消息路由到相应的发件人并停止中间件链;

#. ``call_message_handler`` 中间件，为给定的消息调用消息处理器。

添加自定义中间件
~~~~~~~~~~~~~~~~~~~~~~~~~~

如组件文档中所述，你可以在总线中添加自己的中间件，以添加一些额外的功能，如下所示：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                buses:
                    messenger.bus.default:
                        middleware:
                            - 'App\Middleware\MyMiddleware'
                            - 'App\Middleware\AnotherMiddleware'

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:bus name="messenger.bus.default">
                        <framework:middleware id="App\Middleware\MyMiddleware" />
                        <framework:middleware id="App\Middleware\AnotherMiddleware" />
                    </framework:bus>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'buses' => array(
                    'messenger.bus.default' => array(
                        'middleware' => array(
                            'App\Middleware\MyMiddleware',
                            'App\Middleware\AnotherMiddleware',
                        ),
                    ),
                ),
            ),
        ));

请注意，如果服务是抽象的，则每个总线将创建一个不同的服务实例。

禁用默认中间件
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你不希望总线上存在默认的中间件集合，则可以将其禁用：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                buses:
                    messenger.bus.default:
                        default_middleware: false

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:bus name="messenger.bus.default" default-middleware="false" />
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'buses' => array(
                    'messenger.bus.default' => array(
                        'default_middleware' => false,
                    ),
                ),
            ),
        ));

使用中间件工厂
~~~~~~~~~~~~~~~~~~~~~~~~~~

一些第三方bundle和库通过工厂提供可配置的中间件。
定义此类需要一个基于Symfony :doc:`依赖注入 </service_container>` 功能的两步配置：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:

            # 第1步：将一个工厂类注册为具有所需依赖的服务，以实例化一个中间件
            doctrine.orm.messenger.middleware_factory.transaction:
                class: Symfony\Bridge\Doctrine\Messenger\DoctrineTransactionMiddlewareFactory
                arguments: ['@doctrine']

            # 第2步：一个抽象定义，它将使用默认参数或中间件配置中提供的参数调用工厂
            messenger.middleware.doctrine_transaction_middleware:
                class: Symfony\Bridge\Doctrine\Messenger\DoctrineTransactionMiddleware
                factory: 'doctrine.orm.messenger.middleware_factory.transaction:createMiddleware'
                abstract: true
                # 当配置没有所提供参数时使用的默认参数。例如：
                # middleware:
                #     - doctrine_transaction_middleware: ~
                arguments: ['default']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- Step 1: a factory class is registered as a service with the required
                     dependencies to instantiate a middleware -->
                <service id="doctrine.orm.messenger.middleware_factory.transaction"
                    class="Symfony\Bridge\Doctrine\Messenger\DoctrineTransactionMiddlewareFactory">

                    <argument type="service" id="doctrine" />
                </service>

                <!-- Step 2: an abstract definition that will call the factory with default
                     arguments or the ones provided in the middleware config -->
                <service id="messenger.middleware.doctrine_transaction_middleware"
                    class="Symfony\Bridge\Doctrine\Messenger\DoctrineTransactionMiddleware"
                    abstract="true">

                    <factory service="doctrine.orm.messenger.middleware_factory.transaction"
                        method="createMiddleware" />
                    <argument>default</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Bridge\Doctrine\Messenger\DoctrineTransactionMiddleware;
        use Symfony\Bridge\Doctrine\Messenger\DoctrineTransactionMiddlewareFactory;
        use Symfony\Component\DependencyInjection\Reference;

        // Step 1: a factory class is registered as a service with the required
        // dependencies to instantiate a middleware
        $container
            ->register('doctrine.orm.messenger.middleware_factory.transaction', DoctrineTransactionMiddlewareFactory::class)
            ->setArguments(array(new Reference('doctrine')));

        // Step 2: an abstract definition that will call the factory with default
        // arguments or the ones provided in the middleware config
        $container->register('messenger.middleware.doctrine_transaction_middleware', DoctrineTransactionMiddleware::class)
            ->setFactory(array(
                new Reference('doctrine.orm.messenger.middleware_factory.transaction'),
                'createMiddleware'
            ))
            ->setAbstract(true)
            ->setArguments(array('default'));

此示例中的“default”值是要使用的实体管理器的名称，该值是
``Symfony\Bridge\Doctrine\Messenger\DoctrineTransactionMiddlewareFactory::createMiddleware`` 方法所期望的参数。

然后，你可以将 ``messenger.middleware.doctrine_transaction_middleware`` 服务作为中间件来引用和配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                buses:
                    command_bus:
                        middleware:
                            # 使用默认
                            - doctrine_transaction_middleware
                            # 使用另一个实体管理器
                            - doctrine_transaction_middleware: ['custom']

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:bus name="command_bus">
                        <!-- Using defaults -->
                        <framework:middleware id="doctrine_transaction_middleware" />
                        <!-- Using another entity manager -->
                        <framework:middleware id="doctrine_transaction_middleware">
                            <framework:argument>custom</framework:argument>
                        </framework:middleware>
                    </framework:bus>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'buses' => array(
                    'command_bus' => array(
                        'middleware' => array(
                            // Using defaults
                            'doctrine_transaction_middleware',
                            // Using another entity manager
                            array('id' => 'doctrine_transaction_middleware', 'arguments' => array('custom')),
                        ),
                    ),
                ),
            ),
        ));

.. note::

    该 ``doctrine_transaction_middleware`` 快捷方式是一个惯例。实际的服务ID以 ``messenger.middleware.`` 命名空间为前缀。

.. note::

    中间件工厂仅允许配置中的标量和数组参数（不引用其他服务）。对于大多数高级用例，请手动注册中间件的具体定义并使用其id。

.. tip::

    该 ``doctrine_transaction_middleware`` 是安装并启用DoctrineBundle和Messenger组件时自动装配的内置中间件。

自定义传输
------------------

一旦你编写了传输的发件人和收件人，就可以注册你的传输工厂，以便能够通过Symfony应用中的DSN使用它。

创建传输工厂
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你需要给FrameworkBundle提供从DSN创建你自己的传输的机会。你需要一个传输工厂::

    use Symfony\Component\Messenger\Transport\TransportFactoryInterface;
    use Symfony\Component\Messenger\Transport\TransportInterface;
    use Symfony\Component\Messenger\Transport\ReceiverInterface;
    use Symfony\Component\Messenger\Transport\SenderInterface;

    class YourTransportFactory implements TransportFactoryInterface
    {
        public function createTransport(string $dsn, array $options): TransportInterface
        {
            return new YourTransport(/* ... */);
        }

        public function supports(string $dsn, array $options): bool
        {
            return 0 === strpos($dsn, 'my-transport://');
        }
    }

传输对象需要实现 ``TransportInterface``（简单地组合``SenderInterface`` 和 ``ReceiverInterface``）。
它看起来像这样::

    class YourTransport implements TransportInterface
    {
        public function send($message): void
        {
            // ...
        }

        public function receive(callable $handler): void
        {
            // ...
        }

        public function stop(): void
        {
            // ...
        }
    }

注册工厂
~~~~~~~~~~~~~~~~~~~~~

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            Your\Transport\YourTransportFactory:
                tags: [messenger.transport_factory]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Your\Transport\YourTransportFactory">
                   <tag name="messenger.transport_factory" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Your\Transport\YourTransportFactory;

        $container->register(YourTransportFactory::class)
            ->setTags(array('messenger.transport_factory'));

使用自定义传输
~~~~~~~~~~~~~~~~~~

在 ``framework.messenger.transports.*`` 配置中，使用你自己的DSN创建指定的传输：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    yours: 'my-transport://...'

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:transport name="yours" dsn="my-transport://..." />
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', array(
            'messenger' => array(
                'transports' => array(
                    'yours' => 'my-transport://...',
                ),
            ),
        ));

除了能够将消息路由到该 ``yours`` 发件人之外，还可以访问以下服务：

#. ``messenger.sender.yours``: 发件人;
#. ``messenger.receiver.yours``: 收件人.

.. _`enqueue's transport`: https://github.com/php-enqueue/messenger-adapter
