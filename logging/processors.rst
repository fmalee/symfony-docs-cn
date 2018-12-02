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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="monolog.formatter.session_request"
                    class="Monolog\Formatter\LineFormatter">

                    <argument>[%%datetime%%] [%%extra.token%%] %%channel%%.%%level_name%%: %%message%% %%context%% %%extra%%&#xA;</argument>
                </service>

                <service id="App\Logger\SessionRequestProcessor">
                    <tag name="monolog.processor" />
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
            ->addTag('monolog.processor', array('method' => 'processRecord'));

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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

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
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'main' => array(
                    'type'      => 'stream',
                    'path'      => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level'     => 'debug',
                    'formatter' => 'monolog.formatter.session_request',
                ),
            ),
        ));

如果使用了多个处理器，还可以在处理器级别或通道级别注册一个Processor，而不是全局注册它（请参阅以下部分）。

.. tip::

    .. versionadded:: 2.4
        在Monolog bundle 2.4中引入了Monolog processor的自动配置。

    如果你使用
    :ref:`默认的services.yaml配置 <service-container-services-load-example>`，则实现了
    :class:`Monolog\\Processor\\ProcessorInterface`
    的Processor会自动注册为服务并标记为
    ``monolog.processor``，因此你可以在不添加任何配置的情况下使用它们。
    这同样适用于内置的 :class:`Symfony\\Bridge\\Monolog\\Processor\\TokenProcessor` 和
    :class:`Symfony\\Bridge\\Monolog\\Processor\\WebProcessor`
    Processor，它们可以按如下方式启用：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            services:
                # 将当前安全令牌添加到日志项
                Symfony\Bridge\Monolog\Processor\TokenProcessor: ~
                # 将实际客户端IP添加到日志项
                Symfony\Bridge\Monolog\Processor\WebProcessor: ~

        .. code-block:: xml

            <!-- config/services.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <services>
                    <!-- Adds the current security token to log entries -->
                    <service id="Symfony\Bridge\Monolog\Processor\TokenProcessor" />
                    <!-- Adds the real client IP to log entries -->
                    <service id="Symfony\Bridge\Monolog\Processor\WebProcessor" />
                </services>
            </container>

        .. code-block:: php

            // config/services.php
            use Symfony\Bridge\Monolog\Processor\TokenProcessor;
            use Symfony\Bridge\Monolog\Processor\WebProcessor;

            // Adds the current security token to log entries
            $container->register(TokenProcessor::class);
            // Adds the real client IP to log entries
            $container->register(WebProcessor::class);

为每个处理器注册Processor
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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="App\Logger\SessionRequestProcessor">
                    <tag name="monolog.processor" handler="main" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php

        // ...
        $container
            ->register(SessionRequestProcessor::class)
            ->addTag('monolog.processor', array('handler' => 'main'));

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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <services>
                <service id="App\Logger\SessionRequestProcessor">
                    <tag name="monolog.processor" channel="main" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php

        // ...
        $container
            ->register(SessionRequestProcessor::class)
            ->addTag('monolog.processor', array('channel' => 'main'));
