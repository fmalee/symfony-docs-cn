.. index::
   single: Routing; Scheme requirement

如何强制路由始终使用HTTPS或HTTP
===============================================

有时，你希望保护某些路由，并确保始终通过HTTPS协议访问它们。
Routing组件允许你通过 ``schemes`` 强制执行URI scheme：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/MainController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class MainController extends AbstractController
        {
            /**
             * @Route("/secure", name="secure", schemes={"https"})
             */
            public function secure()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        secure:
            path:       /secure
            controller: App\Controller\MainController::secure
            schemes:    [https]

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="secure" path="/secure" schemes="https">
                <default key="_controller">App\Controller\MainController::secure</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('secure', new Route('/secure', array(
            '_controller' => 'App\Controller\MainController::secure',
        ), array(), array(), '', array('https')));

        return $routes;

以上配置强制 ``secure`` 路由始终使用HTTPS。

生成 ``secure`` URL时，如果当前scheme是HTTP，
Symfony将自动生成一个使用HTTPS作为scheme的绝对URL，即使使用了 ``path()`` 函数：

.. code-block:: twig

    {# 如果当前 scheme 是 HTTPS #}
    {{ path('secure') }}
    {# 生成一个相对URL: /secure #}

    {# 如果当前 scheme 是 HTTP #}
    {{ path('secure') }}
    {# 生成一个绝对URL: https://example.com/secure #}

对传入请求也强制执行该要求。
如果你尝试使用HTTP访问 ``/secure`` 路径，你将自动重定向到使用HTTPS scheme的相同URL。

上面的示例使用 ``https`` 作为 scheme，但你也可以强制一个URL始终使用 ``http``。

.. note::

    Security组件提供了另一种通过 ``requires_channel`` 设置强制执行HTTP或HTTPS的方法。
    此替代方法更适合保护网站的一个“区域”（``/admin`` 下的所有URL）或你希望保护在第三方bundle中定义的URL
    （有关详细信息，请参阅 :doc:`/security/force_https`）。
