.. index::
    single: Security; Creating and Enabling Custom User Checkers

如何创建和启用自定义用户检查器
=============================================

在用户身份认证期间，可能需要进行其他检查以验证已标识的用户是否允许登录。
通过定义自定义用户检查器，你可以为每个防火墙定义应使用哪个检查器。

创建自定义用户检查器
------------------------------

用户检查器是必须实现
:class:`Symfony\\Component\\Security\\Core\\User\\UserCheckerInterface` 的类。
该接口定义了两个分别叫 ``checkPreAuth()`` 和 ``checkPostAuth()`` 的方法，
用于在用户认证之前和之后执行检查。如果不满足一个或多个条件，则会抛出一个继承自
:class:`Symfony\\Component\\Security\\Core\\Exception\\AccountStatusException` 的异常。

.. code-block:: php

    namespace App\Security;

    use App\Exception\AccountDeletedException;
    use App\Security\User as AppUser;
    use Symfony\Component\Security\Core\Exception\AccountExpiredException;
    use Symfony\Component\Security\Core\Exception\CustomUserMessageAuthenticationException;
    use Symfony\Component\Security\Core\User\UserCheckerInterface;
    use Symfony\Component\Security\Core\User\UserInterface;

    class UserChecker implements UserCheckerInterface
    {
        public function checkPreAuth(UserInterface $user)
        {
            if (!$user instanceof AppUser) {
                return;
            }

            // 用户被删除，显示一个通用的“帐户未找到”消息。
            if ($user->isDeleted()) {
                throw new AccountDeletedException('...');

                // 或者自定义要显示的消息
                throw new CustomUserMessageAuthenticationException(
                    'Your account was deleted. Sorry about that!'
                );
            }
        }

        public function checkPostAuth(UserInterface $user)
        {
            if (!$user instanceof AppUser) {
                return;
            }

            // 用户的账户已经过期，应该要通知该用户
            if ($user->isExpired()) {
                throw new AccountExpiredException('...');
            }
        }
    }

启用自定义用户检查器
--------------------------------

接下来，确保你的用户检查器已注册为服务。如果你使用
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，则会自动的注册该服务。

剩下要做的就是将检查器添加到所需的防火墙中，选项值是用户检查器的服务ID：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml

        # ...
        security:
            firewalls:
                main:
                    pattern: ^/
                    user_checker: App\Security\UserChecker
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
                <!-- ... -->
                <firewall name="main" pattern="^/">
                    <user-checker>App\Security\UserChecker</user-checker>
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php

        // ...
        use App\Security\UserChecker;

        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'main' => array(
                    'pattern' => '^/',
                    'user_checker' => UserChecker::class,
                    // ...
                ),
            ),
        ));
