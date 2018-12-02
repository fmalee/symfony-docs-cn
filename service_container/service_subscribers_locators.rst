.. index::
    single: DependencyInjection; Service Subscribers

服务订阅器 & 定位器
==============================

    Service Subscribers & Locators

.. versionadded:: 3.3
    服务订阅器和定位器在Symfony 3.3中引入。

有时，一个服务需要访问其他几个服务，而又不确定是否实际使用了所有这些服务。
在这些情况下，你可能希望服务的实例化是惰性的。
但是，使用显式依赖注入是不可能的，因为并非所有服务都是 ``lazy``（参见 :doc:`/service_container/lazy_services`）。
Sometimes, a service needs access to several other services without being sure
that all of them will actually be used.
In those cases, you may want the instantiation of the services to be lazy.
However, that's not possible using the explicit dependency injection since services are not all meant to
be ``lazy`` (see :doc:`/service_container/lazy_services`).

在控制器中通常可以是这种情况，你可以在构造函数中注入多个服务，但执行的动作仅使用其中一些服务。
另一个示例是使用一个CommandBus通过Command类名来映射命令处理程序来实现 `命令模式`_ 的应用，
并在询问它们时使用它们来处理它们各自的命令：
This can typically be the case in your controllers, where you may inject several
services in the constructor, but the action executed only uses some of them.
Another example are applications that implement the `命令模式`_
using a CommandBus to map command handlers by Command class names and use them
to handle their respective command when it is asked for::

    // src/CommandBus.php
    namespace App;

    // ...
    class CommandBus
    {
        /**
         * @var CommandHandler[]
         */
        private $handlerMap;

        public function __construct(array $handlerMap)
        {
            $this->handlerMap = $handlerMap;
        }

        public function handle(Command $command)
        {
            $commandClass = get_class($command);

            if (!isset($this->handlerMap[$commandClass])) {
                return;
            }

            return $this->handlerMap[$commandClass]->handle($command);
        }
    }

    // ...
    $commandBus->handle(new FooCommand());

考虑到一次只处理一个命令，实例化所有其他命令处理器是不必要的。
延迟加载这些处理器的可能解决方案可以是注入主依赖注入容器。

但是，不鼓励注入整个容器，因为它提供了对现有服务的过多访问，并且隐藏了服务的实际依赖性。
这样做也需要公开服务，而Symfony应用的默认情况并非如此。
However, injecting the entire container is discouraged because it gives too
broad access to existing services and it hides the actual dependencies of the
services. Doing so also requires services to be made public, which isn't the
case by default in Symfony applications.

**服务订阅器** 旨在通过提供对一组预定义服务的访问来解决此问题，同时仅在实际需要时通过一个
**服务定位器**（一个单独的延迟加载的容器）实例化它们。
**Service Subscribers** are intended to solve this problem by giving access to a
set of predefined services while instantiating them only when actually needed
through a **Service Locator**, a separate lazy-loaded container.

定义服务订阅器
-----------------------------

首先，将 ``CommandBus`` 变成一个
:class:`Symfony\\Component\\DependencyInjection\\ServiceSubscriberInterface`
的实现。使用其 ``getSubscribedServices()``
方法在服务订阅器中包含所需数量的服务，并将容器的类型约束更改为一个PSR-11 ``ContainerInterface``::
First, turn ``CommandBus`` into an implementation of :class:`Symfony\\Component\\DependencyInjection\\ServiceSubscriberInterface`.
Use its ``getSubscribedServices()`` method to include as many services as needed
in the service subscriber and change the type hint of the container to
a PSR-11 ``ContainerInterface``::

    // src/CommandBus.php
    namespace App;

    use App\CommandHandler\BarHandler;
    use App\CommandHandler\FooHandler;
    use Psr\Container\ContainerInterface;
    use Symfony\Component\DependencyInjection\ServiceSubscriberInterface;

    class CommandBus implements ServiceSubscriberInterface
    {
        private $locator;

        public function __construct(ContainerInterface $locator)
        {
            $this->locator = $locator;
        }

        public static function getSubscribedServices()
        {
            return [
                'App\FooCommand' => FooHandler::class,
                'App\BarCommand' => BarHandler::class,
            ];
        }

        public function handle(Command $command)
        {
            $commandClass = get_class($command);

            if ($this->locator->has($commandClass)) {
                $handler = $this->locator->get($commandClass);

                return $handler->handle($command);
            }
        }
    }

.. tip::

    如果容器 *未* 包含已订阅的服务，请确认你已经启用 :ref:`自动配置 <services-autoconfigure>`。
    你也可以手动添加 ``container.service_subscriber`` 标签。

被注入的服务是一个实现了PSR-11 ``ContainerInterface`` 的
:class:`Symfony\\Component\\DependencyInjection\\ServiceLocator` 实例，但它也是一个callable::

    // ...
    $handler = ($this->locator)($commandClass);

    return $handler->handle($command);

引入服务
------------------

为了向服务订阅器添加新的依赖，请使用 ``getSubscribedServices()``
方法添加要包含在服务定位器中的服务类型::

    use Psr\Log\LoggerInterface;

    public static function getSubscribedServices()
    {
        return [
            // ...
            LoggerInterface::class,
        ];
    }

服务类型也可以用一个服务名称做为键，以便内部使用::
Service types can also be keyed by a service name for internal use::

    use Psr\Log\LoggerInterface;

    public static function getSubscribedServices()
    {
        return [
            // ...
            'logger' => LoggerInterface::class,
        ];
    }

在继承一个实现了 ``ServiceSubscriberInterface`` 的类时，你的责任是在重写
``getSubscribedServices()`` 时调用其父方法。
这通常出现在继承 ``AbstractController`` 的时候::

    use Psr\Log\LoggerInterface;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class MyController extends AbstractController
    {
        public static function getSubscribedServices()
        {
            return array_merge(parent::getSubscribedServices(), [
                // ...
                'logger' => LoggerInterface::class,
            ]);
        }
    }

可选服务
~~~~~~~~~~~~~~~~~

对于可选的依赖，如果在服务容器中找不到匹配的服务，则在服务类型前加上一个 ``?`` 以防止出现错误::

    use Psr\Log\LoggerInterface;

    public static function getSubscribedServices()
    {
        return [
            // ...
            '?'.LoggerInterface::class,
        ];
    }

.. note::

    在调用服务本身之前，通过在服务定位器上调用 ``has()`` 可确认是否存在该可选服务。

别名服务
~~~~~~~~~~~~~~~~

默认情况下，自动装配用于将服务类型与服务容器中的服务进行匹配。
如果你不使用自动装配或需要将一个非传统服务添加为依赖，请使用
``container.service_subscriber`` 标签将一个服务类型映射到一个服务。
By default, autowiring is used to match a service type to a service from the
service container. If you don't use autowiring or need to add a non-traditional
service as a dependency, use the ``container.service_subscriber`` tag to map a
service type to a service.

.. configuration-block::

    .. code-block:: yaml

        // config/services.yaml
        services:
            App\CommandBus:
                tags:
                    - { name: 'container.service_subscriber', key: 'logger', id: 'monolog.logger.event' }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>

                <service id="App\CommandBus">
                    <tag name="container.service_subscriber" key="logger" id="monolog.logger.event" />
                </service>

            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\CommandBus;

        // ...

        $container
            ->register(CommandBus::class)
            ->addTag('container.service_subscriber', array('key' => 'logger', 'id' => 'monolog.logger.event'))
        ;

.. tip::

    如果内部的服务名称与服务容器中的相同，则可以省略 ``key`` 属性。

Defining a Service Locator
定义服务定位器
--------------------------

要手动定义一个服务定位器，请创建新的服务定义并添加 ``container.service_locator`` 标签。
使用 ``arguments`` 选项可以引入所需数量的服务。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            app.command_handler_locator:
                class: Symfony\Component\DependencyInjection\ServiceLocator
                arguments:
                    -
                        App\FooCommand: '@app.command_handler.foo'
                        App\BarCommand: '@app.command_handler.bar'
                # 如果未使用默认的服务自动配置，请将以下标签添加到服务定义中：
                # tags: ['container.service_locator']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>

                <service id="app.command_handler_locator" class="Symfony\Component\DependencyInjection\ServiceLocator">
                    <argument type="collection">
                        <argument key="App\FooCommand" type="service" id="app.command_handler.foo" />
                        <argument key="App\BarCommand" type="service" id="app.command_handler.bar" />
                    </argument>
                    <!--
                        if you are not using the default service autoconfiguration,
                        add the following tag to the service definition:
                        <tag name="container.service_locator" />
                    -->
                </service>

            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\DependencyInjection\ServiceLocator;
        use Symfony\Component\DependencyInjection\Reference;

        // ...

        $container
            ->register('app.command_handler_locator', ServiceLocator::class)
            ->setArguments(array(array(
                'App\FooCommand' => new Reference('app.command_handler.foo'),
                'App\BarCommand' => new Reference('app.command_handler.bar'),
            )))
            // if you are not using the default service autoconfiguration,
            // add the following tag to the service definition:
            // ->addTag('container.service_locator')
        ;

.. versionadded:: 4.1
    服务定位器自动配置是在Symfony 4.1中引入的。
    在之前的Symfony版本中，你始终需明确添加 ``container.service_locator`` 标签。

.. note::

    服务定位器的参数中定义的服务必须包含键，稍后这些键将在定位器内成为对应服务的唯一标识符。

现在，你可以通过将该服务定位器注入到任何其他服务来使用它：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\CommandBus:
                arguments: ['@app.command_handler_locator']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>

                <service id="App\CommandBus">
                    <argument type="service" id="app.command_handler_locator" />
                </service>

            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\CommandBus;
        use Symfony\Component\DependencyInjection\Reference;

        $container
            ->register(CommandBus::class)
            ->setArguments(array(new Reference('app.command_handler_locator')))
        ;

在 :doc:`编译器传递 </service_container/compiler_passes>` 中，建议使用
:method:`Symfony\\Component\\DependencyInjection\\Compiler\\ServiceLocatorTagPass::register`
方法来创建服务定位器。
这将为你消除一些样板代码，并将在引用它们的所有服务中共享相同的定位器::
In :doc:`编译器传递 </service_container/compiler_passes>` it's recommended
to use the :method:`Symfony\\Component\\DependencyInjection\\Compiler\\ServiceLocatorTagPass::register`
method to create the service locators. This will save you some boilerplate and
will share identical locators amongst all the services referencing them::

    use Symfony\Component\DependencyInjection\Compiler\ServiceLocatorTagPass;
    use Symfony\Component\DependencyInjection\ContainerBuilder;

    public function process(ContainerBuilder $container)
    {
        //...

        $locateableServices = array(
            //...
            'logger' => new Reference('logger'),
        );

        $myService->addArgument(ServiceLocatorTagPass::register($container, $locateableServices));
    }

.. _`命令模式`: https://en.wikipedia.org/wiki/Command_pattern

服务订阅器复用(Trait)
------------------------

.. versionadded:: 4.2
    :class:`Symfony\\Component\\DependencyInjection\\ServiceSubscriberTrait`
    在Symfony的4.2中引入的。

:class:`Symfony\\Component\\DependencyInjection\\ServiceSubscriberTrait` 为
:class:`Symfony\\Component\\DependencyInjection\\ServiceSubscriberInterface`
提供了一个实现，用于拥有零参数和一个返回类型的类的所有方法。
它为那些返回类型的服务提供了一个 ``ServiceLocator``。服务ID是 ``__METHOD__``。
这允许你基于已类型约束的辅助方法来轻松地向你的服务添加依赖::
The :class:`Symfony\\Component\\DependencyInjection\\ServiceSubscriberTrait`
provides an implementation for
:class:`Symfony\\Component\\DependencyInjection\\ServiceSubscriberInterface`
that looks through all methods in your class that have no arguments and a return
type.
It provides a ``ServiceLocator`` for the services of those return types.
The service id is ``__METHOD__``.
This allows you to easily add dependencies to your services based on type-hinted helper methods::

    // src/Service/MyService.php
    namespace App\Service;

    use Psr\Log\LoggerInterface;
    use Symfony\Component\DependencyInjection\ServiceSubscriberInterface;
    use Symfony\Component\DependencyInjection\ServiceSubscriberTrait;
    use Symfony\Component\Routing\RouterInterface;

    class MyService implements ServiceSubscriberInterface
    {
        use ServiceSubscriberTrait;

        public function doSomething()
        {
            // $this->router() ...
            // $this->logger() ...
        }

        private function router(): RouterInterface
        {
            return $this->container->get(__METHOD__);
        }

        private function logger(): LoggerInterface
        {
            return $this->container->get(__METHOD__);
        }
    }

这允许你创建辅助复用，如RouterAware，LoggerAware等...然后用它们来组成你的服务::
This  allows you to create helper traits like RouterAware, LoggerAware, etc...
and compose your services with them::

    // src/Service/LoggerAware.php
    namespace App\Service;

    use Psr\Log\LoggerInterface;

    trait LoggerAware
    {
        private function logger(): LoggerInterface
        {
            return $this->container->get(__CLASS__.'::'.__FUNCTION__);
        }
    }

    // src/Service/RouterAware.php
    namespace App\Service;

    use Symfony\Component\Routing\RouterInterface;

    trait RouterAware
    {
        private function router(): RouterInterface
        {
            return $this->container->get(__CLASS__.'::'.__FUNCTION__);
        }
    }

    // src/Service/MyService.php
    namespace App\Service;

    use Symfony\Component\DependencyInjection\ServiceSubscriberInterface;
    use Symfony\Component\DependencyInjection\ServiceSubscriberTrait;

    class MyService implements ServiceSubscriberInterface
    {
        use ServiceSubscriberTrait, LoggerAware, RouterAware;

        public function doSomething()
        {
            // $this->router() ...
            // $this->logger() ...
        }
    }

.. caution::

    When creating these helper traits, the service id cannot be ``__METHOD__``
    as this will include the trait name, not the class name. Instead, use
    ``__CLASS__.'::'.__FUNCTION__`` as the service id.
    在创建这些辅助复用时，服务ID不能是 ``__METHOD__``，而是包含复用名称，而不包括类名。
    而是，将 ``__CLASS__.'::'.__FUNCTION__`` 用作服务ID。
