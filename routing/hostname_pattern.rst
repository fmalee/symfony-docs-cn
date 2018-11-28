.. index::
   single: Routing; Matching on Hostname

如何匹配基于主机的路由
======================================

你还可以匹配传入请求中的HTTP *主机*。

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
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="mobile_homepage" path="/" host="m.example.com">
                <default key="_controller">App\Controller\MainController::mobileHomepage</default>
            </route>

            <route id="homepage" path="/">
                <default key="_controller">App\Controller\MainController::homepage</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('mobile_homepage', new Route('/', array(
            '_controller' => 'App\Controller\MainController::mobileHomepage',
        ), array(), array(), 'm.example.com'));

        $routes->add('homepage', new Route('/', array(
            '_controller' => 'App\Controller\MainController::homepage',
        )));

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
             * @Route("/", name="projects_homepage", host="{project_name}.example.com")
             */
            public function projectsHomepage()
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
            host:       "{project_name}.example.com"
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
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="projects_homepage" path="/" host="{project_name}.example.com">
                <default key="_controller">App\Controller\MainController::projectsHomepage</default>
            </route>

            <route id="homepage" path="/">
                <default key="_controller">App\Controller\MainController::homepage</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('project_homepage', new Route('/', array(
            '_controller' => 'App\Controller\MainController::projectsHomepage',
        ), array(), array(), '{project_name}.example.com'));

        $routes->add('homepage', new Route('/', array(
            '_controller' => 'App\Controller\MainController::homepage',
        )));

        return $routes;

你还可以为这些占位符设置要求和默认选项。
举例来说，如果你想同时匹配 ``m.example.com`` 和 ``mobile.example.com``，可以这样：

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
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="mobile_homepage" path="/" host="{subdomain}.example.com">
                <default key="_controller">App\Controller\MainController::mobileHomepage</default>
                <default key="subdomain">m</default>
                <requirement key="subdomain">m|mobile</requirement>
            </route>

            <route id="homepage" path="/">
                <default key="_controller">App\Controller\MainController::homepage</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('mobile_homepage', new Route('/', array(
            '_controller' => 'App\Controller\MainController::mobileHomepage',
            'subdomain'   => 'm',
        ), array(
            'subdomain' => 'm|mobile',
        ), array(), '{subdomain}.example.com'));

        $routes->add('homepage', new Route('/', array(
            '_controller' => 'App\Controller\MainController::homepage',
        )));

        return $routes;

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
                    http://symfony.com/schema/routing/routing-1.0.xsd">

                <route id="mobile_homepage" path="/" host="m.{domain}">
                    <default key="_controller">App\Controller\MainController::mobileHomepage</default>
                    <default key="domain">%domain%</default>
                    <requirement key="domain">%domain%</requirement>
                </route>

                <route id="homepage" path="/">
                    <default key="_controller">App\Controller\MainController::homepage</default>
                </route>
            </routes>

        .. code-block:: php

            // config/routes.php
            use Symfony\Component\Routing\RouteCollection;
            use Symfony\Component\Routing\Route;

            $routes = new RouteCollection();
            $routes->add('mobile_homepage', new Route('/', array(
                '_controller' => 'App\Controller\MainController::mobileHomepage',
                'domain' => '%domain%',
            ), array(
                'domain' => '%domain%',
            ), array(), 'm.{domain}'));

            $routes->add('homepage', new Route('/', array(
                '_controller' => 'App\Controller\MainController::homepage',
            )));

            return $routes;

.. tip::

    确保你还为 ``domain`` 占位符包含了默认选项，否则每次使用该路由生成URL时都需要包含一个域值。

.. _component-routing-host-imported:

使用导入的路由的主机匹配
--------------------------------------

你还可以在导入的路由上设置主机选项：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/MainController.php
        namespace App\Controller;

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
            resource: '@ThirdPartyBundle/Resources/config/routing.yaml'
            host:     "hello.example.com"

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@ThirdPartyBundle/Resources/config/routing.xml" host="hello.example.com" />
        </routes>

    .. code-block:: php

        // config/routes.php
        $routes = $loader->import("@ThirdPartyBundle/Resources/config/routing.php");
        $routes->setHost('hello.example.com');

        return $routes;

这将在从新路由资源加载的每个路由上设置 ``hello.example.com`` 主机。

测试控制器
------------------------

如果要在功能测试中获取于此匹配的URL，则需要在请求对象上设置HTTP_HOST标头::

    $crawler = $client->request(
        'GET',
        '/homepage',
        array(),
        array(),
        array('HTTP_HOST' => 'm.' . $client->getContainer()->getParameter('domain'))
    );
