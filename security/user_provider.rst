安全用户提供器
========================

用户提供器是与Symfony安全相关的PHP类，它有两个工作：

**从会话中重新加载用户**
    在每个请求开始时（除非你的防火墙是 ``stateless``），Symfony将从会话中加载 ``User`` 对象。
    为了确保它不是过时的，用户提供器会“刷新它”。例如，一个Doctrine用户提供器会在数据库中查询新数据。
    然后，Symfony会检查该用户是否已“更改”，如果已改变则取消对该用户的认证（请参阅 :ref:`user_session_refresh`）。

**为某些功能加载用户**
    某些功能，比如 :doc:`用户模拟 </security/impersonating_user>`、:doc:`记住我 </security/remember_me>`
    和许多内置的 :doc:`认证提供器 </security/auth_providers>`
    ，通过用户提供器来使用“用户名”（或电子邮件，或任何你想要的字段）来加载一个User对象。

Symfony附带几个内置的用户提供器：

* :ref:`实体用户提供器 <security-entity-user-provider>` (从数据库加载用户);
* :ref:`LDAP用户提供器 <security-ldap-user-provider>` (从LDAP服务器加载用户);
* :ref:`内存用户提供器 <security-memory-user-provider>` (从配置文件加载用户);
* :ref:`Chain用户提供器 <security-chain-user-provider>` (将两个或多个用户提供器合并到新的用户提供器中).

内置用户提供器可满足大多数应用的所有需求，但你也可以创建自己的
:ref:`自定义用户提供器 <custom-user-provider>`。

.. _security-entity-user-provider:

实体用户提供器
--------------------

这是传统Web应用最常用的用户提供器。用户存储在数据库中，用户提供器使用
:doc:`Doctrine </doctrine>` 来检索它们：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
            # ...

            providers:
                users:
                    entity:
                        # 表示用户实体的类
                        class: 'App\Entity\User'
                        # 要查询的属性 - 例如用户名、电子邮件等
                        property: 'username'
                        # 可选：如果你使用多个Doctrine实体管理器，则此选项定义要使用的实体管理器
                        # manager_name: 'customer'

            # ...

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <provider name="users">
                    <!-- 'class' is the entity that represents users and 'property'
                         is the entity property to query by - e.g. username, email, etc -->
                    <entity class="App\Entity\User" property="username"/>

                    <!-- optional: if you're using multiple Doctrine entity
                         managers, this option defines which one to use -->
                    <!-- <entity class="App\Entity\User" property="username"
                                 manager-name="customer"/> -->
                </provider>

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Entity\User;

        $container->loadFromExtension('security', [
            'providers' => [
                'users' => [
                    'entity' => [
                        // the class of the entity that represents users
                        'class'    => User::class,
                        // the property to query by - e.g. username, email, etc
                        'property' => 'username',
                        // optional: if you're using multiple Doctrine entity
                        // managers, this option defines which one to use
                        // 'manager_name' => 'customer',
                    ],
                ],
            ],

            // ...
        ]);

``providers`` 区块创建一个叫 ``users`` 的“用户提供器”，该提供器知道要通过
``username`` 属性从你的 ``App\Entity\User`` 实体进行查询。
你可以为用户提供器选择任何名称，但建议选择一个描述性的名称，因为稍后将在防火墙配置中使用该名称。

.. _authenticating-someone-with-a-custom-entity-provider:

使用自定义查询加载用户
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``entity`` 提供器只能查询一个由 ``property`` 键指定的 *特定* 字段。
如果你想对它有更多的控制权 - 例如你想通过 ``email`` *或* ``username`` 来找到一个用户，
你可以通过让你的 ``UserRepository`` 实现
:class:`Symfony\\Bridge\\Doctrine\\Security\\User\\UserLoaderInterface`
来做到这一点。此接口只要求一个方法：``loadUserByUsername($username)``::

    // src/Repository/UserRepository.php
    namespace App\Repository;

    use Doctrine\ORM\EntityRepository;
    use Symfony\Bridge\Doctrine\Security\User\UserLoaderInterface;

    class UserRepository extends EntityRepository implements UserLoaderInterface
    {
        // ...

        public function loadUserByUsername($usernameOrEmail)
        {
            return $this->createQueryBuilder('u')
                ->where('u.username = :query OR u.email = :query')
                ->setParameter('query', $usernameOrEmail)
                ->getQuery()
                ->getOneOrNullResult();
        }
    }

要完成此操作，请在 ``security.yaml`` 中的用户提供器中移除 ``property`` 键：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            providers:
                users:
                    entity:
                        class: App\Entity\User

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

                <provider name="users">
                    <entity class="App\Entity\User"/>
                </provider>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Entity\User;

        $container->loadFromExtension('security', [
            // ...

            'providers' => [
                'users' => [
                    'entity' => [
                        'class' => User::class,
                    ],
                ],
            ],
        ]);

该操作告诉Symfony *不要* 自动查询用户。相反，在需要时（例如，在
:doc:`用户模拟 </security/impersonating_user>`、:doc:`记住我 </security/remember_me>`
或其他一些安全功能被激活时），``UserRepository`` 中的 ``loadUserByUsername()`` 方法会被调用。

.. _security-memory-user-provider:

内存用户提供器
--------------------

由于其局限性以及管理用户的难度，建议不要在实际应用中使用此提供器。
它可能在应用原型以及不在数据库中存储用户的有限应用中很有用。

此用户提供器将所有用户信息存储在配置文件中，包括其密码。这就是为什么第一步是配置这些用户如何加密他们的密码：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            encoders:
                # Symfony使用此内部类来表示内存中的用户
                Symfony\Component\Security\Core\User\User: 'bcrypt'

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd"
        >
            <config>
                <!-- ... -->

                <!-- this internal class is used by Symfony to represent in-memory users -->
                <encoder class="Symfony\Component\Security\Core\User\User"
                    algorithm="bcrypt"
                />
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php

        // this internal class is used by Symfony to represent in-memory users
        use Symfony\Component\Security\Core\User\User;

        $container->loadFromExtension('security', [
            // ...
            'encoders' => [
                User::class => [
                    'algorithm' => 'bcrypt',
                ],
            ],
        ]);

然后，运行此命令对用户的纯文本密码进行加密：

.. code-block:: terminal

    $ php bin/console security:encode-password

现在你可以在 ``config/packages/security.yaml`` 中配置所有的用户信息：

.. code-block:: yaml

    # config/packages/security.yaml
    security:
        # ...
        providers:
            backend_users:
                memory:
                    users:
                        john_admin: { password: '$2y$13$jxGxc ... IuqDju', roles: ['ROLE_ADMIN'] }
                        jane_admin: { password: '$2y$13$PFi1I ... rGwXCZ', roles: ['ROLE_ADMIN', 'ROLE_SUPER_ADMIN'] }

.. _security-ldap-user-provider:

LDAP用户提供器
------------------

此用户提供器需要安装某些依赖项并使用一些特殊的认证提供器，因此在另一篇文章中进行了解释：
:doc:`/security/ldap`。

.. _security-chain-user-provider:

Chain用户提供器
-------------------

此用户提供器组合了两个或多个其他提供器类型（``entity``、``memory`` 以及 ``ldap``）以创建新的用户提供器。
配置提供器的顺序很重要，因为Symfony将从第一个提供器开始查找用户，然后继续在其他提供器中查找，直到找到用户为止：

.. code-block:: yaml

    # config/packages/security.yaml
    security:
        # ...
        providers:
            backend_users:
                memory:
                    # ...

            legacy_users:
                entity:
                    # ...

            users:
                entity:
                    # ...

            all_users:
                chain:
                    providers: ['legacy_users', 'users', 'backend']

.. _custom-user-provider:

创建自定义用户提供器
-------------------------------

大多数应用不需要创建自定义提供程器。如果将用户存储在数据库、LDAP服务器或配置文件中，Symfony完全支持。
但是, 如果你从自定义位置加载用户（例如，通过API或旧数据库连接），则需要创建一个自定义的用户提供器类。
首先，请确保你已按照 :doc:`安全指南 </security>` 创建了 ``User`` 类。

如果你使用 ``make:user`` 命令创建了你的 ``User``
类（并且你回答了表示你需要一个自定义用户提供器的问题），那么该命令将生成一个很好的框架以帮助你入门::

    // src/Security/UserProvider.php
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
         * @throws UsernameNotFoundException 如果未找到该用户
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
         * 如果你的防火墙是 “stateless: true”（对于纯API），则不会调用本方法。
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

大部分工作已经完成！阅读代码中的注释并更新TODO部分以完成该用户提供器。完成后，通过在
``security.yaml`` 中添加以下内容，来告诉Symfony有关用户提供器的信息：

.. code-block:: yaml

    # config/packages/security.yaml
    security:
        providers:
            # 你的用户提供商器的名称，可以是任何名称
            your_custom_user_provider:
                id: App\Security\UserProvider

最后，更新 ``config/packages/security.yaml`` 文件，在所有将使用此自定义用户提供器的防火墙中将
``provider`` 键设置为 ``your_custom_user_provider`` 。

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

在服务中注入用户提供器
------------------------------------------

Symfony定义了几个与用户提供器相关的服务：

.. code-block:: terminal

    $ php bin/console debug:container user.provider

      Select one of the following services to display its information:
      [0] security.user.provider.in_memory
      [1] security.user.provider.in_memory.user
      [2] security.user.provider.ldap
      [3] security.user.provider.chain
      ...

      这些服务大多数是抽象的，不能注入到你的服务中。
相反，你必须注入Symfony为每个用户提供器所创建的普通服务。
这些服务的名称遵循以下模式：``security.user.provider.concrete.<your-provider-name>``。

例如，如果要 :doc:`构建表单登录 </security/form_login_setup>` 并希望在你的
``LoginFormAuthenticator`` 中注入名为 ``backend_users`` 的 ``memory``
类型的用户提供器，请执行以下操作：

    // src/Security/LoginFormAuthenticator.php
    namespace App\Security;

    use Symfony\Component\Security\Core\User\InMemoryUserProvider;
    use Symfony\Component\Security\Guard\Authenticator\AbstractFormLoginAuthenticator;

    class LoginFormAuthenticator extends AbstractFormLoginAuthenticator
    {
        private $userProvider;

        // 如果要注入不同类型的用户提供器，
        // 请更改构造函数中的 'InMemoryUserProvider' 类型提示
        public function __construct(InMemoryUserProvider $userProvider, /* ... */)
        {
            $this->userProvider = $userProvider;
            // ...
        }
    }

然后，为 ``backend_users`` 用户提供器注入Symfony创建的具体服务：

.. code-block:: yaml

    # config/services.yaml
    services:
        # ...

        App\Security\LoginFormAuthenticator:
            $userProvider: '@security.user.provider.concrete.backend_users'
