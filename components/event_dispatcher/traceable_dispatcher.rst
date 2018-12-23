.. index::
    single: EventDispatcher; Debug
    single: EventDispatcher; Traceable

可追踪事件调度器
==============================

:class:`Symfony\\Component\\EventDispatcher\\Debug\\TraceableEventDispatcher`
是一个封装任何其他事件调度器的事件调度器，它可用于确定该调度器调用了哪些事件监听器。
将要封装的事件调度器及一个 :class:`Symfony\\Component\\Stopwatch\\Stopwatch` 实例传递给其构造函数::

    use Symfony\Component\EventDispatcher\Debug\TraceableEventDispatcher;
    use Symfony\Component\Stopwatch\Stopwatch;

    // 要调试的事件调度器
    $dispatcher = ...;

    $traceableEventDispatcher = new TraceableEventDispatcher(
        $dispatcher,
        new Stopwatch()
    );

现在，可以像任何其他事件调度器一样使用 ``TraceableEventDispatcher`` 来注册事件监听器以及调度事件::

    // ...

    // 注册一个事件监听器
    $eventListener = ...;
    $priority = ...;
    $traceableEventDispatcher->addListener(
        'event.the_name',
        $eventListener,
        $priority
    );

    // 调度一个事件
    $event = ...;
    $traceableEventDispatcher->dispatch('event.the_name', $event);

处理完你的应用之后，可以使用
:method:`Symfony\\Component\\EventDispatcher\\Debug\\TraceableEventDispatcherInterface::getCalledListeners`
方法来检索已在应用中调用的事件监听器数组。同样，
:method:`Symfony\\Component\\EventDispatcher\\Debug\\TraceableEventDispatcherInterface::getNotCalledListeners`
方法返回一个尚未调用的事件监听器数组::

    // ...

    $calledListeners = $traceableEventDispatcher->getCalledListeners();
    $notCalledListeners = $traceableEventDispatcher->getNotCalledListeners();
