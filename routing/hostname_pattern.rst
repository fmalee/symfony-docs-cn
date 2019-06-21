.. index::
   single: Routing; Matching on Hostname

如何匹配基于主机的路由
======================================

你还可以使用传入请求的HTTP的 *host* 来匹配任何路由。

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/MainController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class MainController extends AbstractController
        {
            /**
             * @Route("/", name="mobile_homepage", host="m.example.com")
             */
            public function mobileHomepage()
            {
                // ...
            }

            /**
             * @Route("/", name="homepage")
             */
            public function homepage()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        mobile_homepage:
            path:       /
            host:       m.example.com
            controller: App\Controller\MainController::mobileHomepage

        homepage:
            path:       /
            controller: App\Controller\MainController::homepage

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="mobile_homepage"
                path="/"
                host="m.example.com"
                controller="App\Controller\MainController::mobileHomepage"/>

            <route id="homepage" path="/" controller="App\Controller\MainController::homepage"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\MainController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('mobile_homepage', '/')
                ->controller([MainController::class, 'mobileHomepage'])
                ->host('m.example.com')
            ;
            $routes->add('homepage', '/')
                ->controller([MainController::class, 'homepage'])
            ;
        };

        return $routes;

两条路由都匹配相同的 ``/`` 路径，但第一条路由仅在主机是 ``m.example.com`` 时匹配。

使用占位符
------------------

``host`` 选项使用与路径匹配系统相同的语法。这意味着你可以在主机名称中使用占位符：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/MainController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class MainController extends AbstractController
        {
            /**
             * @Route("/", name="projects_homepage", host="{project}.example.com")
             */
            public function projectsHomepage(string $project)
            {
                // ...
            }

            /**
             * @Route("/", name="homepage")
             */
            public function homepage()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        projects_homepage:
            path:       /
            host:       "{project}.example.com"
            controller: App\Controller\MainController::projectsHomepage

        homepage:
            path:       /
            controller: App\Controller\MainController::homepage

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="projects_homepage"
                path="/"
                host="{project}.example.com"
                controller="App\Controller\MainController::projectsHomepage"/>

            <route id="homepage" path="/" controller="App\Controller\MainController::homepage"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\MainController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('project_homepage', '/')
                ->controller([MainController::class, 'projectHomepage'])
                ->host('{project}.example.com')
            ;
            $routes->add('homepage', '/')
                ->controller([MainController::class, 'homepage'])
            ;
        };

此外，可以为这些占位符设置任何要求或默认值。
举例来说，如果想同时匹配 ``m.example.com`` 和 ``mobile.example.com``，你可以这样：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/MainController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class MainController extends AbstractController
        {
            /**
             * @Route(
             *     "/",
             *     name="mobile_homepage",
             *     host="{subdomain}.example.com",
             *     defaults={"subdomain"="m"},
             *     requirements={"subdomain"="m|mobile"}
             * )
             */
            public function mobileHomepage()
            {
                // ...
            }

            /**
             * @Route("/", name="homepage")
             */
            public function homepage()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        mobile_homepage:
            path:       /
            host:       "{subdomain}.example.com"
            controller: App\Controller\MainController::mobileHomepage
            defaults:
                subdomain: m
            requirements:
                subdomain: m|mobile

        homepage:
            path:       /
            controller: App\Controller\MainController::homepage

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="mobile_homepage"
                path="/"
                host="{subdomain}.example.com"
                controller="App\Controller\MainController::mobileHomepage">
                <default key="subdomain">m</default>
                <requirement key="subdomain">m|mobile</requirement>
            </route>

            <route id="homepage" path="/" controller="App\Controller\MainController::homepage"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\MainController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('mobile_homepage', '/')
                ->controller([MainController::class, 'mobileHomepage'])
                ->host('{subdomain}.example.com')
                ->defaults([
                    'subdomain' => 'm',
                ])
                ->requirements([
                    'subdomain' => 'm|mobile',
                ])
            ;
            $routes->add('homepage', '/')
                ->controller([MainController::class, 'homepage'])
            ;
        };

.. tip::

    如果你不想对主机名进行硬编码，则可以使用服务参数：

    .. configuration-block::

        .. code-block:: php-annotations

            // src/Controller/MainController.php
            namespace App\Controller;

            use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
            use Symfony\Component\Routing\Annotation\Route;

            class MainController extends AbstractController
            {
                /**
                 * @Route(
                 *     "/",
                 *     name="mobile_homepage",
                 *     host="m.{domain}",
                 *     defaults={"domain"="%domain%"},
                 *     requirements={"domain"="%domain%"}
                 * )
                 */
                public function mobileHomepage()
                {
                    // ...
                }

                /**
                 * @Route("/", name="homepage")
                 */
                public function homepage()
                {
                    // ...
                }
            }

        .. code-block:: yaml

            # config/routes.yaml
            mobile_homepage:
                path:       /
                host:       "m.{domain}"
                controller: App\Controller\MainController::mobileHomepage
                defaults:
                    domain: '%domain%'
                requirements:
                    domain: '%domain%'

            homepage:
                path:       /
                controller: App\Controller\MainController::homepage

        .. code-block:: xml

            <!-- config/routes.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <routes xmlns="http://symfony.com/schema/routing"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/routing
                    https://symfony.com/schema/routing/routing-1.0.xsd">

                <route id="mobile_homepage"
                    path="/"
                    host="m.{domain}"
                    controller="App\Controller\MainController::mobileHomepage">
                    <default key="domain">%domain%</default>
                    <requirement key="domain">%domain%</requirement>
                </route>

                <route id="homepage" path="/" controller="App\Controller\MainController::homepage"/>
            </routes>

        .. code-block:: php

            // config/routes.php
            use App\Controller\MainController;
            use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

            return function (RoutingConfigurator $routes) {
                $routes->add('mobile_homepage', '/')
                    ->controller([MainController::class, 'mobileHomepage'])
                    ->host('m.{domain}')
                    ->defaults([
                        'domain' => '%domain%',
                    ])
                    ->requirements([
                        'domain' => '%domain%',
                    ])
                ;
                $routes->add('homepage', '/')
                    ->controller([MainController::class, 'homepage'])
                ;
            };

.. tip::

    确保你还为 ``domain`` 占位符包含了默认选项，否则每次使用该路由生成URL时都需要包含一个域值。

.. _component-routing-host-imported:

使用导入的路由的主机匹配
--------------------------------------

你还可以在导入的路由上设置主机选项：

.. configuration-block::

    .. code-block:: php-annotations

        // vendor/acme/acme-hello-bundle/src/Controller/MainController.php
        namespace Acme\AcmeHelloBundle\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        /**
         * @Route(host="hello.example.com")
         */
        class MainController extends AbstractController
        {
            // ...
        }

    .. code-block:: yaml

        # config/routes.yaml
        app_hello:
            resource: '@AcmeHelloBundle/Resources/config/routing.yaml'
            host:     "hello.example.com"

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@AcmeHelloBundle/Resources/config/routing.xml" host="hello.example.com"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->import("@AcmeHelloBundle/Resources/config/routing.php")
                ->host('hello.example.com')
            ;
        };

这将在从新路由资源加载的每个路由上设置 ``hello.example.com`` 主机。

测试控制器
------------------------

如果要在功能测试中获取于此匹配的URL，则需要在请求对象上设置HTTP_HOST标头::

    $crawler = $client->request(
        'GET',
        '/',
        [],
        [],
        ['HTTP_HOST' => 'm.' . $client->getContainer()->getParameter('domain')]
    );
