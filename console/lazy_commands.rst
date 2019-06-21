如何使命令延迟加载
==================================

.. note::

    如果你使用的是Symfony全栈框架，
    那么你可能正在寻找有关 :ref:`创建延迟命令 <console-command-service-lazy-loading>` 的详细信息

向应用添加命令的传统方法是使用 :method:`Symfony\\Component\\Console\\Application::add`，
该方法会将 ``Command`` 实例作为参数。

为了延迟加载命令，你需要注册一个负责返回 ``Command`` 实例的中间加载器::

    use App\Command\HeavyCommand;
    use Symfony\Component\Console\Application;
    use Symfony\Component\Console\CommandLoader\FactoryCommandLoader;

    $commandLoader = new FactoryCommandLoader([
        'app:heavy' => function () { return new HeavyCommand(); },
    ]);

    $application = new Application();
    $application->setCommandLoader($commandLoader);
    $application->run();

通过这个方式，``HeavyCommand`` 实例只有在 ``app:heavy`` 命令被实际调用时才会创建。

此示例使用内置的 :class:`Symfony\\Component\\Console\\CommandLoader\\FactoryCommandLoader` 类，
但 :method:`Symfony\\Component\\Console\\Application::setCommandLoader` 方法接受任何
:class:`Symfony\\Component\\Console\\CommandLoader\\CommandLoaderInterface` 实例，
因此你可以使用自己的实现。

内置命令加载器
------------------------

``FactoryCommandLoader``
~~~~~~~~~~~~~~~~~~~~~~~~

:class:`Symfony\\Component\\Console\\CommandLoader\\FactoryCommandLoader`
类提供一个延迟加载命令的方法：它使用一个 ``Command`` 工厂数组作为其唯一的构造函数参数::

    use Symfony\Component\Console\CommandLoader\FactoryCommandLoader;

    $commandLoader = new FactoryCommandLoader([
        'app:foo' => function () { return new FooCommand(); },
        'app:bar' => [BarCommand::class, 'create'],
    ]);

工厂可以是任何可调用的PHP，并且每次调用时都会执行
:method:`Symfony\\Component\\Console\\CommandLoader\\FactoryCommandLoader::get`。

``ContainerCommandLoader``
~~~~~~~~~~~~~~~~~~~~~~~~~~

:class:`Symfony\\Component\\Console\\CommandLoader\\ContainerCommandLoader`
类可用于从PSR-11容器中加载命令。
因此，它的构造函数将一个 PSR-11 ``ContainerInterface`` 实现作为其第一个参数，将一个命令映射作为其最后一个参数。
该命令映射必须是一个命令名称为键、服务标识为值的数组::

    use Symfony\Component\Console\CommandLoader\ContainerCommandLoader;
    use Symfony\Component\DependencyInjection\ContainerBuilder;

    $containerBuilder = new ContainerBuilder();
    $containerBuilder->register(FooCommand::class, FooCommand::class);
    $containerBuilder->compile();

    $commandLoader = new ContainerCommandLoader($containerBuilder, [
        'app:foo' => FooCommand::class,
    ]);

像这样，执行 ``app:foo`` 命令将通过调用 ``$containerBuilder->get(FooCommand::class)``
来加载 ``FooCommand`` 服务。
