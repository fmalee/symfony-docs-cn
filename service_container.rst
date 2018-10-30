.. index::
   single: Service Container
   single: DependencyInjection; Container

服务容器
=================

.. admonition:: Screencast
    :class: screencast

    更喜欢视频教程? 可以观看 `Symfony Fundamentals screencast series`_ 系列录像.

你的程序 *全部是* 各有用途的对象: 一个 "Mailer" 对象可以帮助你发送邮件，而另一个对象可以帮你把数据入库。
你程序中的几乎 *所有* 事都是由这些对象中的某一个来完成的。每当你安装一个新bundle，你就拥有了更多的对象！

在Symfony中，这些有用的对象被称为 **服务**，每个服务都存在于一个称为 **服务容器** 的非常特殊的对象中。
如果你有服务容器，那么你可以使用服务的id来取出服务：

容器允许你以集中化化的方式构造对象。它让你的生活更轻松，构筑强大的架构，并且超级快！

取得和使用服务
----------------------

从你开启 Symfony 程序那一刻起，你的容器 *已经* 包含了许多服务。很像是 *工具* : 等待着你使用它们。
在控制器中，你可以通过使用服务的类或接口名称对一个参数进行类型约束来“请求”容器中的服务。
想要 :doc:`记录 </logging>` 一些东西？没有问题::

    // src/Controller/ProductController.php
    // ...

    use Psr\Log\LoggerInterface;

    /**
     * @Route("/products")
     */
    public function list(LoggerInterface $logger)
    {
        $logger->info('Look! I just used a service');

        // ...
    }

还有哪些其他服务？可以通过运行命令找出来：

.. code-block:: terminal

    $ php bin/console debug:autowiring

    # 这只是输出的一个*小*样本......
    ==========================================================  ==================================
    Class/Interface Type                                        Alias Service ID
    ==========================================================  ==================================
    Psr\Cache\CacheItemPoolInterface                            alias for "cache.app.recorder"
    Psr\Log\LoggerInterface                                     alias for "monolog.logger"
    Symfony\Component\EventDispatcher\EventDispatcherInterface  alias for "debug.event_dispatcher"
    Symfony\Component\HttpFoundation\RequestStack               alias for "request_stack"
    Symfony\Component\HttpFoundation\Session\SessionInterface   alias for "session"
    Symfony\Component\Routing\RouterInterface                   alias for "router.default"
    ==========================================================  ==================================

当你在控制器方法或 :ref:`自己的服务 <service-container-creating-service>` 中使用这些类型约束时，
Symfony将自动传递与该类型相匹配的服务对象。

在整个文档中，你将看到如何使用容器中的许多不同服务。

.. tip::

    容器中实际上有 *非常多* 的服务，每个服务在容器中都有唯一的id，如 ``session`` 和 ``router.default``。
    要获取有关的完整列表，你可以运行 ``php bin/console debug:container``。
    但大多数时候，你不必担心这一点。
    请参阅 :ref:`services-wire-specific-service`。
    请参见 :doc:`/service_container/debug`。

.. index::
   single: Service Container; Configuring services

.. _service-container-creating-service:

Creating/Configuring Services in the Container
在容器中创建/配置服务
----------------------------------------------

你还可以将 *自己* 的代码组织到服务中。
例如，假设你需要向用户显示随机、快乐的消息。
如果将此代码放在控制器中，则无法重复使用。
取而代之，你决定创建一个新类::

    // src/Service/MessageGenerator.php
    namespace App\Service;

    class MessageGenerator
    {
        public function getHappyMessage()
        {
            $messages = [
                'You did it! You updated the system! Amazing!',
                'That was one of the coolest updates I\'ve seen all day!',
                'Great work! Keep going!',
            ];

            $index = array_rand($messages);

            return $messages[$index];
        }
    }

恭喜！你刚刚创建了第一个服务类！你可以立即在控制器内使用它::

    use App\Service\MessageGenerator;

    public function new(MessageGenerator $messageGenerator)
    {
        // 得益于类型约束，容器将实例化一个新的 MessageGenerator 并将其传递给你！
        // ...

        $message = $messageGenerator->getHappyMessage();
        $this->addFlash('success', $message);
        // ...
    }

当你请求 ``MessageGenerator`` 服务时，容器构造一个新的 ``MessageGenerator`` 对象并返回它（参见下面的侧栏）。
但是如果你从未请求这项服务，它就 *永远不会* 构建：优化内存和速度。
作为回报，``MessageGenerator`` 服务仅创建 *一次*：每次请求时都返回相同的实例。

.. _service-container-services-load-example:

.. sidebar:: 在 services.yaml 中自动加载服务

    本文档假定你使用以下服务配置，这是新项目的默认配置：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            services:
                # 此文件中的服务的默认配置
                _defaults:
                    autowire: true      # 自动注入服务中的依赖项。
                    autoconfigure: true # 自动将你的服务注册为命令、事件订阅者等等。
                    public: false       # 允许通过删除未使用的服务来优化容器；
                                        # 这也意味着将无法通过 $container->get() 直接从容器中获取服务。
                                        # 最好的做法是明确你的依赖关系。


                # 使 src/ 中的类可作为服务
                # 这会为每个类创建一个服务，其id是完全限定的类名
                App\:
                    resource: '../src/*'
                    exclude: '../src/{Entity,Migrations,Tests,Kernel.php}'

                # ...

        .. code-block:: xml

            <!-- config/services.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <services>
                    <!-- Default configuration for services in *this* file -->
                    <defaults autowire="true" autoconfigure="true" public="false" />

                    <prototype namespace="App\" resource="../src/*" exclude="../src/{Entity,Migrations,Tests}" />
                </services>
            </container>

        .. code-block:: php

            // config/services.php
            use Symfony\Component\DependencyInjection\Definition;

            // To use as default template
            $definition = new Definition();

            $definition
                ->setAutowired(true)
                ->setAutoconfigured(true)
                ->setPublic(false)
            ;

            // $this is a reference to the current loader
            $this->registerClasses($definition, 'App\\', '../src/*', '../src/{Entity,Migrations,Tests}');

    .. tip::

        ``resource`` 和 ``exclude``选项的值可以是任何有效的 `glob模式`_。
        ``exclude`` 选项的值也可以是glob模式的数组。

        .. versionadded:: 4.2
            在Symfony 4.2中引入了将glob模式的数组传递给 ``exclude`` 选项的功能。

    由于这种配置，你可以自动使用 ``src/`` 目录中的任何类作为服务，而无需手动配置它。
    稍后，你将在 :ref:`service-psr4-loader` 中了解更多相关信息。

    如果你更愿意手动连接(wire)你的服务，那也是完全可能的：
    请参阅 :ref:`services-explicitly-configure-wire-services`。

.. _services-constructor-injection:

将服务/配置注入服务
----------------------------------------

如果你需要在 ``MessageGenerator`` 中访问 ``logger`` 服务怎么办？
没问题！使用具有 ``LoggerInterface`` 类型约束的 ``$logger`` 参数创建一个 ``__construct()`` 方法。
在新的 ``$logger`` 属性上赋值并稍后使用它::

    // src/Service/MessageGenerator.php
    // ...

    use Psr\Log\LoggerInterface;

    class MessageGenerator
    {
        private $logger;

        public function __construct(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }

        public function getHappyMessage()
        {
            $this->logger->info('About to find a happy message!');
            // ...
        }
    }

仅此而已！在实例化 ``MessageGenerator`` 时，容器将自动感知并传递 ``logger`` 服务。
这是怎么做到的？靠 :ref:`自动装配 <services-autowire>`。
关键是 ``__construct()`` 方法中的 ``LoggerInterface`` 类型约束和
``services.yaml`` 中的 ``autowire: true`` 配置。
当你类型约束一个参数时，容器将自动查找匹配的服务。
如果未匹配，你会看到一个明确的附带有用建议的的异常。

顺便说一句，这种向 ``__construct()`` 方法添加依赖项的方法称为 *依赖注入*。
对于一个简单的概念来说，这是一个可怕的术语。

.. _services-debug-container-types:

你怎么会知道使用 ``LoggerInterface`` 作为类型约束？
你可以阅读你正在使用的任何功能的文档，也可以通过运行命令来获取可自动装配的类型约束列表：

.. code-block:: terminal

    $ php bin/console debug:autowiring

这个命令是你最好的朋友。但这只是输出的一小部分：

=============================================================== =====================================
类/接口的类型                                                     服务ID的别名
=============================================================== =====================================
``Psr\Cache\CacheItemPoolInterface``                            alias for "cache.app.recorder"
``Psr\Log\LoggerInterface``                                     alias for "monolog.logger"
``Symfony\Component\EventDispatcher\EventDispatcherInterface``  alias for "debug.event_dispatcher"
``Symfony\Component\HttpFoundation\RequestStack``               alias for "request_stack"
``Symfony\Component\HttpFoundation\Session\SessionInterface``   alias for "session"
``Symfony\Component\Routing\RouterInterface``                   alias for "router.default"
=============================================================== =====================================

处理多个服务
~~~~~~~~~~~~~~~~~~~~~~~~~~

假设你还希望每次进行站点更新时向站点管理员发送电子邮件。
为此，你需要创建一个新类::

    // src/Updates/SiteUpdateManager.php
    namespace App\Updates;

    use App\Service\MessageGenerator;

    class SiteUpdateManager
    {
        private $messageGenerator;
        private $mailer;

        public function __construct(MessageGenerator $messageGenerator, \Swift_Mailer $mailer)
        {
            $this->messageGenerator = $messageGenerator;
            $this->mailer = $mailer;
        }

        public function notifyOfSiteUpdate()
        {
            $happyMessage = $this->messageGenerator->getHappyMessage();

            $message = (new \Swift_Message('Site update just happened!'))
                ->setFrom('admin@example.com')
                ->setTo('manager@example.com')
                ->addPart(
                    'Someone just updated the site. We told them: '.$happyMessage
                );

            return $this->mailer->send($message) > 0;
        }
    }

同时需要 ``MessageGenerator`` *和* ``Swift_Mailer`` 服务。那也没问题！
事实上，这项新服务已经可以使用了。例如，在控制器中，你可以类型约束新的 ``SiteUpdateManager`` 类并使用它::

    // src/Controller/SiteController.php

    // ...
    use App\Updates\SiteUpdateManager;

    public function new(SiteUpdateManager $siteUpdateManager)
    {
        // ...

        if ($siteUpdateManager->notifyOfSiteUpdate()) {
            $this->addFlash('success', 'Notification mail was sent successfully.');
        }

        // ...
    }

由于自动装配和 你在 ``__construct()`` 中的类型约束，容器会创建 ``SiteUpdateManager`` 对象并将其传递到正确的参数。
在大多数情况下，这非常有效。

.. _services-manually-wire-args:

手动装配参数
~~~~~~~~~~~~~~~~~~~~~~~~~

但是有一些案例是服务的参数无法自动装配。
例如，假设你要使管理员的电子邮件可配置：

.. code-block:: diff

    // src/Updates/SiteUpdateManager.php
    // ...

    class SiteUpdateManager
    {
        // ...
    +    private $adminEmail;

    -    public function __construct(MessageGenerator $messageGenerator, \Swift_Mailer $mailer)
    +    public function __construct(MessageGenerator $messageGenerator, \Swift_Mailer $mailer, $adminEmail)
        {
            // ...
    +        $this->adminEmail = $adminEmail;
        }

        public function notifyOfSiteUpdate()
        {
            // ...

            $message = \Swift_Message::newInstance()
                // ...
    -            ->setTo('manager@example.com')
    +            ->setTo($this->adminEmail)
                // ...
            ;
            // ...
        }
    }

如果你如此修改后刷新页面，则会看到一个错误:

    Cannot autowire service "App\Updates\SiteUpdateManager": argument "$adminEmail"
    of method "__construct()" must have a type-hint or be given a value explicitly.

这讲得通！因为容器无从得知你想要传递的值是什么。
不过没关系！在你的配置中，你可以显式的设置此参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            # 和之前的一样
            App\:
                resource: '../src/*'
                exclude: '../src/{Entity,Migrations,Tests}'

            # 显式的配置服务
            App\Updates\SiteUpdateManager:
                arguments:
                    $adminEmail: 'manager@example.com'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <!-- Same as before -->
                <prototype namespace="App\" resource="../src/*" exclude="../src/{Entity,Migrations,Tests}" />

                <!-- Explicitly configure the service -->
                <service id="App\Updates\SiteUpdateManager">
                    <argument key="$adminEmail">manager@example.com</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Updates\SiteUpdateManager;
        use Symfony\Component\DependencyInjection\Definition;

        // Same as before
        $definition = new Definition();

        $definition
            ->setAutowired(true)
            ->setAutoconfigured(true)
            ->setPublic(false)
        ;

        $this->registerClasses($definition, 'App\\', '../src/*', '../src/{Entity,Migrations,Tests}');

        // Explicitly configure the service
        $container->getDefinition(SiteUpdateManager::class)
            ->setArgument('$adminEmail', 'manager@example.com');

有了这个配置，在创建 ``SiteUpdateManager`` 服务时，
容器会将 ``manager@example.com`` 传递给 ``__construct`` 的 ``$adminEmail`` 参数。
而其他参数仍然是自动装配的。

但是，这不会很脆弱吗？幸运的是，没有！
如果你将 `$adminEmail`` 参数重命名为其他内容 -- 例如 ``$mainEmail``-- 当你重新加载下一个页面时，你将获得明确的异常（即使该页面不使用此服务）。

.. _service-container-parameters:

服务参数
------------------

除了拥有(holding)服务对象之外，容器还包含称为 ``参数(parameters)`` 的配置。
要创建一个参数，请将其添加到 ``parameters`` 键下，并使用 ``%parameter_name%`` 语法引用它：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            admin_email: manager@example.com

        services:
            # ...

            App\Updates\SiteUpdateManager:
                arguments:
                    $adminEmail: '%admin_email%'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="admin_email">manager@example.com</parameter>
            </parameters>

            <services>
                <!-- ... -->

                <service id="App\Updates\SiteUpdateManager">
                    <argument key="$adminEmail">%admin_email%</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Updates\SiteUpdateManager;
        $container->setParameter('admin_email', 'manager@example.com');

        $container->autowire(SiteUpdateManager::class)
            // ...
            ->setArgument('$adminEmail', '%admin_email%');

实际上，一旦定义了参数，就可以在 *任何* 其他配置文件中用 ``%parameter_name%`` 语法来引用它。
``config/services.yaml`` 文件中定义了许多参数。

然后，你就可以在服务中获取该参数::

    class SiteUpdateManager
    {
        // ...

        private $adminEmail;

        public function __construct($adminEmail)
        {
            $this->adminEmail = $adminEmail;
        }
    }

你还可以直接从容器中获取参数::

    public function new()
    {
        // ...

        // 只有在继承了控制器基类的控制器中才有效
        $adminEmail = $this->container->getParameter('admin_email');

        // 也可以用快捷方式!
        // $adminEmail = $this->getParameter('admin_email');
    }

有关参数的详细信息，请参阅 :doc:`/service_container/parameters`。

.. _services-wire-specific-service:

选择特定服务
-------------------------

之前创建的 ``MessageGenerator`` 服务需要一个 ``LoggerInterface`` 参数::

    // src/Service/MessageGenerator.php
    // ...

    use Psr\Log\LoggerInterface;

    class MessageGenerator
    {
        private $logger;

        public function __construct(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }
        // ...
    }

但是，容器中有*多个*服务实现 ``LoggerInterface``，
例如 ``logger``、``monolog.logger.request``、``monolog.logger.php`` 等等。
容器如何知道要使用哪个服务？

在这些情况下，容器通常配置为自动选择其中一个服务 - 在这个例子中是 ``logger``
（详细情况请参阅  :ref:`service-autowiring-alias`）。
但是，你可以控制它并传入另一个logger：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ... same code as before

            # explicitly configure the service
            App\Service\MessageGenerator:
                arguments:
                    # '@'符号很重要：这就是告诉容器要
                    # 传递id为 'monolog.logger.request'的 *服务*，
                    # 而不是内容为 'monolog.logger.request' 的*字符串*
                    $logger: '@monolog.logger.request'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... same code as before -->

                <!-- Explicitly configure the service -->
                <service id="App\Service\MessageGenerator">
                    <argument key="$logger" type="service" id="monolog.logger.request" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Service\MessageGenerator;
        use Symfony\Component\DependencyInjection\Reference;

        $container->autowire(MessageGenerator::class)
            ->setAutoconfigured(true)
            ->setPublic(false)
            ->setArgument('$logger', new Reference('monolog.logger.request'));

该配置告诉容器 ``__construct`` 的 ``$logger`` 参数应该使用id为 ``monolog.logger.request`` 的服务。

.. _container-debug-container:

有关容器中所有可用服务的完整列表，请运行：

.. code-block:: terminal

    $ php bin/console debug:container

.. _services-binding:

按名称或类型绑定参数
--------------------------------------

你还可以使用 ``bind`` 关键字按名称或类型绑定特定参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            _defaults:
                bind:
                    # 将此值传递给在此文件中定义的任意服务的任何 $adminEmail 参数（包括控制器参数）
                    $adminEmail: 'manager@example.com'

                    # 将此服务传递给在此文件中定义的任意服务的任何 $requestLogger 参数
                    $requestLogger: '@monolog.logger.request'

                    # 将此服务传递给在此文件中定义的任意服务的任何 LoggerInterface 类型约束
                    Psr\Log\LoggerInterface: '@monolog.logger.request'

                    # 你可以同时定义要匹配的参数的名称和类型
                    string $adminEmail: 'manager@example.com'
                    Psr\Log\LoggerInterface $requestLogger: '@monolog.logger.request'

            # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <defaults autowire="true" autoconfigure="true" public="false">
                    <bind key="$adminEmail">manager@example.com</bind>
                    <bind key="$requestLogger"
                        type="service"
                        id="monolog.logger.request"
                    />
                    <bind key="Psr\Log\LoggerInterface"
                        type="service"
                        id="monolog.logger.request"
                    />

                    <!-- optionally you can define both the name and type of the argument to match -->
                    <bind key="string $adminEmail">manager@example.com</bind>
                    <bind key="Psr\Log\LoggerInterface $requestLogger"
                        type="service"
                        id="monolog.logger.request"
                    />
                </defaults>

                <!-- ... -->
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Controller\LuckyController;
        use Symfony\Component\DependencyInjection\Reference;
        use Psr\Log\LoggerInterface;

        $container->register(LuckyController::class)
            ->setPublic(true)
            ->setBindings(array(
                '$adminEmail' => 'manager@example.com',
                '$requestLogger' => new Reference('monolog.logger.request'),
                LoggerInterface::class => new Reference('monolog.logger.request'),
                // optionally you can define both the name and type of the argument to match
                'string $adminEmail' => 'manager@example.com',
                LoggerInterface::class.' $requestLogger' => new Reference('monolog.logger.request'),
            ))
        ;

通过将 ``bind`` 键放在 ``_defaults`` 下，你可以为在此文件中定义的 *任何* 服务指定 *任意* 参数的值！
你可以按名称（例如 ``$adminEmail``），按类型（例如 ``Psr\Log\LoggerInterface``）
或两者（例如 ``Psr\Log\LoggerInterface $requestLogger``）来绑定参数。

.. versionadded:: 4.2
    在Symfony 4.2中引入了按名称和类型绑定参数的功能。

``bind`` 配置也可以应用于特定服务或一次加载多个服务（即 :ref:`service-psr4-loader`）。

将容器参数作为服务获取
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 4.1
    Symfony 4.1中引入了将容器参数作为服务获取的功能。

如果某些服务或控制器需要大量容器参数，
有比使用 ``services._defaults.bind`` 选项绑定所有这些参数更容易的替代方法：
使用 :class:`Symfony\\Component\\DependencyInjection\\ParameterBag\\ParameterBagInterface`
或新的 :class:`Symfony\\Component\\DependencyInjection\\ParameterBag\\ContainerBagInterface`
对该类的构造函数的任意参数进行类型约束，
该服务将可以从 :class:`Symfony\\Component\\DependencyInjection\\ParameterBag\\ParameterBag`
对象中获取所有的容器参数::

    // src/Service/MessageGenerator.php
    // ...

    use Symfony\Component\DependencyInjection\ParameterBag\ParameterBagInterface;

    class MessageGenerator
    {
        private $params;

        public function __construct(ParameterBagInterface $params)
        {
            $this->params = $params;
        }

        public function someMethod()
        {
            // 可以从 $this->params 获取任何参数，它保存着所有的容器参数
            $sender = $this->params->get('mailer_sender');
            // ...
        }
    }

.. _services-autowire:

自动装配选项
-------------------

在上面，``services.yaml`` 文件的 ``_defaults`` 部分中具有 ``autowire: true`` 配置，
以便它适用于该文件中定义的所有服务。
使用此设置，你可以在自己的服务的 ``__construct()`` 方法中对参数进行类型约束，容器将自动传递正确的参数。
整个条目都是围绕自动装配编写的。

有关自动装配的更多详细信息，请查看 :doc:`/service_container/autowiring`。

.. _services-autoconfigure:

自动配置选项
------------------------

在上面，``services.yaml`` 文件的 ``_defaults`` 部分中具有 ``autoconfigure: true`` 配置，
以便它适用于该文件中定义的所有服务。
使用此设置，容器将根据你的服务的 *类* 自动将某些配置应用于你的服务。
这主要用于自动标记(auto-tag)你的服务。

例如，要创建一个Twig扩展，你需要创建一个类，将其注册为服务，
并使用 ``twig.extension`` 作为它的 :doc:`标签 </service_container/tags>`。

但是，使用 ``autoconfigure: true``，你将不需要手动标记标签。
实际上，如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
则无需执行 *任何* 操作：该服务将自动加载。
然后，``autoconfigure`` 将 *为* 你的服务添加 ``twig.extension`` 标签，
因为你的服务类继承了 ``Twig\Extension\ExtensionInterface``。
并且得益于 ``autowire``，你甚至可以直接添加构造函数参数而无需任何配置。

.. _container-public:

共有与私有服务
------------------------------

得益于 ``services.yaml`` 中的 ``_defaults`` 部分，此文件中定义的每个服务默认都是 ``public: false``。

这是什么意思？当服务 *是* 共有时，你可以直接从容器对象中访问它，
该容器对象可以从任何继承了 ``Controller`` 的控制器中访问::

    use App\Service\MessageGenerator;

    // ...
    public function new()
    {
        // 容器中有一个共有属性的 "logger" 服务
        $logger = $this->container->get('logger');

        // 这将不起作用：MessageGenerator 是一个私有服务
        $generator = $this->container->get(MessageGenerator::class);
    }

作为最佳实践，你应该只创建 *私有* 服务(私有为默认属性)。
而且，你 *不* 应该使用 ``$container->get()`` 方法来获取公有服务。

但是，如果你确实需要共有服务，请覆盖 ``public`` 设置：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ... 和之前一样的代码

            # 显式的配置服务
            App\Service\MessageGenerator:
                public: true

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... same code as before -->

                <!-- Explicitly configure the service -->
                <service id="App\Service\MessageGenerator" public="true"></service>
            </services>
        </container>

.. _service-psr4-loader:

使用资源键一次导入多个服务
---------------------------------------------

你之前就已经知道可以使用 ``resource`` 键一次导入许多服务。
例如，默认的Symfony配置包含以下内容：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            # 使 src/ 中的类可作为服务
            # 这会为每个类创建一个服务，其id是完全限定的类名
            App\:
                resource: '../src/*'
                exclude: '../src/{Entity,Migrations,Tests}'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <prototype namespace="App\" resource="../src/*" exclude="../src/{Entity,Migrations,Tests}" />
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        // To use as default template
        $definition = new Definition();

        $definition
            ->setAutowired(true)
            ->setAutoconfigured(true)
            ->setPublic(false)
        ;

        $this->registerClasses($definition, 'App\\', '../src/*', '../src/{Entity,Migrations,Tests}');

.. tip::

    ``resource`` 和 ``exclude`` 选项的值可以是任何有效的 `glob模式`_ 。

该配置可用于快速让许多类可以作为服务并应用一些默认配置。
每个服务的 ``id`` 是其完全限定的类名。
你可以在该配置下面的位置，通过使用相应的ID(类名称)重写任何已经导入的服务（请参阅 :ref:`services-manually-wire-args`）。
如果重写一个服务，则该服务不再从“导入配置”那里继承任何选项（例如 ``public``）
（但重写的服务仍然 *会* 从 ``_defaults`` 继承选项）。

你也可以用 ``exclude`` 排除某些路径。
这是可选项，但会稍微提高 ``dev`` 环境下的性能：排除的路径不再跟踪，因此修改它们不会触发容器重建。

.. note::

    等等，这是否意味着 ``src/`` 中的 *每个* 类都被注册为服务？甚至包括模型类？实际上并没有。
    只要你在 ``_defaults`` 键下有 ``public: false`` （或者你可以在特定的导入中下添加它），
    那么所有导入的服务都是 *私有* 的。
    基于这个原因，``src/`` 中 *未* 明确用作服务的所有类都会自动从最终容器中删除。
    实际上，导入仅仅意味着所有类都无需手动配置而“可以当做服务”。

相同命名空间下的多个服务定义
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果使用YAML配置格式定义服务，则PHP命名空间将用作每个配置的键，
因此你无法为同一命名空间下的类定义不同的服务配置：

.. code-block:: yaml

    # config/services.yaml
    services:
        App\Domain\:
            resource: '../src/Domain/*'
            # ...

要获得多个定义，请添加``namespace`` 选项并使用任何的唯一字符串作为每个服务配置的键：

.. code-block:: yaml

    # config/services.yaml
    services:
        command_handlers:
            namespace: App\Domain\
            resource: '../src/Domain/*/CommandHandler'
            tags: [command_handler]

        event_subscribers:
            namespace: App\Domain\
            resource: '../src/Domain/*/EventSubscriber'
            tags: [event_subscriber]

.. _services-explicitly-configure-wire-services:

显式的配置服务和参数
---------------------------------------------

在Symfony 3.3之前，所有服务和（通常）参数都是显式配置的：无法 :ref:`自动加载服务 <service-container-services-load-example>`，:ref:`autowiring <services-autowire>` 也不常见。

这两个功能都是可选的。即使你使用它们，也可能在某些情况下需要手动装配服务。
例如，假设你要为 ``SiteUpdateManager`` 类注册 *2* 个服务 - 每个服务都有不同的管理员电子邮件。
在这种情况下，每个都需要具有唯一的服务ID：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            # 这是服务的ID
            site_update_manager.superadmin:
                class: App\Updates\SiteUpdateManager
                # 你仍然可以使用自动装配：我们只想展示没有自动装配是什么样子
                autowire: false
                # 手动装配所有参数
                arguments:
                    - '@App\Service\MessageGenerator'
                    - '@mailer'
                    - 'superadmin@example.com'

            site_update_manager.normal_users:
                class: App\Updates\SiteUpdateManager
                autowire: false
                arguments:
                    - '@App\Service\MessageGenerator'
                    - '@mailer'
                    - 'contact@example.com'

            # 创建一个别名，这样的话 - 默认情况下 - 如果你类型约束 SiteUpdateManager，
            # 将使用 site_update_manager.superadmin
            App\Updates\SiteUpdateManager: '@site_update_manager.superadmin'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="site_update_manager.superadmin" class="App\Updates\SiteUpdateManager" autowire="false">
                    <argument type="service" id="App\Service\MessageGenerator" />
                    <argument type="service" id="mailer" />
                    <argument>superadmin@example.com</argument>
                </service>

                <service id="site_update_manager.normal_users" class="App\Updates\SiteUpdateManager" autowire="false">
                    <argument type="service" id="App\Service\MessageGenerator" />
                    <argument type="service" id="mailer" />
                    <argument>contact@example.com</argument>
                </service>

                <service id="App\Updates\SiteUpdateManager" alias="site_update_manager.superadmin" />
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Updates\SiteUpdateManager;
        use App\Service\MessageGenerator;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register('site_update_manager.superadmin', SiteUpdateManager::class)
            ->setAutowired(false)
            ->setArguments(array(
                new Reference(MessageGenerator::class),
                new Reference('mailer'),
                'superadmin@example.com'
            ));

        $container->register('site_update_manager.normal_users', SiteUpdateManager::class)
            ->setAutowired(false)
            ->setArguments(array(
                new Reference(MessageGenerator::class),
                new Reference('mailer'),
                'contact@example.com'
            ));

        $container->setAlias(SiteUpdateManager::class, 'site_update_manager.superadmin')

在这个示例中，注册了 *两个* 服务：
``site_update_manager.superadmin`` 和 ``site_update_manager.normal_users``。
得益于别名，如果你类型约束 ``SiteUpdateManager``，
将传递第一个服务（``site_update_manager.superadmin``）。

.. caution::

    如果你 *没有* 创建别名并且是 :ref:`从 src/ 加载所有服务<service-container-services-load-example>`，
    那么当你类型约束 ``SiteUpdateManager`` 时，将会创建三个服务（自动服务 + 你的两个服务），
    而且，默认情况下将会传递自动加载的那个服务。
    这就是为什么说创建别名是个好主意的原因。

扩展阅读
----------------------

.. toctree::
    :maxdepth: 1
    :glob:

    /service_container/*

.. _`service-oriented architecture`: https://en.wikipedia.org/wiki/Service-oriented_architecture
.. _`Symfony Standard Edition (version 3.3) services.yaml`: https://github.com/symfony/symfony-standard/blob/3.3/app/config/services.yml
.. _`glob模式`: https://en.wikipedia.org/wiki/Glob_(programming)
.. _`Symfony Fundamentals screencast series`: https://symfonycasts.com/screencast/symfony-fundamentals
