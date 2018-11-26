.. index::
   single: Configuration

配置
======================================

Symfony应用可以安装第三方软件包（bundles，库等），为项目引入新功能（:doc:`服务 </service_container>`）。
每个依赖包都可以通过配置文件进行自定义 - 默认情况下 - 在 ``config/`` 目录中。

配置: config/packages/
-------------------------------

每个依赖包的配置可以在 ``config/packages/`` 中找到。
例如，framework bundle 在 ``config/packages/framework.yaml`` 中配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            secret: '%env(APP_SECRET)%'
            #default_locale: en
            #csrf_protection: true
            #http_method_override: true

            # Enables session support. Note that the session will ONLY be started if you read or write from it.
            # Remove or comment this section to explicitly disable session support.
            session:
                handler_id: ~

            #esi: true
            #fragments: true
            php_errors:
                log: true

            cache:
                # Put the unique name of your app here: the prefix seed
                # is used to compute stable namespaces for cache keys.
                #prefix_seed: your_vendor_name/app_name

                # The app cache caches to the filesystem by default.
                # Other options include:

                # Redis
                #app: cache.adapter.redis
                #default_redis_provider: redis://localhost

                # APCu (not recommended with heavy random-write workloads as memory fragmentation can cause perf issues)
                #app: cache.adapter.apcu

    .. code-block:: xml

            <!-- config/packages/framework.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:framework="http://symfony.com/schema/dic/framework"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/framework http://symfony.com/schema/dic/framework/framework-1.0.xsd"
            >
                <framework:config secret="%env(APP_SECRET)%">
                    <!--<framework:csrf-protection enabled="true“ />-->
                    <!--<framework:esi enabled="true" />-->
                    <!--<framework:fragments enabled="true" />-->

                    <!-- Enables session support. Note that the session will ONLY be started if you read or write from it.
                         Remove or comment this section to explicitly disable session support. -->
                    <framework:session />

                    <!-- Put the unique name of your app here: the prefix seed
                         is used to compute stable namespaces for cache keys.
                         <framework:cache prefix-seed="your_vendor_name/app_name">
                         -->
                    <framework:cache>
                        <!-- The app cache caches to the filesystem by default.
                             Other options include: -->

                        <!-- Redis -->
                        <!--<framework:app>cache.adapter.redis</framework:app>-->
                        <!--<framework:default-redis-provider>redis://localhost</framework:default-redis-provider>-->

                        <!-- APCu (not recommended with heavy random-write workloads as memory fragmentation can cause perf issues) -->
                        <!--<framework:app>cache.adapter.apcu</framework:app>-->
                    </framework:cache>

                    <framework:php-errors log="true" />
                </framework:config>
            </container>

    .. code-block:: php

        # config/packages/framework.php
        $container->loadFromExtension('framework', [
            'secret' => '%env(APP_SECRET)%',
            //'default_locale' => 'en',
            //'csrf_protection' => true,
            //'http_method_override' => true,

            // Enables session support. Note that the session will ONLY be started if you read or write from it.
            // Remove or comment this section to explicitly disable session support.
            'session' => [
                'handler_id' => null,
            ],
            //'esi' => true,
            //'fragments' => true,
            'php_errors' => [
                'log' => true,
            ],
            'cache' => [
                //'prefix_seed' => 'your_vendor_name/app_name',

                // The app cache caches to the filesystem by default.
                // Other options include:

                // Redis
                //'app' => 'cache.adapter.redis',
                //'default_redis_provider: 'redis://localhost',

                // APCu (not recommended with heavy random-write workloads as memory fragmentation can cause perf issues)
                //'app' => 'cache.adapter.apcu',
            ],
        ]);

顶级键（此处为 ``framework``）引用一个特定bundle的配置（在本例中为 :doc:`FrameworkBundle </reference/configuration/framework>`）。

.. sidebar:: 配置的格式

    在整个文档中，所有配置示例将以三种格式（YAML，XML和PHP）显示。
    默认情况下使用YAML，但你可以选择你最喜欢的格式。它们之间没有性能差异：

    * :doc:`/components/yaml/yaml_format`: 简单、清楚、可读性强;
    * *XML*: 某些时候比YAML威力强大，而且支持IDE的代码自动完成。
    * *PHP*: 非常强大，但相比标准配置格式缺乏可读性。

配置的参考和展示
---------------------------------

有 *两* 种方法可以知道你可以配置哪些键：

#. 阅读 :doc:`参考章节 </reference/index>`;
#. 使用 ``config:dump-reference`` 命令.

例如，如果要配置与framework bundle相关的内容，
可以通过运行以下命令查看所有可用配置选项的示例展示(dump)：

.. code-block:: terminal

    $ php bin/console config:dump-reference framework

.. index::
   single: Environments; Introduction

.. _page-creation-environments:
.. _page-creation-prod-cache-clear:

.. _config-parameter-intro:

参数键: 参数(变量)
------------------------------------------

配置有一些特殊的顶级(top-level)键。
其中一个叫做 ``parameters``：它用于定义可以在任何其他配置文件中引用的 *变量*。
例如，安装 *translation* 依赖包后，会在 ``config/services.yaml`` 中添加一个 ``locale`` 参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            locale: en

        # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <parameters>
                <parameter key="locale">en</parameter>
            </parameters>

            <!-- ... -->
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('locale', 'en');
        // ...

然后在 ``config/packages/translation.yaml`` 中的 ``framework`` 配置中引用此参数：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/translation.yaml
        framework:
            # any string surrounded by two % is replaced by that parameter value
            default_locale: '%locale%'

            # ...

    .. code-block:: xml

        <!-- config/packages/translation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <!-- any string surrounded by two % is replaced by that parameter value -->
            <framework:config default-locale="%locale%">
                <!-- ... -->
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/translation.php
        $container->loadFromExtension('framework', array(
            // any string surrounded by two % is replaced by that parameter value
            'default_locale' => '%locale%',

            // ...
        ));

你可以在任何配置文件的 ``parameters`` 键下定义所需的任何参数名称。
要引用参数，请用两个 ``%`` 包裹该名称 - 例如 ``%locale%``。

.. seealso::

    你还可以动态设置参数，例如环境变量。请参见 :doc:`/configuration/external_parameters`。

有关参数的更多信息 - 包括如何在控制器内部引用它们 - 请参阅 :ref:`service-container-parameters`。

.. _config-dot-env:
.. _config-parameters-yml:

.env 文件 & 环境变量
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

还有一个已经加载的配置是 ``.env`` 文件，而其内容成为环境变量。
在开发期间，或者如果你的部署很难设置环境变量，那它就很有用。

安装软件包时，会向此文件添加更多环境变量。但你也可以添加自己的变量。

可以使用特殊语法在任何其他配置文件中引用。
例如，如果你安装了 ``doctrine`` 依赖包，那么你的 ``.env`` 文件中将有一个名为 ``DATABASE_URL`` 的环境变量。
该变量已经在 ``config/packages/doctrine.yaml`` 中被引用：

.. code-block:: yaml

    # config/packages/doctrine.yaml
    doctrine:
        dbal:
            url: '%env(DATABASE_URL)%'

            # The `resolve:` prefix replaces container params by their values inside the env variable:
            # url: '%env(resolve:DATABASE_URL)%'

有关环境变量的更多详细信息，请参阅 :ref:`config-env-vars`。

.. caution::

    在2018年11月之前创建的应用有一个稍微不同的系统，涉及一个 ``.env.dist`` 文件。
    有关升级的信息，请参阅 :doc:`/configuration/dot-env-changes`。

``.env`` 文件很特殊，因为它定义了通常在每台服务器上都会变动的值。
例如，你的本地计算机上的数据库凭据可能与你的同事不同。
``.env`` 文件应包含所有环境变量的合理的、非秘密的默认值，并 *应该* 提交到你的仓库。

要使用计算机特定值或敏感值覆盖这些变量，请创建一个 ``env.local`` 文件。
此文件 **不会提交到共享仓库**，而是仅存储在你的计算机上。
事实上，Symfony附带的 ``.gitignore`` 文件阻止了它的提交。

你还可以创建一些可以被加载的其他 ``.env`` 文件：

* ``.env.{environment}``: 例如，``.env.test`` 将在 ``test`` 环境中加载并会提交到你的仓库。

* ``.env.{environment}.local``: 例如，``.env.prod.local`` 将在 ``prod`` 环境中加载，但 *不会* 提交到你的仓库。

如果你决定在生产环境中设置实际的环境变量，则在Symfony检测到存在实际的
``APP_ENV`` 环境变量且被设置为 ``prod`` 的情况下，将不会加载这些 ``.env`` 文件。

环境 & 其他配置文件
-------------------------------------

你可能只有 *一个* 应用，但无论你是否意识到，你需要它在不同的时间有 *不同* 的行为(behave)：

* 在 **开发** 时，你希望你的应用记录所有内容并使用不错的调试工具;

* 部署到 **生产** 后，你希望该应用能针对速度进行优化并仅记录错误。

如何使 *一个* 应用以两种不同的方式运行？根据 *环境*。

你可能已经在不知道的情况下使用了 ``dev`` 环境。在应用部署后，你将使用 ``prod`` 环境。

要了解有关 *如何* 执行和控制每个环境的更多信息，请参阅 :doc:`/configuration/environments`。

继续阅读
-----------

恭喜！你已经了解了Symfony的基础知识。接下来，按照指南了解Symfony的每个部分。阅读：

* :doc:`/forms`
* :doc:`/doctrine`
* :doc:`/service_container`
* :doc:`/security`
* :doc:`/email`
* :doc:`/logging`

还有很多其他主题。

扩展阅读
-------------------

.. toctree::
    :maxdepth: 1
    :glob:

    configuration/*

.. _`Incenteev Parameter Handler`: https://github.com/Incenteev/ParameterHandler
