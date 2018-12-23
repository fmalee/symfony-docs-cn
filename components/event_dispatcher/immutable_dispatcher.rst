.. index::
    single: EventDispatcher; Immutable

不可变事件调度器
==============================

:class:`Symfony\\Component\\EventDispatcher\\ImmutableEventDispatcher`
是一个锁定或冻结的事件调度器。该调度器无法注册新的监听器或订阅器。

``ImmutableEventDispatcher`` 将采用另一个事件调度器的所有监听器和订阅器。
该不可变调度器只是这个原始调度器的一个代理。

要使用它，首先要创建一个普通的 ``EventDispatcher`` 调度器，并注册一些监听器或订阅器::

    use Symfony\Component\EventDispatcher\EventDispatcher;

    $dispatcher = new EventDispatcher();
    $dispatcher->addListener('foo.action', function ($event) {
        // ...
    });

    // ...

然后将其注入到一个 ``ImmutableEventDispatcher`` 中::

    use Symfony\Component\EventDispatcher\ImmutableEventDispatcher;
    // ...

    $immutableDispatcher = new ImmutableEventDispatcher($dispatcher);

现在你可以在项目中使用这个新的调度器了。

如果你尝试执行一个会修改该调度器的方法（例如 ``addListener()``），则会抛出一个 ``BadMethodCallException``。
