.. index::
   single: Configuration

配置Symfony
===================

Symfony应用使用存储在 ``config/`` 目录中的文件进行配置，该目录具有以下默认结构：

.. code-block:: text

    your-project/
    ├─ config/
    │  ├─ packages/
    │  ├─ bundles.php
    │  ├─ routes.yaml
    │  └─ services.yaml
    ├─ ...

``routes.yaml`` 文件定义了 :doc:`路由配置 </routing>`；``services.yaml`` 文件配置
:doc:`服务容器 </service_container>` 的服务；``bundles.php`` 文件启用/禁用应用中的软件包。

你将在 ``config/packages/`` 目录中工作最多。此目录存储应用中安装的每个软件包的配置。
软件包（在Symfony中也称为“bundles”，在其他项目中称为“插件/模块”）为项目添加了现成的功能。

使用在Symfony应用中默认启用的 :doc:`Symfony Flex </setup/flex>`
时，软件包会在安装期间自动更新 ``bundles.php`` 文件，并在 ``config/packages/``
中创建新文件。例如，这是“API Platform”软件包创建的默认文件：

.. code-block:: yaml

    # config/packages/api_platform.yaml
    api_platform:
        mapping:
            paths: ['%kernel.project_dir%/src/Entity']

对于一些Symfony新手来说，将配置拆分成许多小文件是令人生畏的。
但是，你将很快习惯它们，并且在安装软件包后很少需要更改这些文件。

.. tip::

    要了解所有可用的配置选项，请查看 :doc:`Symfony配置参考 </reference/index>`
    或运行 ``config:dump-reference`` 命令。

配置的格式
---------------------

与其他框架不同，Symfony不会强制你使用特定格式来配置应用。
Symfony允许你在YAML、XML和PHP之间进行选择，在整个Symfony文档中，所有配置示例都将以这三种格式展示。

格式之间没有任何实际差异。
实际上，Symfony在运行应用之前会将所有这些转换并缓存到PHP中，因此它们之间甚至没有任何性能差异。

安装软件包时默认使用YAML，因为它简洁且易读。下面是每种格式的主要优点和缺点：

* :doc:`YAML </components/yaml/yaml_format>`：简单、干净、易读，但需要使用专用的解析器；
* *XML*: 支持IDE自动完成/验证，并由PHP原生解析，但有时它会生成过于冗长的配置；
* *PHP*: 非常强大，它允许创建动态配置，但配置结果的可读性低于其他格式。

导入配置文件
-----------------------------

Symfony使用 :doc:`Config组件 </components/config>`
加载配置文件，该组件提供高级功能，例如导入配置文件，即使它们使用不同的格式：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        imports:
            - { resource: 'legacy_config.php' }
            # 如果加载的文件不存在，ignore_errors 将以静默方式丢弃错误
            - { resource: 'my_config_file.xml', ignore_errors: true }
            # 还支持使用glob表达式加载多个文件
            - { resource: '/etc/myapp/*.yaml' }

        # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="legacy_config.php"/>
                <!-- ignore_errors silently discards errors if the loaded file doesn't exist -->
                <import resource="my_config_file.yaml" ignore-errors="true"/>
                <!-- glob expressions are also supported to load multiple files -->
                <import resource="/etc/myapp/*.yaml"/>
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // config/services.php
        $loader->import('legacy_config.xml');
        // the third optional argument of import() is 'ignore_errors', which
        // silently discards errors if the loaded file doesn't exist
        $loader->import('my_config_file.yaml', null, true);
        // glob expressions are also supported to load multiple files
        $loader->import('/etc/myapp/*.yaml');

        // ...

.. tip::

    如果使用任何其他配置格式，则必须定义自己的加载器类，从
    :class:`Symfony\\Component\\DependencyInjection\\Loader\\FileLoader`
    扩展它。此外，你可以定义自己的服务以从数据库或Web服务加载配置。

.. _config-parameter-intro:
.. _config-parameters-yml:

配置的参数
------------------------

有时，需要在多个配置文件中使用相同的配置值。
你可以将其定义为一个“参数”，而不是重复它，这类似于可重用的配置值。
按照约定，参数在 ``config/services.yaml`` 文件中的 ``parameters`` 键下定义：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            # 参数名称是一个任意字符串（建议使用 “app.” 前缀，以便更好地区分你的参数和symfony参数）。
            # 参数值可以是任何有效的标量（数字、字符串、布尔值、数组）
            app.admin_email: 'something@example.com'

        # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <parameters>
                <!-- the parameter name is an arbitrary string (the 'app.' prefix is recommended
                     to better differentiate your parameters from Symfony parameters).
                     the parameter value can be any valid scalar (numbers, strings, booleans, arrays) -->
                <parameter key="app.admin_email">something@example.com</parameter>
            </parameters>

            <!-- ... -->
        </container>

    .. code-block:: php

        // config/services.php
        // the parameter name is an arbitrary string (the 'app.' prefix is recommended
        // to better differentiate your parameters from Symfony parameters).
        // the parameter value can be any valid scalar (numbers, strings, booleans, arrays)
        $container->setParameter('app.admin_email', 'something@example.com');
        // ...

定义好后，你可以使用特殊语法从任何其他配置文件引用此参数的值：将参数名称封装在两个
``%`` 中间 （例如 ``%app.admin_email%``）：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/some_package.yaml
        some_package:
            # 由两个 % 包围的任何字符串都将替换为该参数的值
            email_address: '%app.admin_email%'

            # ...

    .. code-block:: xml

        <!-- config/packages/some_package.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <!-- any string surrounded by two % is replaced by that parameter value -->
            <some-package:config email-address="%app.admin_email%">
                <!-- ... -->
            </some-package:config>
        </container>

    .. code-block:: php

        // config/packages/some_package.php
        $container->loadFromExtension('some_package', [
            // any string surrounded by two % is replaced by that parameter value
            'email_address' => '%app.admin_email%',

            // ...
        ]);

.. include:: /components/dependency_injection/_imports-parameters-note.rst.inc

配置参数在Symfony应用中非常常见。有些软件包甚至定义了自己的参数（例如，在安装翻译软件包时，会在
 ``config/services.yaml`` 文件中添加一个新的 ``locale`` 参数）。

.. seealso::

    阅读 :ref:`服务参数 <service-container-parameters>`
    一文，以了解如何在服务和控制器中使用这些配置参数。

.. index::
   single: Environments; Introduction

.. _page-creation-environments:
.. _page-creation-prod-cache-clear:
.. _configuration-environments:

配置环境
--------------------------

你只有一个应用，但无论你是否意识到，你需要在不同时间表现出不同的行为：

* 在 **开发** 时，你希望记录一切内容并暴露漂亮的调试工具;
* 部署到 **生产** 后，你希望同一个应用能优化以提高速度并仅记录错误。

Symfony使用在 ``config/packages/`` 存储的文件来配置 :doc:`应用服务 </service_container>`。
换句话说，你可以通过更改加载的配置文件来更改应用的行为。这就是Symfony **配置环境** 的思路。

典型的Symfony应用从三个环境开始：``dev``(用于本地开发）、``prod``（用于生产服务器）和
``test``（用于 :doc:`自动化测试 </testing>`）。
运行应用时，Symfony按此顺序加载配置文件（最后一个文件可以覆盖以前的文件中设置的值）：

#. ``config/packages/*.yaml`` (以及 ``.xml`` 和 ``*.php`` 文件);
#. ``config/packages/<environment-name>/*.yaml`` (以及 ``.xml`` 和 ``*.php`` 文件);
#. ``config/packages/services.yaml`` (以及 ``services.xml`` 和 ``services.php`` 文件);

以默认安装的 ``framework`` 软件包为例：

* 首先，``config/packages/framework.yaml`` 会在所有环境中都加载，以使用一些选项来置框架;
* 在 **prod** 环境中，没有任何额外的东西会被设置，因为没有
  ``config/packages/prod/framework.yaml`` 文件;
* 在 **dev** 环境中，也同样没有文件（不存在 ``config/packages/dev/framework.yaml``）。
* 在 **test** 环境中，会加载 ``config/packages/test/framework.yaml``
  文件，以覆盖先前 ``config/packages/framework.yaml`` 中配置的一些设置。

实际上，每个环境与其他环境仅略有不同。
这意味着所有环境都共享大量常见配置，这些配置直接放在 ``config/packages/`` 目录中的文件中。

.. seealso::

    请参阅 :doc:`Kernel类 </configuration/front_controllers_and_kernel>`
    的 ``configureContainer()`` 方法，以了解有关配置文件的加载顺序的所有信息。

.. _selecting-the-active-environment:

选择活动环境
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony应用附带一个位于项目根目录的名为 ``.env``
的文件。此文件用于定义环境变量的值，本文 :ref:`稍后 <config-dot-env>` 将对其进行详细说明。

打开 ``.env`` 文件并编辑 ``APP_ENV`` 变量的值以更改运行应用的环境。例如，要在生产中运行应用：

.. code-block:: bash

    # .env
    APP_ENV=prod

此值同时应用于Web和控制台命令。但是，你可以在运行命名之前通过设置 ``APP_ENV`` 值来重写它们：

.. code-block:: terminal

    # 使用在 .env 文件中定义的环境
    $ php bin/console command_name

    # 忽略 .env 文件并在生产环境中运行此命令
    $ APP_ENV=prod php bin/console command_name

创建新环境
~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony提供的默认三种环境对于大多数项目来说已经足够了，但你也可以定义自己的环境。
例如，这是如何定义一个客户端在开始生产之前可以测试项目的 ``staging`` 环境：

#. 创建一个与环境同名的配置目录（在本例中为 ``config/packages/staging/``）;
#. 在 ``config/packages/staging/`` 中添加所需的配置文件以定义新环境的行为。
   Symfony会首先加载 ``config/packages/*.yaml``
   文件，因此你只需配置与这些文件有差异的部分;
#. 如上一节中所述，使用 ``APP_ENV`` 环境变量来选择 ``staging`` 环境。

.. tip::

    环境在彼此之间相似是很常见的，因此你可以使用在
    ``config/packages/<environment-name>/`` 目录之间使用
    `软连接`_ 来重用相同的配置。

.. _config-env-vars:

基于环境变量的配置
--------------------------------------------

使用 `环境变量`_ 是根据应用运行的位置来配置选项的常见做法（例如，在生产和本地计算机中，数据库凭据通常不同）。

你可以将它们定义为环境变量，并使用 ``%env(ENV_VAR_NAME)%``
特殊语法在配置文件中引用它们，而不是将它们定义为常规选项。
这些选项的值在运行时中解析（每个请求仅解析一次，不会影响性能）。

此示例展示了如何使用一个环境变量来配置数据库连接：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            dbal:
                # 按照惯例，环境变量的名称总是大写的
                url: '%env(DATABASE_URL)%'
            # ...

    .. code-block:: xml

        <!-- config/packages/doctrine.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                https://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <!-- by convention the env var names are always uppercase -->
                <doctrine:dbal url="%env(DATABASE_URL)%"/>
            </doctrine:config>

        </container>

    .. code-block:: php

        // config/packages/doctrine.php
        $container->loadFromExtension('doctrine', [
            'dbal' => [
                // by convention the env var names are always uppercase
                'url' => '%env(DATABASE_URL)%',
            ]
        ]);

下一步是在shell、Web服务器等中定义这些环境变量的值。
以下各节会对此进行解释，但为了保护你的应用免受未定义的环境变量的影响，你可以使用
``parameters`` 键来为它们提供一个默认值：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            env(DATABASE_URL): 'sqlite:///%kernel.project_dir%/var/data.db'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="env(DATABASE_URL)">sqlite:///%kernel.project_dir%/var/data.db</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('env(DATABASE_URL)', 'sqlite:///%kernel.project_dir%/var/data.db');

.. seealso::

    环境变量的值只能是字符串，但Symfony包含了一些
    :doc:`环境变量处理器 </configuration/env_var_processors>`
    来转换它们的内容（例如，将字符串值转换为整数）。

为了定义环境变量的实际值，Symfony根据应用是在生产中还是在本地开发机器中运行，提出了不同的解决方案。

.. _configuration-env-var-in-dev:
.. _config-dot-env:

在开发中配置环境变量
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony不是在shell或Web服务器中定义环境变量，而是基于位于项目根目录的名为
``.env``（带点号前缀）的文件，提出了一种在本地计算机中定义它们的便捷方法。

``.env`` 文件在每个请求中被读取并解析，并将其环境变量添加到 ``$_ENV``
 PHP变量中。现有的环境变量永远不会被 ``.env`` 中定义的值覆盖，因此你可以将两者结合起来。

例如，本文前面所示的 ``.env`` 文件的内容定义了 ``DATABASE_URL`` 环境变量的值：

.. code-block:: bash

    # .env
    DATABASE_URL="mysql://db_user:db_password@127.0.0.1:3306/db_name"

除了你自己的环境变量之外，``.env``
文件还包含了由应用中安装的第三方软件包定义的环境变量（它们在安装软件包时由
:doc:`Symfony Flex </setup/flex>` 自动添加）。

管理多个.env文件
............................

``.env`` 文件定义了所有环境变量的默认值。
但是，根据环境（例如，使用不同的数据库进行测试）或依赖于计算机（例如，在开发时在本地计算机中使用不同的OAuth令牌）来重写其中一些值是很常见的。

这就是为什么你可以定义多个重写默认环境变量的 ``.env`` 文件。这些是按优先级排列的文件（在一个文件中找到的环境变量将重写下面所有文件的相同环境变量）：

* ``.env.<environment>.local`` (例如 ``.env.test.local``)：
  仅为本地计算机中的某些环境定义/重写环境变量；
* ``.env.<environment>`` (例如 ``.env.test``)：为所有计算机的某些环境定义/重写环境变量；
* ``.env.local``：为所有环境定义/重写环境变量，但仅限于本地计算机;
* ``.env``：定义环境变量的默认值。

``.env`` 和 ``.env.<environment>`` 文件应该提交到共享仓库，因为它们对所有开发人员都是相同的。
但是，**不应该提交** ``.env.local`` 和 ``.env.<environment>.local``
文件，因为只有你才会使用它们。实际上，Symfony附带的 ``.gitignore`` 文件会阻止提交它们。

.. caution::

    在2018年11月之前创建的应用有一个稍微不同的系统，涉及一个 ``.env.dist`` 文件。
    有关升级的信息，请参阅 :doc:`/configuration/dot-env-changes`。

.. _configuration-env-var-in-prod:

在生产中配置环境变量
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在生产中，``.env`` 文件同样在每个请求中被读取并解析，以便你可以重写已在服务器中定义的环境变量。
为了提高性能，你可以运行 ``dump-env`` 命令（仅在使用
:doc:`Symfony Flex </setup/flex>` 1.2或更高版本时可用）。

此命令一次性解析所有的 ``.env`` 文件，并将其中的内容编译为一个名为 ``.env.local.php``
的新的PHP优化文件。自此，Symfony将加载被解析的文件，而不是再次解析 ``.env`` 文件：

.. code-block:: terminal

    $ composer dump-env prod

.. tip::

    更新你的部署工具/工作流以在每次部署后运行 ``dump-env`` 命令以提高应用性能。

.. caution::

    请注意，转储(dumping) ``$_SERVER`` 和 ``$_ENV`` 变量或输出
    ``phpinfo()`` 内容将会显示环境变量的值，从而暴露敏感信息（如数据库凭据）。

    环境变量的值也暴露在 :doc:`Symfony分析器 </profiler>` 的Web界面中。
    在实践中，这应该不是问题，因为必须 *永远* 不在生产中启用Web分析器。

在Web服务器中定义环境变量
................................................

使用前面介绍的 ``.env`` 文件是在Symfony应用中使用环境变量的推荐方法。
但是，如果你愿意，还可以在Web服务器配置中定义这些环境变量：

.. configuration-block::

    .. code-block:: apache

        <VirtualHost *:80>
            # ...

            SetEnv DATABASE_URL "mysql://db_user:db_password@127.0.0.1:3306/db_name"
        </VirtualHost>

    .. code-block:: nginx

        fastcgi_param DATABASE_URL "mysql://db_user:db_password@127.0.0.1:3306/db_name";

.. tip::

    SymfonyCloud是针对Symfony应用优化的云服务，它定义了一些在生产中 `管理环境变量的工具`_。

继续阅读
-----------

恭喜！你已经了解了Symfony的基础知识。接下来，按照指南了解Symfony的 *每个* 部分。阅读：

* :doc:`/forms`
* :doc:`/doctrine`
* :doc:`/service_container`
* :doc:`/security`
* :doc:`/email`
* :doc:`/logging`

还有很多其他主题。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    configuration/*

.. _`环境变量`: https://en.wikipedia.org/wiki/Environment_variable
.. _`软连接`: https://en.wikipedia.org/wiki/Symbolic_link
.. _`管理环境变量的工具`: https://symfony.com/doc/master/cloud/cookbooks/env.html
