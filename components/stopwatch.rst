.. index::
   single: Stopwatch
   single: Components; Stopwatch

Stopwatch组件
=======================

    Stopwatch组件提供了一种分析代码的方法。

安装
------------

.. code-block:: terminal

    $ composer require symfony/stopwatch

.. include:: /components/require_autoload.rst.inc

用法
-----

Stopwatch组件提供了一种一致的方法来统计代码的某些部分的执行时间，这样你可以直接使用
:class:`Symfony\\Component\\Stopwatch\\Stopwatch` 类而不必经常自己解析 ``microtime``::

    use Symfony\Component\Stopwatch\Stopwatch;

    $stopwatch = new Stopwatch();
    // 开启一个名为 'eventName' 的事件
    $stopwatch->start('eventName');
    // ... 这里是一些代码
    $event = $stopwatch->stop('eventName');

:class:`Symfony\\Component\\Stopwatch\\StopwatchEvent` 对象可以从
:method:`Symfony\\Component\\Stopwatch\\Stopwatch::start`、
:method:`Symfony\\Component\\Stopwatch\\Stopwatch::stop`、
:method:`Symfony\\Component\\Stopwatch\\Stopwatch::lap` 以及
:method:`Symfony\\Component\\Stopwatch\\Stopwatch::getEvent` 方法中检索到。
当你需要在事件仍在运行时检索事件的持续时间，应使用后面的方法。

.. tip::

    默认情况下，Stopwatch会将任何亚毫秒时间度量截断为 ``0``，因此你无法测量微秒或纳秒。
    如果需要更高的精度，请传递 ``true`` 给 ``Stopwatch`` 类的构造函数以启用完全精度::

        $stopwatch = new Stopwatch(true);

使用 :method:`Symfony\\Component\\Stopwatch\\Stopwatch::reset`
方法可以在任何给定时间将Stopwatch重置为其原始状态，从而删除到目前为止测量的所有数据。

你还可以为一个事件提供分类名称::

    $stopwatch->start('eventName', 'categoryName');

你可以将分类视为标记事件的一个方式。例如，Symfony分析器工具使用类别以对不同事件进行很好的颜色编码。

.. tip::

    阅读 :ref:`这篇文档 <profiler-timing-execution>`
    以了解有关将Stopwatch组件集成到Symfony分析器中的更多信息。

周期
-------

从现实世界来看，所有秒表都有两个按钮：一个用于启动和停止秒表，另一个用于测量圈数。这正是
:method:`Symfony\\Component\\Stopwatch\\Stopwatch::lap` 方法的作用::

    $stopwatch = new Stopwatch();
    // 开启一个名为 'foo' 的事件
    $stopwatch->start('foo');
    // ... 这里是一些代码
    $stopwatch->lap('foo');
    // ... 这里是一些代码
    $stopwatch->lap('foo');
    // ... 这里是一些其他代码
    $event = $stopwatch->stop('foo');

圈数(Lap)信息在事件中存储为“周期(period)”。要获取圈数信息，可以调用::

    $event->getPeriods();

除了period，你还可以从事件对象中获取其他有用信息。例如::

    $event->getCategory();   // 返回事件开始的类别
    $event->getOrigin();     // 以毫秒为单位返回事件开始的时间
    $event->ensureStopped(); // 停止所有尚未停止的周期
    $event->getStartTime();  // 返回第一个周期的开始时间
    $event->getEndTime();    // 返回最后一个周期的结束时间
    $event->getDuration();   // 返回事件的持续时间，包括所有周期
    $event->getMemory();     // 返回所有周期的最大内存使用量

切片
-------

切片(Section)是一种逻辑上将时间线分组的方法。
你可以看到Symfony如何在Symfony分析器工具中使用section来很好地可视化框架的生命周期。
以下是使用切片的基本用法示例::

    $stopwatch = new Stopwatch();

    $stopwatch->openSection();
    $stopwatch->start('parsing_config_file', 'filesystem_operations');
    $stopwatch->stopSection('routing');

    $events = $stopwatch->getSectionEvents('routing');

你可以通过调用 :method:`Symfony\\Component\\Stopwatch\\Stopwatch::openSection`
方法并指定切片的ID来重新打开已关闭的切片::

    $stopwatch->openSection('routing');
    $stopwatch->start('building_config_tree');
    $stopwatch->stopSection('routing');

.. _Packagist: https://packagist.org/packages/symfony/stopwatch
