.. index::
    single: DependencyInjection; Service definitions

如何使用服务定义对象
===========================================

服务定义是描述容器应如何构建一个服务的指令。它们不是你的应用使用的实际服务。
容器将根据定义中的配置来创建实际的类实例。

通常，你将使用YAML、XML或PHP来描述服务定义。
但是，如果你正在使用服务容器进行高级操作，例如使用
:doc:`编译器传递 </service_container/compiler_passes>` 或创建
:doc:`依赖注入扩展 </bundles/extension>`，则可能需要直接使用定义一个服务如何实例化的 ``Definition`` 对象。

获取和设置服务定义
---------------------------------------

有一些有用的方法来处理服务定义::

    use Symfony\Component\DependencyInjection\Definition;

    // 找出是否存在一个 “app.mailer” 定义
    $container->hasDefinition('app.mailer');
    // 找出是否存在一个 “app.mailer” 定义或别名
    $container->has('app.mailer');

    // 获取 "app.user_config_manager" 定义
    $definition = $container->getDefinition('app.user_config_manager');
    // 使用 "app.user_config_manager" ID 或别名来获取定义
    $definition = $container->findDefinition('app.user_config_manager');

    // 添加一个新的 "app.number_generator" 定义
    $definition = new Definition(\App\NumberGenerator::class);
    $container->setDefinition('app.number_generator', $definition);

    // 前一个方法的快捷方式
    $container->register('app.number_generator', \App\NumberGenerator::class);

定义的使用
-------------------------

创建新定义
~~~~~~~~~~~~~~~~~~~~~~~~~

除了操作和检索现有定义之外，你还可以使用
:class:`Symfony\\Component\\DependencyInjection\\Definition` 类来定义新的服务定义。

类
~~~~~

``Definition`` 类的第一个可选参数是从容器中获取服务时返回的对象的完全限定类名::

    use App\Config\CustomConfigManager;
    use App\Config\UserConfigManager;
    use Symfony\Component\DependencyInjection\Definition;

    $definition = new Definition(UserConfigManager::class);

    // 重写该类
    $definition->setClass(CustomConfigManager::class);

    // 获取为此定义而配置的类
    $class = $definition->getClass();

构造函数参数
~~~~~~~~~~~~~~~~~~~~~

``Definition`` 类的第二个可选参数是一个数组，其中的参数传递给从容器中获取服务时返回的对象的构造函数::

    use App\Config\DoctrineConfigManager;
    use Symfony\Component\DependencyInjection\Definition;
    use Symfony\Component\DependencyInjection\Reference;

    $definition = new Definition(DoctrineConfigManager::class, [
        new Reference('doctrine'), // 一个其他服务的引用
        '%app.config_table_name%',  // 将被解析为一个容器参数的值
    ]);

    // 获取为 此定义 配置的所有参数
    $constructorArguments = $definition->getArguments();

    // 获取一个特定的参数
    $firstArgument = $definition->getArgument(0);

    // 添加一个新参数
    $definition->addArgument($argument);

    // 替换一个特定索引的参数（0=第一个参数）
    $definition->replaceArgument($index, $argument);

    // 用被传递的数组替换所有先前配置的参数
    $definition->setArguments($arguments);

.. caution::

    不要使用 ``get()`` 获取一个要作为构造函数参数注入的服务，该服务尚不可用。
    相反，使用如上所示的一个 ``Reference`` 实例。

方法调用
~~~~~~~~~~~~

如果你使用的服务使用setter注入，那么你也可以操作定义中的任何方法调用::

    // 获取所有已配置的方法调用
    $methodCalls = $definition->getMethodCalls();

    // 配置一个新的方法调用
    $definition->addMethodCall('setLogger', [new Reference('logger')]);

    // 用被传递的数组替换所有先前配置的方法调用
    $definition->setMethodCalls($methodCalls);

.. tip::

    在服务容器文档的PHP代码块中有更多使用定义的特定方法示例，例如
    :doc:`/service_container/factories` 以及 :doc:`/service_container/parent_services`。

.. note::

    这些更改服务定义的方法只能在容器被编译之前使用。在容器编译完成后，你将无法进一步操作服务定义。
    要了解有关编译容器的更多信息，请参阅 :doc:`/components/dependency_injection/compilation`。

引入文件
~~~~~~~~~~~~~~~

可能存在一种用例，那就是你可能需要在服务本身加载之前引入另一个文件。
为此，你可以使用
:method:`Symfony\\Component\\DependencyInjection\\Definition::setFile` 方法::

    $definition->setFile('/src/path/to/file/foo.php');

请注意，Symfony将在内部调用PHP的 ``require_once`` 语句，这意味着每个请求只包含(include)一次你的文件。
