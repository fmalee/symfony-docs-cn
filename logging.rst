日志
=======

Symfony配备了极简主义的 `PSR-3`_ 日志器：:class:`Symfony\\Component\\HttpKernel\\Log\\Logger`。
根据 `十二因素应用方法论`_，它将从 ``WARNING`` 级别开始向 `stderr`_ 发送消息。

可以通过设置 ``SHELL_VERBOSITY`` 环境变量来更改最小日志级别：

=========================  =================
``SHELL_VERBOSITY`` 值     最小日志级别
=========================  =================
``-1``                     ``ERROR``
``1``                      ``NOTICE``
``2``                      ``INFO``
``3``                      ``DEBUG``
=========================  =================

通过将适当的参数传递给 :class:`Symfony\\Component\\HttpKernel\\Log\\Logger` 的构造函数，
也可以更改最小日志级别、默认输出和日志格式。
为此，请 :ref:`重写"logger"服务的定义 <service-psr4-loader>`。

记录信息
-----------------

要记录消息，请在控制器中注入默认日志器::

    use Psr\Log\LoggerInterface;

    public function index(LoggerInterface $logger)
    {
        $logger->info('I just got the logger');
        $logger->error('An error occurred');

        $logger->critical('I left the oven on!', array(
            // 在日志中包含额外的“上下文”信息
            'cause' => 'in_hurry',
        ));

        // ...
    }

``logger`` 服务针对不同的日志级别/优先级具有不同的方法。
请参阅 LoggerInterface_ 以获取日志器上所有方法的列表。

Monolog
-------

Symfony与最流行的PHP日志库 `Monolog`_ 无缝集成，可以在各种不同的位置创建和存储日志消息，并触发各种操作。

例如，你可以使用Monolog将日志器配置为根据消息级别执行不同的操作
（例如，:doc:`在发生错误时发送邮件 </logging/monolog_email>`）。

运行此命令以在使用之前安装基于Monolog的日志器：

.. code-block:: terminal

    $ composer require symfony/monolog-bundle

以下章节皆假定你已安装Monolog。

日志储存在哪里
---------------------

默认情况下，当你处于 ``dev`` 环境中时，会将日志写入 ``var/log/dev.log`` 文件。
在 ``prod`` 环境中，日志被写入 ``var/log/prod.log``，但只记录在请求期间发生的错误(或高优先级）的日志
（即 ``error()``、``critical()``、``alert()`` 或 ``emergency()``）。

想控制该行为，你可以配置不同的\* 处理器*\来处理日志条目，有时会修改它们，并最终存储它们。

处理器: 将日志写入不同的位置
---------------------------------------------

日志器具有一堆\ *处理器*，每个处理器可用于将日志写入不同的位置（例如文件，数据库，Slack等）。

.. tip::

    你\ *还*\可以配置类似于分类的日志“通道”。每个通道都可以拥有\ *自己*\的处理器，
    这意味着你可以在不同的位置存储不同的日志。请参阅 :doc:`/logging/channels_handlers`。

Symfony在默认的 ``monolog.yaml`` 配置文件中预配置了一些基础处理器。查看一下这些实际例子。

此示例使用\ *两个*\处理器：``stream`` （写入文件）和使用 :phpfunction:`syslog` 函数的 ``syslog`` 来写入日志：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                # 该 "file_log" 键可以是任何东西
                file_log:
                    type: stream
                    # 记录到 var/log/(environment).log
                    path: "%kernel.logs_dir%/%kernel.environment%.log"
                    # 记录 *所有* 日志 (debug 是最低级别)
                    level: debug

                syslog_handler:
                    type: syslog
                    # 记录 error 及跟高级别的日志
                    level: error

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
                    name="file_log"
                    type="stream"
                    path="%kernel.logs_dir%/%kernel.environment%.log"
                    level="debug"
                />
                <monolog:handler
                    name="syslog_handler"
                    type="syslog"
                    level="error"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'file_log' => array(
                    'type'  => 'stream',
                    'path'  => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level' => 'debug',
                ),
                'syslog_handler' => array(
                    'type'  => 'syslog',
                    'level' => 'error',
                ),
            ),
        ));

这里定义了一\ *堆*\处理器，每个处理器按照它定义的顺序调用。

编辑日志记录的处理器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*有些*\处理器不是记录日志到某个文件，而是用于在将日志发送给\ *其他*\处理器之前过滤或修改日志。
默认情况下，在 ``prod`` 环境中使用一个名为 ``fingers_crossed`` 的强大内置处理器。
它在请求期间存储\ *所有*\日志消息，但\ *只*\有在其中一条消息到达 ``action_level`` 时才将它们传递给第二个处理器。
举个例子：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                filter_for_errors:
                    type: fingers_crossed
                    # 如果有 *一条* 日志是 error 或更高级别，传递 *所有* 日志到 file_log
                    action_level: error
                    handler: file_log

                # 现在忽略(passed) *所有* 日志，仅当一个日志是 error 或更高时记录
                file_log:
                    type: stream
                    path: "%kernel.logs_dir%/%kernel.environment%.log"

                # 还是忽略 *所有* 日志，仍然只记录 error 或更高级别的日志
                syslog_handler:
                    type: syslog
                    level: error

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
                    name="filter_for_errors"
                    type="fingers_crossed"
                    action-level="error"
                    handler="file_log"
                />
                <monolog:handler
                    name="file_log"
                    type="stream"
                    path="%kernel.logs_dir%/%kernel.environment%.log"
                    level="debug"
                />
                <monolog:handler
                    name="syslog_handler"
                    type="syslog"
                    level="error"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'filter_for_errors' => array(
                    'type'         => 'fingers_crossed',
                    'action_level' => 'error',
                    'handler'      => 'file_log',
                ),
                'file_log' => array(
                    'type'  => 'stream',
                    'path'  => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level' => 'debug',
                ),
                'syslog_handler' => array(
                    'type'  => 'syslog',
                    'level' => 'error',
                ),
            ),
        ));

现在，一旦一个日志具有 ``error`` 或更高级别，
那么该请求的所有日志信息都将通过 ``file_log`` 处理器保存到文件中。
这意味着你的日志文件将包含着出现问题的请求的\ *所有*\详细信息 - 这样调试将会容易！

.. tip::

    名为 “file_log” 的处理器不会包含在堆栈本身中，因为它被用作 ``fingers_crossed`` 处理器的嵌套处理器。

.. note::

    如果想要使用另一个配置文件来覆盖 ``monolog`` 的配置，则需要重新定义整个 ``handlers`` 堆栈。
    两个文件中的配置无法合并，因为配置的顺序很重要，而合并无法控制排序。

所有的内置处理器
---------------------

Monolog附带了\ *许多*\ 内置处理器，可用于邮寄日志、将它们发送给Loggly、或者通过Slack通知你。
这些都记录在MonologBu​​ndle本身内部。有关完整列表，请参阅 `Monolog配置`_。

如何切割你的日志文件
----------------------------

随着时间的推移，日志文件在开发和生产时都会变得非常\ *庞大*。
一种最佳实践解决方案是使用 `logrotate`_ Linux命令之类的工具在日志文件变得太大之前切割(rotate)它们。

另一个选择是让Monolog使用 ``rotating_file`` 处理器为你切割文件。
该处理器每​​天创建一个新的日志文件，也可以自动删除旧文件。
要使用它，只需将处理器的 ``type`` 选项设置为 ``rotating_file``：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                main:
                    type:  rotating_file
                    path:  '%kernel.logs_dir%/%kernel.environment%.log'
                    level: debug
                    # 如果日志文件最大数量保持为默认的 0，则意味着不限制文件数量。
                    max_files: 10

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
                <!-- "max_files": max number of log files to keep
                     defaults to zero, which means infinite files -->
                <monolog:handler name="main"
                    type="rotating_file"
                    path="%kernel.logs_dir%/%kernel.environment%.log"
                    level="debug"
                    max-files="10"
                />
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'main' => array(
                    'type'  => 'rotating_file',
                    'path'  => '%kernel.logs_dir%/%kernel.environment%.log',
                    'level' => 'debug',
                    // max number of log files to keep
                    // defaults to zero, which means infinite files
                    'max_files' => 10,
                ),
            ),
        ));

在服务中使用日志
-------------------------------

如果你希望使用自己的服务中的有特定通道（默认情况下是 ``app``）的预配置日志器，
请如 :ref:`依赖注入参考 <dic_tags-monolog>` 中所阐述的那样使用 ``monolog.logger`` 标记并附带 ``channel`` 属性。

向每个日志添加额外数据（例如，唯一的请求令牌）
-----------------------------------------------------------

Monolog还支持 *processors*：可以动态地向日志消息添加额外信息的功能。

参阅 :doc:`/logging/processors` 获得详细信息.

扩展阅读
----------

.. toctree::
    :maxdepth: 1

    logging/monolog_email
    logging/channels_handlers
    logging/formatter
    logging/processors
    logging/monolog_exclude_http_codes
    logging/monolog_console

.. toctree::
    :hidden:

    logging/monolog_regex_based_excludes

.. _`十二因素应用方法论`: https://12factor.net/logs
.. _PSR-3: https://www.php-fig.org/psr/psr-3/
.. _`stderr`: https://en.wikipedia.org/wiki/Standard_streams#Standard_error_(stderr)
.. _Monolog: https://github.com/Seldaek/monolog
.. _LoggerInterface: https://github.com/php-fig/log/blob/master/Psr/Log/LoggerInterface.php
.. _`logrotate`: https://github.com/logrotate/logrotate
.. _`Monolog配置`: https://github.com/symfony/monolog-bundle/blob/master/DependencyInjection/Configuration.php#L25
