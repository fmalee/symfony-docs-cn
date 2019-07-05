.. index::
   single: Messenger

信使
=========================================

Messenger提供了一种消息总线，能够发送消息，然后在应用中立即处理它们，或者通过传输（例如队列）发送消息，以便稍后处理。
要更深入地了解它，请阅读 :doc:`Messenger组件文档 </components/messenger>`。

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令安装信使：

.. code-block:: terminal

    $ composer require messenger

创建消息 & 处理器
----------------------------

Messenger集中在你将创建的两个不同的类上：（1）保存数据的消息类和（2）在调度该消息时将调用的处理器类。
处理器类将读取消息类并执行某些任务。

除了可以被序列化之外，对消息类没有特定要求::

    // src/Message/SmsNotification.php
    namespace App\Message;

    class SmsNotification
    {
        private $content;

        public function __construct(string $content)
        {
            $this->content = $content;
        }

        public function getContent(): string
        {
            return $this->content;
        }
    }

.. _messenger-handler:

消息处理器是一个PHP可调用，创建它的最简单方法是创建一个实现 ``MessageHandlerInterface``
的类，并且该类具有一个用消息类（或消息接口）类型约束的 ``__invoke()`` 方法::

    // src/MessageHandler/SmsNotificationHandler.php
    namespace App\MessageHandler;

    use App\Message\SmsNotification;
    use Symfony\Component\Messenger\Handler\MessageHandlerInterface;

    class SmsNotificationHandler implements MessageHandlerInterface
    {
        public function __invoke(SmsNotification $message)
        {
            // ... 做一些工作 - 比如发送短信！
        }
    }

由于 :ref:`自动配置 <services-autoconfigure>` 和 ``SmsNotification``
类型约束，Symfony知道在调度 ``SmsNotification``
消息时应该调用此处理器。大多数情况下，这就是你需要做的。但你也可以
:ref:`手动配置消息处理器 <messenger-handler-config>`。要查看所有已配置的处理器，请运行：

.. code-block:: terminal

    $ php bin/console debug:messenger

调度消息
-----------------------

你已经准备好了！要调度该消息（并调用对应处理器），请注入 ``message_bus``
服务（通过 ``MessageBusInterface``），比如在控制器中::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use App\Message\SmsNotification;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Messenger\MessageBusInterface;

    class DefaultController extends AbstractController
    {
        public function index(MessageBusInterface $bus)
        {
            // 将会调用 SmsNotificationHandler
            $bus->dispatch(new SmsNotification('Look! I created a message!'));

            // 或者使用快捷方式
            $this->dispatchMessage(new SmsNotification('Look! I created a message!'));

            // ...
        }
    }

传输：异步/队列消息
---------------------------------

默认情况下，消息在调度后会立即处理。如果要异步处理消息，可以配置传输。
传输能够发送消息（例如，到队列系统），然后
:ref:`通过一个Worker接收它们<messenger-worker>`。Messenger支持
:ref:`多种传输 <messenger-transports-config>`。

.. note::

    如果你想使用不受支持的传输，请查看
    `Enqueue的传输`_，它支持Kafka、Amazon SQS和Google Pub/Sub等传输。

可以使用"DSN"注册传输。得益于Messenger的Flex指令，你的 ``.env`` 文件中已经有一些例子。

.. code-block:: bash

    # MESSENGER_TRANSPORT_DSN=amqp://guest:guest@localhost:5672/%2f/messages
    # MESSENGER_TRANSPORT_DSN=doctrine://default
    # MESSENGER_TRANSPORT_DSN=redis://localhost:6379/messages

取消你想要的任何传输的注释（或在 ``.env.local`` 中设置它）。有关详细信息，请参阅
:ref:`messenger-transports-config`。

接下来，让我们在 ``config/packages/messenger.yaml`` 中定义一个使用此配置的名为 ``async`` 的传输：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async: "%env(MESSENGER_TRANSPORT_DSN)%"

                    # 或展开以配置更多选项
                    #async:
                    #    dsn: "%env(MESSENGER_TRANSPORT_DSN)%"
                    #    options: []

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
                <framework:messenger>
                    <framework:transport name="async">%env(MESSENGER_TRANSPORT_DSN)%</framework:transport>

                    <!-- or expanded to configure more options -->
                    <framework:transport name="async"
                        dsn="%env(MESSENGER_TRANSPORT_DSN)%"
                    >
                        <option key="...">...</option>
                    </framework:transport>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async' => '%env(MESSENGER_TRANSPORT_DSN)%',

                    // or expanded to configure more options
                    'async' => [
                       'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                       'options' => []
                    ],
                ],
            ],
        ]);

.. _messenger-routing:

将消息路由到传输
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

现在你已配置好传输，如果不想立即处理消息，你可以将它们配置为发送到一个传输：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async: "%env(MESSENGER_TRANSPORT_DSN)%"

                routing:
                    # "async" 是你在上面为传输指定的任何名称
                    'App\Message\SmsNotification':  async

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
                <framework:messenger>
                    <framework:routing message-class="App\Message\SmsNotification">
                        <!-- async is whatever name you gave your transport above -->
                        <framework:sender service="async"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'routing' => [
                    // async is whatever name you gave your transport above
                    'App\Message\SmsNotification' => 'async',
                ],
            ],
        ]);

现在 ``App\Message\SmsNotification`` 将被发送到 ``async``
传输，并且 *不会* 立即调用其处理器。任何不匹配 ``routing`` 的消息仍会立即处理。

你还可以使用其父类或接口来路由多个类。或者将消息发送到多个传输：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                routing:
                    # 路由所有扩展此示例基类或接口的消息
                    'App\Message\AbstractAsyncMessage': async
                    'App\Message\AsyncMessageInterface': async

                    'My\Message\ToBeSentToTwoSenders': [async, audit]

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
                <framework:messenger>
                    <!-- route all messages that extend this example base class or interface -->
                    <framework:routing message-class="App\Message\AbstractAsyncMessage">
                        <framework:sender service="async"/>
                    </framework:routing>
                    <framework:routing message-class="App\Message\AsyncMessageInterface">
                        <framework:sender service="async"/>
                    </framework:routing>
                    <framework:routing message-class="My\Message\ToBeSentToTwoSenders">
                        <framework:sender service="async"/>
                        <framework:sender service="audit"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'routing' => [
                    // route all messages that extend this example base class or interface
                    'App\Message\AbstractAsyncMessage' => 'async',
                    'App\Message\AsyncMessageInterface' => 'async',
                    'My\Message\ToBeSentToTwoSenders' => ['async', 'audit'],
                ],
            ],
        ]);

消息中的Doctrine实体
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果需要在消息中传递Doctrine实体，最好传递实体的主键（或处理器实际需要的任何相关信息，如
``email`` 等）而不是对象::

    class NewUserWelcomeEmail
    {
        private $userId;

        public function __construct(int $userId)
        {
            $this->userId = $userId;
        }

        public function getUserId(): int
        {
            return $this->userId;
        }
    }

然后，你可以在你的处理器中查询一个“新鲜”的对象::

    // src/MessageHandler/NewUserWelcomeEmailHandler.php
    namespace App\MessageHandler;

    use App\Message\NewUserWelcomeEmail;
    use App\Repository\UserRepository;
    use Symfony\Component\Messenger\Handler\MessageHandlerInterface;

    class NewUserWelcomeEmailHandler implements MessageHandlerInterface
    {
        private $userRepository;

        public function __construct(UserRepository $userRepository)
        {
            $this->userRepository = $userRepository;
        }

        public function __invoke(NewUserWelcomeEmail $welcomeEmail)
        {
            $user = $this->userRepository->find($welcomeEmail->getUserId());

            // ... 发送电子邮件！
        }
    }

这样可以保证实体总是包含新数据。

同步处理消息
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果一个消息不 :ref:`匹配任何路由规则 <messenger-routing>`，则不会将其发送到任何传输，而是立即处理。
在某些情况下（比如 `将处理器绑定到不同的传输`_ 时），显式处理这些会更容易或更灵活 -
通过创建一个 ``sync`` 传输并“发送”消息来立即处理它：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    # ... 其他传输

                    sync: 'sync://'

                routing:
                    App\Message\SmsNotification: sync

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
                <framework:messenger>
                    <!-- ... other transports -->

                    <framework:transport name="sync" dsn="sync://"/>

                    <framework:routing message-class="App\Message\SmsNotification">
                        <framework:sender service="sync"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    // ... other transports

                    'sync' => 'sync://',
                ],
                'routing' => [
                    'App\Message\SmsNotification' => 'sync',
                ],
            ],
        ]);

创建自己的传输
~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你需要从不受支持的地方发送或接收消息，你还可以创建自己的传输。请参见
:doc:`/messenger/custom-transport`。

.. _messenger-worker:

消费消息（运行Worker）
---------------------------------------

一旦你的消息被路由，在大多数情况下你将需要“消费”它们。你可以使用 ``messenger:consume``
命令来执行此操作：

.. code-block:: terminal

    $ php bin/console messenger:consume async

    # 使用 -vv 查看正在发生的事情的详细信息
    $ php bin/console messenger:consume async -vv

.. versionadded:: 4.3

    在Symfony 4.3中重命名了 ``messenger:consume`` 命令（之前名为
    ``messenger:consume-messages``）。

第一个参数是接收方的名称（如果你路由到一个自定义服务，则为服务ID）。
默认情况下，该命令将永久运行：在你的传输上查找新消息并处理它们。此命令称为“worker”。

部署到生产
~~~~~~~~~~~~~~~~~~~~~~~

在生产中，需要考虑一些重要的事情：

**使用 Supervisor 让你的 Worker 持续运行**
    你会希望一个或多个“worker”一直在运行。为此，请使用
    :ref:`Supervisor <messenger-supervisor>` 等进程控制系统。

**不要让 Worker 永久运行**
    一些服务（如Doctrine的EntityManager）将随着时间的推移消耗更多的内存。
    因此，不要让你的Worker永远运行，而是使用类似 ``messenger:consume --limit=10``
    的标志，告诉你的Worker在退出前只处理10条消息（然后Supervisor将创建一个新进程）。
    还有其他选项，如 ``--memory-limit=128M`` 和 ``--time-limit=3600``。

**部署时重新启动 Worker**
    每次部署时，都需要重新启动所有Worker进程，以便新部署的代码能生效。为此，请在部署上运行
    ``messenger:stop-workers``。这将向每个Worker发出信号，指示它应该在完成当前正在处理的消息后正常关闭。
    然后，Supervisor将创建新的Worker进程。该命令在内部使用
    :ref:`app <cache-configuration-with-frameworkbundle>` 缓存 -
    因此请确保将其配置为你喜欢的适配器。

优先传输
~~~~~~~~~~~~~~~~~~~~~~

有时，某些类型的消息应该具有更高的优先级，并先于其他类型的消息进行处理。
为了实现这一点，你可以创建多个传输并将不同的消息路由到它们。例如：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_high:
                        dsn: '%env(MESSENGER_TRANSPORT_DSN)%'
                        options:
                            # “queue_name” 特定于doctrine传输
                            # 尝试使用amqp的“exchange”或redis的“group1”
                            queue_name: high
                    async_priority_low:
                        dsn: '%env(MESSENGER_TRANSPORT_DSN)%'
                        options:
                            queue_name: low

                routing:
                    'App\Message\SmsNotification':  async_priority_low
                    'App\Message\NewUserWelcomeEmail':  async_priority_high

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
                <framework:messenger>
                    <framework:transport name="async_priority_high" dsn="%env(MESSENGER_TRANSPORT_DSN)%">
                        <option key="queue_name">high</option>
                    </framework:transport>
                    <framework:transport name="async_priority_low" dsn="%env(MESSENGER_TRANSPORT_DSN)%">
                        <option key="queue_name">low</option>
                    </framework:transport>

                    <framework:routing message-class="App\Message\SmsNotification">
                        <framework:sender service="async_priority_low"/>
                    </framework:routing>
                    <framework:routing message-class="App\Message\NewUserWelcomeEmail">
                        <framework:sender service="async_priority_high"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async_priority_high' => [
                        'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                        'options' => [
                            'queue_name' => 'high',
                        ],
                    ],
                    'async_priority_low' => [
                        'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                        'options' => [
                            'queue_name' => 'low',
                        ],
                    ],
                ],
                'routing' => [
                    'App\Message\SmsNotification' => 'async_priority_low',
                    'App\Message\NewUserWelcomeEmail' => 'async_priority_high',
                ],
            ],
        ]);

然后，你可以为每个传输运行单个Worker，或者指示一个Worker按优先级顺序处理消息：

.. code-block:: terminal

    $ php bin/console messenger:consume async_priority_high async_priority_low

该Worker总是首先等待查找来自 ``async_priority_high`` 的消息。如果没有，它 *才会* 消费来自
``async_priority_low`` 的消息。

.. _messenger-supervisor:

Supervisor配置
~~~~~~~~~~~~~~~~~~~~~~~~

Supervisor是一个很好的工具，可以保证你的Worker进程 *始终*
在运行（即使它因故障而关闭、达到消息限制或接收到 ``messenger:stop-workers``）。
你可以在Ubuntu上安装它，例如通过：

.. code-block:: terminal

    $ sudo apt-get install supervisor

Supervisor的配置文件通常位于 ``/etc/supervisor/conf.d``
目录中。例如，你可以在那里创建一个新的 ``messenger-worker.conf``
文件，以确保始终运行2个 ``messenger:consume`` 实例：

.. code-block:: ini

    ;/etc/supervisor/conf.d/messenger-worker.conf
    [program:messenger-consume]
    command=php /path/to/your/app/bin/console messenger:consume async --time-limit=3600
    user=ubuntu
    numprocs=2
    autostart=true
    autorestart=true
    process_name=%(program_name)s_%(process_num)02d

更改 ``async`` 参数为需要的（多个）传输的名称以及 ``user``
为你的服务器上的Unix用户。接下来，告诉Supervisor读取你的配置并启动你的Worker：

.. code-block:: terminal

    $ sudo supervisorctl reread

    $ sudo supervisorctl update

    $ sudo supervisorctl start messenger-consume

有关更多详细信息，请参阅 `Supervisor文档`_。

.. _messenger-retries-failures:

重试和失败
------------------

如果在消费来自一个传输的消息时抛出异常，则会自动将该消息重新发送回该传输以便重试。
默认情况下，消息在被丢弃或 :ref:`发送到失败传输 <messenger-failure-transport>` 之前将重试3次。
如果失败是由临时问题引起的，则每次重试的时间也将延迟。所有这些对于每个传输都是可配置的：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_high:
                        dsn: '%env(MESSENGER_TRANSPORT_DSN)%'

                        # 默认配置
                        retry_strategy:
                            max_retries: 3
                            # 毫秒级别的延迟
                            delay: 1000
                            # 在每次重试之前将延迟调至更高
                            # 例如延迟1秒、2秒、4秒
                            multiplier: 2
                            max_delay: 0
                            # 使用实现了 Symfony\Component\Messenger\Retry\RetryStrategyInterface
                            # 的服务重写所有这些
                            # service: null

避免重试
~~~~~~~~~~~~~~~~~

有时处理的消息可能会以你 *明确* 是永久性的方式失败，那么不应该重试该消息。
如果你抛出
:class:`Symfony\\Component\\Messenger\\Exception\\UnrecoverableMessageHandlingException`，
将不会重试该消息。

.. _messenger-failure-transport:

保存并重试失败的消息
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果消息失败，则会多次重试（``max_retries``），然后将被丢弃。
为避免这种情况发生，你可以改为配置一个 ``failure_transport``：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                # 重试后，消息将发送到 "failed" 传输
                failure_transport: failed

                transports:
                    # ... 其他传输

                    failed: 'doctrine://default?queue_name=failed'

在本例中，如果处理一个消息失败了3次（默认的 ``max_retries``），那么它将被发送到 ``failed``
传输。当你 *可以* 像消费普通传输一样使用 ``messenger:consume failed``
时，你通常希望手动查看失败传输中的消息并选择一些重试：

.. code-block:: terminal

    # 查看失败传输中的所有消息
    $ php bin/console messenger:failed:show

    # 查看有关特定失败的详细信息
    $ php bin/console messenger:failed:show 20 -vv

    # 逐个查看和重试消息
    $ php bin/console messenger:failed:retry -vv

    # 重试特定消息
    $ php bin/console messenger:failed:retry 20 30 --force

    # 删除不需要重试的消息
    $ php bin/console messenger:failed:remove 20

如果消息再次失败，则由于正常的
:ref:`重试规则 <messenger-retries-failures>`，它将被重新发送回失败传输。
一旦达到最大重试次数，该消息将被永久丢弃。

.. _messenger-transports-config:

传输配置
-----------------------

Messenger支持许多不同的传输类型，每种类型都有自己的选项。

AMQP传输
~~~~~~~~~~~~~~

``amqp`` 传输配置如下：

.. code-block:: bash

    # .env
    MESSENGER_TRANSPORT_DSN=amqp://guest:guest@localhost:5672/%2f/messages

要使用Symfony的内置AMQP传输，你需要AMQP PHP扩展。

.. note::

    默认情况下，该传输将自动创建所需的任何交换机(exchange)、队列和键绑定。
    你可以禁用它，但某些功能可能无法正常工作（如延迟队列）。

该传输还有许多其他选项，包括配置交换机的方法、队列的绑定键等。具体请参阅
:class:`Symfony\\Component\\Messenger\\Transport\\AmqpExt\\Connection` 文档。

你还可以通过添加 :class:`Symfony\\Component\\Messenger\\Transport\\AmqpExt\\AmqpStamp`
到你的信封来为消息配置特定于AMQP的设置::

    use Symfony\Component\Messenger\Transport\AmqpExt\AmqpStamp;
    // ...

    $attributes = [];
    $bus->dispatch(new SmsNotification(), [
        new AmqpStamp('custom-routing-key', AMQP_NOPARAM, $attributes)
    ]);

Doctrine传输
~~~~~~~~~~~~~~~~~~

.. versionadded:: 4.3

    在Symfony 4.3中引入了Doctrine传输。

Doctrine传输可用于在一个数据库表中存储消息。

.. code-block:: bash

    # .env
    MESSENGER_TRANSPORT_DSN=doctrine://default

如果你有多个连接，并且希望使用除“default”以外的一个连接，则其格式为
``doctrine://<connection_name>``。首次使用该传输时，该传输将自动创建一个名为
``messenger_messages`` （这是可配置的）的表。你可以使用 ``auto_setup``
选项来禁用该行为，并通过调用 ``messenger:setup-transports`` 命令来手动设置该表。

.. tip::

    为避免Doctrine Migrations等工具因为因为该表不是正常模式的一部分而尝试删除它，你可以设置
    ``schema_filter`` 选项：

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            dbal:
                schema_filter: '~^(?!messenger_messages)~'

该传输有很多选项：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_high: "%env(MESSENGER_TRANSPORT_DSN)%?queue_name=high_priority"
                    async_normal:
                        dsn: "%env(MESSENGER_TRANSPORT_DSN)%"
                        options:
                            queue_name: normal_priority

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
                <framework:messenger>
                    <framework:transport name="async_priority_high" dsn="%env(MESSENGER_TRANSPORT_DSN)%?queue_name=high_priority"/>
                    <framework:transport name="async_priority_low" dsn="%env(MESSENGER_TRANSPORT_DSN)%">
                        <framework:option queue_name="normal_priority"/>
                    </framework:transport>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async_priority_high' => 'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%?queue_name=high_priority',
                    'async_priority_low' => [
                        'dsn' => '%env(MESSENGER_TRANSPORT_DSN)%',
                        'options' => [
                            'queue_name' => 'normal_priority'
                        ]
                    ],
                ],
            ],
        ]);

``options`` 中定义的选项优先于DSN中定义的选项。

==================  ===================================  ======================
     选项           描述                                 默认值
==================  ===================================  ======================
table_name          数据表的名称                          messenger_messages
queue_name          队列的名称（表中的一个列，             default
                    以便于多个传输使用一个表）
redeliver_timeout   在重试位于队列中但处于“处理”状态的      3600
                    消息之前的超时时间（如果某个worker
                    由于某种 原因 死亡，就会发生这种情
                    况，而该消息应该要重试）- 秒级。
auto_setup          是否应在发送/接收期间自动创建表。       true
==================  ===================================  ======================

Redis传输
~~~~~~~~~~~~~~~

.. versionadded:: 4.3

    在Symfony 4.3中引入了Redis传输。

Redis传输使用 `流`_ 来队列消息。

.. code-block:: bash

    # .env
    MESSENGER_TRANSPORT_DSN=redis://localhost:6379/messages
    # 完整的DSN示例
    MESSENGER_TRANSPORT_DSN=redis://password@localhost:6379/messages/symfony/consumer?auto_setup=true&serializer=1

要使用Redis传输，你需要Redis PHP扩展（^ 4.3）和正在运行的Redis服务器（^ 5.0）。

.. caution::

    Redis传输不支持“延迟”消息。

可以通过DSN或在 ``messenger.yaml`` 中该传输下的 ``options`` 键来配置许多选项：

==================  =====================================  =======
     选项                 描述                             默认值
==================  =====================================  =======
stream              Redis流的名称                          messages
group               Redis消费组的名称                      symfony
consumer            Redis中使用的消费器名称                 consumer
auto_setup          是否自动创建Redis组？                   true
auth                Redis的密码
serializer          如何序列化Redis中的最终负载             ``Redis::SERIALIZER_PHP``
                    （``Redis::OPT_SERIALIZER`` 选项）
==================  =====================================  =======

内存传输
~~~~~~~~~~~~~~~~~~~

.. versionadded:: 4.3

    在Symfony 4.3中引入了 ``in-memory`` 传输。

``in-memory`` 传输并不实际的投递消息。相反，它在请求期间将消息保存在内存中，这对测试很有用。
例如，如果你有一个 ``async_priority_normal`` 传输，则可以在 ``test`` 环境中重写它以使用此传输：

.. code-block:: yaml

    # config/packages/test/messenger.yaml
    framework:
        messenger:
            transports:
                async_priority_normal: 'in-memory:///'

然后，在测试期间消息将 *不会* 投递到实际传输。
更棒的是，在测试中，你可以检查在请求期间是否发送了一条消息::

    // tests/DefaultControllerTest.php
    namespace App\Tests;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;
    use Symfony\Component\Messenger\Transport\InMemoryTransport;

    class DefaultControllerTest extends WebTestCase
    {
        public function testSomething()
        {
            $client = static::createClient();
            // ...

            $this->assertSame(200, $client->getResponse()->getStatusCode());

            /* @var InMemoryTransport $transport */
            $transport = self::$container->get('messenger.transport.async_priority_normal');
            $this->assertCount(1, $transport->get());
        }
    }

序列化消息
~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 4.3

    在4.3中，默认的序列化器从Symfony序列化器更改为原生的PHP序列化器。
    现有的应用应该重新配置传输以使用Symfony序列化器，以避免在升级后丢失已队列化的消息。

当消息发送到传输（并从传输接收）时，它们使用PHP的原生 ``serialize()`` & ``unserialize()``
函数进行序列化。你可以将此行为全局的（或每个传输）更改为实现了
:class:`Symfony\\Component\\Messenger\\Transport\\Serialization\\SerializerInterface`
的一个服务：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                serializer:
                    default_serializer: messenger.transport.symfony_serializer
                    symfony_serializer:
                        format: json
                        context: { }

                transports:
                    async_priority_normal:
                        dsn: # ...
                        serializer: messenger.transport.symfony_serializer

``messenger.transport.symfony_serializer`` 是一个使用 :doc:`Serializer组件 </serializer>`
的内置服务，它可以通过几种方式进行配置。如果你 *确实* 选择使用Symfony序列化器，则可以通过
:class:`Symfony\\Component\\Messenger\\Stamp\\SerializerStamp`
（请参阅 `Envelopes & Stamps`_）逐个控制上下文。

自定义处理器
--------------------

.. _messenger-handler-config:

手动配置处理器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony通常会 :ref:`自动查找并注册你的处理器 <messenger-handler>`。但是，你也可以手动配置处理器
- 使用 ``messenger.message_handler`` 标签来标记处理器服务，同时还可以传递一些额外的配置。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\MessageHandler\SmsNotificationHandler:
                tags: [messenger.message_handler]

                # 或使用选项进行配置
                tags:
                    -
                        name: messenger.message_handler
                        # 只有在不能通过类型约束猜到时才需要
                        handles: App\Message\SmsNotification

                        # 这里支持 getHandledMessages() 返回的选项

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\MessageHandler\SmsNotificationHandler">
                   <tag name="messenger.message_handler"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\MessageHandler\SmsNotificationHandler;

        $container->register(SmsNotificationHandler::class)
            ->addTag('messenger.message_handler');

处理器订阅器和选项
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

一个处理器类可以通过实现
:class:`Symfony\\Component\\Messenger\\Handler\\MessageSubscriberInterface`
来处理多个消息或配置自身::

    // src/MessageHandler/SmsNotificationHandler.php
    namespace App\MessageHandler;

    use App\Message\OtherSmsNotification;
    use App\Message\SmsNotification;
    use Symfony\Component\Messenger\Handler\MessageSubscriberInterface;

    class SmsNotificationHandler implements MessageSubscriberInterface
    {
        public function __invoke(SmsNotification $message)
        {
            // ...
        }

        public function handleOtherSmsNotification(OtherSmsNotification $message)
        {
            // ...
        }

        public static function getHandledMessages(): iterable
        {
            // 在 __invoke 中处理此消息
            yield SmsNotification::class;

            // 也可以在 handleOtherSmsNotification 中处理此消息
            yield OtherSmsNotification::class => [
                'method' => 'handleOtherSmsNotification',
                //'priority' => 0,
                //'bus' => 'messenger.bus.default',
            ];
        }
    }

将处理器绑定到不同的传输
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

每个消息都可以有多个处理器，当消费一个消息时，将调用其 *所有* 处理器。
但是，你也可以将一个处理器配置为仅在从 *特定* 传输接收时才被调用。
这允许你拥有一个其中每个处理器都由消费不同传输的不同“worker”调用的消息。

假设你有一个带有两个处理器的 ``UploadedImage`` 消息：

* ``ThumbnailUploadedImageHandler``：你希望通过名为 ``image_transport``
  的传输来处理它；

* ``NotifyAboutNewUploadedImageHandler``：你希望通过名为
  ``async_priority_normal`` 的传输来处理它。

为此，请将 ``from_transport`` 选项添加到每个处理器。例如::

    // src/MessageHandler/ThumbnailUploadedImageHandler.php
    namespace App\MessageHandler;

    use App\Message\UploadedImage;
    use Symfony\Component\Messenger\Handler\MessageSubscriberInterface;

    class ThumbnailUploadedImageHandler implements MessageSubscriberInterface
    {
        public function __invoke(UploadedImage $uploadedImage)
        {
            // 做一些缩略图工作
        }

        public static function getHandledMessages(): iterable
        {
            yield UploadedImage::class => [
                'from_transport' => 'image_transport',
            ];
        }
    }

同样地::

    // src/MessageHandler/NotifyAboutNewUploadedImageHandler.php
    // ...

    class NotifyAboutNewUploadedImageHandler implements MessageSubscriberInterface
    {
        // ...

        public static function getHandledMessages(): iterable
        {
            yield UploadedImage::class => [
                'from_transport' => 'async_priority_normal',
            ];
        }
    }

然后，确保将你的消息“路由”到这 *两个* 传输：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async_priority_normal: # ...
                    image_transport: # ...

                routing:
                    # ...
                    'App\Message\UploadedImage': [image_transport, async_priority_normal]

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
                <framework:messenger>
                    <framework:transport name="async_priority_normal" dsn="..."/>
                    <framework:transport name="image_transport" dsn="..."/>

                    <framework:routing message-class="App\Message\UploadedImage">
                        <framework:sender service="image_transport"/>
                        <framework:sender service="async_priority_normal"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'async_priority_normal' => '...',
                    'image_transport' => '...',
                ],
                'routing' => [
                    'App\Message\UploadedImage' => ['image_transport', 'async_priority_normal']
                ]
            ],
        ]);

仅此而已！你现在可以单独消费每个传输：

.. code-block:: terminal

    # 处理该消息时只会调用 ThumbnailUploadedImageHandler
    $ php bin/console messenger:consume image_transport -vv

    $ php bin/console messenger:consume async_priority_normal -vv

.. caution::

    如果一个处理器 *不* 具有 ``from_transport`` 它将在接收对应消息的 *每个* 传输上执行。

扩展信使
-------------------

Envelopes & Stamps
~~~~~~~~~~~~~~~~~~

一个消息可以是任何PHP对象。有时，你可能需要对消息进行一些额外的配置 -
比如在AMQP中处理消息的方式，或者在处理消息之前添加一个延迟。
你可以通过在消息中添加“邮票”来完成此操作::

    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\MessageBusInterface;
    use Symfony\Component\Messenger\Stamp\DelayStamp;

    public function index(MessageBusInterface $bus)
    {
        $bus->dispatch(new SmsNotification('...'), [
            // 处理前等待5秒
            new DelayStamp(5000)
        ]);

        // 或者显式的创建信封
        $bus->dispatch(new Envelope(new SmsNotification('...'), [
            new DelayStamp(5000)
        ]));

        // ...
    }

在内部，每条消息都被封装在一个保存着消息和邮票的 ``Envelope`` 中。
你可以手动创建它，或者允许消息总线执行它。有各种不同用途的邮票，它们在内部用于跟踪有关消息的信息 -
例如处理消息的消息总线，或者在失败后重试。

中间件
~~~~~~~~~~

将消息调度到消息总线时会发生什么取决于它的中间件集合（及其顺序）。
默认情况下，为每个总线配置的中间件如下所示：

#. ``add_bus_name_stamp_middleware`` - 添加一个邮票以记录此消息被调度到哪个总线；

#. ``dispatch_after_current_bus``- 请参阅 :doc:`/messenger/message-recorder`；

#. ``failed_message_processing_middleware`` - 处理正在通过
   :ref:`失败传输 <messenger-failure-transport>`
   重试的消息，使它们就像是从原始传输中接收一样的正常运行；

#. 你自己的 中间件_ 集合；

#. ``send_message`` - 如果为传输配置了路由，则会向该传输发送消息并停止中间件链；

#. ``handle_message`` - 为给定消息调用消息处理器。

.. note::

    这些中间件名称实际上是快捷方式名称。真实的服务ID都有 ``messenger.middleware.`` 前缀。

你可以将自己的中间件添加到此列表中，或者完全禁用默认中间件并 *仅* 包含你自己的中间件：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                buses:
                    messenger.bus.default:
                        middleware:
                            # 实现了 Symfony\Component\Messenger\Middleware 的服务的ID
                            - 'App\Middleware\MyMiddleware'
                            - 'App\Middleware\AnotherMiddleware'

                        #default_middleware: false

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
                <framework:messenger>
                    <framework:middleware id="App\Middleware\MyMiddleware"/>
                    <framework:middleware id="App\Middleware\AnotherMiddleware"/>
                    <framework:bus name="messenger.bus.default" default-middleware="false"/>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'buses' => [
                    'messenger.bus.default' => [
                        'middleware' => [
                            'App\Middleware\MyMiddleware',
                            'App\Middleware\AnotherMiddleware',
                        ],
                        'default_middleware' => false,
                    ],
                ],
            ],
        ]);


.. note::

    如果中间件服务是抽象的，则每个总线将创建不同的服务实例。

Doctrine中间件
~~~~~~~~~~~~~~~~~~~~~~~

如果你在应用中使用Doctrine，则可能需要使用许多可选的中间件：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                buses:
                    command_bus:
                        middleware:
                            # 在单个Doctrine事务中封装所有处理器
                            # 处理器不需要调用 flush()，
                            # 并且任何处理器中的错误都将导致回滚
                            - doctrine_transaction

                            # 每次处理一个消息时，Doctrine连接都会是“pinged”并在关闭时重新连接。
                            # 如果你的worker运行很长时间，并且有时数据库连接丢失，
                            # 那么这个中间件能派上用场。
                            - doctrine_ping_connection

                            # 处理完成后，Doctrine连接将关闭，这可以释放worker中的数据库连接，
                            # 而不是永久保持打开状态
                            - doctrine_close_connection

                            # 或者将不同的实体管理器传递给任何处理器
                            #- doctrine_transaction: ['custom']

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
                <framework:messenger>
                    <framework:bus name="command_bus">
                        <framework:middleware id="doctrine_transaction"/>
                        <framework:middleware id="doctrine_ping_connection"/>
                        <framework:middleware id="doctrine_close_connection"/>

                        <!-- or pass a different entity manager to any -->
                        <!--
                        <framework:middleware id="doctrine_transaction">
                            <framework:argument>custom</framework:argument>
                        </framework:middleware>
                        -->
                    </framework:bus>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'buses' => [
                    'command_bus' => [
                        'middleware' => [
                            'doctrine_transaction',
                            'doctrine_ping_connection',
                            'doctrine_close_connection',
                            // Using another entity manager
                            ['id' => 'doctrine_transaction', 'arguments' => ['custom']],
                        ],
                    ],
                ],
            ],
        ]);

信使事件
~~~~~~~~~~~~~~~~

除了中间件，Messenger还会调度几个事件。你可以 :doc:`创建一个事件监听器 </event_dispatcher>`
来挂钩到进程的各个部分。对于事件，每个事件类即是事件名称：

* :class:`Symfony\\Component\\Messenger\\Event\\SendMessageToTransportsEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerMessageFailedEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerMessageHandledEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerMessageReceivedEvent`
* :class:`Symfony\\Component\\Messenger\\Event\\WorkerStoppedEvent`

多个总线、命令和事件总线
-------------------------------------

默认情况下，Messenger为你提供单个消息总线服务。
但是，你可以根据需要配置任意多个消息总线，创建"command"、"query" 或 "event" 总线并控制它们的中间件。
详情请参阅 :doc:`/messenger/multiple_buses`。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /messenger/*

.. _`Enqueue的传输`: https://github.com/php-enqueue/messenger-adapter
.. _`流`: https://redis.io/topics/streams-intro
.. _`Supervisor文档`: http://supervisord.org/
