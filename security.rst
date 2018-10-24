.. index::
   single: Security

Security
========

.. admonition:: 教学视频
   :class: screencast

   更喜欢视频教程? 可以观看 `Symfony Security screencast series`_ 系列录像.

Symfony的安全系统是非常强大的，但在设置它时也可能令人迷惑。
在本大章中，你将学会如何一步步地设置程序的安全：

#. :ref:`安装安全扩展 <security-installation>`;

#. :ref:`创建用户类 <create-user-class>`;

#. :ref:`认证和防火墙 <security-yaml-firewalls>`;

#. :ref:`拒绝访问你的应用（授权）; <security-authorization>`;

#. :ref:`获取当前的用户对象 <retrieving-the-user-object>`.

A few other important topics are discussed after.

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

无论你将 *如何* 进行身份认证（例如登录表单或API令​​牌）或将用户数据存储的在*哪里*（数据库，单点登录），
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
如果你的 ``User`` 类是实体（如本例所示），则可以使用 :ref:`make:entity 命令 <doctrine-add-more-fields>` 添加更多字段。
此外，请确保为新实体创建并运行迁移：

.. code-block:: terminal

    $ php bin/console make:migration
    $ php bin/console doctrine:migrations:migrate

.. _security-user-providers:
.. _where-do-users-come-from-user-providers:

2b) "User Provider"
-----------------------

除了你的 ``User`` 类之外，你还需要一个“用户提供者”：一个帮助处理一些事情的类，
比如从会话中重新加载用户数据和一些可选功能，
比如 :doc:`保持登录 </security/remember_me>`和
:doc:`模拟 </security/impersonating_user>`。

幸运的是，``make:user`` 命令已经在 ``security.yaml`` 文件的 ``providers`` 键下的为你配置了一个。

如果你的 ``User`` 类是实体，则无需执行任何其他操作。
但是如果你的类不是实体，那么 ``make:user`` 也会生成一个你需要加工的 ``UserProvider`` 类。
在此处详细了解用户提供商： :doc:`用户提供者 </security/user_provider>`。

.. _security-encoding-user-password:
.. _encoding-the-user-s-password:

2c) 加密密码
----------------------

并非所有应用都有需要密码的“用户”。
*如果* 你的用户有密码，你可以在 ``security.yaml`` 中控制这些密码的加密方式。
``make:user``命令将为你预先做了配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            encoders:
                # 在这里用你的用户类名称
                App\Entity\User:
                    # 推荐使用 bcrypt 和 argon21
                    # argon21 更加可靠，但是需要PHP 7.2 或 Sodium 扩展
                    algorithm: bcrypt
                    cost: 12

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

                <encoder class="App\Entity\User"
                    algorithm="bcrypt"
                    cost="12" />

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...

            'encoders' => array(
                'App\Entity\User' => array(
                    'algorithm' => 'bcrypt',
                    'cost' => 12,
                )
            ),
            // ...
        ));

既然Symfony知道你想 *如何* 编码密码，
你可以在将用户保存到数据库之前，使用 ``UserPasswordEncoderInterface`` 服务执行加密操作。

例如，通过使用 :ref:`DoctrineFixturesBundle <doctrine-fixtures>`，你可以创建虚拟的数据库用户：

.. code-block:: terminal

    $ php bin/console make:fixtures

    The class name of the fixtures to create (e.g. AppFixtures):
    > UserFixture

使用此服务对密码进行加密：

.. code-block:: diff

    // src/DataFixtures/UserFixture.php

    + use Symfony\Component\Security\Core\Encoder\UserPasswordEncoderInterface;
    // ...

    class UserFixture extends Fixture
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
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <firewall name="dev"
                    pattern="^/(_(profiler|wdt)|css|images|js)/"
                    security="false" />

                <firewall name="main">
                    <anonymous />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                'dev' => array(
                    'pattern'   => '^/(_(profiler|wdt)|css|images|js)/',
                    'security'  => false,
                ),
                'main' => array(
                    'anonymous' => null,
                ),
            ),
        ));

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
防火墙认证的结果是：它不知道你的身份。因此，你是匿名的：

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
这是因为，你不会通过构建路由和控制器来处理登录，而是激活 *认证提供程序*：在调用控制器 *之前* 自动运行的某些代码。

Symfony有几个 :doc:`内置的认证提供者 </security/auth_providers>`。
如果你的用例 *完全* 符合其中一个，那就太好了！
但是，在大多数情况下 - 包括登录表单 - *我们建议构建一个安保认证器*：
一个允许你控制身份验证过程的 *每个* 部分的类（请参阅下一节）。

.. tip::

    如果你的应用程序通过第三方服务（如Google，Facebook或Twitter）登录用户（社交登录），
    请查看 `HWIOAuthBundle`_ 社区bundle。

安保认证器
....................

安保认证器(Guard authenticator)是一个类，可让你 *完全* 控制身份认证的过程。
有 *许多* 不同的方法来构建认证器，因此这里有一些常见的用例：

* :doc:`/security/form_login_setup`
* :doc:`/security/guard_authentication`

有关认证器及其工作方式的最详细说明，请参阅 :doc:`/security/guard_authentication`。

.. _`security-authorization`:
.. _denying-access-roles-and-other-authorization:

4) 拒绝访问，角色和其他授权
------------------------------------------------

用户现在可以使用登录表单登录你的应用。真是太好了！
现在，你需要了解如何拒绝访问并使用用户对象。
这称为 **授权**，其作用是决定用户是否可以访问某些资源（URL，模型对象，方法调用......）。

授权过程有两个不同的方面：

#. 用户在登录时会获得一组特定的角色（例如 ``ROLE_ADMIN``）。
#. 你添加代码以便确定一个资源（例如URL，控制器）需要特定的“属性”（最常见的是像 ``ROLE_ADMIN`` 的一个角色）才能被访问。

角色
~~~~~

当用户登录时，Symfony会调用 ``User`` 对象上的 ``getRoles()`` 方法来确定此用户具有哪些角色。
在我们之前生成的 ``User`` 类中，角色是存储在数据库中的一个数组，
并且每个用户 *始终* 至少有一个角色--``ROLE_USER``::

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

这是一个不错的默认设置，但您可以执行任何操作以确定用户应具有的角色。以下是一些指导原则：
This is a nice default, but you can do *whatever* you want to determine which roles
a user should have. Here are a few guidelines:

* Every role **must start with** ``ROLE_`` (otherwise, things won't work as expected)每个角色都必须以ROLE_开头（否则，事情将无法按预期工作）

* Other than the above rule, a role is just a string and you can invent what you
  need (e.g. ``ROLE_PRODUCT_ADMIN``).除了上述规则，角色只是一个字符串，你可以发明你需要的东西（例如ROLE_PRODUCT_ADMIN）。

您将使用这些角色来授予对您网站特定部分的访问权限。您还可以使用角色层次结构，其中某些角色会自动为您提供其他角色。
You'll use these roles next to grant access to specific sections of your site.
You can also use a :ref:`role hierarchy <security-role-hierarchy>` where having
some roles automatically give you other roles.

.. _security-role-authorization:

添加代码以拒绝访问
~~~~~~~~~~~~~~~~~~~~~~~

有两种方法可以拒绝访问某些内容：
There are **two** ways to deny access to something:

#. :ref:`access_control in security.yaml <security-authorization-access-control>`
   allows you to protect URL patterns (e.g. ``/admin/*``). This is easy,
   but less flexible;security.yaml中的access_control允许您保护URL模式（例如/ admin / *）。这很容易，但不太灵活;

#. :ref:`in your controller (or other code) <security-securing-controller>`.在你的控制器（或其他代码）。

.. _security-authorization-access-control:

Securing URL patterns (access_control)
......................................

The most basic way to secure part of your app is to secure an entire URL pattern
in ``security.yaml``. For example, to require ``ROLE_ADMIN`` for all URLs that
start with ``/admin``, you can:

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
                # require ROLE_ADMIN for /admin*
                - { path: ^/admin, roles: ROLE_ADMIN }

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

                <firewall name="main">
                    <!-- ... -->
                </firewall>

                <!-- require ROLE_ADMIN for /admin* -->
                <rule path="^/admin" role="ROLE_ADMIN" />
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                // ...
                'main' => array(
                    // ...
                ),
            ),
           'access_control' => array(
               // require ROLE_ADMIN for /admin*
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
        ));

You can define as many URL patterns as you need - each is a regular expression.
**BUT**, only **one** will be matched per request: Symfony starts at the top of
the list and stops when it finds the first match:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...

            access_control:
                # matches /admin/users/*
                - { path: ^/admin/users, roles: ROLE_SUPER_ADMIN }

                # matches /admin/* except for anything matching the above rule
                - { path: ^/admin, roles: ROLE_ADMIN }

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

                <rule path="^/admin/users" role="ROLE_SUPER_ADMIN" />
                <rule path="^/admin" role="ROLE_ADMIN" />
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...

            'access_control' => array(
                array('path' => '^/admin/users', 'role' => 'ROLE_SUPER_ADMIN'),
                array('path' => '^/admin', 'role' => 'ROLE_ADMIN'),
            ),
        ));

Prepending the path with ``^`` means that only URLs *beginning* with the
pattern are matched. For example, a path of simply ``/admin`` (without
the ``^``) would match ``/admin/foo`` but would also match URLs like ``/foo/admin``.

Each ``access_control`` can also match on IP address, hostname and HTTP methods.
It can also be used to redirect a user to the ``https`` version of a URL pattern.
See :doc:`/security/access_control`.

.. _security-securing-controller:

Securing Controllers and other Code
...................................

You can easily deny access from inside a controller::

    // src/Controller/AdminController.php
    // ...

    public function adminDashboard()
    {
        $this->denyAccessUnlessGranted('ROLE_ADMIN');

        // or add an optional message - seen by developers
        $this->denyAccessUnlessGranted('ROLE_ADMIN', null, 'User tried to access a page without having ROLE_ADMIN');
    }

That's it! If access is not granted, a special
:class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`
is thrown and no more code in your controller is executed. Then, one of two things
will happen:

1) If the user isn't logged in yet, they will be asked to log in (e.g. redirected
   to the login page).

2) If the user *is* logged in, but does *not* have the ``ROLE_ADMIN`` role, they'll
   be shown the 403 access denied page (which you can
   :ref:`customize <controller-error-pages-by-status-code>`).

.. _security-securing-controller-annotations:

Thanks to the SensioFrameworkExtraBundle, you can also secure your controller
using annotations:

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
    +      * Require ROLE_ADMIN for only this controller method.
    +      *
    +      * @IsGranted("ROLE_ADMIN")
    +      */
        public function adminDashboard()
        {
            // ...
        }
    }

For more information, see the `FrameworkExtraBundle documentation`_.

.. _security-template:

Access Control in Templates
...........................

If you want to check if the current access inside a template, use
the built-in ``is_granted()`` helper function:

.. code-block:: html+twig

    {% if is_granted('ROLE_ADMIN') %}
        <a href="...">Delete</a>
    {% endif %}

Securing other Services
.......................

See :doc:`/security/securing_services`.

Checking to see if a User is Logged In (IS_AUTHENTICATED_FULLY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you *only* want to check if a user is simply logged in (you don't care about roles),
you have two options. First, if you've given *every* user ``ROLE_USER``, you can
just check for that role. Otherwise, you can use a special "attribute" in place
of a role::

    // ...

    public function adminDashboard()
    {
        $this->denyAccessUnlessGranted('IS_AUTHENTICATED_FULLY');

        // ...
    }

You can use ``IS_AUTHENTICATED_FULLY`` anywhere roles are used: like ``access_control``
or in Twig.

``IS_AUTHENTICATED_FULLY`` isn't a role, but it kind of acts like one, and every
user that has logged in will have this. Actually, there are 3 special attributes
like this:

* ``IS_AUTHENTICATED_REMEMBERED``: *All* logged in users have this, even
  if they are logged in because of a "remember me cookie". Even if you don't
  use the :doc:`remember me functionality </security/remember_me>`,
  you can use this to check if the user is logged in.

* ``IS_AUTHENTICATED_FULLY``: This is similar to ``IS_AUTHENTICATED_REMEMBERED``,
  but stronger. Users who are logged in only because of a "remember me cookie"
  will have ``IS_AUTHENTICATED_REMEMBERED`` but will not have ``IS_AUTHENTICATED_FULLY``.

* ``IS_AUTHENTICATED_ANONYMOUSLY``: *All* users (even anonymous ones) have
  this - this is useful when *whitelisting* URLs to guarantee access - some
  details are in :doc:`/security/access_control`.

.. _security-secure-objects:

Access Control Lists (ACLs): Securing individual Database Objects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Imagine you are designing a blog where users can comment on your posts. You
also want a user to be able to edit their own comments, but not those of
other users. Also, as the admin user, you want to be able to edit *all* comments.

:doc:`Voters </security/voters>` allow you to write *whatever* business logic you
need (e.g. the user can edit this post because they are the creator) to determine
access. That's why voters are officially recommended by Symfony to create ACL-like
security systems.

If you still prefer to use traditional ACLs, refer to the `Symfony ACL bundle`_.

.. _retrieving-the-user-object:

5a) Fetching the User Object
----------------------------

After authentication, the ``User`` object of the current user can be accessed
via the ``getUser()`` shortcut::

    public function index()
    {
        // usually you'll want to make sure the user is authenticated first
        $this->denyAccessUnlessGranted('IS_AUTHENTICATED_FULLY');

        // returns your User object, or null if the user is not authenticated
        // use inline documentation to tell your editor your exact User class
        /** @var \App\Entity\User $user */
        $user = $this->getUser();

        // Call whatever methods you've added to your User class
        // For example, if you added a getFirstName() method, you can use that.
        return new Response('Well hi there '.$user->getFirstName());
    }

5b) Fetching the User from a Service
------------------------------------

If you need to get the logged in user from a service, use the
:class:`Symfony\\Component\\Security\\Core\\Security` service::

    // src/Service/ExampleService.php
    // ...

    use Symfony\Component\Security\Core\Security;

    class ExampleService
    {
        private $security;

        public function __construct(Security $security)
        {
            // Avoid calling getUser() in the constructor: auth may not
            // be complete yet. Instead, store the entire Security object.
            $this->security = $security;
        }

        public function someMethod()
        {
            // returns User object or null if not authenticated
            $user = $this->security->getUser();
        }
    }

Fetch the User in a Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In a Twig Template the user object can be accessed via the :ref:`app.user <reference-twig-global-app>`
key:

.. code-block:: html+twig

    {% if is_granted('IS_AUTHENTICATED_FULLY') %}
        <p>Email: {{ app.user.email }}</p>
    {% endif %}

.. _security-logging-out:

Logging Out
-----------

To enable logging out, activate the  ``logout`` config parameter under your firewall:

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

                        # where to redirect after logout
                        # target: app_any_route

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

                <firewall name="secured_area">
                    <!-- ... -->
                    <logout path="app_logout" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'logout' => array('path' => 'app_logout'),
                ),
            ),
        ));

Next, you'll need to create a route for this URL (but not a controller):

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        app_logout:
            path: /logout

    .. code-block:: php-annotations

        // src/Controller/SecurityController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Routing\Annotation\Route;

        class SecurityController extends AbstractController
        {
            /**
             * @Route("/logout", name="app_logout")
             */
            public function logout()
            {
                // controller can be blank: it will never be executed!
                throw new \Exception('Don\'t forget to activate logout in security.yaml');
            }
        }

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="app_logout" path="/logout" />
        </routes>

    ..  code-block:: php

        // config/routes.php
        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $routes = new RouteCollection();
        $routes->add('app_logout', new Route('/logout'));

        return $routes;

And that's it! By sending a user to the ``app_logout`` route (i.e. to ``/logout``)
Symfony will un-authenticate the current user and redirect them.

.. tip::

    Need more control of what happens after logout? Add a ``success_handler`` key
    under ``logout`` and point it to a service id of a class that implements
    :class:`Symfony\\Component\\Security\\Http\\Logout\\LogoutSuccessHandlerInterface`.

.. _security-role-hierarchy:

Hierarchical Roles
------------------

Instead of giving many roles to each user, you can define role inheritance
rules by creating a role hierarchy:

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
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <role id="ROLE_ADMIN">ROLE_USER</role>
                <role id="ROLE_SUPER_ADMIN">ROLE_ADMIN, ROLE_ALLOWED_TO_SWITCH</role>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...

            'role_hierarchy' => array(
                'ROLE_ADMIN'       => 'ROLE_USER',
                'ROLE_SUPER_ADMIN' => array(
                    'ROLE_ADMIN',
                    'ROLE_ALLOWED_TO_SWITCH',
                ),
            ),
        ));

Users with the ``ROLE_ADMIN`` role will also have the
``ROLE_USER`` role. And users with ``ROLE_SUPER_ADMIN``, will automatically have
``ROLE_ADMIN``, ``ROLE_ALLOWED_TO_SWITCH`` and ``ROLE_USER`` (inherited from ``ROLE_ADMIN``).

For role hierarchy to work, do not try to call ``$user->getRoles()`` manually::

    // BAD - $user->getRoles() will not know about the role hierarchy
    $hasAccess = in_array('ROLE_ADMIN', $user->getRoles());

    // GOOD - use of the normal security methods
    $hasAccess = $this->isGranted('ROLE_ADMIN');
    $this->denyAccessUnlessGranted('ROLE_ADMIN');

.. note::

    The ``role_hierarchy`` values are static - you can't, for example, store the
    role hierarchy in a database. If you need that, create a custom
    :doc:`security voter </security/voters>` that looks for the user roles
    in the database.

Checking for Security Vulnerabilities in your Dependences
---------------------------------------------------------

See :doc:`/security/security_checker`.

Frequently Asked Questions
--------------------------

**Can I have Multiple Firewalls?**
    Yes! But it's usually not necessary. Each firewall is like a separate security
    system. And so, unless you have *very* different authentication needs, one
    firewall usually works well. With :doc:`Guard authentication </security/guard_authentication>`,
    you can create various, diverse ways of allowing authentication (e.g. form login,
    API key authentication and LDAP) all under the same firewall.

**Can I Share Authentication Between Firewalls?**
    Yes, but only with some configuration. If you're using multiple firewalls and
    you authenticate against one firewall, you will *not* be authenticated against
    any other firewalls automatically. Different firewalls are like different security
    systems. To do this you have to explicitly specify the same
    :ref:`reference-security-firewall-context` for different firewalls. But usually
    for most applications, having one main firewall is enough.

**Security doesn't seem to work on my Error Pages**
    As routing is done *before* security, 404 error pages are not covered by
    any firewall. This means you can't check for security or even access the
    user object on these pages. See :doc:`/controller/error_pages`
    for more details.

**My Authentication Doesn't Seem to Work: No Errors, but I'm Never Logged In**
    Sometimes authentication may be successful, but after redirecting, you're
    logged out immediately due to a problem loading the ``User`` from the session.
    To see if this is an issue, check your log file (``var/log/dev.log``) for
    the log message:

    > Cannot refresh token because user has changed.

    If you see this, there are two possible causes. First, there may be a problem
    loading your User from the session. See :ref:`user_session_refresh`. Second,
    if certain user information was changed in the database since the last page
    refresh, Symfony will purposely log out the user for security reasons.

Learn More
----------

Authentication (Identifying/Logging in the User)
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

Authorization (Denying Access)
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

.. _`frameworkextrabundle documentation`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/index.html
.. _`HWIOAuthBundle`: https://github.com/hwi/HWIOAuthBundle
.. _`Symfony ACL bundle`: https://github.com/symfony/acl-bundle
.. _`Symfony Security screencast series`: https://symfonycasts.com/screencast/symfony-security
.. _`MakerBundle`: https://github.com/symfony/maker-bundle
