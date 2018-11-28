如何使用多个安保认证器
========================================

安保认证组件允许你一次使用多个不同的认证器。

入口点是其中一个认证器的服务ID，该认证器的 ``start()`` 方法会被调用以启动认证过程。

具有共享入口点的多个认证器
-----------------------------------------------

有时，你希望为用户提供不同的认证机制，如表单登录和Facebook登录，而两个入口点都会将用户重定向到同一登录页面。
但是，在你的配置中，你必须明确说明要使用的入口点。

以下是对应这种情况的安全配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
             # ...
            firewalls:
                default:
                    anonymous: ~
                    guard:
                        authenticators:
                            - App\Security\LoginFormAuthenticator
                            - App\Security\FacebookConnectAuthenticator
                        entry_point: App\Security\LoginFormAuthenticator

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
                <firewall name="default">
                    <anonymous />
                    <guard entry-point="App\Security\LoginFormAuthenticator">
                        <authenticator>App\Security\LoginFormAuthenticator</authenticator>
                        <authenticator>App\Security\FacebookConnectAuthenticator</authenticator>
                    </guard>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Security\LoginFormAuthenticator;
        use App\Security\FacebookConnectAuthenticator;

        $container->loadFromExtension('security', array(
            // ...
            'firewalls' => array(
                'default' => array(
                    'anonymous' => null,
                    'guard' => array(
                        'entry_point' => '',
                        'authenticators' => array(
                            LoginFormAuthenticator::class,
                            FacebookConnectAuthenticator::class'
                        ),
                    ),
                ),
            ),
        ));

这种方法有一个限制，就是你必须恰好使用同一个入口点。

具有单独入口点的多个认证器
--------------------------------------------------

但是，在某些用例中，你需要使用认证器来保护应用的不同部分。
例如，你有一个用登录表单保护的应用前端的安全区域，以及一个API令牌来保护的API端点。
由于每个防火墙只能配置一个入口点，因此解决方案是将配置拆分为两个单独的防火墙：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            firewalls:
                api:
                    pattern: ^/api/
                    guard:
                        authenticators:
                            - App\Security\ApiTokenAuthenticator
                default:
                    anonymous: ~
                    guard:
                        authenticators:
                            - App\Security\LoginFormAuthenticator
            access_control:
                - { path: ^/login, roles: IS_AUTHENTICATED_ANONYMOUSLY }
                - { path: ^/api, roles: ROLE_API_USER }
                - { path: ^/, roles: ROLE_USER }

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
                <firewall name="api" pattern="^/api/">
                    <guard>
                        <authenticator>App\Security\ApiTokenAuthenticator</authenticator>
                    </guard>
                </firewall>
                <firewall name="default">
                    <anonymous />
                    <guard>
                        <authenticator>App\Security\LoginFormAuthenticator</authenticator>
                    </guard>
                </firewall>
                <rule path="^/login" role="IS_AUTHENTICATED_ANONYMOUSLY" />
                <rule path="^/api" role="ROLE_API_USER" />
                <rule path="^/" role="ROLE_USER" />
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Security\ApiTokenAuthenticator;
        use App\Security\LoginFormAuthenticator;

        $container->loadFromExtension('security', array(
            // ...
            'firewalls' => array(
                'api' => array(
                    'pattern' => '^/api',
                    'guard' => array(
                        'authenticators' => array(
                            ApiTokenAuthenticator::class,
                        ),
                    ),
                ),
                'default' => array(
                    'anonymous' => null,
                    'guard' => array(
                        'authenticators' => array(
                            LoginFormAuthenticator::class,
                        ),
                    ),
                ),
            ),
            'access_control' => array(
                array('path' => '^/login', 'role' => 'IS_AUTHENTICATED_ANONYMOUSLY'),
                array('path' => '^/api', 'role' => 'ROLE_API_USER'),
                array('path' => '^/', 'role' => 'ROLE_USER'),
            ),
        ));
