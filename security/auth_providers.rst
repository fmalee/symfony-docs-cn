内置认证提供者
=================================

如果你需要向应用添加身份验证，我们建议你使用
:doc:`安保认证 </security/guard_authentication>`，因为它可以让你完全控制认证过程。

但是，Symfony还提供了许多内置的身份认证提供器：更易于实现该系统，但更难以自定义。
如果你的身份认证用例完全匹配其中一个，那么它们将是一个很好的选择：

.. toctree::
    :hidden:

    form_login
    json_login_setup

* :doc:`登录表单(form_login) </security/form_login>`
* :ref:`HTTP基本认证(http_basic) <security-http_basic>`
* :doc:`通过HTTP基本或表单登录的LDAP </security/ldap>`
* :doc:`JSON身份验证(json_login) </security/json_login_setup>`
* :ref:`X.509客户端证书认证(x509) <security-x509>`
* :ref:`基于REMOTE_USER的身份验证(remote_user) <security-remote_user>`
* ``simple_form``
* ``simple_pre_auth``

.. _security-http_basic:

HTTP基本认证
-------------------------

HTTP基本认证使用浏览器中的对话框来询问凭据（用户名和密码）。
该凭据在没有任何哈希或加密的情况下发送，因此建议将其与HTTPS一起使用。

要支持HTTP基本认证，请将 ``http_basic`` 键添加到防火墙：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    http_basic:
                        realm: Secured Area

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

                <firewall name="main">
                    <http-basic realm="Secured Area" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array(
                    'http_basic' => array(
                        'realm' => 'Secured Area',
                    ),
                ),
            ),
        ));

仅此而已！Symfony现在将监听任何HTTP基本认证数据。
它将使用你配置的 :doc:`用户提供器 </security/user_provider>` 加载用户信息。

注意：你无法使用 ``http_basic`` 进行 :ref:`注销 <security-logging-out>`。
即使你注销登录，你的浏览器也会“记住”你的凭据，并会在每次请求时发送它们。

.. _security-x509:

X.509客户端证书认证
---------------------------------------

使用客户端证书时，所有的身份验证过程由你的Web服务器本身执行。
例如，如果使用Apache，你将使用它的 ``SSLVerifyClient Require`` 指令。

在安全配置中为特定防火墙启用x509身份验证：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    x509:
                        provider: your_user_provider

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

                <firewall name="main">
                    <!-- ... -->
                    <x509 provider="your_user_provider" />
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
                    'x509' => array(
                        'provider' => 'your_user_provider',
                    ),
                ),
            ),
        ));

默认情况下，防火墙将 ``SSL_CLIENT_S_DN_Email`` 变量提供给用户提供器，并在
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\PreAuthenticatedToken`
中设置 ``SSL_CLIENT_S_DN`` 为凭据。
你可以通过在x509防火墙配置中分别设置 ``user`` 和 ``credentials`` 键来重写这些。

.. _security-pre-authenticated-user-provider-note:

.. note::

    一个认证提供器仅会将创建该请求的用户名通知到用户提供器。
    你将需要创建（或使用）``provider`` 配置参数(在配置示例中为 ``your_user_provider``)引用的一个“用户提供器”。
    此提供器会将该用户名转换为你所选择的用户对象。有关创建或配置用户提供器的更多信息，请参阅：

    * :doc:`/security/user_provider`

.. _security-remote_user:

基于REMOTE_USER的认证
--------------------------------

许多身份验证模块（如Apache的 ``auth_kerb``）使用 ``REMOTE_USER`` 环境变量提供用户名。
应用可以信任此变量，因为身份验证是在请求到达之前发生的。

要使用 ``REMOTE_USER`` 环境变量来配置Symfony，请在安全配置中启用相应的防火墙：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            firewalls:
                main:
                    # ...
                    remote_user:
                        provider: your_user_provider

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services">

            <config>
                <firewall name="main">
                    <remote-user provider="your_user_provider"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array(
                    'remote_user' => array(
                        'provider' => 'your_user_provider',
                    ),
                ),
            ),
        ));

然后，防火墙将为你的用户提供器提供 ``REMOTE_USER`` 环境变量。
你可以通过在 ``remote_user`` 防火墙配置中设置 ``user`` 键来更改使用的变量名称。

.. note::

    就像X509身份验证一样，你需要配置“用户提供器”。请参阅
    :ref:`上一个注释 <security-pre-authenticated-user-provider-note>` 获得更多信息。

.. _`HTTP基本认证`: https://en.wikipedia.org/wiki/Basic_access_authentication
