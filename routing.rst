.. index::
   single: Routing

路由
=======

对于任何严谨的web应用程序而言美观的URL是绝对必须的。
这意味着日渐淘汰的 ``index.php?article_id=57`` 这类丑陋的URL要被 ``/read/intro-to-symfony`` 取代。


拥有灵活性是更加重要的。你把页面的URL从 ``/blog`` 改为 ``/news`` 时需要做些什么？
你需要追踪并更新多少链接，才能做出这种改变？如果你使用Symfony的路由，改变起来很容易。

.. index::
   single: Routing; Basics

.. _routing-creating-routes:

创建路由
---------------

*路由* 是从URL路径到控制器的映射。假设你想要一条完全匹配 ``/blog`` 的路由，以及另一条可以匹配*任何*网址的动态路由，例如 ``/blog/my-post`` 或  ``/blog/all-about-symfony``。

路由可以在YAML，XML和PHP中配置。所有格式都提供相同的功能和性能，因此请选择你喜欢的格式。
如果选择PHP注释，请在应用中运行此命令，以添加对它们的支持：

.. code-block:: terminal

    $ composer require annotations

现在你可以配置路由了：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class BlogController extends AbstractController
        {
            /**
             * Matches /blog exactly
             *
             * @Route("/blog", name="blog_list")
             */
            public function list()
            {
                // ...
            }

            /**
             * Matches /blog/*
             *
             * @Route("/blog/{slug}", name="blog_show")
             */
            public function show($slug)
            {
                // $slug 将等于URL的动态部分
                // 如URL是 /blog/yay-routing, 那么 $slug='yay-routing'

                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog_list:
            path:     /blog
            controller: App\Controller\BlogController::list

        blog_show:
            path:     /blog/{slug}
            controller: App\Controller\BlogController::show

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_list" controller="App\Controller\BlogController::list" path="/blog" >
                <!-- settings -->
            </route>

            <route id="blog_show" controller="App\Controller\BlogController::show" path="/blog/{slug}">
                <!-- settings -->
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;
        use App\Controller\BlogController;

        $routes = new RouteCollection();
        $routes->add('blog_list', new Route('/blog', array(
            '_controller' => [BlogController::class, 'list']
        )));
        $routes->add('blog_show', new Route('/blog/{slug}', array(
            '_controller' => [BlogController::class, 'show']
        )));

        return $routes;

感谢这两条路由:

* 如果用户进入 ``/blog``，则匹配第一个路由并执行 ``list()``;

* 如果用户访问 ``/blog/*``，则匹配第二个路由并执行 ``show()``。
  因为路由路径是 ``/blog/{slug}``，所以与该值匹配的一个 ``$slug`` 变量会传递给 ``show()``。
  例如，如果用户转到 ``/blog/yay-routing``，那么 ``$slug`` 将等于 ``yay-routing``。

每当路由路径中有 ``{placeholder}`` 时，该部分就成为通配符：它匹配*任何*值。
你的控制器现在*也可以*有一个名为 ``$placeholder`` 的参数（通配符和参数名称*必须匹配）。

每个路由同样都有一个内部名称：``blog_list`` 和 ``blog_show``。
这些可以是任何东西（只要它们都是唯一的）并且还没有任何意义。你稍后将使用它们来 :ref:`生成URL <routing-generate>`。

.. sidebar:: 以其他格式配置路由

    每个方法上面的 ``@Route`` 称为注释。如果你更愿意使用YAML，XML或PHP配置路由，那没问题！
    只需创建一个新的路由文件（例如 ``routes.xml``），Symfony就会自动使用它。

.. _i18n-routing:

本地化路由 (i18n)
------------------------

.. versionadded:: 4.1
    在Symfony 4.1中引入了本地化路由的功能。

路由可以本地化以支持每个 :doc:`locale </translation/locale>` 的唯一路径。
Symfony提供了一种方便的方式来声明本地化路由而无需重复。

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/CompanyController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class CompanyController extends AbstractController
        {
            /**
             * @Route({
             *     "nl": "/over-ons",
             *     "en": "/about-us"
             * }, name="about_us")
             */
            public function about()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        about_us:
            path:
                nl: /over-ons
                en: /about-us
            controller: App\Controller\CompanyController::about

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="about_us" controller="App\Controller\CompanyController::about">
                <path locale="nl">/over-ons</path>
                <path locale="en">/about-us</path>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        namespace Symfony\Component\Routing\Loader\Configurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('about_us', ['nl' => '/over-ons', 'en' => '/about-us'])
                ->controller('App\Controller\CompanyController::about');
        };

当本地化路由在请求期间被匹配时，Symfony会自动感知应使用哪个本地配置。
以这种方式定义路由也消除了对路由重复注册的需要，这最小化了由定义不一致引起的任何错误的风险。

.. tip::

    如果应用使用完整语言(full language)+区域语言(territory locales)例如 ``fr_FR``，``fr_BE``），
    你可以路由中只使用语言部分(language part)（例如 ``fr``）。
    当你希望为共享相同语言的不同区域(locales)使用相同的路由路径时，这样处理可以防止必须定义多个路径。

.. versionadded:: 4.2
    The feature to fall back on the language part only was introduced in Symfony 4.2.

国际化应用的一个常见要求是为所有路由添加一个区域(locale)前缀。
这可以通过为每个语言环境定义不同的前缀来完成（如果你愿意，可以为默认语言环境设置一个空前缀）：

.. configuration-block::

    .. code-block:: yaml

        # config/routes/annotations.yaml
        controllers:
            resource: '../../src/Controller/'
            type: annotation
            prefix:
                en: '' # 不要为英语（默认语言环境）添加前缀
                nl: '/nl'

.. _routing-requirements:

添加 {通配符} 条件
------------------------------

想象一下， ``blog_list`` 路由将包含一个博客帖子的分页列表，
其中包含 ``/blog/2`` 和 ``/blog/3`` 等第2页和第3页的URL。
如果你将路径的路径更改为 ``/blog/{page}``，将会造成一个困扰：

* blog_list: ``/blog/{page}`` 会匹配 ``/blog/*``;
* blog_show: ``/blog/{slug}`` *同样*会匹配 ``/blog/*``.

当两个路由匹配相同的URL时，会先加载*第一条*路由。
不幸的是，这意味着 ``/blog/yay-routing`` 将匹配到 ``blog_list``。这就糟糕了！

要解决此问题，可以添加 ``{page}`` 通配符只能匹配数字（digits）的*规定*(requirement)：

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

            /**
             * @Route("/blog/{slug}", name="blog_show")
             */
            public function show($slug)
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

        blog_show:
            # ...

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_list" path="/blog/{page}" controller="App\Controller\BlogController::list">
                <requirement key="page">\d+</requirement>
            </route>

            <!-- ... -->
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;
        use App\Controller\BlogController;

        $routes = new RouteCollection();
        $routes->add('blog_list', new Route('/blog/{page}', array(
            '_controller' => [BlogController::class, 'list'],
        ), array(
            'page' => '\d+'
        )));

        // ...

        return $routes;

``\d+`` 是一个匹配任意长度数字的正则表达式。现在：

========================  =============  ===============================
URL                       路由            参数
========================  =============  ===============================
``/blog/2``               ``blog_list``  ``$page`` = ``2``
``/blog/yay-routing``     ``blog_show``  ``$slug`` = ``yay-routing``
========================  =============  ===============================

如果你愿意，可以使用语法 ``{placeholder_name<requirements>}`` 在每个占位符中内联条件。
此功能使配置更简洁，但当需求复杂时，它会降低路由的可读性：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class BlogController extends AbstractController
        {
            /**
             * @Route("/blog/{page<\d+>}", name="blog_list")
             */
            public function list($page)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog_list:
            path:      /blog/{page<\d+>}
            controller: App\Controller\BlogController::list

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_list" path="/blog/{page<\d+>}"
                   controller="App\Controller\BlogController::list" />

            <!-- ... -->
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;
        use App\Controller\BlogController;

        $routes = new RouteCollection();
        $routes->add('blog_list', new Route('/blog/{page<\d+>}', array(
            '_controller' => [BlogController::class, 'list'],
        )));

        // ...

        return $routes;

.. versionadded:: 4.1
    Symfony 4.1中引入了内联规则的功能。

要了解其他路由匹配条件（如HTTP方法，主机名和动态表达式），请参阅 :doc:`/routing/requirements`。

给 {占位符} 一个默认值
-------------------------------------

在前面的示例中，``blog_list`` 的路径为 ``/blog/{page}``。
如果用户访问 ``/blog/1``，它将匹配。但如果他们访问 ``/blog``，它将无法匹配。
只要向路由添加了 ``{占位符}`` ，该占位符就*必须*有一个值。

那么当用户访问 ``/blog` 时，如何让 ``blog_list`` 再次匹配？通过添加一个*默认*值：

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
            public function list($page = 1)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog_list:
            path:      /blog/{page}
            controller: App\Controller\BlogController::list
            defaults:
                page: 1
            requirements:
                page: '\d+'

        blog_show:
            # ...

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_list" path="/blog/{page}" controller="App\Controller\BlogController::list">
                <default key="page">1</default>

                <requirement key="page">\d+</requirement>
            </route>

            <!-- ... -->
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;
        use App\Controller\BlogController;

        $routes = new RouteCollection();
        $routes->add('blog_list', new Route(
            '/blog/{page}',
            array(
                '_controller' => [BlogController::class, 'list'],
                'page'        => 1,
            ),
            array(
                'page' => '\d+'
            )
        ));

        // ...

        return $routes;

现在，当用户访问 ``/blog`` 时， ``blog_list`` 路由将会匹配，``$page`` 的值将会默认为 ``1``。

与匹配条件一样，使用语法 ``{placeholder_name?default_value}`` 也可以在每个占位符中内联默认值。
此功能与内联条件兼容，因此你可以在同一个占位符中内联：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/BlogController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class BlogController extends AbstractController
        {
            /**
             * @Route("/blog/{page<\d+>?1}", name="blog_list")
             */
            public function list($page)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        blog_list:
            path:      /blog/{page<\d+>?1}
            controller: App\Controller\BlogController::list

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="blog_list" path="/blog/{page <\d+>?1}"
                   controller="App\Controller\BlogController::list" />

            <!-- ... -->
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;
        use App\Controller\BlogController;

        $routes = new RouteCollection();
        $routes->add('blog_list', new Route('/blog/{page<\d+>?1}', array(
            '_controller' => [BlogController::class, 'list'],
        )));

        // ...

        return $routes;

.. tip::

    要为任何一个占位符提供 ``null`` 默认值，就不要在 ``?`` 字符之后添加任何内容（例如``/blog/{page?}``）。

.. versionadded:: 4.1
    Symfony 4.1中引入了内联默认值的功能。

列出所有路由
--------------------------

随着应用的功能增长，你最终会拥有*很多*路由！要查看所有路由，请运行：

.. code-block:: terminal

    $ php bin/console debug:router

    ------------------------------ -------- -------------------------------------
     Name                           Method   Path
    ------------------------------ -------- -------------------------------------
     app_lucky_number              ANY    /lucky/number/{max}
     ...
    ------------------------------ -------- -------------------------------------

.. index::
   single: Routing; Advanced example
   single: Routing; _format parameter

.. _advanced-routing-example:

高级路由范例
------------------------

综合以上这些想法，请浏览此高级示例：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/ArticleController.php

        // ...
        class ArticleController extends AbstractController
        {
            /**
             * @Route(
             *     "/articles/{_locale}/{year}/{slug}.{_format}",
             *     defaults={"_format": "html"},
             *     requirements={
             *         "_locale": "en|fr",
             *         "_format": "html|rss",
             *         "year": "\d+"
             *     }
             * )
             */
            public function show($_locale, $year, $slug)
            {
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        article_show:
          path:     /articles/{_locale}/{year}/{slug}.{_format}
          controller: App\Controller\ArticleController::show
          defaults:
              _format: html
          requirements:
              _locale:  en|fr
              _format:  html|rss
              year:     \d+

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="article_show"
                path="/articles/{_locale}/{year}/{slug}.{_format}"
                controller="App\Controller\ArticleController::show">

                <default key="_format">html</default>
                <requirement key="_locale">en|fr</requirement>
                <requirement key="_format">html|rss</requirement>
                <requirement key="year">\d+</requirement>

            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;
        use App\Controller\ArticleController;

        $routes = new RouteCollection();
        $routes->add(
            'article_show',
            new Route('/articles/{_locale}/{year}/{slug}.{_format}', array(
                '_controller' => [ArticleController::class, 'show'],
                '_format'     => 'html',
            ), array(
                '_locale' => 'en|fr',
                '_format' => 'html|rss',
                'year'    => '\d+',
            ))
        );

        return $routes;

如你所见，只有当URL的 ``{_locale}`` 部分为``en`` 或 ``fr`` 且 ``{year}`` 为数字时，此路由才会匹配。
此路由还显示了如何在占位符之间使用点(``.``)而不是斜杠(``/``)。
与此路由匹配的网址可能如下所示：

* ``/articles/en/2010/my-post``
* ``/articles/fr/2010/my-post.rss``
* ``/articles/en/2013/my-latest-post.html``

.. _routing-format-param:

.. sidebar:: 特殊的 ``_format`` 路由参数

    此示例还突出显示了特殊的 ``_format`` 路由参数。使用此参数时，匹配的值将成为 ``Request`` 对象的“请求格式”。

    最终，请求格式用于诸如设置响应的 ``Content-Type`` 之类的事情
    （例如，``json`` 请求格式会将一个 ``Content-Type`` 转换为 ``application/json``）。

.. note::

    有时你希望使路由的某些部分全局可配置。
    Symfony通过利用服务容器参数为您提供了一种方法。
    在 ":doc:`/routing/service_container_parameters`" 中阅读有关此内容的更多信息。

特殊路由参数
~~~~~~~~~~~~~~~~~~~~~~~~~~

如你所见，每个路由参数或默认值最终都可用作控制器方法中的参数。
此外，还有四个特殊参数：每个参数在应用中添加一个独特的功能：

``_controller``
    如你所见，此参数用于确定路由匹配时执行的控制器。

``_format``
    用于设置请求的格式（:ref:`详细信息 <routing-format-param>`）。

``_fragment``
    用于设置片段(fragment)标识符，URL的以 ``#`` 字符开头的最后部分为可选配置，用于标识一个文档的部分内容。

``_locale``
    用于在请求上设置区域语言（:ref:`详细信息 <translation-locale-url>`）。

.. _routing-trailing-slash-redirection:

使用尾部斜杠(/)重定向URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

从历史上看，URL遵循UNIX约定，即为目录添加尾部斜杠（例如 ``https://example.com/foo/``），而删除则是引用文件(``https://example.com/foo``)。
虽然为两个URL提供不同的内容是可以的，但现在将两个URL视为相同的URL并在它们之间重定向是很常见的。

Symfony遵循这个逻辑，在带有和不带尾部斜杠的URL之间重定向（但仅限于 ``GET`` 和 ``HEAD`` 请求）：

==========  ========================================  ==========================================
路由路径      如果请求URL是 ``/foo``                     如果请求URL是 ``/foo/``
----------  ----------------------------------------  ------------------------------------------
``/foo``    匹配 (请求状态码``200``)                     创建 ``301`` 并重定向到 ``/foo``
``/foo/``   创建 ``301`` 并重定向到 ``/foo/``            匹配 (请求状态码``200``)
==========  ========================================  ==========================================

.. note::

    如果你的应用为每个路径（``/foo`` 和 ``/foo/``）定义了不同的路由，
    则不会发生此自动重定向，并且始终匹配正确的路由。

.. versionadded:: 4.1
    在Symfony 4.1中引入了从 ``/foo/`` 到 ``/foo`` 的``301``自动重定向。在之前的Symfony版本中，则是产生``404``响应。

.. index::
   single: Routing; Controllers
   single: Controller; String naming format

.. _controller-string-syntax:

控制器命名模式
-------------------------

路由中的 ``controller`` 值具有一个非常简单的格式 ``CONTROLLER_CLASS::METHOD``。

.. tip::

    要引用控制器类中的一个 ``__invoke()`` 方法来实现操作，你不必传递方法名称，
    而只需使用完全限定的类名（例如``App\Controller\BlogController``）。

.. index::
   single: Routing; Generating URLs

.. _routing-generate:

生成 URL 地址
---------------

路由系统也可以生成URL。实际上，路由是双向系统：将URL映射到一个控制器以及一个路由对应一个URL。

要生成URL，你需要指定路由的名称（例如 ``blog_show``）以及该路由路径中使用的任何通配符（例如 ``slug = my-blog-post``）。
有了这些信息，就可以轻松生成任何URL::

    class MainController extends AbstractController
    {
        public function show($slug)
        {
            // ...

            // /blog/my-blog-post
            $url = $this->generateUrl(
                'blog_show',
                array('slug' => 'my-blog-post')
            );
        }
    }

如果需要从服务生成URL，
请使用 :class:`Symfony\\Component\\Routing\\Generator\\UrlGeneratorInterface` 服务的类型提示::

    // src/Service/SomeService.php

    use Symfony\Component\Routing\Generator\UrlGeneratorInterface;

    class SomeService
    {
        private $router;

        public function __construct(UrlGeneratorInterface $router)
        {
            $this->router = $router;
        }

        public function someMethod()
        {
            $url = $this->router->generate(
                'blog_show',
                array('slug' => 'my-blog-post')
            );
            // ...
        }
    }

.. index::
   single: Routing; Generating URLs in a template

使用查询字符串生成URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``generate()`` 方法采用有通配符组成的数组来生成URI。
但是如果你传递额外的内容，它们将作为查询字符串添加到URI::

    $this->router->generate('blog', array(
        'page' => 2,
        'category' => 'Symfony',
    ));
    // /blog/2?category=Symfony

生成本地化URL
~~~~~~~~~~~~~~~~~~~~~~~~~

当路由已本地化时，Symfony默认使用当前请求的区域语言来生成URL。
为了生成不同语言环境的URL，你必须在参数数组中传递 ``_locale``::

    $this->router->generate('about_us', array(
        '_locale' => 'nl',
    ));
    // generates: /over-ons

从模板中生成URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

要在 Twig 中生成URL，请参阅模板章节：:ref:`templating-pages`。
如果你还需要在JavaScript中生成URL，请参阅 :doc:`/routing/generate_url_javascript`。

.. index::
   single: Routing; Absolute URLs

生成绝对URL
~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，路由会生成相对URL（例如 ``/blog``）。
要生成绝对URL，在一个控制器中将 ``UrlGeneratorInterface::ABSOLUTE_URL``作为第三个参数传递给 ``generateUrl()`` 方法::

    use Symfony\Component\Routing\Generator\UrlGeneratorInterface;

    $this->generateUrl('blog_show', array('slug' => 'my-blog-post'), UrlGeneratorInterface::ABSOLUTE_URL);
    // http://www.example.com/blog/my-blog-post

.. note::

    生成绝对URL时使用的主机(host)是根据当前的 ``Request`` 对象自动检测得来的。
    如果是在Web上下文(web context)外部生成绝对URL（例如在控制台命令中），它会不起作用。
    请参阅 :doc:`/console/request_context` 以了解如何解决此问题。

故障排除
---------------

以下是使用路由时可能会遇到的一些常见错误:

    Controller "App\\Controller\\BlogController::show()" requires that you
    provide a value for the "$slug" argument.

会出现这种情况，是因为你的控制器方法拥有一个参数（例如``$slug``）::

    public function show($slug)
    {
        // ..
    }

但是你的路由路径中没有 ``{slug}`` 通配符（例如``/blog/show``）。
解决方法：添加一个 ``{slug}`` 到你的路由路径：``/blog/show/{slug}``，或是为该参数赋予默认值（即``$slug = null``）。

    Some mandatory parameters are missing ("slug") to generate a URL for route
    "blog_show".

这意味着你正在尝试生成 ``blog_show`` 路由的URL，但你*没有*给路由路径中的通配符传递 ``slug`` 值（这是必需的，因为该路由有一个 ``{slug}``）。
要解决此问题，请在生成路径时传递一个 ``slug``值::

    $this->generateUrl('blog_show', array('slug' => 'slug-value'));

    // or, in Twig
    // {{ path('blog_show', {'slug': 'slug-value'}) }}

继续了解
-----------

路由部分了解完毕!
现在，揭示 :doc:`控制器 </controller>` 的能力。

更多关于路由的内容
------------------------

.. toctree::
    :hidden:

    controller

.. toctree::
    :maxdepth: 1
    :glob:

    routing/*

.. _`JMSI18nRoutingBundle`: https://github.com/schmittjoh/JMSI18nRoutingBundle
.. _`BeSimpleI18nRoutingBundle`: https://github.com/BeSimple/BeSimpleI18nRoutingBundle
