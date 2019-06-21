.. index::
   single: Security; Customizing form login redirect

使用 ``form_login`` 身份认证提供器
============================================

.. caution::

    要完全控制你的登录表单，我们建议你
    :doc:`使用安保认证器来构建表单登录认证 </security/form_login_setup>`。

Symfony附带一个内置的 ``form_login`` 系统，可自动处理一个登录表单的POST。
在开始之前，请确保已按照 :doc:`安全指南 </security>` 创建了User类。

form_login设置
----------------

首先，在防火墙下启用 ``form_login``::

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    anonymous: ~
                    form_login:
                        login_path: login
                        check_path: login

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="main">
                    <anonymous/>
                    <form-login login-path="login" check-path="login"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            'firewalls' => [
                'main' => [
                    'anonymous'  => null,
                    'form_login' => [
                        'login_path' => 'login',
                        'check_path' => 'login',
                    ],
                ],
            ],
        ]);

.. tip::

    ``login_path`` 和 ``check_path`` 也可以是路由名称。
    但该路由不能有强制性通配符 - 例如 ``/login/{foo}``，在那里，``foo`` 没有默认值。

现在，当安全系统启动认证进程时，它会将用户重定向到 ``/login`` 登录表单。
实现此登录表单是你的工作。首先，创建一个新的 ``SecurityController``::

    // src/Controller/SecurityController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class SecurityController extends AbstractController
    {
    }

接下来，配置你之前在 ``form_login`` 配置下使用的路由（``login``）：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/SecurityController.php

        // ...
        use Symfony\Component\Routing\Annotation\Route;

        class SecurityController extends AbstractController
        {
            /**
             * @Route("/login", name="login", methods={"GET", "POST"})
             */
            public function login()
            {
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        login:
            path:       /login
            controller: App\Controller\SecurityController::login
            methods: GET|POST

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="login" path="/login" controller="App\Controller\SecurityController::login" methods="GET|POST"/>
        </routes>

    ..  code-block:: php

        // config/routes.php
        use App\Controller\SecurityController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('login', '/login')
                ->controller([SecurityController::class, 'login'])
                ->methods(['GET', 'POST'])
            ;
        };

很好！接下来，添加逻辑到 ``login()`` 以显示登录表单::

    // src/Controller/SecurityController.php
    use Symfony\Component\Security\Http\Authentication\AuthenticationUtils;

    public function login(AuthenticationUtils $authenticationUtils)
    {
        // 如果有的话，获取登录错误
        $error = $authenticationUtils->getLastAuthenticationError();

        // 用户最后一次输入的用户名
        $lastUsername = $authenticationUtils->getLastUsername();

        return $this->render('security/login.html.twig', [
            'last_username' => $lastUsername,
            'error'         => $error,
        ]);
    }

.. note::

    如果你收到一个缺少 ``$authenticationUtils`` 参数的错误，可能是因为应用的控制器未定义为服务并使用标记为
    ``controller.service_arguments`` 标签，就如在
    :ref:`默认的services.yaml配置 <service-container-services-load-example>`
    中所做的那样。

不要让这个控制器迷惑你。正如你稍后将看到的那样，当用户提交表单时，安全系统会自动为你处理表单提交。
如果用户提交了无效的用户名或密码，则此控制器会从安全系统中读取表单提交错误，以便将其显示给用户。

换句话说，你的工作是 *显示* 登录表单和可能发生的任何登录错误，但安全系统本身负责检查提交的用户名和密码并认证用户身份。

最后，创建模板：

.. code-block:: html+twig

    {# templates/security/login.html.twig #}
    {# ... 你可能会扩展你的基础模板，如 base.html.twig #}

    {% if error %}
        <div>{{ error.messageKey|trans(error.messageData, 'security') }}</div>
    {% endif %}

    <form action="{{ path('login') }}" method="post">
        <label for="username">Username:</label>
        <input type="text" id="username" name="_username" value="{{ last_username }}"/>

        <label for="password">Password:</label>
        <input type="password" id="password" name="_password"/>

        {#
            如果要控制用户认证成功后重定向的URL（更多详细信息稍后说明）
            <input type="hidden" name="_target_path" value="/account"/>
        #}

        <button type="submit">login</button>
    </form>

.. tip::

    传递给模板的 ``error`` 变量是
    :class:`Symfony\\Component\\Security\\Core\\Exception\\AuthenticationException`
    的实例。它可能包含有关认证失败的更多信息 - 甚至是敏感信息，因此请理智地使用它！

该表单看起来和其他表单差不错，但它通常遵循一些约定：

* ``<form>`` 元素发送一个 ``POST`` 请求到 ``login``
  路由，因为这是你在 ``security.yaml`` 的 ``form_login`` 键下配置的内容;
* 用户名字段使用 ``_username`` 名称，密码字段使用 ``_password`` 名称。

.. tip::

    实际上，所有这些都可以在 ``form_login`` 键下配置。有关详细信息，请参阅
    :ref:`reference-security-firewall-form-login`。

.. caution::

    此登录表单目前不受CSRF攻击保护。阅读 :ref:`form_login-csrf`，了解如何保护你的登录表单。

就是这样！提交表单时，安全系统将自动检查用户的凭据，并对用户进行认证或将用户发送回可以显示错误的登录表单。

浏览整个过程：

#. 用户尝试访问一个受保护的资源;
#. 防火墙通过将用户重定向到登录表单（``/login``）来启动认证进程;
#. ``/login`` 页面通过本例中创建的路由和控制器渲染登录表单;
#. 用户提交登录表单到 ``/login``;
#. 安全系统拦截该请求，然后检查用户提交的凭据，如果凭据正确则对用户进行认证，不正确则将用户发送回登录表单。

.. _form_login-csrf:

登录表单中的CSRF保护
------------------------------

使用将隐藏的CSRF令牌添加到登录表单中的技术，可以防止 `登录CSRF攻击`_。
安全组件已提供CSRF保护，但你需要在使用之前配置一些选项。

首先，在安全配置中配置表单登录时使用的CSRF令牌提供器。
你可以将其设置为使用安全组件中可用的默认提供器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                secured_area:
                    # ...
                    form_login:
                        # ...
                        csrf_token_generator: security.csrf.token_manager

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="secured_area">
                    <!-- ... -->
                    <form-login csrf-token-generator="security.csrf.token_manager"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'secured_area' => [
                    // ...
                    'form_login' => [
                        // ...
                        'csrf_token_generator' => 'security.csrf.token_manager',
                    ],
                ],
            ],
        ]);

.. _csrf-login-template:

然后，使用Twig模板中的 ``csrf_token()`` 函数生成一个CSRF令牌并将其存储为表单的隐藏字段。
默认情况下，该HTML字段必须命名为 ``_csrf_token``，而用于生成值的字符串必须为 ``authenticate``：

.. code-block:: html+twig

    {# templates/security/login.html.twig #}

    {# ... #}
    <form action="{{ path('login') }}" method="post">
        {# ... 用户登录的字段 #}

        <input type="hidden" name="_csrf_token"
            value="{{ csrf_token('authenticate') }}"
        >

        <button type="submit">login</button>
    </form>

在此之后，你已经保护你的登录表单免受CSRF攻击。

.. tip::

    你可以在你的配置中进行一些设置，通过设置 ``csrf_parameter``
    来修改该字段的名称；通过设置 ``csrf_token_id`` 来修改令牌ID。

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/security.yaml
            security:
                # ...

                firewalls:
                    secured_area:
                        # ...
                        form_login:
                            # ...
                            csrf_parameter: _csrf_security_token
                            csrf_token_id: a_private_string

        .. code-block:: xml

            <!-- config/packages/security.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <srv:container xmlns="http://symfony.com/schema/dic/security"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:srv="http://symfony.com/schema/dic/services"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd">

                <config>
                    <!-- ... -->

                    <firewall name="secured_area">
                        <!-- ... -->
                        <form-login csrf-parameter="_csrf_security_token"
                            csrf-token-id="a_private_string"
                        />
                    </firewall>
                </config>
            </srv:container>

        .. code-block:: php

            // config/packages/security.php
            $container->loadFromExtension('security', [
                // ...

                'firewalls' => [
                    'secured_area' => [
                        // ...
                        'form_login' => [
                            // ...
                            'csrf_parameter' => '_csrf_security_token',
                            'csrf_token_id'  => 'a_private_string',
                        ],
                    ],
                ],
            ]);

成功后重定向
-------------------------

默认情况下，表单将重定向到用户请求的URL（即触发登录表单的URL）。
例如，如果用户请求 ``http://www.example.com/admin/post/18/edit``，则在认证成功登录后，他们将被发送回 ``http://www.example.com/admin/post/18/edit``。

这是通过在会话中存储请求的URL来完成的。
如果会话中不存在对应URL（可能用户直接进入登录页面），则用户被重定向到 ``/`` （即主页）。
你可以通过多种方式更改此行为。

更改默认页面
~~~~~~~~~~~~~~~~~~~~~~~~~

如果会话中没有存储先前页面，请定义 ``default_target_path`` 选项来更改用户重定向的目标页面。
该值可以是一个相对/绝对URL或一个Symfony路由名称：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    form_login:
                        # ...
                        default_target_path: after_login_route_name

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <form-login default-target-path="after_login_route_name"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main' => [
                    // ...

                    'form_login' => [
                        // ...
                        'default_target_path' => 'after_login_route_name',
                    ],
                ],
            ],
        ]);

始终重定向到默认页面
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

定义 ``always_use_default_target_path`` 布尔选项以忽略先前请求的URL并始终重定向到默认页面：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    form_login:
                        # ...
                        always_use_default_target_path: true

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login always-use-default-target-path="true"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main' => [
                    // ...

                    'form_login' => [
                        // ...
                        'always_use_default_target_path' => true,
                    ],
                ],
            ],
        ]);

.. _control-the-redirect-url-from-inside-the-form:

使用请求参数控制重定向
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

可以使用GET和POST请求的 ``_target_path`` 参数定义登录后重定向的URL。
其值必须是一个相对或绝对URL，而不是一个Symfony路由名称。

使用一个查询字符串参数和GET请求来定义重定向URL：

.. code-block:: text

    http://example.com/some/path?_target_path=/dashboard

使用一个隐藏的表单字段和POST请求来定义重定向URL：

.. code-block:: html+twig

    {# templates/security/login.html.twig #}
    <form action="{{ path('login') }}" method="post">
        {# ... #}

        <input type="hidden" name="_target_path" value="{{ path('account') }}"/>
        <input type="submit" name="login"/>
    </form>

使用 ``Referer`` 中的URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果会话中没有存储先前的URL且请求中不包含任何 ``_target_path`` 参数，则可以使用
``HTTP_REFERER`` 标头的值来代替，因为这通常是相同的。
请定义 ``use_referer`` 布尔选项以启用此行为：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    form_login:
                        # ...
                        use_referer: true

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login use-referer="true"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main' => [
                    // ...
                    'form_login' => [
                        // ...
                        'use_referer' => true,
                    ],
                ],
            ],
        ]);

.. note::

    引用URL仅在与 ``login_path`` 路由生成的URL不同时使用，以避免重定向循环。

.. _redirecting-on-login-failure:

失败后重定向
-------------------------

登录失败后（例如，提交的用户名或密码无效），用户将被重定向回登录表单本身。
使用 ``failure_path`` 选项通过一个相对/绝对URL或Symfony路由名称来定义一个新的目标页面：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    form_login:
                        # ...
                        failure_path: login_failure_route_name

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login failure-path="login_failure_route_name"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main' => [
                    // ...
                    'form_login' => [
                        // ...
                        'failure_path' => 'login_failure_route_name',
                    ],
                ],
            ],
        ]);

也可以通过 ``_failure_path`` 请求参数来设置此选项：

.. code-block:: text

    http://example.com/some/path?_failure_path=/forgot-password

.. code-block:: html+twig

    {# templates/security/login.html.twig #}
    <form action="{{ path('login') }}" method="post">
        {# ... #}

        <input type="hidden" name="_failure_path" value="{{ path('forgot_password') }}"/>
        <input type="submit" name="login"/>
    </form>

自定义目标页面和失败请求参数
-----------------------------------------------------

要定义登录成功和失败后重定向的请求属性的名称，可以通过防火墙的登录表单下的 ``target_path_parameter`` 和
``failure_path_parameter`` 选项进行自定义。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    form_login:
                        target_path_parameter: go_to
                        failure_path_parameter: back_to

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login target-path-parameter="go_to"/>
                    <form-login failure-path-parameter="back_to"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main' => [
                    // ...
                    'form_login' => [
                        'target_path_parameter' => 'go_to',
                        'failure_path_parameter' => 'back_to',
                    ],
                ],
            ],
        ]);

使用上面的配置，查询字符串参数和隐藏的表单字段现在已经完全自定义：

.. code-block:: text

    http://example.com/some/path?go_to=/dashboard&back_to=/forgot-password

.. code-block:: html+twig

    {# templates/security/login.html.twig #}
    <form action="{{ path('login') }}" method="post">
        {# ... #}

        <input type="hidden" name="go_to" value="{{ path('dashboard') }}"/>
        <input type="hidden" name="back_to" value="{{ path('forgot_password') }}"/>
        <input type="submit" name="login"/>
    </form>

.. _`登录CSRF攻击`: https://en.wikipedia.org/wiki/Cross-site_request_forgery#Forging_login_requests
