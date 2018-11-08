.. index::
   single: Templating; Global variables

如何将变量注入所有模板（即全局变量）
==================================================================

有时你希望所使用的所有模板都可以访问一个变量。
这可以在你的 ``config/packages/twig.yaml`` 文件中配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            # ...
            globals:
                ga_tracking: UA-xxxxx-x

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <!-- ... -->
                <twig:global key="ga_tracking">UA-xxxxx-x</twig:global>
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', array(
             // ...
             'globals' => array(
                 'ga_tracking' => 'UA-xxxxx-x',
             ),
        ));

现在，``ga_tracking`` 变量在所有Twig模板中都可用：

.. code-block:: html+twig

    <p>The google tracking code is: {{ ga_tracking }}</p>

就这么简单！

使用服务容器参数
----------------------------------

你还可以利用内置的 :ref:`service-container-parameters` 系统，该系统允许你隔离或重用该值：

.. code-block:: yaml

    # config/services.yaml
    parameters:
        ga_tracking: UA-xxxxx-x

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            globals:
                ga_tracking: '%ga_tracking%'

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:global key="ga_tracking">%ga_tracking%</twig:global>
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', array(
             'globals' => array(
                 'ga_tracking' => '%ga_tracking%',
             ),
        ));

这就是和之前完全相同的变量。

引用服务
--------------------

你也可以将值设置为服务，而不是使用静态值。
只要在模板中访问该全局变量，就会从服务容器中请求对应服务，让你可以访问该对象。

.. note::

    该服务不会延迟加载。换句话说，只要加载了Twig，即使你从未使用过该全局变量，也会实例化你的服务。

要将服务定义为全局Twig变量，请在服务字符串前加上 ``@``。
这应该是很熟悉的，因为它与你在服务配置中使用的语法相同。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            # ...
            globals:
                # 值就是该服务的ID
                user_management: '@App\Service\UserManagement'

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <!-- ... -->
                <twig:global key="user_management">@App\Service\UserManagement</twig:global>
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', array(
             // ...
             'globals' => array(
                 'user_management' => '@App\Service\UserManagement',
             ),
        ));
