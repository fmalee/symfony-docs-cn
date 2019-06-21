.. index::
   single: EventDispatcher

通用事件对象
========================

EventDispatcher组件提供了一个刻意保持简洁的 :class:`Symfony\\Contracts\\EventDispatcher\\Event`
基类，以允许通过使用OOP继承来创建特定事件对象的API。它让复杂应用中的代码保持优雅和可读。

:class:`Symfony\\Component\\EventDispatcher\\GenericEvent`
对于那些希望在整个应用中只使用一个事件对象的人来说，它是方便的。
它适用于大多数直接开箱即用的目的，因为它遵循标准的观察者(observer)模式，其中事件对象封装了一个事件“主题”，但添加了可选的额外参数。

除了 :class:`Symfony\\Contracts\\EventDispatcher\\Event`
基类之外，:class:`Symfony\\Component\\EventDispatcher\\GenericEvent` 还添加了一些方法：

* :method:`Symfony\\Component\\EventDispatcher\\GenericEvent::__construct`:
  构造函数接受事件主题和任何参数;

* :method:`Symfony\\Component\\EventDispatcher\\GenericEvent::getSubject`:
  获取主题;

* :method:`Symfony\\Component\\EventDispatcher\\GenericEvent::setArgument`:
  通过键来设置参数;

* :method:`Symfony\\Component\\EventDispatcher\\GenericEvent::setArguments`:
  设置参数数组;

* :method:`Symfony\\Component\\EventDispatcher\\GenericEvent::getArgument`:
  通过键来获取参数;

* :method:`Symfony\\Component\\EventDispatcher\\GenericEvent::getArguments`:
  获取所有参数;

* :method:`Symfony\\Component\\EventDispatcher\\GenericEvent::hasArgument`:
  如果给定的参数键存在，则返回 ``true``;

``GenericEvent`` 还在事件的参数上实现
:phpclass:`ArrayAccess`，这使得传递关于事件主题的额外参数变得非常方便。

以下示例展示了可以大致了解其灵活性的用例。这些示例假定对应事件监听器已添加到调度器中。

传递一个主题::

    use Symfony\Component\EventDispatcher\GenericEvent;

    $event = new GenericEvent($subject);
    $dispatcher->dispatch('foo', $event);

    class FooListener
    {
        public function handler(GenericEvent $event)
        {
            if ($event->getSubject() instanceof Foo) {
                // ...
            }
        }
    }

使用 :phpclass:`ArrayAccess` API来访问事件的参数，以传递和处理参数::

    use Symfony\Component\EventDispatcher\GenericEvent;

    $event = new GenericEvent(
        $subject,
        ['type' => 'foo', 'counter' => 0]
    );
    $dispatcher->dispatch('foo', $event);

    class FooListener
    {
        public function handler(GenericEvent $event)
        {
            if (isset($event['type']) && $event['type'] === 'foo') {
                // ... 做一些事情
            }

            $event['counter']++;
        }
    }

过滤数据::

    use Symfony\Component\EventDispatcher\GenericEvent;

    $event = new GenericEvent($subject, ['data' => 'Foo']);
    $dispatcher->dispatch('foo', $event);

    class FooListener
    {
        public function filter(GenericEvent $event)
        {
            $event['data'] = strtolower($event['data']);
        }
    }
