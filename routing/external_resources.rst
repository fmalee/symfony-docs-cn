.. index::
    single: Routing; Importing routing resources

如何引入外部路由资源
=========================================

简单的应用可以在单个配置文件中定义所有路由 - 通常是
``config/routes.yaml`` （请参阅 :ref:`routing-creating-routes`）。
但是，在大多数应用中，通常从不同的资源中导入路由定义：控制器文件中的PHP注释，存储在某个目录中的YAML或XML文件等。

这可以通过从主路由文件中导入路由资源来完成：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        app_file:
            # 从存储在某个bundle中的给定路由文件加载路由
            resource: '@AcmeOtherBundle/Resources/config/routing.yml'

        app_annotations:
            # 从给定目录的控制器中的PHP注释加载路由
            resource: '../src/Controller/'
            type:     annotation

        app_directory:
            # 从给定目录中的YAML或XML文件加载路由
            resource: '../legacy/routing/'
            type:     directory

        app_bundle:
            # 从某些bundle目录中的YAML或XML文件加载路由
            resource: '@AppBundle/Resources/config/routing/public/'
            type:     directory

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <!-- loads routes from the given routing file stored in some bundle -->
            <import resource="@AcmeOtherBundle/Resources/config/routing.yml" />

            <!-- loads routes from the PHP annotations of the controllers found in that directory -->
            <import resource="../src/Controller/" type="annotation" />

            <!-- loads routes from the YAML or XML files found in that directory -->
            <import resource="../legacy/routing/" type="directory" />

            <!-- loads routes from the YAML or XML files found in some bundle directory -->
            <import resource="@AppBundle/Resources/config/routing/public/" type="directory" />
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;

        $routes = new RouteCollection();
        $routes->addCollection(
            // loads routes from the given routing file stored in some bundle
            $loader->import("@AcmeOtherBundle/Resources/config/routing.yml")

            // loads routes from the PHP annotations of the controllers found in that directory
            $loader->import("../src/Controller/", "annotation")

            // loads routes from the YAML or XML files found in that directory
            $loader->import("../legacy/routing/", "directory")

            // loads routes from the YAML or XML files found in some bundle directory
            $loader->import("@AppBundle/Resources/config/routing/public/", "directory")
        );

        return $routes;

.. note::

    从YAML导入资源时，键（例如 ``app_file``）是没有意义的。只要它是唯一的，以确保它不会被其他行重写。

.. _prefixing-imported-routes:

为导入的路由的URL添加前缀
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以选择为导入的路由提供“前缀”。
例如，假设你要为应用的所有路由添加 ``/site`` 前缀（例如
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
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import
                resource="../src/Controller/"
                type="annotation"
                prefix="/site" />
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;

        $app = $loader->import('../src/Controller/', 'annotation');
        $app->addPrefix('/site');

        $routes = new RouteCollection();
        $routes->addCollection($app);

        return $routes;

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
                    http://symfony.com/schema/routing/routing-1.0.xsd">

                <import
                    resource="../src/Controller/"
                    type="annotation"
                    prefix="/site"
                    trailing-slash-on-root="false" />
            </routes>

        .. code-block:: php

            // config/routes.php
            use Symfony\Component\Routing\RouteCollection;

            $app = $loader->import('../src/Controller/', 'annotation');
            // the second argument is the $trailingSlashOnRoot option
            $app->addPrefix('/site', false);
            // ...

    .. versionadded:: 4.1
        ``trailing_slash_on_root`` 选项在Symfony 4.1中引入。

为导入路由的名称添加前缀
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以为控制器类中定义的或从配置文件导入的所有路由的名称添加前缀：

.. configuration-block::

    .. code-block:: php-annotations

        use Symfony\Component\Routing\Annotation\Route;

        /**
         * @Route(name="blog_")
         */
        class BlogController extends AbstractController
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
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import
                resource="../src/Controller/"
                type="annotation"
                name-prefix="blog_" />
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;

        $app = $loader->import('../src/Controller/', 'annotation');
        $app->addNamePrefix('blog_');

        $collection = new RouteCollection();
        $collection->addCollection($app);

        return $collection;

在此示例中，路由的名称将会是 ``blog_index`` 和 ``blog_post``。

.. versionadded:: 4.1
    在Symfony 4.1中引入了在YAML，XML和PHP文件中为路由名称添加前缀的选项。
    以前只有 ``@Route()`` 注释才支持此功能。

为导入的路由添加主机要求
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你可以在导入的路由上设置主机正则表达式。有关更多信息，请参阅 :ref:`component-routing-host-imported`。
