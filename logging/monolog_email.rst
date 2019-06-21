.. index::
   single: Logging; Emailing errors

如何配置Monolog来邮寄错误信息
========================================

可以将 Monolog_ 配置为在应用发生错误时发送一个电子邮件。
为此配置需要一些嵌套处理器，以避免收到太多电子邮件。
这个配置起初看起来很复杂，但每个处理器在分解后都相当直接明了。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                main:
                    type:         fingers_crossed
                    # 在 critical 级别记录500错误
                    action_level: critical
                    # 也可以记录 400 级别的错误 (但不包含 404):
                    # action_level: error
                    # excluded_404s:
                    #     - ^/
                    handler:      deduplicated
                deduplicated:
                    type:    deduplication
                    handler: swift
                swift:
                    type:       swift_mailer
                    from_email: 'error@example.com'
                    to_email:   'error@example.com'
                    # 或收件人列表
                    # to_email:   ['dev1@example.com', 'dev2@example.com', ...]
                    subject:    'An Error Occurred! %%message%%'
                    level:      debug
                    formatter:  monolog.formatter.html
                    content_type: text/html

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <!--
                500 errors are logged at the critical level,
                to also log 400 level errors (but not 404's):
                action-level="error"
                And add this child inside this monolog:handler
                <monolog:excluded-404>^/</monolog:excluded-404>
                -->
                <monolog:handler
                    name="main"
                    type="fingers_crossed"
                    action-level="critical"
                    handler="deduplicated"
                />
                <monolog:handler
                    name="deduplicated"
                    type="deduplication"
                    handler="swift"
                />
                <monolog:handler
                    name="swift"
                    type="swift_mailer"
                    from-email="error@example.com"
                    subject="An Error Occurred! %%message%%"
                    level="debug"
                    formatter="monolog.formatter.html"
                    content-type="text/html">

                    <monolog:to-email>error@example.com</monolog:to-email>

                    <!-- or list of recipients -->
                    <!--
                    <monolog:to-email>dev1@example.com</monolog:to-email>
                    <monolog:to-email>dev2@example.com</monolog:to-email>
                    ...
                    -->
                </monolog:handler>
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', [
            'handlers' => [
                'main' => [
                    'type'         => 'fingers_crossed',
                    // 500 errors are logged at the critical level
                    'action_level' => 'critical',
                    // to also log 400 level errors (but not 404's):
                    // 'action_level' => 'error',
                    // 'excluded_404s' => [
                    //     '^/',
                    // ],
                    'handler'      => 'deduplicated',
                ],
                'deduplicated' => [
                    'type'    => 'deduplication',
                    'handler' => 'swift',
                ],
                'swift' => [
                    'type'         => 'swift_mailer',
                    'from_email'   => 'error@example.com',
                    'to_email'     => 'error@example.com',
                    // or a list of recipients
                    // 'to_email'   => ['dev1@example.com', 'dev2@example.com', ...],
                    'subject'      => 'An Error Occurred! %%message%%',
                    'level'        => 'debug',
                    'formatter'    => 'monolog.formatter.html',
                    'content_type' => 'text/html',
                ],
            ],
        ]);

``main`` 处理器是一个 ``fingers_crossed``
处理器，这意味着它只在指定的动作级别才会触发，在这个例子是 ``critical``。
``critical`` 级别仅在5xx HTTP代码错误时才触发。
此级别一旦触发，则 ``fingers_crossed`` 处理器将记录所有消息，而不管其级别如何。
而 ``handler`` 设置意味着该输出随后被传递到 ``deduplicated`` 处理器。

.. tip::

    如果你想要400级和500级错误来触发一个邮件，请设置 ``action_level`` 为
    ``error``，而不是 ``critical``。有关示例，请参阅上面的代码。

``deduplicated`` 处理器保留着一个请求的所有消息，然后一次性将它们传递到嵌套处理器，
但前提是这些记录在一个给定时间段内（默认为60秒）是唯一的。如果给定时间段内有重复记录，则将其丢弃。
添加此处理器可将通知量减少到可管理级别，特别是在发生严重故障的情况下。
你可以使用 ``time`` 选项来调整该时间段：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                # ...
                deduplicated:
                    type: deduplication
                    # 丢弃重复项的时间（以秒为单位）（默认值：60）
                    time: 10
                    handler: swift

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml -->

        <!-- time: the time in seconds during which duplicate entries are discarded (default: 60) -->
        <monolog:handler name="deduplicated"
            type="deduplication"
            time="10"
            handler="swift"/>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', [
            'handlers' => [
                // ...
                'deduplicated' => [
                    'type'    => 'deduplication',
                    // the time in seconds during which duplicate entries are discarded (default: 60)
                    'time' => 10,
                    'handler' => 'swift',
                ],
            ],
        ]);

然后将消息传递给 ``swift`` 处理器。这是实际处理邮寄错误消息的处理器。
对此功能的设置很简单：来往地址、格式化器、内容类型和主题。

你可以将这些处理器与其他处理器结合使用，以便仍在服务器上记录错误的同时发送邮件：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                main:
                    type:         fingers_crossed
                    action_level: critical
                    handler:      grouped
                grouped:
                    type:    group
                    members: [streamed, deduplicated]
                streamed:
                    type:  stream
                    path:  '%kernel.logs_dir%/%kernel.environment%.log'
                    level: debug
                deduplicated:
                    type:    deduplication
                    handler: swift
                swift:
                    type:         swift_mailer
                    from_email:   'error@example.com'
                    to_email:     'error@example.com'
                    subject:      'An Error Occurred! %%message%%'
                    level:        debug
                    formatter:    monolog.formatter.html
                    content_type: text/html

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler
                    name="main"
                    type="fingers_crossed"
                    action_level="critical"
                    handler="grouped"
                />
                <monolog:handler
                    name="grouped"
                    type="group"
                >
                    <member type="stream"/>
                    <member type="deduplicated"/>
                </monolog:handler>
                <monolog:handler
                    name="stream"
                    path="%kernel.logs_dir%/%kernel.environment%.log"
                    level="debug"
                />
                <monolog:handler
                    name="deduplicated"
                    type="deduplication"
                    handler="swift"
                />
                <monolog:handler
                    name="swift"
                    type="swift_mailer"
                    from-email="error@example.com"
                    subject="An Error Occurred! %%message%%"
                    level="debug"
                    formatter="monolog.formatter.html"
                    content-type="text/html">

                    <monolog:to-email>error@example.com</monolog:to-email>

                    <!-- or list of recipients -->
                    <!--
                    <monolog:to-email>dev1@example.com</monolog:to-email>
                    <monolog:to-email>dev2@example.com</monolog:to-email>
                    ...
                    -->
                </monolog:handler>
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', [
            'handlers' => [
                'main' => [
                    'type'         => 'fingers_crossed',
                    'action_level' => 'critical',
                    'handler'      => 'grouped',
                ],
                'grouped' => [
                    'type'    => 'group',
                    'members' => ['streamed', 'deduplicated'],
                ],
                'streamed'  => [
                    'type'  => 'stream',
                    'path'  => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level' => 'debug',
                ],
                'deduplicated' => [
                    'type'     => 'deduplication',
                    'handler'  => 'swift',
                ],
                'swift' => [
                    'type'         => 'swift_mailer',
                    'from_email'   => 'error@example.com',
                    'to_email'     => 'error@example.com',
                    // or a list of recipients
                    // 'to_email'   => ['dev1@example.com', 'dev2@example.com', ...],
                    'subject'      => 'An Error Occurred! %%message%%',
                    'level'        => 'debug',
                    'formatter'    => 'monolog.formatter.html',
                    'content_type' => 'text/html',
                ],
            ],
        ]);

这里使用 ``group`` 处理器将消息发送给两个组成员，即 ``deduplicated`` 和 ``stream`` 处理器。
现在，消息将写入日志文件并同时通过电子邮件发送。

.. _Monolog: https://github.com/Seldaek/monolog
