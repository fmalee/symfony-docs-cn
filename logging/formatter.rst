如何自定义一个日志格式化器
========================================

每个日志处理器在记录之前都使用一个 ``Formatter`` 来格式化日志。
所有Monolog处理器都默认使用一个 ``Monolog\Formatter\LineFormatter`` 实例，但你可以替换它。
你的格式化器必须实现 ``Monolog\Formatter\FormatterInterface``。

例如，要使用内置 ``JsonFormatter`` 函数，请将其注册为服务，然后配置你的处理器以使用它：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            Monolog\Formatter\JsonFormatter: ~

        # config/packages/prod/monolog.yaml (和/或 config/packages/dev/monolog.yaml)
        monolog:
            handlers:
                file:
                    type: stream
                    level: debug
                    formatter: Monolog\Formatter\JsonFormatter

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
                <service id="Monolog\Formatter\JsonFormatter"/>
            </services>

            <!-- config/packages/prod/monolog.xml (and/or config/packages/dev/monolog.xml) -->
            <monolog:config>
                <monolog:handler
                    name="file"
                    type="stream"
                    level="debug"
                    formatter="Monolog\Formatter\JsonFormatter"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // config/services.php
        use Monolog\Formatter\JsonFormatter;

        $container->register(JsonFormatter::class);

        // config/packages/prod/monolog.php (and/or config/packages/dev/monolog.php)
        $container->loadFromExtension('monolog', [
            'handlers' => [
                'file' => [
                    'type'      => 'stream',
                    'level'     => 'debug',
                    'formatter' => JsonFormatter::class,
                ],
            ],
        ]);
