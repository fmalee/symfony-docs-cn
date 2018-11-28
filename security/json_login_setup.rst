如何构建JSON身份认证端点
===========================================

在此章节中，你将构建一个JSON端点以登录你的用户。
当用户登录时，你可以从任何地方加载用户 - 比如数据库。
有关详细信息，请参见 :ref:`security-user-providers`。

首先，在防火墙下启用JSON登录：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    anonymous: ~
                    json_login:
                        check_path: /login

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="main">
                    <anonymous />
                    <json-login check-path="/login" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array(
                    'anonymous'  => null,
                    'json_login' => array(
                        'check_path' => '/login',
                    ),
                ),
            ),
        ));

.. tip::

    ``check_path`` 也可以是一个路由名称。
    但该路由不能有强制性通配符 - 例如 ``/login/{foo}``，在那里，``foo`` 没有默认值。

下一步是在你的应用中配置与此路径匹配的一个路由：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/SecurityController.php

        // ...
        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\HttpFoundation\Request;
        use Symfony\Component\Routing\Annotation\Route;

        class SecurityController extends AbstractController
        {
            /**
             * @Route("/login", name="login")
             */
            public function login(Request $request)
            {
                $user = $this->getUser();

                return $this->json(array(
                    'username' => $user->getUsername(),
                    'roles' => $user->getRoles(),
                ));
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        login:
            path:       /login
            controller: App\Controller\SecurityController::login

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="login" path="/login">
                <default key="_controller">App\Controller\SecurityController::login</default>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('login', new Route('/login', array(
            '_controller' => 'App\Controller\SecurityController::login',
        )));

        return $routes;

现在，当你使用 ``Content-Type: application/json`` 标头以及以下JSON文档作为正文，向
``/login`` URL发出一个 ``POST`` 请求时，安全系统会拦截该请求并启动认证进程：

.. code-block:: json

    {
        "username": "dunglas",
        "password": "MyPassword"
    }

Symfony负责使用提交的用户名和密码对用户进行认证，或者在认证进程失败时触发一个错误。
如果认证成功，则将执行先前定义的控制器。

如果JSON文档具有不同的结构，则可以使用 ``username_path`` 和 ``password_path``
键（它们分别默认为 ``username`` 和 ``password``）来指定访问 ``username`` 和 ``password`` 属性的路径。
例如，如果JSON文档具有以下结构：

.. code-block:: json

    {
        "security": {
            "credentials": {
                "login": "dunglas",
                "password": "MyPassword"
            }
        }
    }

此时安全配置应该是：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    anonymous: ~
                    json_login:
                        check_path:    login
                        username_path: security.credentials.login
                        password_path: security.credentials.password

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="main">
                    <anonymous />
                    <json-login check-path="login"
                                username-path="security.credentials.login"
                                password-path="security.credentials.password" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array(
                    'anonymous'  => null,
                    'json_login' => array(
                        'check_path' => 'login',
                        'username_path' => 'security.credentials.login',
                        'password_path' => 'security.credentials.password',
                    ),
                ),
            ),
        ));
