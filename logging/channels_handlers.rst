.. index::
   single: Logging

如何将日志信息写入不同的文件
======================================

Symfony框架将日志消息组织到通道中。
默认有几种渠道，包括 ``doctrine``、``event``、``security``、``request`` 以及更多。
通道被打印在日志消息中，也可用于将不同的通道定向到不同的位置/文件。

默认情况下，Symfony将每条消息记录到单个文件中（无论什么通道）。

.. note::

    每个通道都对应于容器中的一个日志服务（``monolog.logger.XXX``），并且可以将它们注入到不同的服务中。
    可以使用 ``debug:container`` 命令来查看完整列表。

.. _logging-channel-handler:

切换一个通道到其他处理器
------------------------------------------

现在，假设你想要将 ``security`` 通道记录到其他文件。
为此，请创建一个新处理器并将其配置为仅记录来自 ``security`` 通道的消息：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                security:
                    # 记录所有消息（因为 debug 是最低级别）
                    level:    debug
                    type:     stream
                    path:     '%kernel.logs_dir%/security.log'
                    channels: [security]

                # *不* 记录安全通道的消息的处理器的示例
                main:
                    # ...
                    # channels: ['!security']

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml-->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler name="security" type="stream" path="%kernel.logs_dir%/security.log">
                    <monolog:channels>
                        <monolog:channel>security</monolog:channel>
                    </monolog:channels>
                </monolog:handler>

                <monolog:handler name="main" type="stream" path="%kernel.logs_dir%/main.log">
                    <!-- ... -->
                    <monolog:channels>
                        <monolog:channel>!security</monolog:channel>
                    </monolog:channels>
                </monolog:handler>
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', [
            'handlers' => [
                'security' => [
                    'type'     => 'stream',
                    'path'     => '%kernel.logs_dir%/security.log',
                    'channels' => [
                        'security',
                    ],
                ],
                'main'     => [
                    // ...
                    'channels' => [
                        '!security',
                    ],
                ],
            ],
        ]);

.. caution::

    ``channels`` 配置仅适用于顶级处理器。
    嵌套在 ``group``、``buffer``、``filter``、``fingers crossed``
    或其他此类处理器中的处理器将忽略此配置，然后所有消息都会被传递给它们。

YAML规范
------------------

你可以通过多种形式来指定配置：

.. code-block:: yaml

    channels: ~    # 包含所有通道

    channels: foo  # 仅包含 'foo' 通道
    channels: '!foo' # 包含所有通道，除了 'foo'

    channels: [foo, bar]   # 仅包含 'foo' 和 'bar' 两个通道
    channels: ['!foo', '!bar'] # 包含所有通道，除了 'foo' 和 'bar'

创建自己的通道
-------------------------

你可以每次修改一个monolog的日志通道，将其指向一个服务。
这可以通过以下 :ref:`配置 <monolog-channels-config>` 完成，也可以使用
:ref:`monolog.logger<dic_tags-monolog>` 来标记你的服务并指定该服务应该记录到哪个通道。
使用标签，注入到服务的记录器已被预先配置为使用你指定的通道。

.. _monolog-channels-config:

不标记服务来配置额外通道
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以配置额外的通道，而无需标记你的服务：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            channels: ['foo', 'bar']

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:channel>foo</monolog:channel>
                <monolog:channel>bar</monolog:channel>
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', [
            'channels' => [
                'foo',
                'bar',
            ],
        ]);

Symfony自动为每个通道注册一个服务（在此示例中，``foo`` 通道创建了一个名为 ``monolog.logger.foo`` 的服务）。
要将此服务注入其他服务，你必须更新服务配置以 :ref:`选择要注入的特定服务 <services-wire-specific-service>`。
