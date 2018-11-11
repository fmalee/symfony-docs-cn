.. index::
   single: Security; User provider
   single: Security; Entity provider

如何从数据库（实体提供器）加载安全用户
==================================================================

你应用中的每个用户类通常都需要自己的 :doc:`用户提供器 </security/user_provider>`。
如果你从数据库加载用户，则可以使用内置的 ``entity`` 提供器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
            # ...

            providers:
                our_db_provider:
                    entity:
                        class: App\Entity\User
                        # 查询的属性 - 例如用户名、电子邮件等
                        property: username
                        # 如果你使用多个实体管理器
                        # manager_name: customer

            # ...

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <provider name="our_db_provider">
                    <!-- if you're using multiple entity managers, add:
                         manager-name="customer" -->
                    <entity class="App\Entity\User" property="username" />
                </provider>

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Entity\User;

        $container->loadFromExtension('security', array(
            'providers' => array(
                'our_db_provider' => array(
                    'entity' => array(
                        'class'    => User::class,
                        'property' => 'username',
                    ),
                ),
            ),

            // ...
        ));

``providers`` 区块创建一个叫 ``our_db_provider`` 的“用户提供器”，
该提供器知道要通过 ``username`` 属性从你的 ``App\Entity\User`` 实体进行查询。
``our_db_provider`` 这个名称并不重要：
除非你有多个用户提供器，并且需要通过防火墙下的 ``provider`` 键指定要使用的用户提供器，否则不会用到该名称。

.. _authenticating-someone-with-a-custom-entity-provider:

使用自定义查询加载用户
-------------------------------------

``entity`` 提供器只能查询一个由 ``property`` 键指定的 *特定* 字段。
如果你想对它有更多的控制权 - 例如你想通过 ``email`` *或* ``username`` 来找到一个用户，
你可以通过让你的 ``UserRepository`` 实现一个特殊的
:class:`Symfony\\Bridge\\Doctrine\\Security\\User\\UserLoaderInterface`
来做到这一点。
此接口只要求一个方法：``loadUserByUsername($username)``::

    // src/Repository/UserRepository.php
    namespace App\Repository;

    use Symfony\Bridge\Doctrine\Security\User\UserLoaderInterface;
    use Doctrine\ORM\EntityRepository;

    class UserRepository extends EntityRepository implements UserLoaderInterface
    {
        public function loadUserByUsername($username)
        {
            return $this->createQueryBuilder('u')
                ->where('u.username = :username OR u.email = :email')
                ->setParameter('username', $username)
                ->setParameter('email', $username)
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
                our_db_provider:
                    entity:
                        class: App\Entity\User

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

                <provider name="our_db_provider">
                    <entity class="App\Entity\User" />
                </provider>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Entity\User;

        $container->loadFromExtension('security', array(
            // ...

            'providers' => array(
                'our_db_provider' => array(
                    'entity' => array(
                        'class' => User::class,
                    ),
                ),
            ),
        ));

该操作告诉Symfony *不要* 自动查询用户。
相反，在需要时（例如，在 ``switch_user``、``remember_me`` 或其他一些安全功能被激活时），
``UserRepository`` 中的 ``loadUserByUsername()`` 方法会被调用。
