.. index::
    single: Security; Impersonating User

如何模拟一个用户
=========================

有时，能够从一个用户切换到另一个用户而不必注销并再次登录是有用的（例如，当你调试用户能看到无法重现的内容时）。

.. caution::

    用户模拟与某些认证机制（例如 ``REMOTE_USER``）不兼容，因为它们期望在每个请求上发送认证信息。

可以通过激活 ``switch_user`` 防火墙监听器来模拟用户：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    switch_user: true

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
                    <switch-user/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main'=> [
                    // ...
                    'switch_user' => true,
                ],
            ],
        ]);

要切换到另一个用户，请添加一个查询字符串作为当前URL的值，其中包含
``_switch_user`` 参数和对应用户名（或任何用户提供器用于加载用户的字段）：

.. code-block:: text

    http://example.com/somewhere?_switch_user=thomas

要切换回原始用户，请使用特殊的 ``_exit`` 用户名：

.. code-block:: text

    http://example.com/somewhere?_switch_user=_exit

此功能仅适用于具有一个 ``ROLE_ALLOWED_TO_SWITCH`` 特殊角色的用户。

知道模拟何时被激活
------------------------------------

在模拟期间，该用户被添加了一个名为 ``ROLE_PREVIOUS_ADMIN`` 的特殊角色。
例如，在模板中，此角色可用于显示一个退出模拟的链接：

.. code-block:: html+twig

    {% if is_granted('ROLE_PREVIOUS_ADMIN') %}
        <a href="{{ path('homepage', {'_switch_user': '_exit'}) }}">Exit impersonation</a>
    {% endif %}

查找原始用户
-------------------------

.. versionadded:: 4.3

    Symfony 4.3中引入了 ``SwitchUserToken`` 类。

在某些情况下，你可能需要获取代表该模拟用户的对象，而不是该模拟用户。
当模拟用户时，保存在令牌存储中的令牌将是一个 ``SwitchUserToken`` 实例。
使用以下代码片段获取允许你访问模拟用户的原始令牌::

    use Symfony\Component\Security\Core\Authentication\Token\SwitchUserToken;
    use Symfony\Component\Security\Core\Security;
    // ...

    public class SomeService
    {
        private $security;

        public function __construct(Security $security)
        {
            $this->security = $security;
        }

        public function someMethod()
        {
            // ...

            $token = $this->security->getToken();

            if ($token instanceof SwitchUserToken) {
                $impersonatorUser = $token->getOriginalToken()->getUser();
            }

            // ...
        }
    }

控制查询参数
-------------------------------

模拟功能只能供有限的用户组使用。
默认情况下，仅限于访问具有 ``ROLE_ALLOWED_TO_SWITCH`` 角色的用户。
可以通过 ``role`` 设置来修改此角色的名称。你还可以通过 ``parameter`` 设置来调整查询参数名称：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    switch_user: { role: ROLE_ADMIN, parameter: _want_to_be_this_user }

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
                    <switch-user role="ROLE_ADMIN" parameter="_want_to_be_this_user"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main'=> [
                    // ...
                    'switch_user' => [
                        'role' => 'ROLE_ADMIN',
                        'parameter' => '_want_to_be_this_user',
                    ],
                ],
            ],
        ]);

限制用户切换
-----------------------

如果你需要更多地控制用户切换，你可以使用安全表决器。首先，配置 ``switch_user``
以检查一些新的自定义属性。它可以是任何东西，但 *不能* 以 ``ROLE_`` 开头（确保只有你的表决器将被调用）：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    switch_user: { role: CAN_SWITCH_USER }

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
                    <switch-user role="CAN_SWITCH_USER"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'main'=> [
                    // ...
                    'switch_user' => [
                        'role' => 'CAN_SWITCH_USER',
                    ],
                ],
            ],
        ]);

然后，创建一个响应此角色的表决器类，并包含你想要的任何自定义逻辑::

    namespace App\Security\Voter;

    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Authorization\Voter\Voter;
    use Symfony\Component\Security\Core\Security;
    use Symfony\Component\Security\Core\User\UserInterface;

    class SwitchToCustomerVoter extends Voter
    {
        private $security;

        public function __construct(Security $security)
        {
            $this->security = $security;
        }

        protected function supports($attribute, $subject)
        {
            return in_array($attribute, ['CAN_SWITCH_USER'])
                && $subject instanceof UserInterface;
        }

        protected function voteOnAttribute($attribute, $subject, TokenInterface $token)
        {
            $user = $token->getUser();
            // 如果是匿名用户或主题不是一个用户，请不要授予访问权限
            if (!$user instanceof UserInterface || !$subject instanceof UserInterface) {
                return false;
            }

            // 你仍然可以检查 ROLE_ALLOWED_TO_SWITCH
            if ($this->security->isGranted('ROLE_ALLOWED_TO_SWITCH')) {
                return true;
            }

            // 检查你想要的任何角色
            if ($this->security->isGranted('ROLE_TECH_SUPPORT')) {
                return true;
            }

            /*
             * 或使用 User 对象中的一些自定义数据
            if ($user->isAllowedToSwitch()) {
                return true;
            }
            */

            return false;
        }
    }

仅此而已！切换用户时，你的表决器现在可以完全控制是否允许这样做。如果你的表决器未被调用，请参阅
:ref:`declaring-the-voter-as-a-service`。

事件
------

防火墙在模拟完成后会立即调度 ``security.switch_user`` 事件。
:class:`Symfony\\Component\\Security\\Http\\Event\\SwitchUserEvent`
将被传递给监听器，你可以用它来获取你现在模拟的用户。

依据 :doc:`/session/locale_sticky_session` 章节，当你模拟用户时不会更新对应的语言环境。
如果你 *确实* 希望在切换用户时更新对应的语言环境，请在此事件上添加一个事件订阅器::

    // src/EventListener/SwitchUserSubscriber.php
    namespace App\EventListener;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Security\Http\Event\SwitchUserEvent;
    use Symfony\Component\Security\Http\SecurityEvents;

    class SwitchUserSubscriber implements EventSubscriberInterface
    {
        public function onSwitchUser(SwitchUserEvent $event)
        {
            $request = $event->getRequest();

            if ($request->hasSession() && ($session = $request->getSession())) {
                $session->set(
                    '_locale',
                    // 假设你的User类有一个 getLocale() 方法
                    $event->getTargetUser()->getLocale()
                );
            }
        }

        public static function getSubscribedEvents()
        {
            return [
                // security.switch_user的常量
                SecurityEvents::SWITCH_USER => 'onSwitchUser',
            ];
        }
    }

仅此而已！如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`
，Symfony将自动发现你的服务并在切换用户时调用 ``onSwitchUser``。

有关事件订阅器的更多详细信息，请参阅 :doc:`/event_dispatcher`。
