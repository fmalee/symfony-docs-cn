.. index::
   single: Emails; Spooling

如何假脱机邮件
===================

Symfony邮件程序的默认行为是立即发送电子邮件。
但是，你可能希望避免与邮件服务器通信的性能损失，因为这可能导致邮件发送时用户需要等待下一页的加载。
选择“假脱机(spool)”邮件而不是直接发送邮件可以避免这种情况。

这使邮件程序不会尝试发送邮件消息，而是将其保存在某个位置，例如文件。
然后，另一个进程可以负责在假脱机中读取并发送电子邮件。目前仅支持假脱机到文件或内存。

.. _email-spool-memory:

使用内存假脱机
------------------

当你使用假脱机将邮件存储到内存时，它们将在内核终止之前发送。
这意味着只有在整个请求期间没有任何未处理的异常或错误的情况下才会发送邮件。
要配置此假脱机，请使用以下配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/swiftmailer.yaml
        swiftmailer:
            # ...
            spool: { type: memory }

    .. code-block:: xml

        <!-- config/packages/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config>
                <swiftmailer:spool type="memory"/>
            </swiftmailer:config>
        </container>

    .. code-block:: php

        // config/packages/swiftmailer.php
        $container->loadFromExtension('swiftmailer', [
            // ...
            'spool' => ['type' => 'memory'],
        ]);

.. _spool-using-a-file:

使用文件假脱机
------------------

当你使用文件系统进行假脱机时，Symfony会在每个邮件服务的给定路径中创建一个文件夹（例如，默认服务的“default”）。
此文件夹将包含假脱机中每封邮件的文件。因此，请确保Symfony（或你的 webserver/php）可以写入该目录！

要将假脱机与文件一起使用，请使用以下配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/swiftmailer.yaml
        swiftmailer:
            # ...
            spool:
                type: file
                path: /path/to/spooldir

    .. code-block:: xml

        <!-- config/packages/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config>
                <swiftmailer:spool
                    type="file"
                    path="/path/to/spooldir"
                />
            </swiftmailer:config>
        </container>

    .. code-block:: php

        // config/packages/swiftmailer.php
        $container->loadFromExtension('swiftmailer', [
            // ...

            'spool' => [
                'type' => 'file',
                'path' => '/path/to/spooldir',
            ],
        ]);

.. tip::

    如果要将假脱机存储在项目目录的某处，请记住可以使用 ``%kernel.project_dir%`` 参数来引用项目的根目录：

    .. code-block:: yaml

        path: '%kernel.project_dir%/var/spool'

现在，当你的应用发送邮件时，它实际上不会被发送，而是添加到假脱机中。
从假脱机发送消息是单独完成的。有一个控制台命令可以在假脱机中发送消息：

.. code-block:: terminal

    $ APP_ENV=prod php bin/console swiftmailer:spool:send

它有一个选项来限制要发送的消息数量：

.. code-block:: terminal

    $ APP_ENV=prod php bin/console swiftmailer:spool:send --message-limit=10

你还可以以秒为单位来设置时间限制：

.. code-block:: terminal

    $ APP_ENV=prod php bin/console swiftmailer:spool:send --time-limit=10

实际上，你不希望手动运行此操作。相反，控制台命令应由cron或计划任务触发，并以固定间隔运行。

.. caution::

    使用SwiftMailer创建一个消息时，它会生成一个 ``Swift_Message`` 类。
    如果swiftmailer服务是延迟加载的，它会生成一个名为 ``Swift_Message_<someRandomCharacters>`` 的代理类。

    如果使用内存假脱机，则此更改是透明的，不会产生任何影响。
    但是，在使用文件系统假脱机时，消息类将在具有随机类名的文件中序列化。
    问题是这个随机类名在每个缓存清除后都会改变。因此，如果你发送一个邮件然后清除了缓存，则该邮件将无法被反序列化。

    在下一次的 ``swiftmailer:spool:send`` 执行过程中会报错，因为该 ``Swift_Message_<someRandomCharacters>`` 类不(再)存在。

    解决方案是要么使用内存假脱机，要么在不带 ``lazy`` 选项的情况下加载 ``swiftmailer``
    服务（请参阅 :doc:`/service_container/lazy_services`）。
