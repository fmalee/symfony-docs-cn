.. index::
    single: DependencyInjection; Tags
    single: Service Container; Tags

如何使用服务标签
=============================

**服务标签** 是告诉Symfony或其他第三方bundle以某种特殊方式注册你的服务的方法。思考以下示例：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Twig\AppExtension:
                public: false
                tags: ['twig.extension']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Twig\AppExtension" public="false">
                    <tag name="twig.extension" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Twig\AppExtension;

        $container->register(AppExtension::class)
            ->setPublic(false)
            ->addTag('twig.extension');

标记为 ``twig.extension`` 标签的服务在TwigBundle初始化期间被收集，并作为扩展添加到Twig。

其他标签用于将你的服务集成到其他系统中。
有关核心Symfony Framework中可用的所有标签的列表，请查看 :doc:`/reference/dic_tags`。
其中每个都对你的服务产生不同的影响，许多标签可能需要额外的参数（不仅仅是 ``name`` 参数）。

**对于大多数用户而言，这是你需要了解的全部内容**。如果你想进一步了解如何创建自己的自定义标签，请继续阅读。

自动配置标签
--------------------

如果启用 :ref:`自动配置 <services-autoconfigure>`，则会自动为你应用某些标记。
就对于 ``twig.extension`` 标签来说：容器看到你的类继承 ``AbstractExtension`` （或更准确地说是实现了 ``ExtensionInterface``），然后以此给类添加上 ``twig.extension`` 标签。

如果要自动为自己的服务应用标签，请使用 ``_instanceof`` 选项：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # 此配置仅适用于此文件创建的服务
            _instanceof:
                # 是一个CustomInterface实例的类的服务将自动标记
                App\Security\CustomInterface:
                    tags: ['app.custom_tag']
            # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="utf-8"?>
        <container xmlns="http://symfony.com/schema/dic/services" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">
            <services>
                <!-- this config only applies to the services created by this file -->
                <instanceof id="App\Security\CustomInterface" autowire="true">
                    <!-- services whose classes are instances of CustomInterface will be tagged automatically -->
                    <tag name="app.custom_tag" />
                </instanceof>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Security\CustomInterface;
        // ...

        // services whose classes are instances of CustomInterface will be tagged automatically
        $container->registerForAutoconfiguration(CustomInterface::class)
            ->addTag('app.custom_tag')
            ->setAutowired(true);

对于更高级的需求，你可以从你的内核或在 :doc:`扩展 </bundles/extension>` 中使用
:method:`Symfony\\Component\\DependencyInjection\\ContainerBuilder::registerForAutoconfiguration`
方法来定义自动化的标签::

    // src/Kernel.php
    class Kernel extends BaseKernel
    {
        // ...

        protected function build(ContainerBuilder $container)
        {
            $container->registerForAutoconfiguration(CustomInterface::class)
                ->addTag('app.custom_tag')
            ;
        }
    }

创建自定义标签
--------------------

标签本身并不会以任何方式改变你的服务功能。
但是如果你愿意，你可以向容器构建器询问所有使用某些特定标签标记的服务的列表。
这在编译器传递中很有用，你可以在其中找到这些服务并以某种特定方式使用或修改它们。

例如，如果你使用的是Swift Mailer，你可能会想到要实现一个“传输链”，它是一个实现了
``\Swift_Transport`` 的类的集合。使用该链，你将希望Swift Mailer尝试多种传输消息的方法，直到成功为止。

首先，定义 ``TransportChain`` 类::

    // src/Mail/TransportChain.php
    namespace App\Mail;

    class TransportChain
    {
        private $transports;

        public function __construct()
        {
            $this->transports = array();
        }

        public function addTransport(\Swift_Transport $transport)
        {
            $this->transports[] = $transport;
        }
    }

然后，将该链定义为一个服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Mail\TransportChain: ~

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Mail\TransportChain" />
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mail\TransportChain;

        $container->autowire(TransportChain::class);

使用自定义标记定义服务
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

现在，你可能希望实例化几个 ``\Swift_Transport`` 类，并使用 ``addTransport()`` 方法自动添加到链中。
例如，你可以将以下传输添加为服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            Swift_SmtpTransport:
                arguments: ['%mailer_host%']
                tags: ['app.mail_transport']

            Swift_SendmailTransport:
                tags: ['app.mail_transport']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Swift_SmtpTransport">
                    <argument>%mailer_host%</argument>

                    <tag name="app.mail_transport" />
                </service>

                <service class="\Swift_SendmailTransport">
                    <tag name="app.mail_transport" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        $container->register(\Swift_SmtpTransport::class)
            ->addArgument('%mailer_host%')
            ->addTag('app.mail_transport');

        $container->register(\Swift_SendmailTransport::class)
            ->addTag('app.mail_transport');

请注意，每个服务都有一个名为 ``app.mail_transport`` 的标签。这是你将在编译器传递中使用的自定义标签。
编译器传递是使这个标签“意味着什么”的东西。

.. _service-container-compiler-pass-tags:

创建编译器传递
~~~~~~~~~~~~~~~~~~~~~~

你现在可以使用一个 :ref:`编译器传递 <components-di-separate-compiler-passes>`
向容器询问带有 ``app.mail_transport`` 标签的任何服务::

    // src/DependencyInjection/Compiler/MailTransportPass.php
    namespace App\DependencyInjection\Compiler;

    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\DependencyInjection\Compiler\CompilerPassInterface;
    use Symfony\Component\DependencyInjection\Reference;
    use App\Mail\TransportChain;

    class MailTransportPass implements CompilerPassInterface
    {
        public function process(ContainerBuilder $container)
        {
            // 始终首先检查是否定义了主要服务
            if (!$container->has(TransportChain::class)) {
                return;
            }

            $definition = $container->findDefinition(TransportChain::class);

            // 使用 app.mail_transport 标签查找所有的服务ID
            $taggedServices = $container->findTaggedServiceIds('app.mail_transport');

            foreach ($taggedServices as $id => $tags) {
                // 将每个传输服务添加到TransportChain服务
                $definition->addMethodCall('addTransport', array(new Reference($id)));
            }
        }
    }

使用容器注册传递
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

为了在编译容器时运行该编译器传递，你必须从你的内核或在一个
:doc:`bundle扩展 </bundles/extension>` 中将该编译器传递添加到容器::

    // src/Kernel.php
    namespace App;

    use App\DependencyInjection\Compiler\MailTransportPass;
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;
    // ...

    class Kernel extends BaseKernel
    {
        // ...

        protected function build(ContainerBuilder $container)
        {
            $container->addCompilerPass(new MailTransportPass());
        }
    }

.. tip::

    如果是在一个服务扩展中实现了 ``CompilerPassInterface`` ，那么你不需要注册它。
    有关更多信息，请参阅 :ref:`组件文档 <components-di-compiler-pass>`。

在标签上添加附加属性
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

有时，你需要有关使用你的标签的标记的每项服务的额外信息。例如，你可能希望为传输链的每个成员添加一个别名。

首先，更改 ``TransportChain`` 类::

    class TransportChain
    {
        private $transports;

        public function __construct()
        {
            $this->transports = array();
        }

        public function addTransport(\Swift_Transport $transport, $alias)
        {
            $this->transports[$alias] = $transport;
        }

        public function getTransport($alias)
        {
            if (array_key_exists($alias, $this->transports)) {
                return $this->transports[$alias];
            }
        }
    }

如你所见，在调用 ``addTransport()`` 时，它不仅需要一个 ``Swift_Transport`` 对象，还需要该传输的别名字符串。
那么，如何为每个被标记的传输服务都提供一个别名呢？

要回答这个问题，请更改该服务声明：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            Swift_SmtpTransport:
                arguments: ['%mailer_host%']
                tags:
                    - { name: 'app.mail_transport', alias: 'smtp' }

            Swift_SendmailTransport:
                tags:
                    - { name: 'app.mail_transport', alias: 'sendmail' }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Swift_SmtpTransport">
                    <argument>%mailer_host%</argument>

                    <tag name="app.mail_transport" alias="smtp" />
                </service>

                <service id="Swift_SendmailTransport">
                    <tag name="app.mail_transport" alias="sendmail" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        $container->register(\Swift_SmtpTransport::class)
            ->addArgument('%mailer_host%')
            ->addTag('app.mail_transport', array('alias' => 'foo'));

        $container->register(\Swift_SendmailTransport::class)
            ->addTag('app.mail_transport', array('alias' => 'bar'));

.. tip::

    在YAML格式中，只要你不需要指定其他属性，就可以将标签简化为一个简单字符串。以下定义是等效的。

    .. code-block:: yaml

        # config/services.yaml
        services:
            # Compact syntax
            Swift_SendmailTransport:
                class: \Swift_SendmailTransport
                tags: ['app.mail_transport']

            # Verbose syntax
            Swift_SendmailTransport:
                class: \Swift_SendmailTransport
                tags:
                    - { name: 'app.mail_transport' }

请注意，你已为标签添加了一个通用的 ``alias`` 键。要实际使用它，请更新编译器::

    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\DependencyInjection\Compiler\CompilerPassInterface;
    use Symfony\Component\DependencyInjection\Reference;

    class TransportCompilerPass implements CompilerPassInterface
    {
        public function process(ContainerBuilder $container)
        {
            // ...

            foreach ($taggedServices as $id => $tags) {

                // 一个服务可以拥有相同的标签两次或更多
                foreach ($tags as $attributes) {
                    $definition->addMethodCall('addTransport', array(
                        new Reference($id),
                        $attributes["alias"]
                    ));
                }
            }
        }
    }

双循环可能令人困惑。这是因为一个服务可以拥有多个标签。你使用 ``app.mail_transport`` 标签将一个服务标记了两次或更多次。
第二个foreach循环遍历为当前服务设置的 ``app.mail_transport`` 标签，并为你提供相关属性。

引用被标记的服务
~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony提供了一个快捷方式来注入标记有一个特定标签的所有服务，这在某些应用中是常见的需求，因此你不必再为此编写一个编译器传递。

在以下示例中，标记为 ``app.handler`` 的所有服务都将作为第一个构造函数参数传递给 ``App\HandlerCollection`` 服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Handler\One:
                tags: ['app.handler']

            App\Handler\Two:
                tags: ['app.handler']

            App\HandlerCollection:
                # 注入所有使用 app.handler 标签的服务作为第一个参数
                arguments: [!tagged app.handler]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Handler\One">
                    <tag name="app.handler" />
                </service>

                <service id="App\Handler\Two">
                    <tag name="app.handler" />
                </service>

                <service id="App\HandlerCollection">
                    <!-- inject all services tagged with app.handler as first argument -->
                    <argument type="tagged" tag="app.handler" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\DependencyInjection\Argument\TaggedIteratorArgument;

        $container->register(App\Handler\One::class)
            ->addTag('app.handler');

        $container->register(App\Handler\Two::class)
            ->addTag('app.handler');

        $container->register(App\HandlerCollection::class)
            // inject all services tagged with app.handler as first argument
            ->addArgument(new TaggedIteratorArgument('app.handler'));

编译后，``HandlerCollection`` 服务就可以遍历你的 ``app.handler`` 了。

.. code-block:: php

    // src/HandlerCollection.php
    namespace App;

    class HandlerCollection
    {
        public function __construct(iterable $handlers)
        {
        }
    }

.. tip::

    可以使用 ``priority`` 属性对收集的服务进行优先级排序：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            services:
                App\Handler\One:
                    tags:
                        - { name: 'app.handler', priority: 20 }

        .. code-block:: xml

            <!-- config/services.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <services>
                    <service id="App\Handler\One">
                        <tag name="app.handler" priority="20" />
                    </service>
                </services>
            </container>

        .. code-block:: php

            // config/services.php
            $container->register(App\Handler\One::class)
                ->addTag('app.handler', array('priority' => 20));

    请注意，此功能将忽略任何其他自定义属性。
