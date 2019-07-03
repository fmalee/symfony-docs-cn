.. index::
    single: Using Parameters within a Dependency Injection Class

在依赖注入类中使用参数
----------------------------------------------------

你已经了解了如何在 :ref:`Symfony服务容器 <service-container-parameters>` 中使用配置参数。
但有些特殊情况，例如需要使用 ``%kernel.debug%`` 参数来使Bundle中的服务进入调试模式。
对于这种情况，还有更多工作要做，以使系统理解参数值。
默认情况下，你的 ``%kernel.debug%`` 参数将被视为一个简单的字符串。
请考虑以下示例::

    // 在配置类内部
    $rootNode
        ->children()
            ->booleanNode('logging')->defaultValue('%kernel.debug%')->end()
            // ...
        ->end()
    ;

    // 在扩展类内部
    $config = $this->processConfiguration($configuration, $configs);
    var_dump($config['logging']);

现在，检查结果以便仔细观察：

.. configuration-block::

    .. code-block:: yaml

        my_bundle:
            logging: true
            # true, 正如预期的那样

        my_bundle:
            logging: '%kernel.debug%'
            # true/false（取决于Kernel类的第二个参数），正如预期的那样，
            # 因为配置中的 ％kernel.debug％ 在传递给扩展之前得到评估(evaluated)

        my_bundle: ~
        # 传递 "%kernel.debug%" 字符串。
        # 该字符串始终被认为是 true。
        # 配置器对作为参数的 “％kernel.debug％” 一无所知。

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:my-bundle="http://example.org/schema/dic/my_bundle">

            <my-bundle:config logging="true"/>
            <!-- true, as expected -->

            <my-bundle:config logging="%kernel.debug%"/>
            <!-- true/false (depends on 2nd parameter of Kernel),
                 as expected, because %kernel.debug% inside configuration
                 gets evaluated before being passed to the extension -->

            <my-bundle:config/>
            <!-- passes the string "%kernel.debug%".
                 Which is always considered as true.
                 The Configurator does not know anything about
                 "%kernel.debug%" being a parameter. -->
        </container>

    .. code-block:: php

        $container->loadFromExtension('my_bundle', [
                'logging' => true,
                // true, as expected
            )
        ];

        $container->loadFromExtension('my_bundle', [
                'logging' => "%kernel.debug%",
                // true/false (depends on 2nd parameter of Kernel),
                // as expected, because %kernel.debug% inside configuration
                // gets evaluated before being passed to the extension
            )
        ];

        $container->loadFromExtension('my_bundle');
        // passes the string "%kernel.debug%".
        // Which is always considered as true.
        // The Configurator does not know anything about
        // "%kernel.debug%" being a parameter.
为了支持这个用例，``Configuration`` 类必须通过扩展注入此参数，如下所示::

    namespace App\DependencyInjection;

    use Symfony\Component\Config\Definition\Builder\TreeBuilder;
    use Symfony\Component\Config\Definition\ConfigurationInterface;

    class Configuration implements ConfigurationInterface
    {
        private $debug;

        public function  __construct($debug)
        {
            $this->debug = (bool) $debug;
        }

        public function getConfigTreeBuilder()
        {
            $treeBuilder = new TreeBuilder('my_bundle');

            $treeBuilder->getRootNode()
                ->children()
                    // ...
                    ->booleanNode('logging')->defaultValue($this->debug)->end()
                    // ...
                ->end()
            ;

            return $treeBuilder;
        }
    }

通过 ``Extension`` 类在 ``Configuration`` 的构造函数中设置它::

    namespace App\DependencyInjection;

    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\DependencyInjection\Extension;

    class AppExtension extends Extension
    {
        // ...

        public function getConfiguration(array $config, ContainerBuilder $container)
        {
            return new Configuration($container->getParameter('kernel.debug'));
        }
    }

.. tip::

    在 ``Configurator`` 类中有一些 ``%kernel.debug%`` 使用实例，比如在TwigBundle中。
    但是，这是因为默认参数值是由Extension类设置的。
