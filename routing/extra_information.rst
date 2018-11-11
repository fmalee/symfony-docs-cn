.. index::
   single: Routing; Extra Information

如何将路由中的额外信息传递给控制器
==========================================================

``defaults`` 集合中的参数不一定必须与路由 ``path`` 中的占位符匹配。
实际上，你可以使用 ``defaults`` 数组指定额外的参数，
然后可以将这些参数作为控制器的参数进行访问，同时也作为 ``Request`` 对象的属性：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        blog:
            path:       /blog/{page}
            controller: App\Controller\BlogController::index
            defaults:
                page: 1
                title: "Hello world!"

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog/{page}">
                <default key="_controller">App\Controller\BlogController::index</default>
                <default key="page">1</default>
                <default key="title">Hello world!</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'App\Controller\BlogController::index',
            'page'        => 1,
            'title'       => 'Hello world!',
        )));

        return $routes;

现在，你可以在控制器中将此额外参数作为该控制器方法的参数来进行访问::

    public function index($page, $title)
    {
        // ...
    }

或者，可以通过 ``Request`` 对象来访问该标题::

    use Symfony\Component\HttpFoundation\Request;

    public function index(Request $request, $page)
    {
        $title = $request->attributes->get('title');

        // ...
    }

如你所见，``$title`` 变量从未在路由路径中定义，但你仍然可以在控制器内部，通过该控制器方法的参数或 ``Request`` 对象的 ``attributes`` 包来访问其值。
