.. index::
    single: Security; Custom Authentication

带安保的自定义身份认证系统（API令牌示例）
===========================================================

无论你是需要构建传统的登录表单、API令牌认证系统还是需要与某些专有的单点登录系统集成，Guard组件将是不错的选择！

安保认证器可用于：

* :doc:`构建登录表单， </security/form_login_setup>`,
* 创建API令牌认证系统（在此页面上完成！）
* `社交认证`_ （或使用 `HWIOAuthBundle`_ 获得强大但非安保的解决方案）

或者其他事情。
在这个例子中，我们将构建一个API令牌认证系统，以便我们可以详细了解Guard的更多信息。

步骤 1) 准备用户类
-------------------------------

假设你要构建一个API，客户端将使用其API令牌在每个请求上发送 ``X-AUTH-TOKEN`` 标头。
你的工作是阅读此内容并找到关联的用户（如果有）。

首先，请确保你已按照 :doc:`安全指南 </security>` 创建 ``User`` 类。
然后，为了简单起见，直接将 ``apiToken`` 属性添加到你的 ``User`` 类中
（``make:entity`` 命令是执行此操作的好方法）：

.. code-block:: diff

    // src/Entity/User.php
    // ...

    class User implements UserInterface
    {
        // ...

    +     /**
    +      * @ORM\Column(type="string", unique=true)
    +      */
    +     private $apiToken;

        // getter和setter方法
    }

不要忘记生成并执行迁移：

.. code-block:: terminal

    $ php bin/console make:migration
    $ php bin/console doctrine:migrations:migrate

步骤 2) 创建认证器类
--------------------------------------

要创建自定义认证系统，请创建一个类并使其实现
:class:`Symfony\\Component\\Security\\Guard\\AuthenticatorInterface`。或者，继承更简单的
:class:`Symfony\\Component\\Security\\Guard\\AbstractGuardAuthenticator`。

这需要你实现几种方法::

    // src/Security/TokenAuthenticator.php
    namespace App\Security;

    use App\Entity\User;
    use Doctrine\ORM\EntityManagerInterface;
    use Symfony\Component\HttpFoundation\JsonResponse;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Exception\AuthenticationException;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Guard\AbstractGuardAuthenticator;

    class TokenAuthenticator extends AbstractGuardAuthenticator
    {
        private $em;

        public function __construct(EntityManagerInterface $em)
        {
            $this->em = $em;
        }

        /**
         * 在每个请求上调用以决定是否应该将此认证器器用于该请求。
         * 返回false将会跳过此认证器。
         */
        public function supports(Request $request)
        {
            return $request->headers->has('X-AUTH-TOKEN');
        }

        /**
         * 会被每个请求调用。
         * 返回你需要的任何凭据，该凭据将被作为 $credentials 传递到 getUser()。
         */
        public function getCredentials(Request $request)
        {
            return [
                'token' => $request->headers->get('X-AUTH-TOKEN'),
            ];
        }

        public function getUser($credentials, UserProviderInterface $userProvider)
        {
            $apiToken = $credentials['token'];

            if (null === $apiToken) {
                return;
            }

            // 如果是一个User对象，则调用checkCredentials()
            return $this->em->getRepository(User::class)
                ->findOneBy(['apiToken' => $apiToken]);
        }

        public function checkCredentials($credentials, UserInterface $user)
        {
            // 检查凭据 - 例如确保密码有效
            // 在本例中没有凭据需要检查

            // 返回 true，意味着认证成功
            return true;
        }

        public function onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)
        {
            // 成功后，让该请求继续
            return null;
        }

        public function onAuthenticationFailure(Request $request, AuthenticationException $exception)
        {
            $data = [
                'message' => strtr($exception->getMessageKey(), $exception->getMessageData())

                // 或翻译此消息
                // $this->translator->trans($exception->getMessageKey(), $exception->getMessageData())
            ];

            return new JsonResponse($data, Response::HTTP_FORBIDDEN);
        }

        /**
         * 需要认证时被调用，但不会发送
         */
        public function start(Request $request, AuthenticationException $authException = null)
        {
            $data = [
                // 你可以翻译此消息
                'message' => 'Authentication Required'
            ];

            return new JsonResponse($data, Response::HTTP_UNAUTHORIZED);
        }

        public function supportsRememberMe()
        {
            return false;
        }
    }

干得好！每种方法都在下面说明：:ref:`安保认证器的方法<guard-auth-methods>`。

步骤 3) 配置认证器
-----------------------------------

要完成此操作，请确保你的认证器已注册为服务。如果你使用的是
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，则会自动执行此操作。

最后，在 ``security.yaml`` 配置你的 ``firewalls`` 键以使用此认证器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                # ...

                main:
                    anonymous: ~
                    logout: ~

                    guard:
                        authenticators:
                            - App\Security\TokenAuthenticator

                    # 如果需要，禁用将用户存储在会话中
                    # stateless: true

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
                <!-- ... -->

                <firewall name="main"
                    pattern="^/"
                    anonymous="true"
                >
                    <logout/>

                    <guard>
                        <authenticator>App\Security\TokenAuthenticator</authenticator>
                    </guard>

                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php

        // ...
        use App\Security\TokenAuthenticator;

        $container->loadFromExtension('security', [
            'firewalls' => [
                'main'       => [
                    'pattern'        => '^/',
                    'anonymous'      => true,
                    'logout'         => true,
                    'guard'          => [
                        'authenticators'  => [
                            TokenAuthenticator::class
                        ],
                    ],
                    // ...
                ],
            ],
        ]);

你做到了！你现在拥有一个完全可用的API令牌认证系统。
如果你的主页需要 ``ROLE_USER``，那么你可以在不同的条件下测试它：

.. code-block:: terminal

    # 没有令牌的测试
    curl http://localhost:8000/
    # {"message":"Authentication Required"}

    # 无效令牌的测试
    curl -H "X-AUTH-TOKEN: FAKE" http://localhost:8000/
    # {"message":"Username could not be found."}

    # 有效令牌的测试
    curl -H "X-AUTH-TOKEN: REAL" http://localhost:8000/
    # 主页控制器被执行：页面正常加载

现在，详细了解每种方法的作用。

.. _guard-auth-methods:

Guard认证器的方法
-------------------------------

每个认证器都需要以下方法：

**supports(Request $request)**
    这将在 *每个* 请求上调用，
    你的工作是决定是否应该将此认证器用于此请求（返回 ``true``）或是否应该跳过（返回 ``false``）。

**getCredentials(Request $request)**
    这将在 *每个* 请求上调用，你的工作是从请求中读取令牌（或其他任何“认证验证”信息）并将其返回。
    这些凭据稍后作为第一个参数传递给 ``getUser()``。

**getUser($credentials, UserProviderInterface $userProvider)**
    ``$credentials`` 参数是 ``getCredentials()`` 返回的值。
    你的工作是返回一个实现 ``UserInterface`` 的对象。
    如果你这样做，那么 ``checkCredentials()`` 就会被调用。
    如果你返回 ``null`` （或抛出一个
    :ref:`AuthenticationException <guard-customize-error>`）认证将失败。

**checkCredentials($credentials, UserInterface $user)**
    如果 ``getUser()`` 返回一个User对象，则调用此方法。
    你的工作是验证该凭据是否正确。对于登录表单，你可以在此处检查密码是否和该用户对应。
    要通过认证，请返回 ``true``。如果你返回 *任何* 其他内容（或抛出一个
    :ref:`AuthenticationException <guard-customize-error>`），认证将失败。

**onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)**
    此方法成功认证后调用，你的工作是返回一个要发送到客户端的
    :class:`Symfony\\Component\\HttpFoundation\\Response` 对象或 ``null`` 以继续该请求
    （例如，允许像往常一样调用路由/控制器）。
    由于这是一个API，每个请求都会对自身进行认证，因此你希望返回 ``null``。

**onAuthenticationFailure(Request $request, AuthenticationException $exception)**
    如果认证失败，此方法将被调用。
    你的工作是返回应发送给客户端的
    :class:`Symfony\\Component\\HttpFoundation\\Response` 对象。
    ``$exception`` 会告诉你认证过程中 *哪里* 出错了。

**start(Request $request, AuthenticationException $authException = null)**
    如果客户端访问需要认证的URI/资源，但未发送认证详细信息，则调用此方法。
    你的工作是返回一个帮助用户进行认证的
    :class:`Symfony\\Component\\HttpFoundation\\Response` 对象（例如表示“令牌丢失！”的401响应）。

**supportsRememberMe()**
    如果要支持“记住我”功能，请从此方法返回 ``true``。
    但你仍然需要在防火墙下激活 ``remember_me`` 才能生效。
    由于这是一个无状态API，因此你不希望在此示例中支持“记住我”功能。

**createAuthenticatedToken(UserInterface $user, string $providerKey)**
    如果你是实现
    :class:`Symfony\\Component\\Security\\Guard\\AuthenticatorInterface` 而不是继承
    :class:`Symfony\\Component\\Security\\Guard\\AbstractGuardAuthenticator`
    类，则必须实现此方法。
    它将在认证成功后调用，以创建并返回作为第一个参数传递的用户的令牌。

下图显示了Symfony如何调用安保认证器的方法：

.. raw:: html

    <object data="../_images/security/authentication-guard-methods.svg" type="image/svg+xml"></object>

.. _guard-customize-error:

自定义错误消息
--------------------------

当 ``onAuthenticationFailure()`` 被调用，它被传递了一个 ``AuthenticationException``，
该异常通过自身的 ``$exception->getMessageKey()`` (和 ``$exception->getMessageData()``)
方法来描述认证是如何失败的。
认证失败的 *位置* 不同，该消息也会不同（例如，``getUser()`` 相对于 ``checkCredentials()``）。

但是，你也可以通过抛出一个
:class:`Symfony\\Component\\Security\\Core\\Exception\\CustomUserMessageAuthenticationException`
来自定义该消息。你可以从 ``getCredentials()``、``getUser()`` 或 ``checkCredentials()``
中抛出该异常以引出一个失败操作::

    // src/Security/TokenAuthenticator.php
    // ...

    use Symfony\Component\Security\Core\Exception\CustomUserMessageAuthenticationException;

    class TokenAuthenticator extends AbstractGuardAuthenticator
    {
        // ...

        public function getCredentials(Request $request)
        {
            // ...

            if ($token == 'ILuvAPIs') {
                throw new CustomUserMessageAuthenticationException(
                    'ILuvAPIs is not a real API key: it\'s just a silly phrase'
                );
            }

            // ...
        }

        // ...
    }

在这个例子中，由于“ILuvAPIs”是一个荒谬的API令牌，如果有人试图这样做，你可以返回一个包含复活节彩蛋的自定义消息：

.. code-block:: terminal

    curl -H "X-AUTH-TOKEN: ILuvAPIs" http://localhost:8000/
    # {"message":"ILuvAPIs is not a real API key: it's just a silly phrase"}

.. _guard-manual-auth:

手动认证用户
------------------------------

有时你可能需要手动认证用户身份 - 比如用户完成注册后。
为此，请使用你的认证器和名为 ``GuardAuthenticatorHandler`` 的服务::

    // src/Controller/RegistrationController.php
    // ...

    use App\Security\LoginFormAuthenticator;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Guard\GuardAuthenticatorHandler;

    class RegistrationController extends AbstractController
    {
        public function register(LoginFormAuthenticator $authenticator, GuardAuthenticatorHandler $guardHandler, Request $request)
        {
            // ...

            // 验证该用户并将其保存到数据库后
            // 认证该用户并在认证器上使用onAuthenticationSuccess
            return $guardHandler->authenticateUserAndHandleSuccess(
                $user,          // 刚刚创建的User对象
                $request,
                $authenticator, // 你想要在 onAuthenticationSuccess 使用的认证器
                'main'          // 在security.yaml中的你的防火墙的名称
            );
        }
    }

避免认证浏览器的每个请求
-------------------------------------------------

如果你创建了一个由浏览器使用的安保登录系统，并且你遇到了会话或CSRF令牌的问题，那么可能是你的认证器行为导致了这些错误。
当安保认证器是由浏览器使用的，你 *不* 应该在 *每个* 请求中都对用户进行认证。
换句话说，你需要确保 ``supports()`` 方法 *仅* 在你确实 *需要* 对用户进行认证时才返回 ``true``。
为什么？因为，当 ``supports()`` 返回 ``true`` （并且认证最终成功）时，出于安全考虑，该用户的会话将“迁移”到一个新的会话ID中。

这是一个边缘情况，除非你遇到会话或CSRF令牌问题，否则可以忽略它。这是一个好的和坏的行为的例子::

    public function supports(Request $request)
    {
        // 好的行为: 只在特定路由上认证 (即 return true) on a specific route
        return 'login_route' === $request->attributes->get('_route') && $request->isMethod('POST');

        // 例如，你的登录系统通过用户的IP地址进行认证
        // 坏的行为:
        // 因此，你决定 *总是* 返回true，以便你可以在每个请求中检查用户的IP地址
        return true;
    }

当你的基于浏览器的认证器尝试在 *每个* 请求上对用户进行认证时会出现此问题 - 例如，上面基于IP地址的示例。
有两种可能的修复方法：

1. 如果 *不* 需要将认证存储到会话中，在防火墙下设置 ``stateless: true``。
2. 如果用户已经过认证，请更新你的认证器以避免再次认证：

.. code-block:: diff

    // src/Security/MyIpAuthenticator.php
    // ...

    + use Symfony\Component\Security\Core\Security;

    class MyIpAuthenticator
    {
    +     private $security;

    +     public function __construct(Security $security)
    +     {
    +         $this->security = $security;
    +     }

        public function supports(Request $request)
        {
    +         // 如果该用户已经经过认证（比如已存在于会话），
    +         // 则返回false并跳过此认证：没有必要再次认证。
    +         if ($this->security->getUser()) {
    +             return false;
    +         }

    +         // 用户未登录，因此该认证器应继续
    +         return true;
        }
    }

如果你使用自动装配，``Security``  服务将自动传递到你的认证器。

常见问题
--------------------------

**我可以拥有多个认证器吗？**
    可以! 但是当你这样做时，你需要只选择 *一个* 认证器作为你的“入口点”。
    这意味着当匿名用户尝试访问受保护资源时，你需要选择应调用 *哪个* 认证器的 ``start()`` 方法。
    有关更多详细信息，请参阅 :doc:`/security/multiple_guard_authenticators`。

**我可以在form_login中使用它吗？**
    可以! ``form_login`` 是 *一个* 对用户进行认证的途径，因此你可以使用它，*然后* 添加一个或多个认证器。
    使用安保认证器不会与其他认证方式发生冲突。

**我可以在FOSUserBundle中使用它吗？**
    可以! 实际上，FOSUserBundle不处理安全性：
    它只是给你一个 ``User`` 对象和一些路由、控制器来辅助登录、注册、忘记密码等。
    当你使用FOSUserBundle时，你通常用 ``form_login`` 来实际认证用户。
    你可以继续这样做（请参阅上一个问题）或使用UserFOSUserBundle中的 ``User`` 对象并创建自己的认证器（就像本文中一样）。

.. _`must be quoted with backticks`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#quoting-reserved-words
.. _`社交认证`: https://github.com/knpuniversity/oauth2-client-bundle#authenticating-with-guard
.. _`HWIOAuthBundle`: https://github.com/hwi/HWIOAuthBundle
