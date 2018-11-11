.. index::
   single: Security; "Remember me"

如何添加“记住我”登录功能
============================================

用户通过身份验证后，其凭据通常会存储在会话中。
这意味着当会话结束时，该凭据会被注销，并且在下次访问应用时用户必须再次提供其登录信息。
使用带有 ``remember_me`` 防火墙选项的cookie，你可以让用户选择将登录的保留时间比会话的周期更长：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    remember_me:
                        secret:   '%kernel.secret%'
                        lifetime: 604800 # 604800是一周的秒数
                        path:     /
                        # 默认情况下，通过选中登录表单中的复选框启用该功能（请参阅下文）
                        # 取消注释以下行以始终启用它。
                        #always_remember_me: true

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="utf-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->

                    <!-- 604800 is 1 week in seconds -->
                    <remember-me
                        secret="%kernel.secret%"
                        lifetime="604800"
                        path="/" />
                    <!-- by default, the feature is enabled by checking a checkbox
                         in the login form (see below), add always-remember-me="true"
                         to always enable it. -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array(
                    // ...
                    'remember_me' => array(
                        'secret'   => '%kernel.secret%',
                        'lifetime' => 604800, // 1 week in seconds
                        'path'     => '/',
                        // by default, the feature is enabled by checking a
                        // checkbox in the login form (see below), uncomment
                        // the following line to always enable it.
                        //'always_remember_me' => true,
                    ),
                ),
            ),
        ));

``remember_me`` 防火墙定义了以下配置选项：

``secret`` (**required**)
    用于加密cookie内容的值。通常使用 ``APP_SECRET`` 环境变量中定义的 ``secret`` 值。

``name`` (默认值: ``REMEMBERME``)
    用于保持用户登录的cookie的名称。
    如果在同一应用的多个防火墙中启用了 ``remember_me`` 功能，请确保为每个防火墙的cookie选择其他名称。
    否则，你将面临许多与安全相关的问题。

``lifetime`` (默认值: ``31536000``)
    保持用户登录状态的秒数。默认情况下，用户将保持登录一年。

``path`` (默认值: ``/``)
    使用与此功能关联的cookie的路径。
    默认情况下，该cookie将被应用到整个网站，但你可以限制为特定的部分（例如 ``/forum``、``/admin``）。

``domain`` (默认值: ``null``)
    使用与此功能关联的cookie的域。默认情况下，该cookie使用从 ``$_SERVER`` 中获取的当前域。

``secure`` (默认值: ``false``)
    如果值为 ``true``，与此功能关联的cookie将通过HTTPS安全连接发送给用户。

``httponly`` (默认值: ``true``)
    如果值为 ``true``，与此功能关联的cookie只能通过HTTP协议访问。
    这意味着脚本语言（例如JavaScript）无法访问该cookie。

``samesite`` (默认值: ``null``)
    如果设置为 ``strict``，则与此功能关联的cookie将不会与跨站点请求一起发送，即使跟随常规链接也是如此。

``remember_me_parameter`` (默认值: ``_remember_me``)
    表示表单字段的名称，系统会检查该字段以确定是否应启用“记住我”功能。
    继续阅读本文，了解如何有条件地启用此功能。

``always_remember_me`` (default value: ``false``)
    如果值为 ``true``，``remember_me_parameter`` 的值会被忽略，并且始终启用“记住我”功能，无论最终用户的意愿如何。

``token_provider`` (默认值: ``null``)
    定义要使用的令牌提供器的服务标识。
    默认情况下，令牌存储在cookie中。
    例如，你可能希望将令牌存储在数据库中，以使cookie中没有（已哈希）密码版本。
    DoctrineBridge附带一个
    ``Symfony\Bridge\Doctrine\Security\RememberMe\DoctrineTokenProvider``
    供你使用。

强制用户选择记住我的功能
------------------------------------------------------

为用户提供一个选择记住我的功能的机会是个好实践，因为该功能并不总是适用的。
通常的做法是在登录表单中添加一个复选框。
通过给复选框指定 ``_remember_me`` 名称（或使用你指定的 ``remember_me_parameter``），
当用户选中复选框并且成功登录时，cookie将被自动设置。
因此，你的特定登录表单最终可能如下所示：

.. code-block:: html+twig

    {# templates/security/login.html.twig #}

    <form method="post">
        {# ... your form fields #}

        <input type="checkbox" id="remember_me" name="_remember_me" checked />
        <label for="remember_me">Keep me logged in</label>

        {# ... #}
    </form>

然后，当cookie保持有效时，用户将在后续访问时自动登录。

强制用户在访问某些资源之前重新进行身份验证
----------------------------------------------------------------------

当用户返回你的站点时，根据cookie中存储的记住我信息，他们会自动进行身份验证。
这允许用户访问受保护的资源，就相当于用户在访问站点时实际的进行了身份验证一样。

但是，在某些情况下，你可能希望在访问某些资源之前强制用户实际的重新进行身份验证。
例如，你可能不想“记住我”用户直接更改其密码。你可以通过一些特殊的“角色”来做到这一点::

    // src/Controller/AccountController.php
    // ...

    public function accountInfo()
    {
        // 允许任何已认证用户 - 我们不关心他是刚登录的，还是通过记住我的cookie来登录的
        $this->denyAccessUnlessGranted('IS_AUTHENTICATED_REMEMBERED');

        // ...
    }

    public function resetPassword()
    {
        // 要求用户是在 *当前* 会话中登录的
        // 如果他们是通过记住我的cookie登录的，那他们将被重定向到登录页面
        $this->denyAccessUnlessGranted('IS_AUTHENTICATED_FULLY');

        // ...
    }
