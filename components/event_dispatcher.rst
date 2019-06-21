.. index::
   single: EventDispatcher
   single: Components; EventDispatcher

EventDispatcher组件
=============================

    EventDispatcher组件提供的工具允许应用组件通过调度事件并监听它们来相互通信。

介绍
------------

面向对象的代码在确保代码可扩展方面已经走了很长的路。
通过创建具有明确定义职责的类，你的代码变得更加灵活，而且开发人员可以使用子类扩展它们以修改其行为。
但是，如果他们想与已经创建自己的子类的其他开发人员共享更改，则代码继承不再是答案。

思考一下你希望为你的项目提供插件系统的实际示例。
一个插件应该能够添加方法，或者在执行一个方法之前或之后进行某些操作而不会干扰其他插件。
使用单继承不再能简单的解决这些问题，即使PHP可以实现多重继承，它也有其自身的缺点。

Symfony的EventDispatcher组件实现了 `中介者`_ 和 `观察者`_
设计模式，使所有这些设想成为可能，并让你的项目真正可扩展。

以 :doc:`HttpKernel组件 </components/http_kernel>` 为例。
一旦创建了一个 ``Response`` 对象，它将允许系统中的其他元素在实际使用之前修改它（例如添加一些缓存头）。
为了实现这一点，Symfony内核抛出一个事件 - ``kernel.response``。以下是它的工作原理：

* 一个 *监听器* （PHP对象）告诉了一个中心 *调度器* 对象，它希望监听 ``kernel.response`` 事件;

* 在某些时候，Symfony内核告诉 *调度器* 对象调度 ``kernel.response``
  事件，并向其传递一个可以访问 ``Response`` 对象的 ``Event`` 对象;

* 该调度器通知（即调用一个方法）
  ``kernel.response`` 事件的所有监听器，并允许每个监听器对 ``Response`` 对象进行修改。

.. index::
   single: EventDispatcher; Events

安装
------------

.. code-block:: terminal

    $ composer require symfony/event-dispatcher

.. include:: /components/require_autoload.rst.inc

用法
-----

.. seealso::

    本文介绍如何在任何PHP应用中将EventDispatcher功能用作独立组件。
    请阅读 :doc:`/event_dispatcher` 一文，了解如何在Symfony应用中使用它。

事件
~~~~~~

当一个事件被调度时，它由一个唯一名称（例如 ``kernel.response``）标识，任意数量的监听器可能正在监听该名称。
还会创建一个 :class:`Symfony\\Contracts\\EventDispatcher\\Event`
实例并将其传递给所有监听器。正如你稍后将看到的，``Event`` 对象本身通常包含有关正在调度的事件的数据。

.. index::
   pair: EventDispatcher; Naming conventions

命名约定
..................

唯一的事件名称可以是任何字符串，但可以选择遵循一些命名约定：

* 仅使用小写字母、数字、点号（``.``）以及下划线（``_``）;

* 使用一个命名空间前缀，后跟一个点号（例如 ``order.*``、``user.*``）;

* 使用一个表示已采取的动作的动词后缀（例如 ``order.placed``）。

.. index::
   single: EventDispatcher; Event subclasses

事件名称和事件对象
.............................

当调度器通知监听器时，它会将一个实际的 ``Event`` 对象传递给这些监听器。
``Event`` 基类包含一个停止
:ref:`事件传播 <event_dispatcher-event-propagation>` 的方法，不再包含其他方法。

.. seealso::

    有关此基础事件对象的更多信息，请阅读 :doc:`/components/event_dispatcher/generic_event`。

通常，有关一个特定事件的数据需要与 ``Event`` 对象一起传递，以便对应监听器具有其所需的信息。
在这种情况下，可以在调度事件时传递一个具有用于检索和重写信息的额外方法的特殊子类。
例如，``kernel.response`` 事件使用了一个包含获取甚至替换 ``Response`` 对象的方法的
:class:`Symfony\\Component\\HttpKernel\\Event\\FilterResponseEvent`。

调度器
~~~~~~~~~~~~~~

调度器是事件调度器系统的核心对象。通常情况下，会创建单个维护着一个监听器注册表的调度器。
通过该调度器来调度一个事件时，它会通知所有注册到该事件的监听器::

    use Symfony\Component\EventDispatcher\EventDispatcher;

    $dispatcher = new EventDispatcher();

.. index::
   single: EventDispatcher; Listeners

连接监听器
~~~~~~~~~~~~~~~~~~~~

要使用现有事件，你需要将一个监听器连接到调度器，以便在调度事件时通知它。
一个对调度器的 ``addListener()`` 方法的调用，会将任何有效的PHP可调用对象关联到一个事件::

    $listener = new AcmeListener();
    $dispatcher->addListener('acme.foo.action', [$listener, 'onFooAction']);

``addListener()`` 方法最多需要三个参数：

#. 此监听器要监听的事件名称（字符串）;
#. 在调度指定事件时将执行的一个PHP可调用对象;
#. 一个定义为正整数或负整数（默认为 ``0``）的可选优先级。
   数字越大，该监听器越早调用。如果两个监听器具有相同的优先级，则按照将它们添加到调度器的顺序执行。

.. note::

    `PHP可调用对象`_ 是一个能够由 ``call_user_func()`` 函数使用的PHP变量，并在传递给
    ``is_callable()`` 函数时返回 ``true``。它可以是一个 ``\Closure`` 实例，一个实现
    ``__invoke()`` 方法的对象（实际上是闭包），一个表示函数的字符串或一个表示对象方法或类方法的数组。

    到目前为止，你已经了解了如何将PHP对象注册为监听器。你还可以将PHP `闭包`_ 注册为事件监听器::

        use Symfony\Contracts\EventDispatcher\Event;

        $dispatcher->addListener('acme.foo.action', function (Event $event) {
            // 将在调度 acme.foo.action 事件时执行
        });

一旦监听器注册到调度器，它就会一直等待被通知事件。
在上面的示例中，当 ``acme.foo.action`` 事件被调度时，调度器将调用
``AcmeListener::onFooAction()`` 方法并将 ``Event`` 对象作为单个参数传递::

    use Symfony\Contracts\EventDispatcher\Event;

    class AcmeListener
    {
        // ...

        public function onFooAction(Event $event)
        {
            // ... do something
        }
    }

``$event`` 参数是调度事件时所传递的事件对象。在许多案例中，一个包含额外信息的特殊事件子类会一起传递。
你可以检查每个事件的文档或实现，以确定传递的具体实例。

.. sidebar:: 在服务容器中注册事件监听器和订阅器

    仅注册服务的定义并将它们标记为 ``kernel.event_listener`` 和 ``kernel.event_subscriber``
    标签，还不足以启用事件监听器和事件订阅器。你还必须在容器构建器中注册一个名为
    ``RegisterListenersPass()`` 的编译器传递::

        use Symfony\Component\DependencyInjection\ContainerBuilder;
        use Symfony\Component\DependencyInjection\ParameterBag\ParameterBag;
        use Symfony\Component\DependencyInjection\Reference;
        use Symfony\Component\EventDispatcher\DependencyInjection\RegisterListenersPass;
        use Symfony\Component\EventDispatcher\EventDispatcher;

        $containerBuilder = new ContainerBuilder(new ParameterBag());
        // 注册处理 'kernel.event_listener' 和
        // 'kernel.event_subscriber' 服务标签的编译器传递
        $containerBuilder->addCompilerPass(new RegisterListenersPass());

        $containerBuilder->register('event_dispatcher', EventDispatcher::class);

        // 注册一个事件监听器
        $containerBuilder->register('listener_service_id', \AcmeListener::class)
            ->addTag('kernel.event_listener', [
                'event' => 'acme.foo.action',
                'method' => 'onFooAction',
            ]);

        // 注册一个事件订阅器
        $containerBuilder->register('subscriber_service_id', \AcmeSubscriber::class)
            ->addTag('kernel.event_subscriber');

    默认情况下，*监听器传递* 假设：事件调度器的服务id是 ``event_dispatcher``，事件监听器使用
    ``kernel.event_listener`` 标签来标记，并且事件订阅器使用 ``kernel.event_subscriber``
    标签进行标记。你可以通过将自定义值传递给 ``RegisterListenersPass`` 的构造函数来更改这些默认值。

.. _event_dispatcher-closures-as-listeners:

.. index::
   single: EventDispatcher; Creating and dispatching an event

创建和调度事件
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

除了使用现有事件来注册监听器之外，你还可以创建和调度自己的事件。
这在创建第三方库以及你希望保持自己系统的不同组件的灵活性和分离性时非常有用。

.. _creating-an-event-object:

创建事件类
.......................

假设你要创建一个新事件 - ``order.placed`` - 每次客户使用你的应用订购产品时都会调度该事件。
调度此事件时，你将传递一个可以访问已下订单的自定义事件实例。首先创建此自定义事件类并编写它::

    namespace Acme\Store\Event;

    use Acme\Store\Order;
    use Symfony\Contracts\EventDispatcher\Event;

    /**
     * 每次在系统中创建一个订单时，都会调度 order.placed 事件。
     */
    class OrderPlacedEvent extends Event
    {
        public const NAME = 'order.placed';

        protected $order;

        public function __construct(Order $order)
        {
            $this->order = $order;
        }

        public function getOrder()
        {
            return $this->order;
        }
    }

现在，每个监听器都可以通过 ``getOrder()`` 方法来访问订单。

.. note::

    如果你不需要将任何额外数据传递给事件监听器，则可以使用默认的
    :class:`Symfony\\Contracts\\EventDispatcher\\Event` 类。
    在这种情况下，你可以在一个通用的 ``StoreEvents`` 类中记录事件及其名称，类似于
    :class:`Symfony\\Component\\HttpKernel\\KernelEvents` 类。

调度事件
..................

:method:`Symfony\\Component\\EventDispatcher\\EventDispatcher::dispatch`
方法会通知给定事件的所有监听器。
它需要两个参数：要调度的事件的名称以及要传递给该事件的每个监听器的 ``Event`` 实例::

    use Acme\Store\Event\OrderPlacedEvent;
    use Acme\Store\Order;

    // 以某种方式创建或检索订单
    $order = new Order();
    // ...

    // 创建 OrderPlacedEvent 并调度它
    $event = new OrderPlacedEvent($order);
    $dispatcher->dispatch(OrderPlacedEvent::NAME, $event);

请注意，特定的 ``OrderPlacedEvent`` 对象已创建并传递给 ``dispatch()`` 方法。
现在，任何监听 ``order.placed`` 事件的监听器都会接收到 ``OrderPlacedEvent``。

.. index::
   single: EventDispatcher; Event subscribers

.. _event_dispatcher-using-event-subscribers:

使用事件订阅器
~~~~~~~~~~~~~~~~~~~~~~~

监听事件的最常用方法是向调度器注册一个 *事件监听器*。
此监听器可以监听一个或多个事件，并在每次这些事件被调度时得到通知。

监听事件的另一种方式是通过 *事件订阅器*。
事件订阅器是一个能够告诉调度器它应该订阅哪些事件的PHP类。它实现了一个需要一个名为
:method:`Symfony\\Component\\EventDispatcher\\EventSubscriberInterface::getSubscribedEvents`
的静态方法的
:class:`Symfony\\Component\\EventDispatcher\\EventSubscriberInterface`
接口。以订阅了 ``kernel.response`` 和 ``order.placed`` 事件的订阅器为例::

    namespace Acme\Store\Event;

    use Acme\Store\Event\OrderPlacedEvent;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\HttpKernel\Event\FilterResponseEvent;
    use Symfony\Component\HttpKernel\KernelEvents;

    class StoreSubscriber implements EventSubscriberInterface
    {
        public static function getSubscribedEvents()
        {
            return [
                KernelEvents::RESPONSE => [
                    ['onKernelResponsePre', 10],
                    ['onKernelResponsePost', -10],
                ],
                OrderPlacedEvent::NAME => 'onStoreOrder',
            ];
        }

        public function onKernelResponsePre(FilterResponseEvent $event)
        {
            // ...
        }

        public function onKernelResponsePost(FilterResponseEvent $event)
        {
            // ...
        }

        public function onStoreOrder(OrderPlacedEvent $event)
        {
            // ...
        }
    }

这与一个监听器类非常相似，除了它的类本身可以告诉调度器它应该监听哪些事件外。要向调度器注册订阅器，请使用
:method:`Symfony\\Component\\EventDispatcher\\EventDispatcher::addSubscriber` 方法::

    use Acme\Store\Event\StoreSubscriber;
    // ...

    $subscriber = new StoreSubscriber();
    $dispatcher->addSubscriber($subscriber);

调度器将自动为通过 ``getSubscribedEvents()`` 方法返回的每个事件注册该订阅器。
此方法返回由事件名称索引的数组，其值是要调用的方法名称或由要调用的方法名称和优先级（一个正或负整数，默认为 ``0``）组成的数组。

上面的示例展示了如何为订阅器中的同一事件注册多个监听器方法，还展示了如何传递每个监听器方法的优先级。
优先级数字越大，该方法调用越早。在上面的例子中，当 ``kernel.response``
事件被触发，``onKernelResponsePre()`` 和 ``onKernelResponsePost()`` 将按顺序被调用。

.. index::
   single: EventDispatcher; Stopping event flow

.. _event_dispatcher-event-propagation:

停止事件流/传播
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在某些情况下，一个监听器可能会阻止任何其他监听器被调用。
换句话说，该监听器需要能够告诉调度器停止将事件传播给后面的监听器（即不通知任何更多的监听器）的方法。
这可以通过
:method:`Symfony\\Component\\EventDispatcher\\Event::stopPropagation`
方法从监听器内部完成::

    use Acme\Store\Event\OrderPlacedEvent;

    public function onStoreOrder(OrderPlacedEvent $event)
    {
        // ...

        $event->stopPropagation();
    }

现在，任何尚未调用的监听 ``order.placed`` 的监听器都 *不会* 被调用。

可以通过使用返回布尔值的
:method:`Symfony\\Contracts\\EventDispatcher\\Event::isPropagationStopped`
方法来检测一个事件是否已停止::

    // ...
    $dispatcher->dispatch('foo.event', $event);
    if ($event->isPropagationStopped()) {
        // ...
    }

.. index::
   single: EventDispatcher; EventDispatcher aware events and listeners

.. _event_dispatcher-dispatcher-aware-events:

EventDispatcher感知事件和监听器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``EventDispatcher`` 始终传递被调度的事件、该事件的名称以及一个本身的引用到监听器。
这可能引出一些 ``EventDispatcher``
的高级应用，包括在监听器内部调度其他事件、事件链，甚至延迟加载监听器到调度器对象中。

.. index::
   single: EventDispatcher; Dispatcher shortcuts

.. _event_dispatcher-shortcuts:

调度器快捷方式
~~~~~~~~~~~~~~~~~~~~

如果你不需要一个自定义事件对象，则可以依赖一个原生的
:class:`Symfony\\Contracts\\EventDispatcher\\Event` 对象。
你甚至不需要将该对象传递给调度器，因为它将默认创建一个，除非你特别传递一个::

    $dispatcher->dispatch('order.placed');

此外，事件调度器始终返回调度的任何事件对象，即传递的事件或调度器内部创建的事件。
这就产生了一个友好的快捷方式::

    if (!$dispatcher->dispatch('foo.event')->isPropagationStopped()) {
        // ...
    }

或者::

    $event = new OrderPlacedEvent($order);
    $order = $dispatcher->dispatch('bar.event', $event)->getOrder();

等等。

.. index::
   single: EventDispatcher; Event name introspection

.. _event_dispatcher-event-name-introspection:

事件名称自省
~~~~~~~~~~~~~~~~~~~~~~~~

``EventDispatcher`` 实例以及被调度事件的名称，都会作为参数传递给监听器::

    use Symfony\Contracts\EventDispatcher\Event;
    use Symfony\Contracts\EventDispatcher\EventDispatcherInterface;

    class Foo
    {
        public function myEventListener(Event $event, $eventName, EventDispatcherInterface $dispatcher)
        {
            // ... 用事件名称做一些事情
        }
    }

其他调度器
-----------------

除了常用的 ``EventDispatcher``，本组件还附带一些其他调度器：

* :doc:`/components/event_dispatcher/container_aware_dispatcher`
* :doc:`/components/event_dispatcher/immutable_dispatcher`
* :doc:`/components/event_dispatcher/traceable_dispatcher`

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /components/event_dispatcher/*
    /event_dispatcher/*

* :ref:`kernel.event_listener标签 <dic-tags-kernel-event-listener>`
* :ref:`kernel.event_subscriber标签 <dic-tags-kernel-event-subscriber>`

.. _中介者: https://en.wikipedia.org/wiki/Mediator_pattern
.. _观察者: https://en.wikipedia.org/wiki/Observer_pattern
.. _闭包: https://php.net/manual/en/functions.anonymous.php
.. _PHP可调用对象: https://php.net/manual/en/language.pseudo-types.php#language.types.callback
.. _Packagist: https://packagist.org/packages/symfony/event-dispatcher
