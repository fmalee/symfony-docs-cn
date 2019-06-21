.. index::
   single: Templating; Render template without custom controller

如何在没有自定义控制器的情况下渲染模板
====================================================

通常，当你需要创建页面时，需要创建一个控制器并从该控制器中渲染模板。
但是，如果你要渲染一个不需要传递任何数据的简单模板，则可以通过使用内置
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\TemplateController`
控制器来避免创建完全的控制器。

例如，假设你要渲染一个 ``static/privacy.html.twig`` 模板，该模板不需要传递任何变量。
你则无需创建控制器即可完成此操作：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        acme_privacy:
            path:         /privacy
            controller:   Symfony\Bundle\FrameworkBundle\Controller\TemplateController
            defaults:
                template: static/privacy.html.twig
            methods: GET

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="acme_privacy"
                path="/privacy"
                controller="Symfony\Bundle\FrameworkBundle\Controller\TemplateController"
                methods="GET">
                <default key="template">static/privacy.html.twig</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Bundle\FrameworkBundle\Controller\TemplateController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('acme_privacy', '/privacy')
                ->controller(TemplateController::class)
                ->methods(['GET'])
                ->defaults([
                    'template'  => 'static/privacy.html.twig',
                ])
            ;
        };

``TemplateController`` 会将你传递的任何模板渲染为 ``template`` 默认值。

在模板中渲染嵌入式控制器时，你也可以使用此技巧。
但是，由于在模板中渲染一个控制器的目的通常是在自定义控制器中准备一些数据，因此这可能仅在你要缓存此页面的部分时才有用
（请参阅 :ref:`templating-no-controller-caching`）。

.. code-block:: twig

    {{ render(url('acme_privacy')) }}

.. _templating-no-controller-caching:

缓存静态模板
---------------------------

由于以这种方式渲染的模板通常是静态的，因此缓存它们应该是有意义的。
幸运的是，你可以配置路由以准确控制页面的缓存方式：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        acme_privacy:
            path:          /privacy
            controller:    Symfony\Bundle\FrameworkBundle\Controller\TemplateController
            defaults:
                template:  'static/privacy.html.twig'
                maxAge:    86400
                sharedAge: 86400
            methods: GET

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="acme_privacy"
                path="/privacy"
                controller="Symfony\Bundle\FrameworkBundle\Controller\TemplateController"
                methods="GET">
                <default key="template">static/privacy.html.twig</default>
                <default key="maxAge">86400</default>
                <default key="sharedAge">86400</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Bundle\FrameworkBundle\Controller\TemplateController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('acme_privacy', '/privacy')
                ->controller(TemplateController::class)
                ->methods(['GET'])
                ->defaults([
                    'template'  => 'static/privacy.html.twig',
                    'maxAge'    => 86400,
                    'sharedAge' => 86400,
                ])
            ;
        };

``maxAge`` 和 ``sharedAge`` 的值被用于修改在控制器中创建的响应对象。
有关缓存的更多信息，请参阅 :doc:`/http_cache`。

还有一个 ``private`` 变量（示例中没有显示）。
默认情况下，只要传递 ``maxAge`` 或 ``sharedAge``，响应将被设置为公有。
如果该变量设置为 ``true``，则响应将被标记为私有。
