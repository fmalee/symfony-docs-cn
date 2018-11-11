.. index::
    single: Routing; Optional Placeholders

如何定义可选的占位符
===================================

为了使事情更令人兴奋，请添加一个新路由，以显示此虚构博客应用的所有可用博客帖子的列表：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogController.php

        // ...
        class BlogController extends AbstractController
        {
            // ...

            /**
             * @Route("/blog")
             */
            public function index()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog:
            path:       /blog
            controller: App\Controller\BlogController::index

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog">
                <default key="_controller">App\Controller\BlogController::index</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('blog', new Route('/blog', array(
            '_controller' => 'App\Controller\BlogController::index',
        )));

        return $routes;

到目前为止，这个路由尽可能简单 - 它不包含占位符，只匹配确切的 ``/blog`` URL。
但是，如果你需要这个路由支持分页，比如 ``/blog/2`` 显示博客条目的第二页呢？
更新路由以获得新的 ``{page}`` 占位符：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogController.php

        // ...

        /**
         * @Route("/blog/{page}")
         */
        public function index($page)
        {
            // ...
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog:
            path:       /blog/{page}
            controller: App\Controller\BlogController::index

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog" path="/blog/{page}">
                <default key="_controller">App\Controller\BlogController::index</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('blog', new Route('/blog/{page}', array(
            '_controller' => 'App\Controller\BlogController::index',
        )));

        return $routes;

与之前的 ``{slug}`` 占位符一样，匹配 ``{page}`` 的值将在你的控制器中可用。
它的值可用于确定要为给定页面显示的博客帖子集合。

但还需要继续！由于默认情况下需要占位符，因此该路由将不再匹配 ``/blog``。
同样的，要查看博客的第1页，你需要使用 ``/blog/1`` URL！
由于富Web应用无法执行此操作，因此请修改路由以使 ``{page}`` 参数可选。
这是通过将其包含在 ``defaults`` 集合中来完成的：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogController.php

        // ...

        /**
         * @Route("/blog/{page}", defaults={"page"=1})
         */
        public function index($page)
        {
            // ...
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog:
            path:       /blog/{page}
            controller: App\Controller\BlogController::index
            defaults:   { page: 1 }

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
        )));

        return $routes;

通过添加 ``page`` 到 ``defaults`` 键，不再永远都需要 ``{page}`` 占位符。
``/blog`` URL将匹配此路由，``page`` 参数的值将被设置为 ``1``。
``/blog/2`` URL也将匹配，并给 ``page`` 参数赋值为 ``2``。完美！

===========  ========  ==================
网址          路由       参数
===========  ========  ==================
``/blog``    ``blog``  ``{page}`` = ``1``
``/blog/1``  ``blog``  ``{page}`` = ``1``
``/blog/2``  ``blog``  ``{page}`` = ``2``
===========  ========  ==================

.. caution::

    你可以拥有多个可选占位符（例如 ``/blog/{slug}/{page}``），但可选占位符后面的所有内容就都必须是可选的。
    例如，``/{page}/blog`` 是一个有效路径，但 ``page`` 将变为总是必需的（即简单的 ``/blog`` 将不匹配此路由）。

.. tip::

    带有可选参数的路由，最终会与具有尾斜杠的对应请求不匹配（即 ``/blog/`` 将不匹配，而 ``/blog`` 匹配）。
