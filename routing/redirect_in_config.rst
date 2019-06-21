.. index::
   single: Routing; Redirect using Framework:RedirectController

如何在没有自定义控制器的情况下配置重定向
=======================================================

有时，URL需要重定向到另一个URL。
你可以通过创建一个新的控制器动作来完成此操作，该动作的唯一任务是重定向。
但使用FrameworkBundle的
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\RedirectController` 会更加简单。

你可以重定向到一个指定路径（例如 ``/about``）或一个指定路由（使用其路由名称，例如 ``homepage``）。

使用路径重定向
------------------------

假设你的应用的 ``/`` 路径没有默认控制器，并且你希望将这些请求重定向到 ``/app``。
你需要使用
:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\RedirectController::urlRedirectAction`
动作来定向到此网址：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml

        # 加载一些路由 - 其中一个最终应该有 "/app" 路径
        controllers:
            resource: '../src/Controller/'
            type:     annotation
            prefix:   /app

        # 重定向到主页
        homepage:
            path: /
            controller: Symfony\Bundle\FrameworkBundle\Controller\RedirectController::urlRedirectAction
            defaults:
                path: /app
                permanent: true

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <!-- load some routes - one should ultimately have the path "/app" -->
            <import resource="../src/Controller/" type="annotation" prefix="/app"/>

            <!-- redirecting the homepage -->
            <route id="homepage"
                path="/"
                controller="Symfony\Bundle\FrameworkBundle\Controller\RedirectController::urlRedirectAction">
                <default key="path">/app</default>
                <default key="permanent">true</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Bundle\FrameworkBundle\Controller\RedirectController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            // load some routes - one should ultimately have the path "/app"
            $routes->import('../src/Controller/', 'annotation')
                ->prefix('/app')
            ;
            // redirecting the homepage
            $routes->add('homepage', '/')
                ->controller([RedirectController::class, 'urlRedirectAction'])
                ->defaults([
                    'path'      => '/app',
                    'permanent' => true,
                ])
            ;
        };

在此示例中，你为 ``/`` 路径配置了一个路由并让 ``RedirectController`` 将其重定向到 ``/app`` 路径。
``permanent`` 开关通知该动作创建一个 ``301`` HTTP状态代码，而不是默认的 ``302`` HTTP状态代码。

使用路由重定向
-------------------------

假设你要将网站从WordPress迁移到Symfony，你想要重定向 ``/wp-admin`` 到 ``sonata_admin_dashboard`` 路由。
但你不知道具体路径，只知道对应的路由名称。这可以通过
:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\RedirectController::redirectAction`
动作来实现：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml

        # ...

        admin:
            path: /wp-admin
            controller: Symfony\Bundle\FrameworkBundle\Controller\RedirectController::redirectAction
            defaults:
                route: sonata_admin_dashboard
                # 创建永久重定向...
                permanent: true
                # ...并保留原始查询字符串参数
                keepQueryParams: true

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <!-- ... -->

            <route id="admin"
                path="/wp-admin"
                controller="Symfony\Bundle\FrameworkBundle\Controller\RedirectController::redirectAction">
                <default key="route">sonata_admin_dashboard</default>
                <!-- make a permanent redirection... -->
                <default key="permanent">true</default>
                <!-- ...and keep the original query string parameters -->
                <default key="keepQueryParams">true</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Bundle\FrameworkBundle\Controller\RedirectController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            // redirecting the homepage
            $routes->add('admin', '/wp-admin')
                ->controller([RedirectController::class, 'redirectAction'])
                ->defaults([
                    'route' => 'sonata_admin_dashboard',
                    // make a permanent redirection...
                    'permanent' => true,
                    // ...and keep the original query string parameters
                    'keepQueryParams' => true,
                ])
            ;
        };

.. caution::

    由于你要重定向到路由而不是路径，因此在 ``redirect()`` 动作中所需的选项为
    ``route``，而不是 ``urlRedirect()`` 动作中的 ``path``。

重定向时保持请求方法
-------------------------------------------

前面示例中执行的重定向使用 ``301`` 和 ``302`` HTTP状态代码。
由于遗留原因，这些HTTP重定向会将 ``POST`` 请求方法更改为 ``GET`` （因为在旧浏览器中不能重定向一个 ``POST`` 请求）。

但是，在某些情况下，会预期或要求重定向的请求使用相同的HTTP方法。
这就是为什么HTTP标准定义了两个额外的状态代码（``307`` 和 ``308``）来执行维持原始请求方法的临时/永久重定向。

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\RedirectController::urlRedirectAction`
和 :method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\RedirectController::redirectAction`
方法都接受一个名为 ``keepRequestMethod`` 的额外参数。
当该参数设置为 ``true`` 时，临时重定向将使用 ``307`` 状态码，而不是
``302``；而永久重定向则使用 ``308`` 状态码来取代 ``301`` 状态码：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml

        # 使用308状态代码重定向
        route_foo:
            # ...
            controller: Symfony\Bundle\FrameworkBundle\Controller\RedirectController::redirectAction
            defaults:
                # ...
                permanent: true
                keepRequestMethod: true

        # 使用307状态代码重定向
        route_bar:
            # ...
            controller: Symfony\Bundle\FrameworkBundle\Controller\RedirectController::redirectAction
            defaults:
                # ...
                permanent: false
                keepRequestMethod: true

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <!-- redirects with the 308 status code -->
            <route id="route_foo"
                path="..."
                controller="Symfony\Bundle\FrameworkBundle\Controller\RedirectController::urlRedirectAction">
                <!-- ... -->
                <default key="permanent">true</default>
                <default key="keepRequestMethod">true</default>
            </route>

            <!-- redirects with the 307 status code -->
            <route id="route_bar"
                path="..."
                controller="Symfony\Bundle\FrameworkBundle\Controller\RedirectController::urlRedirectAction">
                <!-- ... -->
                <default key="permanent">false</default>
                <default key="keepRequestMethod">true</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\Route;
        use Symfony\Component\Routing\RouteCollection;

        $collection = new RouteCollection();

        // redirects with the 308 status code
        $collection->add('route_foo', new Route('...', [
            // ...
            '_controller'       => 'Symfony\Bundle\FrameworkBundle\Controller\RedirectController::urlRedirectAction',
            'permanent'         => true,
            'keepRequestMethod' => true,
        ]));

        // redirects with the 307 status code
        $collection->add('route_bar', new Route('...', [
            // ...
            '_controller'       => 'Symfony\Bundle\FrameworkBundle\Controller\RedirectController::urlRedirectAction',
            'permanent'         => false,
            'keepRequestMethod' => true,
        ]));

        return $collection;
