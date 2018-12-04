.. index::
    single: Routing; Conditions

如何通过条件限制路由匹配
=================================================

可以使路由仅匹配某些路由占位符（通过正则表达式）、HTTP方法或主机名。
如果你需要更灵活地定义任意匹配逻辑，请使用 ``conditions`` 路由选项：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/DefaultController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class DefaultController extends AbstractController
        {
            /**
             * @Route(
             *     "/contact",
             *     name="contact",
             *     condition="context.getMethod() in ['GET', 'HEAD'] and request.headers.get('User-Agent') matches '/firefox/i'"
             * )
             *
             * 表达式还可以包含配置参数
             * 条件: "request.headers.get('User-Agent') matches '%app.allowed_browsers%'"
             */
            public function contact()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        contact:
            path:       /contact
            controller: 'App\Controller\DefaultController::contact'
            condition:  "context.getMethod() in ['GET', 'HEAD'] and request.headers.get('User-Agent') matches '/firefox/i'"
            # expressions can also include config parameters
            # condition: "request.headers.get('User-Agent') matches '%app.allowed_browsers%'"

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" path="/contact">
                <default key="_controller">App\Controller\DefaultController::contact</default>
                <condition>context.getMethod() in ['GET', 'HEAD'] and request.headers.get('User-Agent') matches '/firefox/i'</condition>
                <!-- expressions can also include config parameters -->
                <!-- <condition>request.headers.get('User-Agent') matches '%app.allowed_browsers%'</condition> -->
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('contact', new Route(
            '/contact', array(
                '_controller' => 'App\Controller\DefaultController::contact',
            ),
            array(),
            array(),
            '',
            array(),
            array(),
            'context.getMethod() in ["GET", "HEAD"] and request.headers.get("User-Agent") matches "/firefox/i"'
            // expressions can also include config parameters
            // 'request.headers.get("User-Agent") matches "%app.allowed_browsers%"'
        ));

        return $collection;

``condition`` 是一个表达式，你可以在此处了解有关其语法的更多信息：
:doc:`/components/expression_language/syntax`。
由此，此路由将一直不匹配，直到HTTP方法是GET或HEAD，并且 ``User-Agent`` 标头和 ``firefox`` 相匹配。

你可以通过利用传递到表达式中的两个变量来执行表达式中所需的任何复杂逻辑：

``context``
    一个 :class:`Symfony\\Component\\Routing\\RequestContext` 的实例，其中包含有关被匹配路由的最基本信息。
``request``
    Symfony :class:`Symfony\\Component\\HttpFoundation\\Request` 对象
    （请参阅 :ref:`component-http-foundation-request`）。

.. caution::

    生成URL时 *不会* 考虑这些条件。

.. sidebar:: 表达式编译为PHP

    在幕后，表达式被编译为原生PHP。我们的示例将在缓存目录中生成以下PHP::

        if (rtrim($pathInfo, '/contact') === '' && (
            in_array($context->getMethod(), array(0 => "GET", 1 => "HEAD"))
            && preg_match("/firefox/i", $request->headers->get("User-Agent"))
        )) {
            // ...
        }

    因此，使用 ``condition`` 键不会导致超出底层PHP执行所需的时间的额外开销。
