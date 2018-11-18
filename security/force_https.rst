.. index::
   single: Security; Force HTTPS

如何为不同的URL强制使用HTTPS或HTTP
=============================================

.. tip::

    *最好* 的策略是在所有网址上强制使用 ``https``，你可以通过你的Web服务器配置或 ``access_control`` 来完成。

你可以在安全配置中强制站点的区域使用HTTPS协议。
这是通过 ``access_control`` 规则来使用 ``requires_channel`` 选项完成的。
要在所有URL上强制执行HTTPS，请将 ``requires_channel`` 配置添加到每个访问控制上：

.. configuration-block::

        .. code-block:: yaml

            # config/packages/security.yaml
            security:
                # ...

                access_control:
                    - { path: ^/secure, roles: ROLE_ADMIN, requires_channel: https }
                    - { path: ^/login, roles: IS_AUTHENTICATED_ANONYMOUSLY, requires_channel: https }
                    # catch all other URLs
                    - { path: ^/, roles: IS_AUTHENTICATED_ANONYMOUSLY, requires_channel: https }

        .. code-block:: xml

            <!-- config/packages/security.xml -->
            <?xml version="1.0" encoding="UTF-8"?>
            <srv:container xmlns="http://symfony.com/schema/dic/security"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:srv="http://symfony.com/schema/dic/services"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <config>
                    <!-- ... -->

                    <rule path="^/secure" role="ROLE_ADMIN" requires_channel="https" />
                    <rule path="^/login"
                        role="IS_AUTHENTICATED_ANONYMOUSLY"
                        requires_channel="https"
                    />
                    <rule path="^/"
                        role="IS_AUTHENTICATED_ANONYMOUSLY"
                        requires_channel="https"
                    />
                </config>
            </srv:container>

        .. code-block:: php

            // config/packages/security.php
            $container->loadFromExtension('security', array(
                // ...

                'access_control' => array(
                    array(
                        'path'             => '^/secure',
                        'role'             => 'ROLE_ADMIN',
                        'requires_channel' => 'https',
                    ),
                    array(
                        'path'             => '^/login',
                        'role'             => 'IS_AUTHENTICATED_ANONYMOUSLY',
                        'requires_channel' => 'https',
                    ),
                    array(
                        'path'             => '^/',
                        'role'             => 'IS_AUTHENTICATED_ANONYMOUSLY',
                        'requires_channel' => 'https',
                    ),
                ),
            ));

为了在开发过程中更简便，你还可以使用环境变量，例如 ``requires_channel: '%env(SECURE_SCHEME)%'``。
在你的 ``.env`` 文件中，将 ``SECURE_SCHEME`` 设置默认的 ``http``，而在生产环境中则重写为 ``https``。

请参阅 :doc:`/security/access_control` 以获得关于 ``access_control`` 的更多信息。

也可以在路由配置中指定使用HTTPS，有关详细信息，请参阅 :doc:`/routing/scheme`。

.. note::

    在使用反向代理或负载平衡器时强制HTTPS，需要确保配置正确以避免无限重定向循环;
    有关更多详细信息，请参阅 :doc:`/deployment/proxies`。
