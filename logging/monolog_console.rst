.. index::
   single: Logging; Console messages

如何配置Monolog显示命令台信息
====================================================

当执行一个命令时，可以使用被传递的
:class:`Symfony\\Component\\Console\\Output\\OutputInterface`
实例来控制某些 :doc:`冗余度级别 </console/verbosity>` 消息的打印。

当产生大量的记录时，打印那些依赖于冗余度设置（
``-v``, ``-vv``, ``-vvv``）的消息就会变得很麻烦，因为调用都需要被封装在条件中。例如::

    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        if ($output->isDebug()) {
            $output->writeln('Some info');
        }

        if ($output->isVerbose()) {
            $output->writeln('Some more info');
        }
    }


`MonologBridge`_ 不是使用这些语义方法来测试每个冗余度的级别，而是提供了一个
`ConsoleHandler`_，它监听控制台事件并根据当前日志级别和控制台冗余度将日志消息写入到控制台输出。

然后可以将上面的示例重写为::

    use Psr\Log\LoggerInterface;
    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;
    // ...

    class YourCommand extends Command
    {
        private $logger;

        public function __construct(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $this->logger->debug('Some info');
            // ...
            $this->logger->notice('Some more info');
        }
    }

根据运行命令的冗余度级别和用户的配置（见下文），这些消息可能会也可能不会显示在控制台上。
如果显示它们，则会对它们加上时间戳并进行适当的着色。
此外，错误日志将写入到错误输出（php://stderr）。
无需再有条件地处理冗余度设置。

Monolog的控制台处理器默认已启用：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/dev/monolog.yaml
        monolog:
            handlers:
                # ...
                console:
                    type:   console
                    process_psr_3_messages: false
                    channels: ['!event', '!doctrine', '!console']

                    # 可以有选择的配置冗余度级别和日志级别之间的映射
                    # verbosity_levels:
                    #     VERBOSITY_NORMAL: NOTICE

    .. code-block:: xml

        <!-- config/packages/dev/monolog.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <monolog:config>
                <!-- ... -->

                <monolog:handler name="console" type="console" process-psr-3-messages="false">
                    <monolog:channels>
                        <monolog:channel>!event</monolog:channel>
                        <monolog:channel>!doctrine</monolog:channel>
                        <monolog:channel>!console</monolog:channel>
                    </monolog:channels>
                </monolog:handler>
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/dev/monolog.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'console' => array(
                   'type' => 'console',
                   'process_psr_3_messages' => false,
                   'channels' => array('!event', '!doctrine', '!console'),
                ),
            ),
        ));

现在，日志消息将根据日志级别和冗余度显示在控制台上。
默认情况下（正常冗余度级别），将显示警告及更高级别。但在
:doc:`完整冗余度模式 </console/verbosity>` 下，将显示所有消息。

.. _ConsoleHandler: https://github.com/symfony/MonologBridge/blob/master/Handler/ConsoleHandler.php
.. _MonologBridge: https://github.com/symfony/MonologBridge
