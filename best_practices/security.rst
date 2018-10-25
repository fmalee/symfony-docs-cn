安全
========

认证和防火墙 (如获取用户凭据)
------------------------------------------------------------------

你可以配置Symfony以使用你想要的任何方法对用户进行身份认证，并从任何数据来源加载用户信息。
这是一个复杂的主题，但 :doc:`Security guide </security>` 中有很多相关信息。

不管你的需求是什么，认证都要在 ``security.yaml`` 中进行配置，主要在 ``firewalls`` 键下进行。

.. best-practice::

    除非你有两个合理的不同认证系统及其用户（比如一个是主站的表单登陆系统，还有一个是基于API的令牌系统），
    我们推荐只使用一个防火墙入口 ，并且开启其下的 ``anonymous`` 选项。、

大多数应用只有一个身份验证系统和一组用户。
因此，你只需要*一个*防火墙项。
当然也有例外，特别是如果你的网站上有网页和API部分。但关键是要保持简单。

此外，你应该使用防火墙下的 ``anonymous`` 键。
如果你需要要求用户登录站点的不同部分（或几乎*所有*部分），请使用``access_control`` 区域。

.. best-practice::

    使用 ``bcrypt`` 编码器对用户密码进行哈希处理。

如果您的用户使用密码，我们建议使用 ``bcrypt``编码器对其进行散列，而不是传统的 SHA-512 散列编码器。
``bcrypt`` 的主要优点是包含一个*salt*值来防止彩虹表攻击，以及它的自适应性，能令暴力破解的过程变得愈发之长。

.. note::

    :ref:`Argon2i <reference-security-argon2i>` 是行业标准推荐的散列算法，
    但除非你使用的是PHP 7.2+或安装了 `libsodium`_ 扩展，否则将无法使用此算法。
    ``bcrypt`` 足以满足大多数应用程序的需求。

基于这种考虑，下面是我们的程序和验证有关的配置，使用了表单登陆，并且从数据库中取得用户：

.. code-block:: yaml

    # config/packages/security.yaml
    security:
        encoders:
            App\Entity\User: bcrypt

        providers:
            database_users:
                entity: { class: App\Entity\User, property: username }

        firewalls:
            secured_area:
                pattern: ^/
                anonymous: true
                form_login:
                    check_path: login
                    login_path: login

                logout:
                    path: security_logout
                    target: homepage

    # ... access_control exists, but is not shown here

.. tip::

    我们这个项目的源代码中包含了注释在内，用于解释每一部分。

授权 (如拒绝访问)
-----------------------------------

Symfony为你提供了几种实施授权的方法，包括 :doc:`security.yaml </reference/configuration/security>` 中的 ``access_control``配置，:ref:`@Security annotation <best-practices-security-annotation>` 以及直接在 ``security.authorization_checker`` 服务上使用 :ref:`isGranted <best-practices-directly-isGranted>`。

.. best-practice::

    * 为了保护泛URL内容，在 ``access_control`` 中使用正则匹配；
    * 尽最大可能使用 ``@Security`` 注释；
    * 一旦遇到复杂状况，直接利用 ``security.authorization_checker`` 服务来检查安全性。

还有不同的方法可以集中管理授权逻辑，例如自定义安全选民(security voter)：

.. best-practice::

    定义一个自定义安全选民以实现精细化（fine-grained）的访问控制。

.. _best-practices-security-annotation:

@Security 注释
------------------------

在控制器里，实施访问控制时，尽量使用@Security注释。位于动作上方的它们，不光容易理解，还容易替换。

在我们这个程序中，你需要使用 ``ROLE_ADMIN`` 授权，才能创建一个新贴子。
使用 ``@Security``时，代码会像下面这样：

.. code-block:: php

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;
    use Symfony\Component\Routing\Annotation\Route;
    // ...

    /**
     * 显示一个表单以创建新的Post实体。
     *
     * @Route("/new", name="admin_post_new")
     * @Security("is_granted('ROLE_ADMIN')")
     */
    public function new()
    {
        // ...
    }

在复杂安全限制中使用表达式
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果您的安全逻辑稍微复杂一些，可以在 ``@Security``中使用 :doc:`expression </components/expression_language>`。
在以下示例中，如果用户的电子邮件与 ``Post`` 对象上的 ``getAuthorEmail()`` 方法返回的值匹配，则用户才能访问控制器::

    use App\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;
    use Symfony\Component\Routing\Annotation\Route;

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("user.getEmail() == post.getAuthorEmail()")
     */
    public function edit(Post $post)
    {
        // ...
    }

请注意，这里我们使用了 `ParamConverter`_，
它会自动查询所需的 ``Post`` 对象并将其作为 ``$post`` 参数传递给控制器。
这使得在表达式中使用 ``post`` 变量成为可能。

这有一个主要缺点：注释中的表达式不能轻易地在应用的其他部分中重用。
想象一下，你想在模板中添加一个只有作者才能看到的链接。
现在，你需要使用Twig语法重复表达式代码：

.. code-block:: html+jinja

    {% if app.user and app.user.email == post.authorEmail %}
        <a href=""> ... </a>
    {% endif %}

最简单的解决方案 - 如果您的逻辑足够简单 - 就是在 ``Post`` 实体中添加一个新方法，检查给定用户是否是其作者::

    // src/Entity/Post.php
    // ...

    class Post
    {
        // ...

        /**
         * 给定的用户是否是本帖子的作者？
         *
         * @return bool
         */
        public function isAuthor(User $user = null)
        {
            return $user && $user->getEmail() === $this->getAuthorEmail();
        }
    }

现在，你可以在模板和安全表达式中重用此方法::

    use App\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;
    use Symfony\Component\Routing\Annotation\Route;

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("post.isAuthor(user)")
     */
    public function edit(Post $post)
    {
        // ...
    }

.. code-block:: html+jinja

    {% if post.isAuthor(app.user) %}
        <a href=""> ... </a>
    {% endif %}

.. _best-practices-directly-isGranted:
.. _checking-permissions-without-security:
.. _manually-checking-permissions:

不使用@Security来检查权限
--------------------------------------

上面用到 ``@Security`` 的例子，仅在我们使用 :ref:`ParamConverter <best-practices-paramconverter>` 时才能工作，
是它让表达式能够访问 ``post`` 变量。
如果你不使用它，或者使用其他更高级的用例，那么你始终可以在PHP中执行相同的安全检查::

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     */
    public function edit($id)
    {
        $post = $this->getDoctrine()
            ->getRepository(Post::class)
            ->find($id);

        if (!$post) {
            throw $this->createNotFoundException();
        }

        if (!$post->isAuthor($this->getUser())) {
            $this->denyAccessUnlessGranted('edit', $post);
        }
        // 不使用 “denyAccessUnlessGranted（）” 快捷方式的等效代码：
        //
        // use Symfony\Component\Security\Core\Exception\AccessDeniedException;
        // use Symfony\Component\Security\Core\Authorization\AuthorizationCheckerInterface
        //
        // ...
        //
        // public function __construct(AuthorizationCheckerInterface $authorizationChecker) {
        //      $this->authorizationChecker = $authorizationChecker;
        // }
        //
        // ...
        //
        // if (!$this->authorizationChecker->isGranted('edit', $post)) {
        //    throw $this->createAccessDeniedException();
        // }
        //
        // ...
    }

Security Voters
---------------

如果你的安全逻辑很复杂并且无法集中到像 ``isAuthor()`` 这样的方法中，那么你应该利用自定义选民(voter)。
这些比 :doc:`ACLs </security/acl>` 更容易，并且几乎在所有情况下都能为你提供所需的灵活性。

首先，要创建一个voter类。以下示例展示了与前例用过的 ``getAuthorEmail()`` 相同的逻辑::

    namespace App\Security;

    use App\Entity\Post;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Authorization\AccessDecisionManagerInterface;
    use Symfony\Component\Security\Core\Authorization\Voter\Voter;
    use Symfony\Component\Security\Core\User\UserInterface;

    class PostVoter extends Voter
    {
        const CREATE = 'create';
        const EDIT   = 'edit';

        private $decisionManager;

        public function __construct(AccessDecisionManagerInterface $decisionManager)
        {
            $this->decisionManager = $decisionManager;
        }

        protected function supports($attribute, $subject)
        {
            if (!in_array($attribute, [self::CREATE, self::EDIT])) {
                return false;
            }

            if (!$subject instanceof Post) {
                return false;
            }

            return true;
        }

        protected function voteOnAttribute($attribute, $subject, TokenInterface $token)
        {
            $user = $token->getUser();
            /** @var Post */
            $post = $subject; // $subject must be a Post instance, thanks to the supports method

            if (!$user instanceof UserInterface) {
                return false;
            }

            switch ($attribute) {
                // if the user is an admin, allow them to create new posts
                case self::CREATE:
                    if ($this->decisionManager->decide($token, ['ROLE_ADMIN'])) {
                        return true;
                    }

                    break;

                // if the user is the author of the post, allow them to edit the posts
                case self::EDIT:
                    if ($user->getEmail() === $post->getAuthorEmail()) {
                        return true;
                    }

                    break;
            }

            return false;
        }
    }

如果你使用 :ref:`default services.yaml configuration <service-container-services-load-example>`配置，
你的应用将自动配置你的安全选民，并通过 :ref:`autoconfigure <services-autoconfigure>` 将 ``AccessDecisionManagerInterface`` 实例注入其中。

现在，你可以在 ``@Security`` 注释中使用这个选民了::

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     * @Security("is_granted('edit', post)")
     */
    public function edit(Post $post)
    {
        // ...
    }

你也可以直接使用 ``security.authorization_checker`` 服务或更简单的通过控制器中的快捷方式使用它::

    /**
     * @Route("/{id}/edit", name="admin_post_edit")
     */
    public function edit($id)
    {
        $post = ...; // query for the post

        $this->denyAccessUnlessGranted('edit', $post);

        // use Symfony\Component\Security\Core\Exception\AccessDeniedException;
        // use Symfony\Component\Security\Core\Authorization\AuthorizationCheckerInterface
        //
        // ...
        //
        // public function __construct(AuthorizationCheckerInterface $authorizationChecker) {
        //      $this->authorizationChecker = $authorizationChecker;
        // }
        //
        // ...
        //
        // if (!$this->authorizationChecker->isGranted('edit', $post)) {
        //    throw $this->createAccessDeniedException();
        // }
        //
        // ...
    }

下一章: :doc:`/best_practices/web-assets`

.. _`ParamConverter`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`@Security annotation`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/security.html
.. _`FOSUserBundle`: https://github.com/FriendsOfSymfony/FOSUserBundle
.. _`libsodium`: https://pecl.php.net/package/libsodium
