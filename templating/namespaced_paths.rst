.. index::
   single: Templating; Namespaced Twig Paths

如何使用和注册Twig路径的命名空间
=============================================

通常，当你引用模板时，你将使用项目根目录中 ``templates/`` 主目录的相对路径：

.. code-block:: twig

    {# 此模板位于 templates/layout.html.twig #}
    {% extends "layout.html.twig" %}

    {# 此模板位于 templates/user/profile.html.twig #}
    {{ include('user/profile.html.twig') }}

如果应用定义了大量模板并将它们存储在深层嵌套目录中，你可以考虑使用 **Twig命名空间**，这会为模板目录创建快捷方式。

注册自己的命名空间
-------------------------------

假设你正在使用包含Twig模板的第三方库，而该模板位于 ``vendor/acme/foo-bar/templates/``。
这个路径太长了，因此你可以将 ``foo_bar`` 命名空间定义为快捷方式。

首先，为此目录注册名称空间：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            # ...
            paths:
                '%kernel.project_dir%/vendor/acme/foo-bar/templates': foo_bar

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <twig:config debug="%kernel.debug%" strict-variables="%kernel.debug%">
                <twig:path namespace="foo_bar">%kernel.project_dir%/vendor/acme/foo-bar/templates</twig:path>
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'paths' => [
                '%kernel.project_dir%/vendor/acme/foo-bar/templates' => 'foo_bar',
            ],
        ]);

需要在模板中要调用已注册的 ``foo_bar`` 命名空间，则必须为该命名空间添加 ``@`` 字符前缀
（这是为了让Twig区分命名空间与常规路径）。
假设调用了一个在 ``vendor/acme/foo-bar/templates/`` 目录中的 ``sidebar.twig`` 文件，你可以将其引用为：

.. code-block:: twig

    {{ include('@foo_bar/sidebar.twig') }}

执行此命令以验证模板名称是否正确并查看将加载哪个模板文件：

.. code-block:: terminal

    $ php bin/console debug:twig @foo_bar/sidebar.twig

单个命名空间的多个路径
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

单个Twig命名空间可以与多个路径相关联。
配置的路径的顺序非常重要，因为Twig将始终从第一个配置的路径开始加载存在的第一个模板。
当指定的模板不存在时，此功能可用作加载通用模板时的回退机制。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            # ...
            paths:
                '%kernel.project_dir%/vendor/acme/themes/theme1': theme
                '%kernel.project_dir%/vendor/acme/themes/theme2': theme
                '%kernel.project_dir%/vendor/acme/themes/common': theme

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <twig:config debug="%kernel.debug%" strict-variables="%kernel.debug%">
                <twig:path namespace="theme">%kernel.project_dir%/vendor/acme/themes/theme1</twig:path>
                <twig:path namespace="theme">%kernel.project_dir%/vendor/acme/themes/theme2</twig:path>
                <twig:path namespace="theme">%kernel.project_dir%/vendor/acme/themes/common</twig:path>
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'paths' => [
                '%kernel.project_dir%/vendor/acme/themes/theme1' => 'theme',
                '%kernel.project_dir%/vendor/acme/themes/theme2' => 'theme',
                '%kernel.project_dir%/vendor/acme/themes/common' => 'theme',
            ],
        ]);

现在，你可以使用同样的 ``@theme`` 命名空间来引用位于之前三个目录中的任何模板：

.. code-block:: twig

    {{ include('@theme/header.twig') }}
