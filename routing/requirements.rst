.. index::
    single: Routing; Requirements

如何定义路由要求
================================

:ref:`路由要求 <routing-requirements>` 可用于使一个特定路由仅在特定条件下匹配。
最简单的示例是将一个路由 ``{wildcard}`` 限制为仅匹配某些正则表达式：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class BlogController extends AbstractController
        {
            /**
             * @Route("/blog/{page}", name="blog_list", requirements={"page"="\d+"})
             */
            public function list($page)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog_list:
            path:      /blog/{page}
            controller: App\Controller\BlogController::list
            requirements:
                page: '\d+'

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_list" path="/blog/{page}" controller="App\Controller\BlogController::list">
                <requirement key="page">\d+</requirement>
            </route>

            <!-- ... -->
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\BlogController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('blog_list', '/blog/{page}')
                ->controller([BlogController::class, 'list'])
                ->requirements([
                    'page' => '\d+',
                ])
            ;
            // ...
        };

得益于 ``\d+`` 要求（即任意长度的“数字”），``/blog/2`` 将匹配这这个路由，但
``/blog/some-string`` 将不会匹配。

.. sidebar:: 总是较早的路由胜出

    为什么你要关心要求？如果一个请求与 *两个* 路由匹配，则第一条路由总是获胜。
    通过向第一个路由添加要求，你可以在恰当的情况下使每个路由都能匹配。
    有关示例，请参阅 :ref:`routing-requirements`。

由于参数要求是正则表达式，因此每个要求的复杂性和灵活性完全取决于你自己。
假设你的应用的主页基于URL提供两种不同的语言支持：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/MainController.php

        // ...
        class MainController extends AbstractController
        {
            /**
             * @Route("/{_locale}", defaults={"_locale"="en"}, requirements={
             *     "_locale"="en|fr"
             * })
             */
            public function homepage($_locale)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        homepage:
            path:       /{_locale}
            controller: App\Controller\MainController::homepage
            defaults:   { _locale: en }
            requirements:
                _locale:  en|fr

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="homepage" path="/{_locale}" controller="App\Controller\MainController::homepage">
                <default key="_locale">en</default>
                <requirement key="_locale">en|fr</requirement>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\MainController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('homepage', '/{_locale}')
                ->controller([MainController::class, 'homepage'])
                ->defaults([
                    '_locale' => 'en',
                ])
                ->requirements([
                    '_locale' => 'en|fr',
                ])
            ;
        };

对于传入请求，URL的 ``{_locale}`` 部分会与正则表达式 ``(en|fr)`` 进行匹配。

=======  ========================
路径      参数
=======  ========================
``/``    ``{_locale}`` = ``"en"``
``/en``  ``{_locale}`` = ``"en"``
``/fr``  ``{_locale}`` = ``"fr"``
``/es``  *不会匹配本路由*
=======  ========================

.. note::

    通过在声明或导入路由时设置 ``utf8`` 选项，可以启用UTF-8路由匹配。
    这将使得要求中的例如 ``.`` 匹配任何UTF-8字符而不是单个字节。

.. tip::

    如 :doc:`本文档 </routing/service_container_parameters>` 中所述，路由要求还可以包含容器参数。
    它通常在正则表达式非常复杂并会在你的应用中重复使用时派上用场。

.. index::
    single: Routing; Method requirement

.. _routing-method-requirement:

添加HTTP方法要求
-------------------------------

除了URL之外，你还可以匹配传入请求的 *方法* （即GET、HEAD、POST、PUT、DELETE）。
假设你为博客创建了一个API，并且你有两个路由：
一个用于显示帖子（在GET或HEAD请求上），另一个用于更新帖子（在PUT请求中）。
那么可以通过以下路由配置来完成：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogApiController.php
        namespace App\Controller;

        // ...

        class BlogApiController extends AbstractController
        {
            /**
             * @Route("/api/posts/{id}", methods={"GET","HEAD"})
             */
            public function show($id)
            {
                // ... 使用帖子来返回一个 JSON 响应
            }

            /**
             * @Route("/api/posts/{id}", methods={"PUT"})
             */
            public function edit($id)
            {
                // ... 编辑一个帖子
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        api_post_show:
            path:       /api/posts/{id}
            controller: App\Controller\BlogApiController::show
            methods:    GET|HEAD

        api_post_edit:
            path:       /api/posts/{id}
            controller: App\Controller\BlogApiController::edit
            methods:    PUT

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="api_post_show"
                path="/api/posts/{id}"
                controller="App\Controller\BlogApiController::show"
                methods="GET|HEAD"/>

            <route id="api_post_edit"
                path="/api/posts/{id}"
                controller="App\Controller\BlogApiController::edit"
                methods="PUT"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\BlogApiController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('api_post_show', '/api/posts/{id}')
                ->controller([BlogApiController::class, 'show'])
                ->methods(['GET', 'HEAD'])
            ;
            $routes->add('api_post_edit', '/api/posts/{id}')
                ->controller([BlogApiController::class, 'edit'])
                ->methods(['PUT'])
            ;

            // or use collection
            $api = $routes->collection('api_post_')
                ->prefix('/api/posts/{id}')
            ;
            $api->add('show')
                ->controller([BlogApiController::class, 'show'])
                ->methods(['GET', 'HEAD'])
            ;
            $api->add('edit')
                ->controller([BlogApiController::class, 'edit'])
                ->methods(['PUT'])
            ;
        };

尽管这两个路由具有相同的路径（``/api/posts/{id}``），但第一个路由仅匹配
``GET``或 ``HEAD`` 请求，第二个路由仅匹配 ``PUT`` 请求。
这意味着你可以使用相同的URL来显示和编辑帖子，同时为这两个动作使用不同的控制器。

.. note::

    如果未指定 ``methods``，则该路由将匹配所有方法。

.. tip::

    如果你要使用 ``GET`` 和 ``POST`` *以外* 的HTML表单和HTTP方法，你需要包含一个
    ``_method`` 参数来 *伪造* 对应的HTTP方法。
    有关更多信息，请参阅 :doc:`/form/action_method` 。

添加主机要求
-------------------------

你还可以要求匹配传入请求的HTTP *主机*。

使用表达式添加动态要求
--------------------------------------------

对于非常复杂的要求，你可以使用动态表达式来匹配请求中的 *任何* 信息。
请参阅 :doc:`/routing/conditions`。

.. _`PCRE Unicode property`: http://php.net/manual/en/regexp.reference.unicode.php
