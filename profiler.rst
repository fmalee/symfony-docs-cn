分析器
========

分析器是一个强大的 **开发工具**，它能将任何请求的执行结果详细的展示出来。**不要**
在生产环境启用分析器，它会让你的项目出现一些安全隐患。

安装
------------

在应用中使用 :doc:`Symfony Flex </setup/flex>`，在使用前运行该命令安装分析器：

.. code-block:: terminal

    $ composer require --dev symfony/profiler-pack

现在，在开发环境中浏览应用的任何页面，以让分析器收集信息。
然后，单击页面底部注入的调试工具栏的任何元素，打开Symfony分析器的Web界面，类似于：

.. image:: /_images/profiler/web-interface.png
   :align: center
   :class: with-browser

以编程方式访问分析数据
-----------------------------------------

大多数情况下，使用基于Web界面来访问和分析分析器的信息。
但是，你还可以通过 ``profiler`` 服务提供的方法以编程方式检索分析信息。

当响应对象可用时，使用
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::loadProfileFromResponse`
方法访问其关联的配置文件::

    // ... $profiler 就是 'profiler' 服务
    $profile = $profiler->loadProfileFromResponse($response);

当分析器存储有关一个请求的数据时，它还会将一个令牌与其关联;
此令牌在响应的 ``X-Debug-Token`` HTTP标头中可用。
使用此令牌，你可以通过
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::loadProfile`
方法访问任何过去响应的配置文件::

    $token = $response->headers->get('X-Debug-Token');
    $profile = $profiler->loadProfile($token);

.. tip::

    启用分析器但未启用Web调试工具栏时，请使用浏览器的”开发人员工具”检查页面以获取 ``X-Debug-Token`` HTTP标头的值。

``profiler`` 服务还提供了基于某些标准查找令牌的
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::find` 方法::

    // 获取最新的10个令牌
    $tokens = $profiler->find('', '', 10, '', '', '');

    // 获取包含 /admin/ 的所有URL的最新的10个令牌
    $tokens = $profiler->find('', '/admin/', 10, '', '', '');

    // 获取本地POST请求的最新10个令牌
    $tokens = $profiler->find('127.0.0.1', '', 10, 'POST', '', '');

    // 获取2到4天前发生的请求的最近的10个令牌
    $tokens = $profiler->find('', '', 10, '', '4 days ago', '2 days ago');

数据收集器
---------------

分析器使用一些名为“数据收集器”的服务获取其信息。
Symfony附带了几个收集器，可以获取有关请求、日志、路由、缓存等的信息。

运行此命令以获取应用中实际启用的收集器列表：

.. code-block:: terminal

    $ php bin/console debug:container --tag=data_collector

你还可以 :doc:`创建自己的数据收集器 </profiler/data_collector>`
来存储应用生成的任何数据，并将其显示在调试工具栏和分析器Web界面中。

.. _profiler-timing-execution:

定时执行应用
---------------------------------------

如果要计算某些任务在应用中的时间，并不需要创建自定义数据收集器。
而是使用 `Stopwatch组件`_ ，该组件提供实用工具来分析代码并在分析器的Web界面的“性能”面板上显示结果。

使用 :ref:`动装配 <services-autowire>` 时，类型约束任何带有
:class:`Symfony\\Component\\Stopwatch\\Stopwatch` 类的参数，Symfony将注入秒表服务。
然后，使用 ``start()``、``lapse()`` 以及 ``stop()`` 方法来衡量时间::

    // 用户注册并启动计时器...
    $stopwatch->start('user-sign-up');

    // ...做一些事情来注册用户...
    $stopwatch->lapse('user-sign-up');

    // ...注册过程结束
    $stopwatch->stop('user-sign-up');

.. tip::

    可以考虑使用一个专业的分析器（如 `Blackfire`_）来详细测量和分析应用的执行情况。

有条件地启用分析器
-----------------------------------

.. caution::

    在Symfony 4.0中删除了使用匹配器有条件地启用分析器的可能性。

现在无法使用匹配器有条件地启用/禁用Symfony分析器，因为在Symfony 4.0中删除了该功能。
但是，你可以在控制器中使用 :class:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler`
类的 ``enable()`` 和 ``disable()`` 方法以编程方式管理分析器::

    use Symfony\Component\HttpKernel\Profiler\Profiler;
    // ...

    class DefaultController
    {
        // ...

        public function someMethod(?Profiler $profiler)
        {
            // 如果你的环境没有分析器（如默认下的 ``prod``），则不会设置 $profiler
            if (null !== $profiler) {
                // 如果分析器存在，则禁用此特定控制器动作中的分析器
                $profiler->disable();
            }

            // ...
        }
    }

为了将分析器注入到控制器，你需要创建指向现有 ``profiler`` 服务的一个别名：

.. configuration-block::

    .. code-block:: yaml

        # config/services_dev.yaml
        services:
            Symfony\Component\HttpKernel\Profiler\Profiler: '@profiler'

    .. code-block:: xml

        <!-- config/services_dev.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Symfony\Component\HttpKernel\Profiler\Profiler" alias="profiler"/>
            </services>
        </container>

    .. code-block:: php

        // config/services_dev.php
        use Symfony\Component\HttpKernel\Profiler\Profiler;

        $container->setAlias(Profiler::class, 'profiler');

使Web调试工具栏在AJAX请求后自动更新
--------------------------------------------------

`单页面应用`_ （SPA）是通过动态重写当前页面而不是从服务器加载整个新页面来与用户交互的Web应用。

默认情况下，调试工具栏显示初始页面加载的信息，可是在每个AJAX请求后不会刷新。
但是，你可以在对AJAX请求的响应中将 ``Symfony-Debug-Toolbar-Replace``
标头设置为 ``1``，以强制刷新工具栏::

    $response->headers->set('Symfony-Debug-Toolbar-Replace', 1);

理想情况下，此标头只应在开发期间设置，而不能用于生产。为此，请创建一个 :doc:`事件订阅器 </event_dispatcher>`
并监听 :ref:`kernel.response<component-http-kernel-kernel-response>` 事件::

    use Symfony\Component\HttpKernel\Event\FilterResponseEvent;

    // ...

    public function onKernelResponse(FilterResponseEvent $event)
    {
        if (!$this->getKernel()->isDebug()) {
            return;
        }

        $response = $event->getResponse();
        $response->headers->set('Symfony-Debug-Toolbar-Replace', 1);
    }

.. toctree::
    :hidden:

    profiler/data_collector

.. _`单页面应用`: https://en.wikipedia.org/wiki/Single-page_application
.. _`Stopwatch组件`: https://symfony.com/components/Stopwatch
.. _`Blackfire`: https://blackfire.io/docs/introduction?utm_source=symfony&utm_medium=symfonycom_docs&utm_campaign=profiler
