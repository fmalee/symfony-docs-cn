.. index::
   single: Security; Data Permission Voters

如何使用表决器检查用户权限
===========================================

安全表决器是检查权限的最细粒度(granular)的方式（例如“这个特定的用户可以编辑给定的项目吗？”）。
这篇文档将详细的解释表决器。

.. tip::

    请参阅 :doc:`授权 </components/security/authorization>` 文档，以便更深入地了解表决器。

Symfony如何使用表决器
-----------------------

为了使用表决器，你必须了解Symfony如何与他们合作的。
每次在Symfony的授权检查器上使用 ``isGranted()``
方法或在控制器（使用了授权检查器）中调用 ``denyAccessUnlessGranted()`` 时，都会调用所有的表决器。

最终，Symfony接受所有表决器的响应，并根据应用中定义的策略做出最终决定（允许或拒绝访问资源），
该策略可以是：``affirmative``、``consensus`` 或 ``unanimous``。

有关更多信息，请查看 :ref:`有关访问决策管理器的章节 <components-security-access-decision-manager>`。

表决器接口
-------------------

自定义表决器需要实现
:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`
或继承 :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\Voter`，
它们使得创建表决器更加容易::

    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Authorization\Voter\VoterInterface;

    abstract class Voter implements VoterInterface
    {
        abstract protected function supports($attribute, $subject);
        abstract protected function voteOnAttribute($attribute, $subject, TokenInterface $token);
    }

.. _how-to-use-the-voter-in-a-controller:

设置：检查控制器中的访问权限
------------------------------------------

假设你有一个 ``Post`` 对象，你需要决定当前用户是否可以 *编辑* 或 *查看* 该对象。
在你的控制器中，你将使用以下代码检查访问权限::

    // src/Controller/PostController.php
    // ...

    class PostController extends AbstractController
    {
        /**
         * @Route("/posts/{id}", name="post_show")
         */
        public function show($id)
        {
            // 获取一个POST对象 - e.g. 查询它
            $post = ...;

            // 检查“view”访问权限：调用所有表决器
            $this->denyAccessUnlessGranted('view', $post);

            // ...
        }

        /**
         * @Route("/posts/{id}/edit", name="post_edit")
         */
        public function edit($id)
        {
            // 获取一个POST对象 - e.g. 查询它
            $post = ...;

            // 检查“edit”访问权限：调用所有表决器
            $this->denyAccessUnlessGranted('edit', $post);

            // ...
        }
    }

``denyAccessUnlessGranted()`` 方法（以及 ``isGranted()`` 方法）会调出“表决器”系统。
现在，没有表决器会投票决定用户是否可以“查看”或“编辑”一个 ``Post``。
但你可以创建自己的表决器，使用你想要的任何逻辑来决定这一点。

创建自定义表决器
-------------------------

假设决定用户是否可以“查看”或“编辑”一个 ``Post`` 对象的逻辑非常复杂。
例如，一个 ``User`` 总是可以编辑或查看他们自己创建一个 ``Post``。
并且如果一个 ``Post`` 被标记为“公开”，任何人都可以查看它。这种情况下的表决器看起来像这样::

    // src/Security/PostVoter.php
    namespace App\Security;

    use App\Entity\Post;
    use App\Entity\User;
    use Symfony\Component\Security\Core\Authentication\Token\TokenInterface;
    use Symfony\Component\Security\Core\Authorization\Voter\Voter;

    class PostVoter extends Voter
    {
        // 这些字符串刚刚被发明：你可以使用任何东西
        const VIEW = 'view';
        const EDIT = 'edit';

        protected function supports($attribute, $subject)
        {
            // 如果该属性不是我们支持属性之一，则返回 false
            if (!in_array($attribute, [self::VIEW, self::EDIT])) {
                return false;
            }

            // 这个表决器只投票给 Post 对象
            if (!$subject instanceof Post) {
                return false;
            }

            return true;
        }

        protected function voteOnAttribute($attribute, $subject, TokenInterface $token)
        {
            $user = $token->getUser();

            if (!$user instanceof User) {
                // 用户必须已经登录; 如果没有，拒绝访问
                return false;
            }

            // 你知道 $subject 是一个 Post 对象，感谢 supports
            /** @var Post $post */
            $post = $subject;

            switch ($attribute) {
                case self::VIEW:
                    return $this->canView($post, $user);
                case self::EDIT:
                    return $this->canEdit($post, $user);
            }

            throw new \LogicException('This code should not be reached!');
        }

        private function canView(Post $post, User $user)
        {
            // 如果他们有编辑权限，就意味着有查看权限
            if ($this->canEdit($post, $user)) {
                return true;
            }

            // 假设Post对象会有一个 isPrivate() 方法来检查一个布尔类型的 $private 属性
            return !$post->isPrivate();
        }

        private function canEdit(Post $post, User $user)
        {
            // 这假设数据对象具有一个 getOwner() 方法来获取拥有此数据对象的用户的实体
            return $user === $post->getOwner();
        }
    }

仅此而已！表决器就完工了！接下来，:ref:`配置它 <declaring-the-voter-as-a-service>`。

回顾一下，这是预期的两种抽象方法：

``Voter::supports($attribute, $subject)``
    当调用 ``isGranted()`` （或 ``denyAccessUnlessGranted()``）时，
    第一个参数在此传递为 ``$attribute`` （例如 ``ROLE_USER``、``edit``），
    第二个参数（如果有的话）被传递为 ``$subject`` （例如 ``null``、一个 ``Post`` 对象）。
    你的工作是确定你的表决器是否应该对 ”attribute/subject” 组合进行投票。
    如果你返回 ``true``，``voteOnAttribute()`` 将被调用。
    否则，你的表决器就完成任务了：其他的表决器会处理这个问题。
    在此示例中，如果属性为 ``view`` 或 ``edit`` 且对象是一个 ``Post`` 实例，则返回 ``true`` 。

``voteOnAttribute($attribute, $subject, TokenInterface $token)``
    如果从 ``supports()`` 中返回 ``true``，则调用此方法。
    你的工作很简单：返回 ``true`` 来允许访问，返回  ``false`` 则表示拒绝访问。
    ``$token`` 可用于找到当前用户对象（如果有的话）。
    在此示例中，包含了所有复杂的业务逻辑以确定访问权限。

.. _declaring-the-voter-as-a-service:

配置表决器
---------------------

要将表决器注入安全层，你必须将其声明为服务并将其标记为 ``security.voter``。
但是，如果你使用的是
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，则会自动为你完成配置！
当你
:ref:`使用view/edit调用isGranted()并传递Post对象 <how-to-use-the-voter-in-a-controller>`
时，你的表决器将被执行然后你就可以控制访问权限了。

在表决器中检查角色
---------------------------------

如果你想从你的表决器 *内部* 调用 ``isGranted()`` 怎么办 -
例如你想看看当前用户是否有 ``ROLE_SUPER_ADMIN``。
这可以通过注入 :class:`Symfony\\Component\\Security\\Core\\Security` 到你的表决器来实现。
例如，你可以使用它来 *始终* 允许 ``ROLE_SUPER_ADMIN`` 用户的访问::

    // src/Security/PostVoter.php

    // ...
    use Symfony\Component\Security\Core\Security;

    class PostVoter extends Voter
    {
        // ...

        private $security;

        public function __construct(Security $security)
        {
            $this->security = $security;
        }

        protected function voteOnAttribute($attribute, $subject, TokenInterface $token)
        {
            // ...

            // ROLE_SUPER_ADMIN 可以做任何事情! 无比强大!
            if ($this->security->isGranted('ROLE_SUPER_ADMIN')) {
                return true;
            }

            // ... 所有的常规的表决器逻辑
        }
    }

如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么你就完工了！
Symfony将在实例化你的表决器时自动传递 ``security.helper`` 服务（得益于自动装配）。

.. _security-voters-change-strategy:

修改访问决策策略
-------------------------------------

通常情况下，只有一个表决器会在任何特定时间投票
（其余的将“弃权”，这意味着他们会从 ``supports()`` 中返回 ``false``）。
但理论上，你可以让多个表决器投票支持一个动作和对象。
例如，假设你有一个表决器检查用户是否是网站的成员，而另一个检查用户是否大于18岁。

为了处理这些情况，访问决策管理器使用一个访问策略。你可以根据需要进行配置。有三种策略可供选择：

``affirmative`` (默认)
    一旦有一个表决器授予访问权限，就会允许访问;

``consensus``
    如果授权访问的表决器比拒绝的更多，则允许访问;

``unanimous``
    仅在没有任何表决器拒绝访问的情况下授予访问权限。
    如果所有的表决器都放弃权票，则会基于 ``allow_if_all_abstain`` 配置选项（默认为 ``false``）做出决定。

在上面的场景中，两个表决器都应该授予访问权限，以便允许用户阅读帖子。
在这种情况下，默认的策略不再有效，而应该使用 ``unanimous``。你可以在安全配置中进行设置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            access_decision_manager:
                strategy: unanimous
                allow_if_all_abstain: false

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd"
        >

            <config>
                <access-decision-manager strategy="unanimous" allow-if-all-abstain="false"/>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', [
            'access_decision_manager' => [
                'strategy' => 'unanimous',
                'allow_if_all_abstain' => false,
            ],
        ]);
