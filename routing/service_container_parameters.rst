.. index::
   single: Routing; Service Container Parameters

如何在路由中使用服务容器参数
======================================================

有时你可能会发现使路由的某些部分全局可配置很有用。
例如，如果你构建国际化站点，则可能从一个或两个语言环境开始。
当然，你将向路由添加一个要求(requirement)，以防止用户匹配你支持之外的语言环境。

你 *可以* 在所有路由中硬编码你的 ``_locale`` 要求，但更好的解决方案是在路由配置中使用可配置的服务容器参数：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        contact:
            path:       /{_locale}/contact
            controller: App\Controller\MainController::contact
            requirements:
                _locale: '%app.locales%'

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" path="/{_locale}/contact">
                <default key="_controller">App\Controller\MainController::contact</default>
                <requirement key="_locale">%app.locales%</requirement>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('contact', new Route('/{_locale}/contact', array(
            '_controller' => 'App\Controller\MainController::contact',
        ), array(
            '_locale' => '%app.locales%',
        )));

        return $routes;

你现在可以在容器中的某个位置控制和设置 ``app.locales`` 参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            app.locales: en|es

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="app.locales">en|es</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('app.locales', 'en|es');

你还可以使用一个参数来定义路由路径（或路径的一部分）：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        some_route:
            path:       /%app.route_prefix%/contact
            controller: App\Controller\MainController::contact

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="some_route" path="/%app.route_prefix%/contact">
                <default key="_controller">App\Controller\MainController::contact</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('some_route', new Route('/%app.route_prefix%/contact', array(
            '_controller' => 'App\Controller\MainController::contact',
        )));

        return $routes;

.. note::

    就像在普通的服务容器配置文件中一样，如果你真的在路由中需要一个 ``%``，
    你可以通过加倍百分号来转义它，例如 ``/score-50%%`` 将解析为 ``/score-50%``。

    但是，由于任何URL中包含的 ``%`` 字符都是自动编码的，因此该示例的最终URL是 ``/score-50%25``（``%25`` 是对 ``%`` 字符进行编码的结果）。

.. seealso::

    有关依赖注入类中的参数处理，请参阅 :doc:`/configuration/using_parameters_in_dic`。
