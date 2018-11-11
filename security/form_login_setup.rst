如何构建登录表单
=========================

.. seealso::

    如果你正在查找 ``form_login`` 防火墙选项，请参阅 :doc:`/security/form_login`。

准备创建登录表单了吗？首先，请确保你已按照 :doc:`安全指南 </security>`
安装了安全功能并创建了你的 ``User`` 类。

生成登录表单
-------------------------

可以使用 `MakerBundle`_ 中的 ``make:auth`` 命令来引导创建功能强大的登录表单。
根据你的设置，该命令可能会向你提出不同的问题，最终生成的代码可能会略有不同：

.. code-block:: terminal

    $ php bin/console make:auth

    What style of authentication do you want? [Empty authenticator]:
     [0] Empty authenticator
     [1] Login form authenticator
    > 1

    The class name of the authenticator to create (e.g. AppCustomAuthenticator):
    > LoginFormAuthenticator

    Choose a name for the controller class (e.g. SecurityController) [SecurityController]:
    > SecurityController

     created: src/Security/LoginFormAuthenticator.php
     updated: config/packages/security.yaml
     created: src/Controller/SecurityController.php
     created: templates/security/login.html.twig

.. versionadded:: 1.8
    MakerBundle 1.8中的 ``make:auth`` 添加了对登录表单认证证的支持。

这会生成三件事：（1）一个登录路由和控制器，（2）一个渲染登录表单的模板和（3）
一个处理登录提交的 :doc:`安保认证器 </security/guard_authentication>` 类。

``/login`` 路由与控制器::

    // src/Controller/SecurityController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing\Annotation\Route;
    use Symfony\Component\Security\Http\Authentication\AuthenticationUtils;

    class SecurityController extends AbstractController
    {
        /**
         * @Route("/login", name="app_login")
         */
        public function login(AuthenticationUtils $authenticationUtils): Response
        {
            // 如果有登陆错误，请在此处获取
            $error = $authenticationUtils->getLastAuthenticationError();
            // 用户输入的最后一个用户名
            $lastUsername = $authenticationUtils->getLastUsername();

            return $this->render('security/login.html.twig', [
                'last_username' => $lastUsername,
                'error' => $error
            ]);
        }
    }

此模板与安全几乎没有关系：它只是生成一个提交到 ``/login`` 的传统的HTML表单：

.. code-block:: twig

    {% extends 'base.html.twig' %}

    {% block title %}Log in!{% endblock %}

    {% block body %}
    <form method="post">
        {% if error %}
            <div class="alert alert-danger">{{ error.messageKey|trans(error.messageData, 'security') }}</div>
        {% endif %}

        <h1 class="h3 mb-3 font-weight-normal">Please sign in</h1>
        <label for="inputEmail" class="sr-only">Email</label>
        <input type="email" value="{{ last_username }}" name="email" id="inputEmail" class="form-control" placeholder="Email" required autofocus>
        <label for="inputPassword" class="sr-only">Password</label>
        <input type="password" name="password" id="inputPassword" class="form-control" placeholder="Password" required>

        <input type="hidden" name="_csrf_token"
               value="{{ csrf_token('authenticate') }}"
        >

        {#
            取消注释此部分，并在防火墙下添加remember_me选项，以激活记住我的功能。
            参阅 https://symfony.com/doc/current/security/remember_me.html

            <div class="checkbox mb-3">
                <label>
                    <input type="checkbox" name="_remember_me"> Remember me
                </label>
            </div>
        #}

        <button class="btn btn-lg btn-primary" type="submit">
            Sign in
        </button>
    </form>
    {% endblock %}

安保认证器将处理表单的提交::

    // src/Security/LoginFormAuthenticator.php
    namespace App\Security;

    use App\Entity\User;
    use Doctrine\ORM\EntityManagerInterface;
    use Symfony\Component\HttpFoundation\RedirectResponse;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Routing\RouterInterface;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Encoder\UserPasswordEncoderInterface;
    use Symfony\Component\Security\Core\Exception\InvalidCsrfTokenException;
    use Symfony\Component\Security\Core\Security;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Csrf\CsrfToken;
    use Symfony\Component\Security\Csrf\CsrfTokenManagerInterface;
    use Symfony\Component\Security\Guard\Authenticator\AbstractFormLoginAuthenticator;
    use Symfony\Component\Security\Http\Util\TargetPathTrait;

    class LoginFormAuthenticator extends AbstractFormLoginAuthenticator
    {
        use TargetPathTrait;

        private $entityManager;
        private $router;
        private $csrfTokenManager;
        private $passwordEncoder;

        public function __construct(EntityManagerInterface $entityManager, RouterInterface $router, CsrfTokenManagerInterface $csrfTokenManager, UserPasswordEncoderInterface $passwordEncoder)
        {
            $this->entityManager = $entityManager;
            $this->router = $router;
            $this->csrfTokenManager = $csrfTokenManager;
            $this->passwordEncoder = $passwordEncoder;
        }

        public function supports(Request $request)
        {
            return 'app_login' === $request->attributes->get('_route')
                && $request->isMethod('POST');
        }

        public function getCredentials(Request $request)
        {
            $credentials = [
                'email' => $request->request->get('email'),
                'password' => $request->request->get('password'),
                'csrf_token' => $request->request->get('_csrf_token'),
            ];
            $request->getSession()->set(
                Security::LAST_USERNAME,
                $credentials['email']
            );

            return $credentials;
        }

        public function getUser($credentials, UserProviderInterface $userProvider)
        {
            $token = new CsrfToken('authenticate', $credentials['csrf_token']);
            if (!$this->csrfTokenManager->isTokenValid($token)) {
                throw new InvalidCsrfTokenException();
            }

            $user = $this->entityManager->getRepository(User::class)->findOneBy(['email' => $credentials['email']]);

            if (!$user) {
                // 为失败的认证自定义一个错误
                throw new CustomUserMessageAuthenticationException('Email could not be found.');
            }

            return $user;
        }

        public function checkCredentials($credentials, UserInterface $user)
        {
            return $this->passwordEncoder->isPasswordValid($user, $credentials['password']);
        }

        public function onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)
        {
            if ($targetPath = $this->getTargetPath($request->getSession(), $providerKey)) {
                return new RedirectResponse($targetPath);
            }

            // 例如 : return new RedirectResponse($this->router->generate('some_route'));
            throw new \Exception('TODO: provide a valid redirect inside '.__FILE__);
        }

        protected function getLoginUrl()
        {
            return $this->router->generate('app_login');
        }
    }

完成登录表单
------------------------

哇哦。``make:auth`` 命令为你做了 *大量* 工作。但是，你还没有完工。
首先，转到 ``/login`` 查看新的登录表单。你可以根据需要随意自定义。

当你提交表单时，``LoginFormAuthenticator`` 将拦截该请求，并从表单中读取电子邮箱（或你正在使用的任何字段）和密码、
查找 ``User`` 对象、验证CSRF令牌并检查密码。

但是，根据你的设置，你需要在整个进程运行之前完成一个或多个TODO。
你将 *至少* 需要填写完你希望你的用户能够认证成功后重定向到 *哪里*：

.. code-block:: diff

    // src/Security/LoginFormAuthenticator.php

    // ...
    public function onAuthenticationSuccess(Request $request, TokenInterface $token, $providerKey)
    {
        // ...

    -     throw new \Exception('TODO: provide a valid redirect inside '.__FILE__);
    +     // 重定向到某个 “app_homepage” 路由 - 无论你想要什么
    +     return new RedirectResponse($this->router->generate('app_homepage'));
    }

除非你在该文件中有任何其他TODO，否则就已经完工了！
如果你从数据库加载用户，请确保已加载一些 :ref:`虚拟用户 <doctrine-fixtures>`。
然后，尝试登录一下。

如果你成功登录了，Web调试工具栏将告显示你的身份以及你拥有的角色：

.. image:: /_images/security/symfony_loggedin_wdt.png
   :align: center

安保认证系统功能强大，你可以自定义认证器类以执行你需要的任何操作。
要了解有关各个方法的更多信息，请参阅 :doc:`/security/guard_authentication`。

控制错误信息
--------------------------

通过抛出一个自定义
:class:`Symfony\\Component\\Security\\Core\\Exception\\CustomUserMessageAuthenticationException`
，你可以在任何步骤中自定义一个认证失败的消息。
这是一种控制错误消息的简便方法。

但在某些情况下，如果你从 ``checkCredentials()`` 中返回 ``false``，你可能会看到来自Symfony核心的错误 - 比如 ``Invalid credentials.``。

要自定义此消息，你可以改为抛出一个 ``CustomUserMessageAuthenticationException``。
或者，你可以通过 ``security`` 域来 :doc:`翻译 </translation>` 该消息：

.. configuration-block::

    .. code-block:: xml

        <!-- translations/security.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="Invalid credentials.">
                        <source>Invalid credentials.</source>
                        <target>The password you entered was invalid!</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: yaml

        # translations/security.en.yaml
        'Invalid credentials.': 'The password you entered was invalid!'

    .. code-block:: php

        // translations/security.en.php
        return array(
            'Invalid credentials.' => 'The password you entered was invalid!',
        );

如果该消息未翻译，请确保已安装 ``translator`` 并尝试清除你的缓存：

.. code-block:: terminal

    $ php bin/console cache:clear

使用 ``TargetPathTrait`` 重定向到上次访问页面
--------------------------------------------------------------

最后一个请求URI存储在一个名为 ``_security.<your providerKey>.target_path``
的会话变量中（例如，如果防火墙的名称是 ``main``，则名为 ``_security.main.target_path``）。
大多数情况下，你不必处理此底层会话变量。
但是，:class:`Symfony\\Component\\Security\\Http\\Util\\TargetPathTrait`
工具可用于读取（如上例所示）或手动设置该值。

.. _`MakerBundle`: https://github.com/symfony/maker-bundle
