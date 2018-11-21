关于用户提供器的一切
========================

应用中的每个 ``User`` 类通常都需要自己的“用户提供器”：一个有两个工作的类：

**从会话中重新加载用户**
    在每个请求开始时（除非你的防火墙是 ``stateless``），Symfony将从会话中加载 ``User`` 对象。
    为了确保它不是过时的，用户提供器会“刷新它”。例如，一个Doctrine用户提供器会在数据库中查询新数据。
    然后，Symfony会检查该用户是否已“更改”，如果已改变则取消对该用户的认证（请参阅 :ref:`user_session_refresh`）。

**为某些功能加载用户**
    某些功能，比如 ``switch_user``、``remember_me`` 和许多内置的
    :doc:`认证提供器 </security/auth_providers>`
    ，通过用户提供器来使用“用户名”（或电子邮件，或任何你想要的字段）来加载一个User对象。

Symfony附带几个内置的用户提供器：

.. toctree::
    :hidden:

    entity_provider

* :doc:`实体:（从数据库加载用户）</security/entity_provider>`
* :doc:`ldap </security/ldap>`
* ``memory`` (在配置中硬编码用户)
* ``chain`` (尝试多个用户提供器)

或者，你可以创建一个 :ref:`自定义用户提供器 <custom-user-provider>`。

用户提供器都被配置在 ``config/packages/security.yaml`` 的 ``providers`` 键下面，每个都有不同的配置选项：

.. code-block:: yaml

    # config/packages/security.yaml
    security:
        # ...
        providers:
            # 这将成为该提供器的内部名称，通常该名称并不重要
            # 但可用于指定你需要将哪个提供器应用于哪个防火墙（高级案例）
            # 或用于指定一个特殊的认证提供器
            some_provider_key:

                # 提供器类型- 上面所述中的一个
                memory:
                    # 该提供器的自定义选项
                    users:
                        user:  { password: '%env(USER_PASSWORD)%', roles: [ 'ROLE_USER' ] }
                        admin: { password: '%env(ADMIN_PASSWORD)%', roles: [ 'ROLE_ADMIN' ] }

            a_chain_provider:
                chain:
                    providers: [some_provider_key, another_provider_key]

.. _custom-user-provider:

创建自定义用户提供程序
-------------------------------

如果你从自定义位置加载用户（例如，通过API或旧数据库连接），则需要创建一个自定义的用户提供器类。
首先，请确保你已按照 :doc:`安全指南 </security>` 创建了 ``User`` 类。

如果你使用 ``make:user`` 命令创建了你的 ``User``
类（并且你回答了表示你需要一个自定义用户提供器的问题），那么该命令将生成一个很好的框架以帮助你入门::

    // .. src/Security/UserProvider.php
    namespace App\Security;

    use Symfony\Component\Security\Core\Exception\UnsupportedUserException;
    use Symfony\Component\Security\Core\Exception\UsernameNotFoundException;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\User\UserProviderInterface;

    class UserProvider implements UserProviderInterface
    {
        /**
         * 如果你使用switch_user或remember_me等功能，Symfony将会调用本方法。
         *
         * 如果你不使用这些功能，则无需实现本方法。
         *
         * @return UserInterface
         *
         * @throws UsernameNotFoundException if the user is not found
         */
        public function loadUserByUsername($username)
        {
            // 从数据源加载一个User对象或抛出UsernameNotFoundException。
            // 该 $username 参数实际上可能不是一个用户名：
            // 它是你的User类中的 getUsername() 方法返回的任何值。
            throw new \Exception('TODO: fill in loadUserByUsername() inside '.__FILE__);
        }

        /**
         * 从会话中重新加载用户对象，然后刷新该用户。
         *
         * 当一个用户登录后，在每个请求开始时，都将从会话中加载User对象，然后调用本方法。
         * 你的工作是确保该用户的数据仍然新鲜，例如，重新查询以获取新的用户数据。
         *
         * 如果你的防火墙是 “stateless: false”（对于纯API），则不会调用本方法。
         *
         * @return UserInterface
         */
        public function refreshUser(UserInterface $user)
        {
            if (!$user instanceof User) {
                throw new UnsupportedUserException(sprintf('Invalid user class "%s".', get_class($user)));
            }

            // 确保其数据是“新鲜”的之后，返回一个User对象。
            // 如果用户不再存在，则抛出一个UsernameNotFoundException。
            throw new \Exception('TODO: fill in refreshUser() inside '.__FILE__);
        }

        /**
         * 告诉Symfony将此User类用于此提供器。
         */
        public function supportsClass($class)
        {
            return User::class === $class;
        }
    }

大部分工作已经完成！阅读代码中的注释并更新TODO部分以完成该用户提供器。

完成后，通过在 ``security.yaml`` 中添加以下内容，来告诉Symfony有关用户提供器的信息：

.. code-block:: yaml

    # config/packages/security.yaml
    security:
        providers:
            # 内部名称 - 可以使任何东西
            your_custom_user_provider:
                id: App\Security\UserProvider

仅此而已！当你使用需要一个用户提供器的任何功能时，你的提供器都将被使用！
如果你有多个防火墙和多个供应器，你可以指定使用 *哪个* 供应器。
具体方法是在你的防火墙下添加一个 ``provider`` 键，并且为你的用户提供器配置一个内部名称。

.. _user_session_refresh:

了解用户是如何从会话中刷新的
------------------------------------------------------

在每个请求结束时（除非你的防火墙是 ``stateless``），你的 ``User`` 对象被序列化到会话中。
在下一个请求开始时，它会被反序列化，然后传递给你的用户提供器以“刷新”它（例如，使用Doctrine查询一个用户）。

然后，“比较”两个User对象（来自会话的原始对象和已刷新的User对象）以查看它们是否“相等”。
默认情况下，内核的 ``AbstractToken`` 类将同
``getPassword()``、``getSalt()`` 以及 ``getUsername()`` 的返回值进行比较。
如果其中的任何一个有区别，你的用户将被注销。
这是一项安全措施，可确保在核心用户数据发生更改时可以解除恶意用户的认证。

但是，在某些情况下，此过程可能会导致意外的认证问题。
如果你在认证时遇到问题，可能是你 *已经* 认证成功，但在第一次重定向后你立即失去了该认证信息。

在这种情况下，如果你有序列化逻辑（例如 ``SerializableInterface``），则请审查它，以确保所有必要的字段都已序列化。

用EquatableInterface手动比较用户
------------------------------------------------

或者，如果你需要更多地控制“比较用户”的过程，请使你的User类实现
:class:`Symfony\\Component\\Security\\Core\\User\\EquatableInterface`。
然后，在比较用户时将调用你的 ``isEqualTo()`` 方法。
