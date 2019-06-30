.. index::
    single: Security; Named Encoders

如何为每个用户使用不同的密码编码器算法
==========================================================

通常，所有用户使用相同的密码编码器，这是通过将其配置为应用于特定类的所有实例来实现的：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            encoders:
                App\Entity\User:
                    algorithm: bcrypt
                    cost: 12

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
                <encoder class="App\Entity\User"
                    algorithm="bcrypt"
                    cost=12
                />
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Entity\User;

        $container->loadFromExtension('security', [
            // ...
            'encoders' => [
                User::class => [
                    'algorithm' => 'bcrypt',
                    'cost' => 12,
                ],
            ],
        ]);

另一种选择是使用一个“命名”编码器，然后选择要动态使用的编码器。

在上一个示例中，你已为 ``App\Entity\User`` 设置了 ``bcrypt`` 算法。
对于普通用户而言，这可能已经足够安全。
但如果你希望管理员拥有更强大的算法（例如更高cost的 ``bcrypt``），该怎么办呢？
这可以使用命名编码器来完成：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            encoders:
                harsh:
                    algorithm: bcrypt
                    cost: 15

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd"
        >

            <config>
                <!-- ... -->
                <encoder class="harsh"
                    algorithm="bcrypt"
                    cost="15"/>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...
            'encoders' => [
                'harsh' => [
                    'algorithm' => 'bcrypt',
                    'cost'      => '15',
                ],
            ],
        ]);

.. note::

    如果你运行的是PHP 7.2+或安装了 `libsodium`_ 扩展，那么推荐使用的散列算法是
    :ref:`Sodium <reference-security-sodium>`。

这将创建一个名为 ``harsh`` 的编码器。
为了让一个 ``User`` 实例使用它，该类必须实现
:class:`Symfony\\Component\\Security\\Core\\Encoder\\EncoderAwareInterface`。
该接口需要一个 ``getEncoderName()`` 方法，该方法应该返回要使用的编码器的名称::

    // src/Acme/UserBundle/Entity/User.php
    namespace Acme\UserBundle\Entity;

    use Symfony\Component\Security\Core\Encoder\EncoderAwareInterface;
    use Symfony\Component\Security\Core\User\UserInterface;

    class User implements UserInterface, EncoderAwareInterface
    {
        public function getEncoderName()
        {
            if ($this->isAdmin()) {
                return 'harsh';
            }

            return null; // 使用默认编码器
        }
    }

如果你创建了自己的实现了
:class:`Symfony\\Component\\Security\\Core\\Encoder\\PasswordEncoderInterface`
的密码编码器，则必须为其注册服务，以便将其用作命名编码器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            encoders:
                app_encoder:
                    id: 'App\Security\Encoder\MyCustomPasswordEncoder'

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd"
        >

            <config>
                <!-- ... -->
                <encoder class="app_encoder"
                    id="App\Security\Encoder\MyCustomPasswordEncoder"/>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        // ...
        use App\Security\Encoder\MyCustomPasswordEncoder;

        $container->loadFromExtension('security', [
            // ...
            'encoders' => [
                'app_encoder' => [
                    'id' => MyCustomPasswordEncoder::class,
                ],
            ],
        ]);

这将创建一个名为 ``app_encoder`` 的编码器，该编码器使用了一个ID为
``App\Security\Encoder\MyCustomPasswordEncoder`` 的服务。

.. _`libsodium`: https://pecl.php.net/package/libsodium
