如何通过处理器向日志消息添加额外数据
=====================================================

Monolog允许你在记录之前处理记录以添加一些额外的数据。
一个处理器(Processor)可以应用于整个处理器（handler）堆栈，也可以仅应用于一个特定处理器。

Processor是一个可调用对象，它接收记录作为其第一个参数。
可以使用 ``monolog.processor`` DIC标签来配置Processor。请参阅
:ref:`有关它的参考 <dic_tags-monolog-processor>`。

添加一个会话/请求令牌
------------------------------

有时很难判断日志中的哪些项属于哪个会话和/或请求。
以下示例将使用一个Processor为每个请求添加一个唯一令牌::

    namespace App\Logger;

    use Symfony\Component\HttpFoundation\Session\SessionInterface;

    class SessionRequestProcessor
    {
        private $session;
        private $sessionId;

        public function __construct(SessionInterface $session)
        {
            $this->session = $session;
        }

        public function __invoke(array $record)
        {
            if (!$this->session->isStarted()) {
                return $record;
            }

            if (!$this->sessionId) {
                $this->sessionId = substr($this->session->getId(), 0, 8) ?: '????????';
            }

            $record['extra']['token'] = $this->sessionId.'-'.substr(uniqid('', true), -8);

            return $record;
        }
    }

接下来，将你的类注册为服务，以及使用额外信息的一个格式化器：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            monolog.formatter.session_request:
                class: Monolog\Formatter\LineFormatter
                arguments:
                    - "[%%datetime%%] [%%extra.token%%] %%channel%%.%%level_name%%: %%message%% %%context%% %%extra%%\n"

            App\Logger\SessionRequestProcessor:
                tags:
                    - { name: monolog.processor }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="monolog.formatter.session_request"
                    class="Monolog\Formatter\LineFormatter">

                    <argument>[%%datetime%%] [%%extra.token%%] %%channel%%.%%level_name%%: %%message%% %%context%% %%extra%%&#xA;</argument>
                </service>

                <service id="App\Logger\SessionRequestProcessor">
                    <tag name="monolog.processor"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Logger\SessionRequestProcessor;
        use Monolog\Formatter\LineFormatter;

        $container
            ->register('monolog.formatter.session_request', LineFormatter::class)
            ->addArgument('[%%datetime%%] [%%extra.token%%] %%channel%%.%%level_name%%: %%message%% %%context%% %%extra%%\n');

        $container
            ->register(SessionRequestProcessor::class)
            ->addTag('monolog.processor', ['method' => 'processRecord']);

最后，在任何你想要的处理器上设置要使用的格式化器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                main:
                    type: stream
                    path: '%kernel.logs_dir%/%kernel.environment%.log'
                    level: debug
                    formatter: monolog.formatter.session_request

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler
                    name="main"
                    type="stream"
                    path="%kernel.logs_dir%/%kernel.environment%.log"
                    level="debug"
                    formatter="monolog.formatter.session_request"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', [
            'handlers' => [
                'main' => [
                    'type'      => 'stream',
                    'path'      => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level'     => 'debug',
                    'formatter' => 'monolog.formatter.session_request',
                ],
            ],
        ]);

如果使用了多个处理器，还可以在处理器级别或通道级别注册一个Processor，而不是全局注册它（请参阅以下部分）。

Symfony的MonologBridge提供了可在你的应用内注册的一些处理器。

:class:`Symfony\\Bridge\\Monolog\\Processor\\DebugProcessor`
    添加对调试有用的其他信息到记录，如时间戳或错误消息。

:class:`Symfony\\Bridge\\Monolog\\Processor\\TokenProcessor`
    将当前用户令牌的信息添加到记录中，即用户名、角色以及用户是否经过身份验证。

:class:`Symfony\\Bridge\\Monolog\\Processor\\WebProcessor`
    使用Symfony的请求对象内部的数据覆盖请求中的数据。

:class:`Symfony\\Bridge\\Monolog\\Processor\\RouteProcessor`
    添加有关当前路由的信息（控制器、动作、路由参数）。

:class:`Symfony\\Bridge\\Monolog\\Processor\\ConsoleCommandProcessor`
    添加有关当前控制台命令的信息。

.. versionadded:: 4.3

    Symfony 4.3中引入了 ``RouteProcessor`` 和 ``ConsoleCommandProcessor``。

为每个处理器注册处理器
----------------------------------

你可以使用 ``monolog.processor`` 标签的 ``handler`` 选项为每个处理器注册一个Processor：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Logger\SessionRequestProcessor:
                tags:
                    - { name: monolog.processor, handler: main }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="App\Logger\SessionRequestProcessor">
                    <tag name="monolog.processor" handler="main"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php

        // ...
        $container
            ->register(SessionRequestProcessor::class)
            ->addTag('monolog.processor', ['handler' => 'main']);

为每个通道注册Processor
----------------------------------

你可以使用 ``monolog.processor`` 标签的 ``channel`` 选项为每个通道注册一个Processor：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Logger\SessionRequestProcessor:
                tags:
                    - { name: monolog.processor, channel: main }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="App\Logger\SessionRequestProcessor">
                    <tag name="monolog.processor" channel="main"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php

        // ...
        $container
            ->register(SessionRequestProcessor::class)
            ->addTag('monolog.processor', ['channel' => 'main']);
