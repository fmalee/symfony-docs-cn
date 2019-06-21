.. index::
   single: Security

安全
========

.. admonition:: 教学视频
   :class: screencast

   更喜欢视频教程? 可以观看 `Symfony Security screencast series`_ 系列录像.

Symfony的安全系统是非常强大的，但在设置它时也可能令人迷惑。
在本大章中，你将学会如何一步步地设置程序的安全：

#. :ref:`安装安全扩展 <security-installation>`;

#. :ref:`创建用户类 <create-user-class>`;

#. :ref:`认证和防火墙 <security-yaml-firewalls>`;

#. :ref:`拒绝访问你的应用（授权） <security-authorization>`;

#. :ref:`获取当前的用户对象 <retrieving-the-user-object>`.

之后讨论了一些其他重要的主题。

.. _security-installation:

1) 安装
---------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令以在使用之前安装安全功能：

.. code-block:: terminal

    $ composer require symfony/security-bundle

.. _initial-security-yml-setup-authentication:
.. _initial-security-yaml-setup-authentication:
.. _create-user-class:

2a) 创建用户类
--------------------------

无论你将 *如何* 进行身份认证（例如登录表单或API令​​牌）或将用户数据存储的在\ *哪里*\（数据库，单点登录），
下一步始终是相同的：创建“用户”类。最简单的方法是使用 `MakerBundle`_。

假设你希望使用Doctrine将用户数据存储在数据库中：

.. code-block:: terminal

    $ php bin/console make:user

    The name of the security user class (e.g. User) [User]:
    > User

    Do you want to store user data in the database (via Doctrine)? (yes/no) [yes]:
    > yes

    Enter a property name that will be the unique "display" name for the user (e.g.
    email, username, uuid [email]
    > email

    Does this app need to hash/check user passwords? (yes/no) [yes]:
    > yes

    created: src/Entity/User.php
    created: src/Repository/UserRepository.php
    updated: src/Entity/User.php
    updated: config/packages/security.yaml

仅此而已！该命令会询问几个问题，以便它可以准确生成你需要的内容。
最重要的是 ``User.php`` 文件本身。
关于 ``User`` 类的唯一规则是它 *必须* 实现 :class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`。
然后你可以随意添加你需要的任何其他字段或逻辑。
如果你的 ``User`` 类是实体（如本例所示），则可以使用 :ref:`make:entity命令 <doctrine-add-more-fields>` 添加更多字段。
此外，请确保为新实体创建并运行迁移：

.. code-block:: terminal

    $ php bin/console make:migration
    $ php bin/console doctrine:migrations:migrate

.. _security-user-providers:
.. _where-do-users-come-from-user-providers:

2b) "User Provider"
-----------------------

除了你的 ``User`` 类之外，你还需要一个“用户提供器”：一个帮助处理一些事情的类，
比如从会话中重新加载用户数据和一些可选功能，
比如 :doc:`保持登录 </security/remember_me>` 和 :doc:`模拟 </security/impersonating_user>`。

幸运的是，``make:user`` 命令已经在 ``security.yaml`` 文件的 ``providers`` 键下的为你配置了一个。

如果你的 ``User`` 类是实体，则无需执行任何其他操作。
但是如果你的类不是实体，那么 ``make:user`` 也会生成一个你需要加工的 ``UserProvider`` 类。
在此处详细了解用户提供器： :doc:`用户提供器 </security/user_provider>`。

.. _security-encoding-user-password:
.. _encoding-the-user-s-password:

2c) 加密密码
----------------------

并非所有应用都有需要密码的“用户”。
*如果*\你的用户有密码，你可以在 ``security.yaml`` 中控制这些密码的加密方式。
``make:user`` 命令将为你做一些预先配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            encoders:
                # 在这里用你的用户类名称
                App\Entity\User:
                    # 推荐使用 bcrypt 和 argon2i
                    # argon2i 更加可靠，但是需要PHP 7.2 或 Sodium 扩展
                    algorithm: bcrypt
                    cost: 12

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

                <encoder class="App\Entity\User"
                    algorithm="bcrypt"
                    cost="12"/>

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'encoders' => [
                'App\Entity\User' => [
                    'algorithm' => 'bcrypt',
                    'cost' => 12,
                ]
            ],

            // ...
        ]);

既然Symfony知道你想 *如何* 编码密码，
你可以在将用户保存到数据库之前，使用 ``UserPasswordEncoderInterface`` 服务执行加密操作。

例如，通过使用 :ref:`DoctrineFixturesBundle <doctrine-fixtures>`，你可以创建虚拟的数据库用户：

.. code-block:: terminal

    $ php bin/console make:fixtures

    The class name of the fixtures to create (e.g. AppFixtures):
    > UserFixtures

使用此服务对密码进行加密：

.. code-block:: diff

    // src/DataFixtures/UserFixtures.php

    + use Symfony\Component\Security\Core\Encoder\UserPasswordEncoderInterface;
    // ...

    class UserFixtures extends Fixture
    {
    +     private $passwordEncoder;

    +     public function __construct(UserPasswordEncoderInterface $passwordEncoder)
    +     {
    +         $this->passwordEncoder = $passwordEncoder;
    +     }

        public function load(ObjectManager $manager)
        {
            $user = new User();
            // ...

    +         $user->setPassword($this->passwordEncoder->encodePassword(
    +             $user,
    +             'the_new_password'
    +         ));

            // ...
        }
    }

你可以通过运行以下命令手动加密密码：

.. code-block:: terminal

    $ php bin/console security:encode-password

.. _security-yaml-firewalls:
.. _security-firewalls:
.. _firewalls-authentication:

3a) 认证 & 防火墙
------------------------------

安全系统在 ``config/packages/security.yaml`` 中配置。
最重要的部分是 ``firewalls``：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            firewalls:
                dev:
                    pattern: ^/(_(profiler|wdt)|css|images|js)/
                    security: false
                main:
                    anonymous: ~

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="dev"
                    pattern="^/(_(profiler|wdt)|css|images|js)/"
                    security="false"/>

                <firewall name="main">
                    <anonymous/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            'firewalls' => [
                'dev' => [
                    'pattern'   => '^/(_(profiler|wdt)|css|images|js)/',
                    'security'  => false,
                ),
                'main' => [
                    'anonymous' => null,
                ],
            ],
        ]);

“防火墙”是你的身份验证系统：它下面的配置定义了你的用户将 *如何* 进行身份验证（例如登录表单，API令牌等）。

每个请求只会有一个防火墙处于激活状态：
Symfony使用 ``pattern`` 键查找第一个匹配项
（你也可以 :doc:`匹配主机或其他内容 </security/firewall_restriction>`）。
``dev`` 防火墙实际上是一个虚假的防火墙：
它只是确保你不会意外阻止Symfony的开发工具 - 它们存在于 ``/_profiler``、``/_wdt`` 之类的URL下。

所有真实的URL都由 ``main`` 防火墙处理（没有 ``pattern`` 键意味着它匹配 *所有* URL）。
但这并 *不* 意味着每个URL都需要身份验证。
如果添加了 ``anonymous`` 键，这个防火墙就允许匿名访问。

事实上，如果现在打开主页，你 *是* 有权访问的，你会看到你被“认证”为 ``anon.``，不要被"已认证"旁边的“是”所欺骗。
防火墙认证的结果是：未知身份的身份。因此，你是匿名的：

.. image:: /_images/security/anonymous_wdt.png
   :align: center

稍后你将了解如何拒绝访问某些URL或控制器。

.. note::

    如果没有看到工具栏，请安装 :doc:`调试器 </profiler>`：

    .. code-block:: terminal

        $ composer require --dev symfony/profiler-pack

现在我们了解了防火墙，下一步是为用户创建一种进行身份认证的方法！

.. _security-form-login:

3b) 认证你的用户
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony中的身份认证起初可能会感觉有些“神奇”。
这是因为，你不会通过构建路由和控制器来处理登录，而是激活\ *认证提供器*：在调用控制器\ *之前*\自动运行的某些代码。

Symfony有几个 :doc:`内置的认证提供器 </security/auth_providers>`。
如果你的用例 *完全* 符合其中一个，那就太好了！
但是，在大多数情况下 - 包括登录表单 - *我们建议构建一个安保认证器*：
一个允许你控制身份验证过程的\ *每个*\部分的类（请参阅下一节）。

.. tip::

    如果你的应用通过第三方服务（如Google，Facebook或Twitter）登录用户（社交登录），
    请查看 `HWIOAuthBundle`_ 社区bundle。

安保认证器
....................

安保认证器(Guard authenticator)是一个类，可让你\ *完全*\控制身份认证的过程。
有\ *许多*\不同的方法来构建认证器，因此这里有一些常见的用例：

* :doc:`/security/form_login_setup`
* :doc:`/security/guard_authentication`

有关认证器及其工作方式的最详细说明，请参阅 :doc:`/security/guard_authentication`。

.. _`security-authorization`:
.. _denying-access-roles-and-other-authorization:

4) 拒绝访问，角色和其他授权
------------------------------------------------

用户现在可以使用登录表单登录你的应用。真是太好了！
现在，你需要了解如何拒绝访问并使用用户对象。
这称为\ **授权**，其作用是决定用户是否可以访问某些资源（URL，模型对象，方法调用......）。

授权过程有两个不同的方面：

#. 用户在登录时会获得一组特定的角色（例如 ``ROLE_ADMIN``）。
#. 你添加代码以便确定一个资源（例如URL，控制器）需要特定的“属性”（最常见的是像 ``ROLE_ADMIN`` 的一个角色）才能被访问。

角色
~~~~~

当用户登录时，Symfony会调用 ``User`` 对象上的 ``getRoles()`` 方法来确定此用户具有哪些角色。
在我们之前生成的 ``User`` 类中，角色是存储在数据库中的一个数组，
并且每个用户\ *始终*\至少有一个角色--``ROLE_USER``::

    // src/Entity/User.php
    // ...

    /**
     * @ORM\Column(type="json")
     */
    private $roles = [];

    public function getRoles(): array
    {
        $roles = $this->roles;
        // 保证每个用户至少拥有 ROLE_USER
        $roles[] = 'ROLE_USER';

        return array_unique($roles);
    }

这是一个不错的默认设置，但你可以执行\ *任何*\操作以决定用户应具有的角色。以下是一些指导原则：

* 每个角色都必须以 ``ROLE_`` 开头（否则，认证器将无法按预期工作）

* 除了上述规则，角色只是一个字符串，你可以创造你需要的东西（例如 ``ROLE_PRODUCT_ADMIN``）。

你将使用这些角色来授予对你网站特定部分的访问权限。
你还可以使用 :ref:`角色层级 <security-role-hierarchy>` 结构，其中某些角色会自动为你提供其他角色。

.. _security-role-authorization:

添加代码以拒绝访问
~~~~~~~~~~~~~~~~~~~~~~~

有\ **两种**\方法可以拒绝访问某些内容：

#. :ref:`yaml中的access_control <security-authorization-access-control>` 允许你保护URL模式（例如 ``/admin/*``）。这很容易配置，但不太灵活;

#. :ref:`在你的控制器（或其他代码）中 <security-securing-controller>`。

.. _security-authorization-access-control:

保护之URL模式（access_control）
......................................

保护应用的部分安全的最基本方法是在 ``security.yaml`` 中保护整个URL模式。
例如，要为以 ``/admin`` 开头的所有网址要求 ``ROLE_ADMIN``，你可以：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                # ...
                main:
                    # ...

            access_control:
                # /admin* 必须是 ROLE_ADMIN 角色
                - { path: ^/admin, roles: ROLE_ADMIN }

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
                </firewall>

                <!-- require ROLE_ADMIN for /admin* -->
                <rule path="^/admin" role="ROLE_ADMIN"/>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                // ...
                'main' => [
                    // ...
                ],
            ],
            'access_control' => [
                // require ROLE_ADMIN for /admin*
                ['path' => '^/admin', 'role' => 'ROLE_ADMIN'],
            ],
        ]);

你可以根据需要定义任意数量的URL模式 - 每个模式都是正则表达式。
**但是**，每个请求仅匹配一\ **个**：Symfony从列表顶部开始，并在找到第一个匹配时停止：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            access_control:
                # 匹配 /admin/users/*
                - { path: ^/admin/users, roles: ROLE_SUPER_ADMIN }

                # 匹配 /admin/* 中除了符合上述规则的任何内容
                - { path: ^/admin, roles: ROLE_ADMIN }

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

                <rule path="^/admin/users" role="ROLE_SUPER_ADMIN"/>
                <rule path="^/admin" role="ROLE_ADMIN"/>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'access_control' => [
                ['path' => '^/admin/users', 'role' => 'ROLE_SUPER_ADMIN'],
                ['path' => '^/admin', 'role' => 'ROLE_ADMIN'],
            ],
        ]);

使用 ``^`` 添加到路径之前意味着只匹配以该模式\ *开头*\的URL。
例如，一个 ``/admin`` （没有 ``^``）的路径将匹配 ``/admin/foo``，
但也会匹配 ``/foo/admin`` 之类的URL。

每个 ``access_control`` 也可以匹配IP地址，主机名和HTTP方法。
它还可用于将用户重定向到URL模式的 ``https`` 版本。
请参阅 :doc:`/security/access_control`。

.. _security-securing-controller:

保护之控制器和其他代码
...................................

你可以在控制器内部使用拒绝访问::

    // src/Controller/AdminController.php
    // ...

    public function adminDashboard()
    {
        $this->denyAccessUnlessGranted('ROLE_ADMIN');

        // 或者添加一个可被开发者看见的可选消息
        $this->denyAccessUnlessGranted('ROLE_ADMIN', null, 'User tried to access a page without having ROLE_ADMIN');
    }

仅此而已！如果没有访问权限，
则抛出一个特殊的 :class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`，
并且不再执行控制器中的代码。
然后，将触发以下两件事中的一件：

1) 如果用户尚未登录，则会要求他们登录（例如，重定向到登录页面）。

2) 如果用户\ *已*\登录，但\ *没有*\ ``ROLE_ADMIN`` 角色，则会显示403拒绝访问页面（你可以 :ref:`自定义 <controller-error-pages-by-status-code>`）。

.. _security-securing-controller-annotations:

得益于 SensioFrameworkExtraBundle，你还可以使用注释来保护你的控制器：

.. code-block:: diff

    // src/Controller/AdminController.php
    // ...

    + use Sensio\Bundle\FrameworkExtraBundle\Configuration\IsGranted;

    + /**
    +  * Require ROLE_ADMIN for *every* controller method in this class.
    +  *
    +  * @IsGranted("ROLE_ADMIN")
    +  */
    class AdminController extends AbstractController
    {
    +     /**
    +      * 仅对此控制器方法要求 ROLE_ADMIN
    +      *
    +      * @IsGranted("ROLE_ADMIN")
    +      */
        public function adminDashboard()
        {
            // ...
        }
    }

有关更多信息，请参阅 `FrameworkExtraBundle 文档`_。

.. _security-template:

模板中的访问控制
...........................

如果要检查当前用户是否具有某个角色，可以在任何Twig模板中使用内置的 ``is_granted()`` 辅助函数：

.. code-block:: html+twig

    {% if is_granted('ROLE_ADMIN') %}
        <a href="...">Delete</a>
    {% endif %}

保护其他服务
.......................

参阅 :doc:`/security/securing_services`.

检查用户是否登录（IS_AUTHENTICATED_FULLY）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你\ *只*\想检查用户是否只是登录（你不关心角色），则有两种选择。
首先，如果你已经为\ *每个*\用户提供了 ``ROLE_USER``，那么你可以检查该角色。
要不然，你可以使用特殊的“属性”代替角色::

    // ...

    public function adminDashboard()
    {
        $this->denyAccessUnlessGranted('IS_AUTHENTICATED_FULLY');

        // ...
    }

你可以在任何使用角色的地方使用 ``IS_AUTHENTICATED_FULLY``：例如 ``access_control`` 或在Twig中。

``IS_AUTHENTICATED_FULLY`` 不是一个角色，但它有点像一个角色，每个已登录的用户都会拥有此角色。
实际上，有3个这样的特殊属性：

* ``IS_AUTHENTICATED_REMEMBERED``：\ *所有*\登录的用户都有这个属性，即使他们通过“记住我的cookie”而登录。
  即使你不使用 :doc:`保持登录 </security/remember_me>`，也可以使用此功能检查用户是否已登录。

* ``IS_AUTHENTICATED_FULLY``：这类似于 ``IS_AUTHENTICATED_REMEMBERED``，
  但更严格。仅通过“记住我的cookie”登录的用户将拥有 ``IS_AUTHENTICATED_REMEMBERED``，
  但不会有 ``IS_AUTHENTICATED_FULLY``。

* ``IS_AUTHENTICATED_ANONYMOUSLY``：\ *所有*\用户（甚至是匿名用户）都有此属性 -
  这在将URL列入\ *白名单*\以保障正常访问时非常有用 - 一些详细信息在 :doc:`/security/access_control`

.. _security-secure-objects:

访问控制列表（ACL）：保护单个数据库对象
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

想象一下，你正在设计一个博客，用户可以对你的帖子发表评论。
你还希望用户能够编辑自己的评论，但不能编辑其他用户的评论。
此外，作为管理员用户，你希望能够编辑\ *所有*\评论。

:doc:`表决器 </security/voters>` 允许你编写所需的\ *任何*\业务逻辑
（例如，用户可以编辑此帖子，是因为他们是创建者）来确定访问权限。
这就是为什么Symfony正式推荐使用表决器来创建类似ACL的安全系统的原因。

如果你仍然喜欢使用传统的ACL，请参阅 `Symfony ACL bundle`_。

.. _retrieving-the-user-object:

5a) 获取用户对象
----------------------------

认证之后，可以通过 ``getUser()`` 快捷方式访问当前用户的 ``User`` 对象::

    public function index()
    {
        // 通常，你首先需要确保用户已经认证
        $this->denyAccessUnlessGranted('IS_AUTHENTICATED_FULLY');

        // 返回用户对象，如果用户未认证，则返回null
        // 使用内联文档告诉码农你确切的用户类
        /** @var \App\Entity\User $user */
        $user = $this->getUser();

        // 调用你添加到用户类的任何方法
        // 例如，如果添加了一个 getFirstName() 方法，则可以使用该方法。
        return new Response('Well hi there '.$user->getFirstName());
    }

5b) 在服务中获取用户
------------------------------------

如果你需要在服务中获取登录用户，可以使用 :class:`Symfony\\Component\\Security\\Core\\Security` 服务::

    // src/Service/ExampleService.php
    // ...

    use Symfony\Component\Security\Core\Security;

    class ExampleService
    {
        private $security;

        public function __construct(Security $security)
        {
            // 要避免在构造函数中调用 getUser()：认证可能尚未完成。
            // 取而代之，应该存储整个Security对象。
            $this->security = $security;
        }

        public function someMethod()
        {
            // 返回用户对象，如果未登录则返回 null
            $user = $this->security->getUser();
        }
    }

在模板中获取用户
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在Twig模板中，可以通过 :ref:`app.user <reference-twig-global-app>` 键访问用户对象：

.. code-block:: html+twig

    {% if is_granted('IS_AUTHENTICATED_FULLY') %}
        <p>Email: {{ app.user.email }}</p>
    {% endif %}

.. _security-logging-out:

注销登录
-----------

要启用注销，请激活防火墙下的 ``logout`` 配置参数：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    logout:
                        path:   app_logout

                        # 注销后的重定向目标
                        # target: app_any_route

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

                <firewall name="secured_area">
                    <!-- ... -->
                    <logout path="app_logout"/>
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'firewalls' => [
                'secured_area' => [
                    // ...
                    'logout' => ['path' => 'app_logout'],
                ],
            ],
        ]);

接下来，你需要为此改URL创建路由（但不是控制器）：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/SecurityController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class SecurityController extends AbstractController
        {
            /**
             * @Route("/logout", name="app_logout", methods={"GET"})
             */
            public function logout()
            {
                // 控制器可以为空: 因为它永远不会执行！
                throw new \Exception('Don\'t forget to activate logout in security.yaml');
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        app_logout:
            path: /logout
            methods: GET

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="app_logout" path="/logout" methods="GET"/>
        </routes>

    ..  code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('logout', '/logout')
                ->methods(['GET'])
            ;
        };

就是这样！通过将用户发送到 ``app_logout`` 路由（即 ``/logout``），
Symfony将取消对当前用户的认证并重定向它们。

.. tip::

    需要在注销后控制一些事情？在 ``logout`` 下添加 ``success_handler`` 键并将其指向实现
    :class:`Symfony\\Component\\Security\\Http\\Logout\\LogoutSuccessHandlerInterface` 的类的服务ID。

.. _security-role-hierarchy:

角色层级
------------------

你可以通过创建角色层级来定义角色继承规则，而不是为每个用户分配许多角色：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            role_hierarchy:
                ROLE_ADMIN:       ROLE_USER
                ROLE_SUPER_ADMIN: [ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH]

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

                <role id="ROLE_ADMIN">ROLE_USER</role>
                <role id="ROLE_SUPER_ADMIN">ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH</role>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            // ...

            'role_hierarchy' => [
                'ROLE_ADMIN'       => 'ROLE_USER',
                'ROLE_SUPER_ADMIN' => [
                    'ROLE_ADMIN',
                    'ROLE_ALLOWED_TO_SWITCH',
                ],
            ],
        ]);

具有 ``ROLE_ADMIN`` 角色的用户也将具有 ``ROLE_USER`` 角色。
使用 ``ROLE_SUPER_ADMIN`` 的用户将自动拥有 ``ROLE_ADMIN``、
``ROLE_ALLOWED_TO_SWITCH`` 和 ``ROLE_USER`` （继承自 ``ROLE_ADMIN``）。

要使角色层级起作用，请不要尝试手动调用 ``$user->getRoles()``。例如，在继承自
:ref:`基础控制器 <the-base-controller-class-services>` 的控制器中::

    // 错误 - $user->getRoles() 将不知道角色的层级
    $hasAccess = in_array('ROLE_ADMIN', $user->getRoles());

    // 正确 - 使用正常的安全方法
    $hasAccess = $this->isGranted('ROLE_ADMIN');
    $this->denyAccessUnlessGranted('ROLE_ADMIN');

.. note::

    ``role_hierarchy`` 值是静态的 - 例如，你不能将角色层级存储在数据库中。
    如果有需要，请创建一个自定义 :doc:`安全表决器 </security/voters>` 来查找数据库中的用户角色。

检查依赖中的安全漏洞
---------------------------------------------------------

参阅 :doc:`/security/security_checker`.

常见问题
--------------------------

**我可以有多个防火墙吗？**
    没问题!但通常没有必要。每个防火墙就像一个单独的安全系统。
    因此，除非你有\ *非常*\不同的认证需求，否则一个防火墙通常表现良好。
    使用 :doc:`安保认证器 </security/guard_authentication>`，
    你可以在同一防火墙下创建各种认证（例如表单登录，API令牌和LDAP）方式。

**我能在防火墙之间共享身份验证吗？**
    可以，但只有些许配置项。如果你使用多个防火墙，且只对一个防火墙进行身份验证，则\ *不会*\自动对任何其他防火墙进行认证。
    不同的防火墙就像不同的安全系统。
    为此，你必须为不同的防火墙明确指定相同的 :ref:`reference-security-firewall-context`。
    但对于大多数应用，通常拥有一个主防火墙就足够了。

**安全似乎不适用于我的错误页面**
    由于路由在安全\ *之前*\完成，因此任何防火墙都不会覆盖404错误页面。
    这意味着你无法在这些页面上检查安全性，甚至无法访问用户对象。
    有关更多详细信息，请参阅 :doc:`/controller/error_pages`。

**我的身份认证似乎不起作用：没有错误，但我从未登录过**
    有时认证可能成功了，但重定向后，由于从会话中加载 ``User`` 时出现问题，认证会被立即注销。
    要查看是否是这个问题，请检查日志文件（``var/log/dev.log``）以获取日志消息：

**无法刷新令牌，因为用户已更改**

    如果你看到了这个，有两个可能的原因。
    首先，从会话中加载用户可能出现了问题，详情请参阅 :ref:`user_session_refresh`。
    其次，如果自上次刷新页面后数据库中的某些用户信息发生了变化，Symfony会出于安全原因而故意注销该用户。

扩展阅读
----------

认证（识别/登录用户）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. toctree::
    :maxdepth: 1

    security/form_login_setup
    security/guard_authentication
    security/auth_providers
    security/user_provider
    security/ldap
    security/remember_me
    security/impersonating_user
    security/user_checkers
    security/named_encoders
    security/multiple_guard_authenticators
    security/firewall_restriction
    security/csrf
    security/custom_authentication_provider

授权 (拒绝访问)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. toctree::
    :maxdepth: 1

    security/voters
    security/securing_services
    security/access_control
    security/access_denied_handler
    security/acl
    security/force_https
    security/security_checker

.. _`Frameworkextrabundle 文档`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/index.html
.. _`HWIOAuthBundle`: https://github.com/hwi/HWIOAuthBundle
.. _`Symfony ACL bundle`: https://github.com/symfony/acl-bundle
.. _`Symfony Security screencast series`: https://symfonycasts.com/screencast/symfony-security
.. _`MakerBundle`: https://symfony.com/doc/current/bundles/SymfonyMakerBundle/index.html
