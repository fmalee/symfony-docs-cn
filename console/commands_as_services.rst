.. index::
    single: Console; Commands as Services

如何将命令定义为服务
==================================

如果你使用的是 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么你的命令类已经注册为服务。非常好！这是推荐的设置。

.. note::

    你还可以通过配置服务并将其 :doc:`标记 </service_container/tags>` 为 ``console.command``
    来手动将命令注册为服务。

例如，假设你要在命令中记录某些内容::

    namespace App\Command;

    use Psr\Log\LoggerInterface;
    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    class SunshineCommand extends Command
    {
        private $logger;

        public function __construct(LoggerInterface $logger)
        {
            $this->logger = $logger;

            // 你 *必须* 调用其父构造函数
            parent::__construct();
        }

        protected function configure()
        {
            $this
                ->setName('app:sunshine')
                ->setDescription('Good morning!');
        }

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $this->logger->info('Waking up the sun');
            // ...
        }
    }

如果你使用的是 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
则该命令类将自动注册为服务并被传递 ``$logger`` 参数（得益于自动装配）。
换句话说，*只要* 创建这个类，就完工了！你可以调用 ``app:sunshine`` 命令并开始记录。

.. caution::

    你 *确实* 可以在 ``configure()`` 中访问服务。
    但是，如果你的命令不是 :ref:`惰性的 <console-command-service-lazy-loading>`，
    请避免在其中执行任何工作（例如，进行数据库查询），因为即使你使用执行控制台的其他命令，该代码也会运行。

.. note::

    在以前的Symfony版本中，你可以在继承
    :class:`Symfony\\Bundle\\FrameworkBundle\\Command\\ContainerAwareCommand`
    的命令类中通过 ``$this->getContainer()->get('SERVICE_ID')`` 来获取服务。
    但这在Symfony 4.2中已弃用，并在将来的Symfony版本中不起作用。

.. _console-command-service-lazy-loading:

延迟加载
------------

要使命令延迟加载，请定义其 ``$defaultName`` 静态属性::

    class SunshineCommand extends Command
    {
        protected static $defaultName = 'app:sunshine';

        // ...
    }

或者在服务定义中的 ``console.command`` 标签上设置 ``command`` 属性：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Command\SunshineCommand:
                tags:
                    - { name: 'console.command', command: 'app:sunshine' }
                # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Command\SunshineCommand">
                     <tag name="console.command" command="app:sunshine" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Command\SunshineCommand;
        //...

        $container
            ->register(SunshineCommand::class)
            ->addTag('console.command', array('command' => 'app:sunshine'))
        ;

.. note::

    如果命令定义了别名（使用 :method:`Symfony\\Component\\Console\\Command\\Command::getAliases` 方法），
    则必须为每个别名添加一个 ``console.command`` 标签。

仅此而已。无论如何，``SunshineCommand`` 只有在 ``app:sunshine`` 命令被实际调用时才会实例化。

.. note::

    在延迟时，你无需调用 ``setName()`` 来配置该命令。

.. caution::

    调用 ``list`` 命令将实例化所有命令，包括惰性命令。
