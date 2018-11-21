.. index::
   single: Security, Authorization

授权
=============

当任何认证提供器（请参阅 :ref:`authentication_providers`）验证过“仍未认证的”令牌后，将返回一个已认证的令牌。
认证监听器应使用它的
:method:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\Storage\\TokenStorageInterface::setToken`
方法直接设置此令牌到
:class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\Storage\\TokenStorageInterface` 中。

从那时起，该用户已经被认证，即被识别。
现在，应用的其他部分可以使用此令牌来决定用户是否可以请求某个URI，或者修改某个对象。
该决策将由一个 :class:`Symfony\\Component\\Security\\Core\\Authorization\\AccessDecisionManagerInterface`
实例给出。

授权的决策将始终基于以下几点：

* 当前令牌
    例如，令牌的
    :method:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface::getRoles`
    方法可用于检索当前用户的角色（例如 ``ROLE_SUPER_ADMIN``），或者基于令牌的类做出一个决策。
* 一组属性
    每个属性代表该用户应具有的特定权限，例如 ``ROLE_ADMIN``，以确保该用户是管理员。
* 一个对象（可选）
    需要检查访问控制的任何对象，如一篇文章或一个评论对象。

.. _components-security-access-decision-manager:

访问决策管理器
-----------------------

由于决定一个用户是否被授权执行某项行动可能是一个复杂的过程，标准的
:class:`Symfony\\Component\\Security\\Core\\Authorization\\AccessDecisionManager`
本身将依赖于多个表决器，并根据其收到的所有投票（正面、负面或中立）来作出最终裁决。它承认以下几种策略：

``affirmative`` (默认)
    一旦有一个表决器授予访问权限，就授予访问权限;

``consensus``
    如果授予访问权限的表决器多于拒绝访问的表决器，则授予访问权限;

``unanimous``
    只有在没有表决器拒绝访问的情况下，才授予访问权限;

.. code-block:: php

    use Symfony\Component\Security\Core\Authorization\AccessDecisionManager;

    // Symfony\Component\Security\Core\Authorization\Voter\VoterInterface 的实例
    $voters = array(...);

    // "affirmative", "consensus", "unanimous" 之一
    $strategy = ...;

    // 是否在所有表决器弃权时授权访问
    $allowIfAllAbstainDecisions = ...;

    // 是否在不占多数时授予访问权限（仅适用于 "consensus" 策略）
    $allowIfEqualGrantedDeniedDecisions = ...;

    $accessDecisionManager = new AccessDecisionManager(
        $voters,
        $strategy,
        $allowIfAllAbstainDecisions,
        $allowIfEqualGrantedDeniedDecisions
    );

.. seealso::

    你可以在 :ref:`配置 <security-voters-change-strategy>` 中更改此默认策略。

表决器
------

表决器是一个
:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`
实例，这意味着它们需要实现一些允许决策管理器使用它们的方法：

``vote(TokenInterface $token, $object, array $attributes)``
    这个方法将进行实际投票并返回一个等于
    :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\VoterInterface`
    中的一个类常量的值，即 ``VoterInterface::ACCESS_GRANTED``、
    ``VoterInterface::ACCESS_DENIED`` 或 ``VoterInterface::ACCESS_ABSTAIN``;

安全组件包含一些涵盖了许多用例的标准表决器：

AuthenticatedVoter
~~~~~~~~~~~~~~~~~~

:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\AuthenticatedVoter`
表决器支持 ``IS_AUTHENTICATED_FULLY``、``IS_AUTHENTICATED_REMEMBERED``
和 ``IS_AUTHENTICATED_ANONYMOUSLY``
属性，并基于认证的当前级别授予访问权限，即，该用户是完全认证还是仅根据“记住我”的cookie认证，甚至是匿名认证？

.. code-block:: php

    use Symfony\Component\Security\Core\Authentication\AuthenticationTrustResolver;
    use Symfony\Component\Security\Core\Authentication\Token\AnonymousToken;
    use Symfony\Component\Security\Core\Authentication\Token\RememberMeToken;

    $trustResolver = new AuthenticationTrustResolver(AnonymousToken::class, RememberMeToken::class);

    $authenticatedVoter = new AuthenticatedVoter($trustResolver);

    // Symfony\Component\Security\Core\Authentication\Token\TokenInterface 的实例
    $token = ...;

    // 任何对象
    $object = ...;

    $vote = $authenticatedVoter->vote($token, $object, array('IS_AUTHENTICATED_FULLY'));

RoleVoter
~~~~~~~~~

:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\RoleVoter`
支持以 ``ROLE_`` 开头的属性，如果所需的 ``ROLE_*`` 属性可以在令牌的
:method:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface::getRoles`
方法中找到，则授予该用户访问权限::

    use Symfony\Component\Security\Core\Authorization\Voter\RoleVoter;

    $roleVoter = new RoleVoter('ROLE_');

    $roleVoter->vote($token, $object, array('ROLE_ADMIN'));

RoleHierarchyVoter
~~~~~~~~~~~~~~~~~~

:class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\RoleHierarchyVoter`
继承 :class:`Symfony\\Component\\Security\\Core\\Authorization\\Voter\\RoleVoter`
并提供一些额外的功能：它知道如何处理一个角色的层级。
举例来说，一个 ``ROLE_SUPER_ADMIN`` 角色可以具有 ``ROLE_ADMIN`` 和 ``ROLE_USER``
子角色，这样当某个对象需要用户有 ``ROLE_ADMIN`` 角色时，它将被允许访问，因为它不仅有
``ROLE_SUPER_ADMIN`` 角色，事实上还拥有 ``ROLE_ADMIN`` 角色::

    use Symfony\Component\Security\Core\Authorization\Voter\RoleHierarchyVoter;
    use Symfony\Component\Security\Core\Role\RoleHierarchy;

    $hierarchy = array(
        'ROLE_SUPER_ADMIN' => array('ROLE_ADMIN', 'ROLE_USER'),
    );

    $roleHierarchy = new RoleHierarchy($hierarchy);

    $roleHierarchyVoter = new RoleHierarchyVoter($roleHierarchy);

.. note::

    当你创建自己的表决器时，你可以使用它的构造函数来注入一个决策所需的任何依赖。

Roles
-----

角色是表达用户具有的某种权利的对象。
唯一的要求是他们必须定义一个返回代表角色本身的字符串的 ``getRole()`` 方法。
为此，你可以选择继承默认 :class:`Symfony\\Component\\Security\\Core\\Role\\Role`
，该类的 ``getRole()`` 方法中会返回其构造函数的第一个参数::

    use Symfony\Component\Security\Core\Role\Role;

    $role = new Role('ROLE_ADMIN');

    // 显示 'ROLE_ADMIN'
    var_dump($role->getRole());

.. note::

    大多数认证令牌都是继承自
    :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\AbstractToken`
    ，这意味着赋值到它的构造函数的角色将自动从字符串转换为这些简单的 ``Role`` 对象。

使用决策管理器
--------------------------

访问监听器
~~~~~~~~~~~~~~~~~~~

可以在请求中的任何点中使用访问决策管理器来决定当前用户是否有权访问一个给定资源。
基于一个URL模式的来限制访问的一种可选但有用的方法是
:class:`Symfony\\Component\\Security\\Http\\Firewall\\AccessListener`
，它是与防火墙映射（请参阅 :ref:`firewall`）相匹配的每个请求都会触发的防火墙监听器（请参阅 :ref:`firewall_listeners`）之一。

它使用一个访问映射（应是 :class:`Symfony\\Component\\Security\\Http\\AccessMapInterface`
的一个实例）来完成工作，而该映射包含一个请求匹配器和一个当前用户访问应用所需的相应的属性集::

    use Symfony\Component\Security\Http\AccessMap;
    use Symfony\Component\HttpFoundation\RequestMatcher;
    use Symfony\Component\Security\Http\Firewall\AccessListener;

    $accessMap = new AccessMap();
    $requestMatcher = new RequestMatcher('^/admin');
    $accessMap->add($requestMatcher, array('ROLE_ADMIN'));

    $accessListener = new AccessListener(
        $securityContext,
        $accessDecisionManager,
        $accessMap,
        $authenticationManager
    );

授权检查器
~~~~~~~~~~~~~~~~~~~~~

也可以通过
:class:`Symfony\\Component\\Security\\Core\\Authorization\\AuthorizationChecker` 类的
:method:`Symfony\\Component\\Security\\Core\\Authorization\\AuthorizationChecker::isGranted`
方法将访问决策管理器用于应用的其他部分。
对该方法的调用会直接将问题委托给访问对应的决策管理器::

    use Symfony\Component\Security\Core\Authorization\AuthorizationChecker;
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;

    $authorizationChecker = new AuthorizationChecker(
        $tokenStorage,
        $authenticationManager,
        $accessDecisionManager
    );

    if (!$authorizationChecker->isGranted('ROLE_ADMIN')) {
        throw new AccessDeniedException();
    }
