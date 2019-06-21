.. index::
    single: Environment Variables; env vars

如何使用Symfony的环境变量配置
=======================================================

在 :doc:`/configuration` 中，你学习了如何管理应用配置。
在本文中，你将学习如何使用环境变量（或简称“env vars”）来配置其中一些选项，
这是配置敏感选项（如凭据和密码）的常见做法。

.. _config-env-vars:

在配置文件中引用环境变量
---------------------------

首先，使用shell环境或项目根目录下的 ``.env`` 文件来定义环境变量的值。
例如，考虑安装 ``doctrine`` 指令时定义的 ``DATABASE_URL``
环境变量（按照惯例，环境变量名称总是大写的）：

.. code-block:: bash

    # .env
    DATABASE_URL="mysql://db_user:db_password@127.0.0.1:3306/db_name"

然后，你可以在任何配置选项中引用这些环境变量，并用 ``env()`` 来包含它们的名称。 它们的实际值将在运行时中解析（每个请求一次），这样即使在编译之后也可以动态地重新配置转储（dumped）容器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            dbal:
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
                <doctrine:dbal
                    url="%env(DATABASE_URL)%"
                />
            </doctrine:config>

        </container>

    .. code-block:: php

        // config/packages/doctrine.php
        $container->loadFromExtension('doctrine', [
            'dbal' => [
                'url' => '%env(DATABASE_URL)%',
            ]
        ]);

你也可以给 ``env()`` 参数一个默认值，只要要相应的环境变量 *没有* 找到，该值将被使用：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            env(DATABASE_HOST): localhost

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="env(DATABASE_HOST)">localhost</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('env(DATABASE_HOST)', 'localhost');

.. _configuration-env-var-in-prod:

在生产中配置环境变量
-----------------------------------------------

在开发期间，你将使用 ``.env`` 文件来配置环境变量。
在生产服务器上，建议在Web服务器级别配置它们。
如果你使用的是Apache或Nginx，则可以使用以下某项：

.. configuration-block::

    .. code-block:: apache

        <VirtualHost *:80>
            # ...

            SetEnv DATABASE_URL "mysql://db_user:db_password@127.0.0.1:3306/db_name"
        </VirtualHost>

    .. code-block:: nginx

        fastcgi_param DATABASE_URL "mysql://db_user:db_password@127.0.0.1:3306/db_name";

.. caution::

    请注意，转储(dumping) ``$_SERVER`` 和 ``$_ENV`` 变量或输出
    ``phpinfo()`` 内容将会显示环境变量的值，从而暴露敏感信息（如数据库凭据）。

    环境变量的值也暴露在 :doc:`Symfony分析器 </profiler>` 的Web界面中。
    在实践中，这应该不是问题，因为必须 *永远* 不在生产中启用Web分析器。

环境变量处理器
-------------------------------

默认情况下，环境变量的值被视为字符串。
但是，你的代码可能需要其他数据类型，如整数或布尔值。
Symfony通过 *处理器* 解决了这个问题，处理器修改了给定环境变量的内容。
以下示例使用整数处理器将 ``HTTP_PORT`` 环境变量的值转换为整数：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            router:
                http_port: '%env(int:HTTP_PORT)%'

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:router http-port="%env(int:HTTP_PORT)%"/>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', [
            'router' => [
                'http_port' => '%env(int:HTTP_PORT)%',
            ],
        ]);

Symfony提供以下环境变量处理器：

``env(string:FOO)``
    转换 ``FOO`` 为字符串：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            parameters:
                env(SECRET): 'some_secret'
            framework:
                secret: '%env(string:SECRET)%'

        .. code-block:: xml

            <!-- config/packages/framework.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/symfony
                    https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <parameters>
                    <parameter key="env(SECRET)">some_secret</parameter>
                </parameters>

                <framework:config secret="%env(string:SECRET)%"/>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->setParameter('env(SECRET)', 'some_secret');
            $container->loadFromExtension('framework', [
                'secret' => '%env(string:SECRET)%',
            ]);

``env(bool:FOO)``
    转换 ``FOO`` 为布尔值：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            parameters:
                env(HTTP_METHOD_OVERRIDE): 'true'
            framework:
                http_method_override: '%env(bool:HTTP_METHOD_OVERRIDE)%'

        .. code-block:: xml

            <!-- config/packages/framework.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/symfony
                    https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <parameters>
                    <parameter key="env(HTTP_METHOD_OVERRIDE)">true</parameter>
                </parameters>

                <framework:config http-methode-override="%env(bool:HTTP_METHOD_OVERRIDE)%"/>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->setParameter('env(HTTP_METHOD_OVERRIDE)', 'true');
            $container->loadFromExtension('framework', [
                'http_method_override' => '%env(bool:HTTP_METHOD_OVERRIDE)%',
            ]);

``env(int:FOO)``
    转换 ``FOO`` 为整数：

``env(float:FOO)``
    转换 ``FOO`` 为浮点：

``env(const:FOO)``
    查找以 ``FOO`` 命名的常量值：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/security.yaml
            parameters:
                env(HEALTH_CHECK_METHOD): 'Symfony\Component\HttpFoundation\Request::METHOD_HEAD'
            security:
                access_control:
                    - { path: '^/health-check$', methods: '%env(const:HEALTH_CHECK_METHOD)%' }

        .. code-block:: xml

            <!-- config/packages/security.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:security="http://symfony.com/schema/dic/security"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd">

                <parameters>
                    <parameter key="env(HEALTH_CHECK_METHOD)">Symfony\Component\HttpFoundation\Request::METHOD_HEAD</parameter>
                </parameters>

                <security:config>
                    <rule path="^/health-check$" methods="%env(const:HEALTH_CHECK_METHOD)%"/>
                </security:config>
            </container>

        .. code-block:: php

            // config/packages/security.php
            $container->setParameter('env(HEALTH_CHECK_METHOD)', 'Symfony\Component\HttpFoundation\Request::METHOD_HEAD');
            $container->loadFromExtension('security', [
                'access_control' => [
                    [
                        'path' => '^/health-check$',
                        'methods' => '%env(const:HEALTH_CHECK_METHOD)%',
                    ],
                ],
            ]);

``env(base64:FOO)``
    解码 ``FOO`` 的内容，这是base64编码的字符串。

``env(json:FOO)``
    解码 ``FOO`` 内容，这是一个JSON编码的字符串。它返回一个数组或 ``null``：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            parameters:
                env(TRUSTED_HOSTS): '["10.0.0.1", "10.0.0.2"]'
            framework:
                trusted_hosts: '%env(json:TRUSTED_HOSTS)%'

        .. code-block:: xml

            <!-- config/packages/framework.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/symfony
                    https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <parameters>
                    <parameter key="env(TRUSTED_HOSTS)">["10.0.0.1", "10.0.0.2"]</parameter>
                </parameters>

                <framework:config trusted-hosts="%env(json:TRUSTED_HOSTS)%"/>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->setParameter('env(TRUSTED_HOSTS)', '["10.0.0.1", "10.0.0.2"]');
            $container->loadFromExtension('framework', [
                'trusted_hosts' => '%env(json:TRUSTED_HOSTS)%',
            ]);

``env(resolve:FOO)``
    使用相同名称的配置参数值替换 ``FOO`` 字符串：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/sentry.yaml
            parameters:
                env(HOST): '10.0.0.1'
                sentry_host: '%env(HOST)%'
                env(SENTRY_DSN): 'http://%sentry_host%/project'
            sentry:
                dsn: '%env(resolve:SENTRY_DSN)%'

        .. code-block:: xml

            <!-- config/packages/sentry.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd">

                <parameters>
                    <parameter key="env(HOST)">10.0.0.1</parameter>
                    <parameter key="sentry_host">%env(HOST)%</parameter>
                    <parameter key="env(SENTRY_DSN)">http://%sentry_host%/project</parameter>
                </parameters>

                <sentry:config dsn="%env(resolve:SENTRY_DSN)%"/>
            </container>

        .. code-block:: php

            // config/packages/sentry.php
            $container->setParameter('env(HOST)', '10.0.0.1');
            $container->setParameter('sentry_host', '%env(HOST)%');
            $container->setParameter('env(SENTRY_DSN)', 'http://%sentry_host%/project');
            $container->loadFromExtension('sentry', [
                'dsn' => '%env(resolve:SENTRY_DSN)%',
            ]);

``env(csv:FOO)``
    解码CSV编码的 ``FOO`` 字符串的内容：

    .. code-block:: yaml

        parameters:
            env(TRUSTED_HOSTS): "10.0.0.1, 10.0.0.2"
        framework:
           trusted_hosts: '%env(csv:TRUSTED_HOSTS)%'

``env(file:FOO)``
    返回文件的内容，该文件的路径是 ``FOO`` 环境变量的值：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            parameters:
                env(AUTH_FILE): '../config/auth.json'
            google:
                auth: '%env(file:AUTH_FILE)%'

        .. code-block:: xml

            <!-- config/packages/framework.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/symfony
                    https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <parameters>
                    <parameter key="env(AUTH_FILE)">../config/auth.json</parameter>
                </parameters>

                <google auth="%env(file:AUTH_FILE)%"/>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->setParameter('env(AUTH_FILE)', '../config/auth.json');
            $container->loadFromExtension('google', [
                'auth' => '%env(file:AUTH_FILE)%',
            ]);

``env(key:FOO:BAR)``
    用 ``FOO`` 作为键，从储存在 ``BAR`` 环境变量的数组中取出对应值：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            parameters:
                env(SECRETS_FILE): '/opt/application/.secrets.json'
                database_password: '%env(key:database_password:json:file:SECRETS_FILE)%'
                # 如果 SECRETS_FILE 的内容是: {"database_password": "secret"}，它将返回 "secret"

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
                    <parameter key="env(SECRETS_FILE)">/opt/application/.secrets.json</parameter>
                    <parameter key="database_password">%env(key:database_password:json:file:SECRETS_FILE)%</parameter>
                </parameters>
            </container>

        .. code-block:: php

            // config/services.php
            $container->setParameter('env(SECRETS_FILE)', '/opt/application/.secrets.json');
            $container->setParameter('database_password', '%env(key:database_password:json:file:SECRETS_FILE)%');

也可以组合任意数量的处理器：

.. code-block:: yaml

    parameters:
        env(AUTH_FILE): "%kernel.project_dir%/config/auth.json"
    google:
        # 1. 取得 AUTH_FILE 环境变量的值
        # 2. 替换任何配置参数的值以获取配置路径
        # 3. 读取储存在该路径的文件的内容
        # 4. JSON-decodes 该文件的内容并返回它
        auth: '%env(json:file:resolve:AUTH_FILE)%'

自定义环境变量处理器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

也可以为环境变量添加自己的处理器。
首先，创建一个实现
:class:`Symfony\\Component\\DependencyInjection\\EnvVarProcessorInterface` 的类::

    use Symfony\Component\DependencyInjection\EnvVarProcessorInterface;

    class LowercasingEnvVarProcessor implements EnvVarProcessorInterface
    {
        public function getEnv($prefix, $name, \Closure $getEnv)
        {
            $env = $getEnv($name);

            return strtolower($env);
        }

        public static function getProvidedTypes()
        {
            return [
                'lowercase' => 'string',
            ];
        }
    }

要在应用中启用新处理器，请将其注册为服务并使用
``container.env_var_processor`` 标签 :doc:`标记它 </service_container/tags>`。
如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
:ref:`自动配置 <services-autoconfigure>` 已经为你完成该操作了。

常量
---------

该容器还支持将PHP常量设置为参数。有关详细信息，请参阅 :ref:`component-di-parameters-constants`。

其他配置
---------------------------

你可以在 ``config/packages/`` 中混合任何你喜欢的配置格式（YAML，XML和PHP）。
导入PHP文件使你可以灵活地在容器中添加所需的任何内容。
例如，你可以创建一个 ``drupal.php`` 文件，在该文件中根据Drupal的数据库配置设置数据库URL::

    // config/packages/drupal.php

    // 导入 Drupal 的配置
    include_once('/path/to/drupal/sites/default/settings.php');

    // 设置一个 app.database_url 参数
    $container->setParameter('app.database_url', $db_url);

.. _`SetEnv`: http://httpd.apache.org/docs/current/env.html
.. _`fastcgi_param`: http://nginx.org/en/docs/http/ngx_http_fastcgi_module.html#fastcgi_param
