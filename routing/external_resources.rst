.. index::
    single: Routing; Importing routing resources

如何引入外部路由资源
=========================================

简单的应用可以在单个配置文件中定义所有路由 - 通常是
``config/routes.yaml`` （请参阅 :ref:`routing-creating-routes`）。
但是，在大多数应用中，通常从不同的资源中导入路由定义：
控制器文件中的PHP注释，存储在某个目录中的YAML、XML或PHP文件等。

这可以通过从主路由文件中导入路由资源来完成：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        app_file:
            # 从存储在某个bundle中的给定路由文件加载路由
            resource: '@AcmeBundle/Resources/config/routing.yaml'

        app_annotations:
            # 从给定目录的控制器中的PHP注释加载路由
            resource: '../src/Controller/'
            type:     annotation

        app_directory:
            # 从给定目录中的YAML、XML或PHP文件加载路由
            resource: '../legacy/routing/'
            type:     directory

        app_bundle:
            # 从某些bundle目录中的YAML、XML或PHP文件加载路由
            resource: '@AcmeOtherBundle/Resources/config/routing/'
            type:     directory

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <!-- loads routes from the given routing file stored in some bundle -->
            <import resource="@AcmeBundle/Resources/config/routing.yaml"/>

            <!-- loads routes from the PHP annotations of the controllers found in that directory -->
            <import resource="../src/Controller/" type="annotation"/>

            <!-- loads routes from the YAML or XML files found in that directory -->
            <import resource="../legacy/routing/" type="directory"/>

            <!-- loads routes from the YAML or XML files found in some bundle directory -->
            <import resource="@AcmeOtherBundle/Resources/config/routing/" type="directory"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            // loads routes from the given routing file stored in some bundle
            $routes->import('@AcmeBundle/Resources/config/routing.yaml');

            // loads routes from the PHP annotations of the controllers found in that directory
            $routes->import('../src/Controller/', 'annotation');

            // loads routes from the YAML or XML files found in that directory
            $routes->import('../legacy/routing/', 'directory');

            // loads routes from the YAML or XML files found in some bundle directory
            $routes->import('@AcmeOtherBundle/Resources/config/routing/', 'directory');
        };

.. note::

    导入资源时，键（例如 ``app_file``）是集合的名称。请确保它对每个文件都是唯一的，这样就没有其他行可以覆盖它。

.. _prefixing-imported-routes:

为导入的路由的URL添加前缀
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以选择为导入的路由提供“前缀”。
例如，应用的所有路由都添加 ``/site`` 前缀（例如
``/site/blog/{slug}``，而不是 ``/blog/{slug}``）：

.. configuration-block::

    .. code-block:: php-annotations

        use Symfony\Component\Routing\Annotation\Route;

        /**
         * @Route("/site")
         */
        class DefaultController
        {
            // ...
        }

    .. code-block:: yaml

        # config/routes.yaml
        controllers:
            resource: '../src/Controller/'
            type:     annotation
            prefix:   /site

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="../src/Controller/" type="annotation" prefix="/site"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->import('../src/Controller/', 'annotation')
                ->prefix('/site')
            ;
        };

现在，从新路由资源加载的每个路由的路径都会添加 ``/site`` 字符串前缀。

.. note::

    如果带前缀的任何路由定义了一个空路径，Symfony会向其添加一个尾斜杠。
    在前面的示例中，带 ``/site`` 前缀的空路径，将会变成 ``/site/`` URL。
    如果要避免此行为，请将 ``trailing_slash_on_root`` 选项设置为 ``false``：

    .. configuration-block::

        .. code-block:: yaml

            # config/routes.yaml
            controllers:
                resource: '../src/Controller/'
                type:     annotation
                prefix:   /site
                trailing_slash_on_root: false

        .. code-block:: xml

            <!-- config/routes.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <routes xmlns="http://symfony.com/schema/routing"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/routing
                    https://symfony.com/schema/routing/routing-1.0.xsd">

                <import resource="../src/Controller/"
                    type="annotation"
                    prefix="/site"
                    trailing-slash-on-root="false"/>
            </routes>

        .. code-block:: php

            // config/routes.php
            use App\Controller\ArticleController;
            use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

            return function (RoutingConfigurator $routes) {
                $routes->import('../src/Controller/', 'annotation')
                    ->prefix('/site', false)
                ;
            };

为导入路由的名称添加前缀
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以为控制器类中定义的或从配置文件导入的所有路由的名称添加前缀：

.. configuration-block::

    .. code-block:: php-annotations

        use Symfony\Component\Routing\Annotation\Route;

        /**
         * @Route(name="blog_")
         */
        class BlogController
        {
            /**
             * @Route("/blog", name="index")
             */
            public function index()
            {
                // ...
            }

            /**
             * @Route("/blog/posts/{slug}", name="post")
             */
            public function show(Post $post)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        controllers:
            resource:    '../src/Controller/'
            type:        annotation
            name_prefix: 'blog_'

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="../src/Controller/"
                type="annotation"
                name-prefix="blog_"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->import('../src/Controller/', 'annotation')
                ->namePrefix('blog_')
            ;
        };

在此示例中，路由的名称将会是 ``blog_index`` 和 ``blog_post``。

为导入的路由添加主机要求
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你可以在导入的路由上设置主机正则表达式。有关更多信息，请参阅 :ref:`component-routing-host-imported`。
