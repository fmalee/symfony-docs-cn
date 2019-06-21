会话
========

Symfony提供了一个会话对象和几个实用工具，你可以使用这些实用工具在请求之间存储有关用户的信息。

配置
-------------

会话由 `HttpFoundation组件`_ 提供，无论你如何安装Symfony，该组件都包含在所有Symfony应用中。
在使用会话之前，请检查其默认配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            session:
                # 启用应用中的会话支持
                enabled: true
                # 用于会话存储的服务的ID。
                # NULL = 表示使用PHP的默认会话机制
                handler_id: null
                # 提高用于会话的cookie的安全性
                cookie_secure: 'auto'
                cookie_samesite: 'lax'

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <!--
                    enabled: enables the support of sessions in the app
                    handler-id: ID of the service used for session storage
                                NULL means that PHP's default session mechanism is used
                    cookie-secure and cookie-samesite: improves the security of the cookies used for sessions
                -->
                <framework:session enabled="true"
                                   handler-id="null"
                                   cookie-secure="auto"
                                   cookie-samesite="lax"/>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', [
            'session' => [
                // enables the support of sessions in the app
                'enabled' => true,
                // ID of the service used for session storage
                // NULL means that PHP's default session mechanism is used
                'handler_id' => null,
                // improves the security of the cookies used for sessions
                'cookie_secure' => 'auto',
                'cookie_samesite' => 'lax',
            ],
        ]);

将 ``handler_id`` 选项设置为 ``null`` 表示Symfony将使用本地PHP的会话机制。
会话元数据文件将存储在Symfony应用之外的PHP控制目录中。
虽然这通常会简化操作，但是如果写入同一目录的其他应用具有较短的最大生存期设置，
则某些于会话到期相关的选项可能无法按预期工作。

如果你愿意，可以使用 ``session.handler.native_file`` 服务作为 ``handler_id``
以让Symfony自行管理会话。另一个有用的选项是
``save_path``，它定义Symfony将存储会话元数据文件的目录：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            session:
                # ...
                handler_id: 'session.handler.native_file'
                save_path: '%kernel.project_dir%/var/sessions/%kernel.environment%'

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:session enabled="true"
                                   handler-id="session.handler.native_file"
                                   save-path="%kernel.project_dir%/var/sessions/%kernel.environment%"/>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', [
            'session' => [
                // ...
                'handler_id' => 'session.handler.native_file',
                'save_path' => '%kernel.project_dir%/var/sessions/%kernel.environment%',
            ],
        ]);

查看Symfony配置参考以了解有关其他可用的 :ref:`会话配置选项 <config-framework-session>`
的更多信息 。此外，如果你希望将会话元数据存储在数据库而不是文件系统中，请查看
:doc:`/doctrine/pdo_session_storage` 一文。

基本用法
-----------

如果你使用
:class:`Symfony\\Component\\HttpFoundation\\Session\\SessionInterface`
对参数进行类型约束，Symfony会提供一个会话服务，该服务会注入你的服务和控制器中::

    use Symfony\Component\HttpFoundation\Session\SessionInterface;

    class SomeService
    {
        private $session;

        public function __construct(SessionInterface $session)
        {
            $this->session = $session;
        }

        public function someMethod()
        {
            // 在会话中存储属性以供以后重用
            $this->session->set('attribute-name', 'attribute-value');

            // 按名称获取属性
            $foo = $this->session->get('foo');

            // 第二个参数是属性不存在时返回的值
            $filters = $this->session->get('filters', []);

            // ...
        }
    }

.. tip::

    每个 ``SessionInterface`` 的实现都被支持。如果你有自己的实现，请在参数中进行类型约束。

存储的属性保留在会话中，用于该用户会话的剩余部分。默认情况下，会话属性是使用
:class:`Symfony\\Component\\HttpFoundation\\Session\\Attribute\\AttributeBag`
类来管理的键值对。

如果你的应用需求很复杂，你可能更喜欢使用由
:class:`Symfony\\Component\\HttpFoundation\\Session\\Attribute\\NamespacedAttributeBag`
类管理的 :ref:`命名空间化的会话属性 <namespaced-attributes>`。在使用它们之前，请重写
``session`` 服务的定义，以将默认的 ``AttributeBag``替换为 ``NamespacedAttributeBag``：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        session:
            class: Symfony\Component\HttpFoundation\Session\Session
            arguments: ['@session.storage', '@session.namespacedattributebag', '@session.flash_bag']

        session.namespacedattributebag:
            class: Symfony\Component\HttpFoundation\Session\Attribute\NamespacedAttributeBag

.. _session-avoid-start:

避免为匿名用户启动会话
-------------------------------------------

无论何时读取、写入甚至检查会话中的数据，都会自动启动会话。
这意味着如果你需要避免为某些用户创建一个会话cookie，则可能很困难：你必须\ *完全*\避免访问会话。

例如，在这种情况下的一个常见操作需要检查存储在会话中的闪存消息。
以下代码将保证(guarantee)\ *始终*\启动一个会话：

.. code-block:: html+twig

    {# 此检查可防止在没有闪存消息时启动会话 #}
    {% if app.request.hasPreviousSession %}
        {% for message in app.flashes('notice') %}
            <div class="flash-notice">
                {{ message }}
            </div>
        {% endfor %}
    {% endif %}

扩展阅读
-------------------

.. toctree::
    :maxdepth: 1

    /doctrine/pdo_session_storage
    session/locale_sticky_session
    session/php_bridge
    session/proxy_examples

.. _`HttpFoundation组件`: https://symfony.com/components/HttpFoundation
