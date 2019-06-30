.. index::
    single: Environment Variable Processors; env vars

.. _env-var-processors:

环境变量处理器
===============================

:ref:`使用环境变量来配置Symfony应用 <config-env-vars>` 是隐藏敏感配置（例如数据库凭据）和使应用真正动态的常见做法。

环境变量的主要问题是它们的值只能是字符串，而你的应用可能需要其他数据类型（整数，布尔值等）。
Symfony用“环境变量处理器”解决了这个问题，它改变了给定环境变量的原始内容。
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

内置环境变量处理器
----------------------------------------

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

``env(require:FOO)``
    ``require()`` 路径是 ``FOO`` 环境变量值的PHP文件，并返回从该文件返回的值。

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            parameters:
                env(PHP_FILE): '../config/.runtime-evaluated.php'
            app:
                auth: '%env(require:PHP_FILE)%'

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
                    <parameter key="env(PHP_FILE)">../config/.runtime-evaluated.php</parameter>
                </parameters>

                <app auth="%env(require:PHP_FILE)%"/>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->setParameter('env(PHP_FILE)', '../config/.runtime-evaluated.php');
            $container->loadFromExtension('app', [
                'auth' => '%env(require:AUTH_FILE)%',
            ]);

    .. versionadded:: 4.3

        Symfony 4.3中引入了 ``require`` 处理器。

``env(trim:FOO)``
    修剪 ``FOO`` 环境变量的内容，从字符串的开头和结尾删除空白。
    这在与 ``file`` 处理器结合使用时特别有用，因为它将删除文件末尾的换行符。

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            parameters:
                env(AUTH_FILE): '../config/auth.json'
            google:
                auth: '%env(trim:file:AUTH_FILE)%'

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

                <google auth="%env(trim:file:AUTH_FILE)%"/>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->setParameter('env(AUTH_FILE)', '../config/auth.json');
            $container->loadFromExtension('google', [
                'auth' => '%env(trim:file:AUTH_FILE)%',
            ]);

    .. versionadded:: 4.3

        Symfony 4.3中引入了 ``trim`` 处理器。

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

``env(default:fallback_param:BAR)``
    当 ``BAR`` 环境变量不可用时，检索参数 ``fallback_param`` 的值：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            parameters:
                # 如果 PRIVATE_KEY 不是有效的文件路径，则返回 raw_key 的内容。
                private_key: '%env(default:raw_key:file:PRIVATE_KEY)%'
                raw_key: '%env(PRIVATE_KEY)%'

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
                    <!-- if PRIVATE_KEY is not a valid file path, the content of raw_key is returned -->
                    <parameter key="private_key">%env(default:raw_key:file:PRIVATE_KEY)%</parameter>
                    <parameter key="raw_key">%env(PRIVATE_KEY)%</parameter>
                </parameters>
            </container>

        .. code-block:: php

            // config/services.php

            // if PRIVATE_KEY is not a valid file path, the content of raw_key is returned
            $container->setParameter('private_key', '%env(default:raw_key:file:PRIVATE_KEY)%');
            $container->setParameter('raw_key', '%env(PRIVATE_KEY)%');

    如果省略fallback参数（例如 ``env(default::API_KEY)``），则返回的值为 ``null``。

    .. versionadded:: 4.3

        Symfony 4.3中引入了 ``default`` 处理器。

``env(url:FOO)``
    解析一个绝对URL并将其组成部分作为关联数组返回。

    .. code-block:: bash

        # .env
        MONGODB_URL="mongodb://db_user:db_password@127.0.0.1:27017/db_name"

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/mongodb.yaml
            mongo_db_bundle:
                clients:
                    default:
                        hosts:
                            - { host: '%env(key:host:url:MONGODB_URL)%', port: '%env(key:port:url:MONGODB_URL)%' }
                        username: '%env(key:user:url:MONGODB_URL)%'
                        password: '%env(key:pass:url:MONGODB_URL)%'
                connections:
                    default:
                        database_name: '%env(key:path:url:MONGODB_URL)%'

        .. code-block:: xml

            <!-- config/packages/mongodb.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd">

                <mongodb:config>
                    <mongodb:client name="default" username="%env(key:user:url:MONGODB_URL)%" password="%env(key:pass:url:MONGODB_URL)%">
                        <mongodb:host host="%env(key:host:url:MONGODB_URL)%" port="%env(key:port:url:MONGODB_URL)%"/>
                    </mongodb:client>
                    <mongodb:connections name="default" database_name="%env(key:path:url:MONGODB_URL)%"/>
                </mongodb:config>
            </container>

        .. code-block:: php

            // config/packages/mongodb.php
            $container->loadFromExtension('mongodb', [
                'clients' => [
                    'default' => [
                        'hosts' => [
                            [
                                'host' => '%env(key:host:url:MONGODB_URL)%',
                                'port' => '%env(key:port:url:MONGODB_URL)%',
                            ],
                        ],
                        'username' => '%env(key:user:url:MONGODB_URL)%',
                        'password' => '%env(key:pass:url:MONGODB_URL)%',
                    ],
                ],
                'connections' => [
                    'default' => [
                        'database_name' => '%env(key:path:url:MONGODB_URL)%',
                    ],
                ],
            ]);

    .. caution::

        为了便于从URL中提取资源，将从 ``path`` 组件中修剪前导的 ``/``。

    .. versionadded:: 4.3

        Symfony 4.3中引入了 ``url`` 处理器。

``env(query_string:FOO)``
    解析给定URL的查询字符串部分，并将其组成部分作为关联数组返回。

    .. code-block:: bash

        # .env
        MONGODB_URL="mongodb://db_user:db_password@127.0.0.1:27017/db_name?timeout=3000"

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/mongodb.yaml
            mongo_db_bundle:
                clients:
                    default:
                        # ...
                        connectTimeoutMS: '%env(int:key:timeout:query_string:MONGODB_URL)%'

        .. code-block:: xml

            <!-- config/packages/mongodb.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd">

                <mongodb:config>
                    <mongodb:client name="default" connectTimeoutMS="%env(int:key:timeout:query_string:MONGODB_URL)%"/>
                </mongodb:config>
            </container>

        .. code-block:: php

            // config/packages/mongodb.php
            $container->loadFromExtension('mongodb', [
                'clients' => [
                    'default' => [
                        // ...
                        'connectTimeoutMS' => '%env(int:key:timeout:query_string:MONGODB_URL)%',
                    ],
                ],
            ]);

    .. versionadded:: 4.3

        Symfony 4.3中引入了 ``query_string`` 处理器。

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
--------------------------------------

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
