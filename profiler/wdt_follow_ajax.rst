.. index::
    single: Profiling: WDT Auto-update after AJAX Request

如何使Web调试工具栏在AJAX请求后自动更新
=================================================================

对于单页面应用，如果工具栏显示最新AJAX请求的信息而不是初始加载的页面，则会更方便。

通过将AJAX请求中的 ``Symfony-Debug-Toolbar-Replace`` 标头值设置为 ``1``，将为该请求自动重新加载工具栏。
该标头可以在响应对象上设置::

    $response->headers->set('Symfony-Debug-Toolbar-Replace', 1);

仅在开发期间设置标头
-------------------------------------------

理想情况下，此标头只应在开发期间设置，而不能用于生产环境。
这可以通过在一个
:ref:`kernel.response <component-http-kernel-kernel-response>`
事件监听器中设置该标头来完成::

    public function onKernelResponse(FilterResponseEvent $event)
    {
        $response = $event->getResponse();

        $response->headers->set('Symfony-Debug-Toolbar-Replace', 1);
    }

.. seealso::

    阅读更多关于 :doc:`Symfony events </reference/events>` 的信息。

如果你使用的是Symfony Flex，则应在 ``config/services_dev.yml``
文件中定义你的事件监听器服务，以使其仅存在于 ``dev`` 环境中。

.. seealso::

    更多关于
    :doc:`创建仅在开发环境下生效的服务 </configuration/configuration_organization>`
    的信息。
