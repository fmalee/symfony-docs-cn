.. index::
   single: Expressions in the Framework

安全性：带表达式的复杂访问控制
==================================================

.. seealso::

    处理复杂授权规则的最佳解决方案是使用 :doc:`表决器系统 </security/voters>`。

除了像 ``ROLE_ADMIN`` 之类的角色之外，``isGranted()`` 方法还接受一个
:class:`Symfony\\Component\\ExpressionLanguage\\Expression` 对象::

    use Symfony\Component\ExpressionLanguage\Expression;
    // ...

    public function index()
    {
        $this->denyAccessUnlessGranted(new Expression(
            '"ROLE_ADMIN" in roles or (not is_anonymous() and user.isSuperAdmin())'
        ));

        // ...
    }

在此示例中，如果当前用户具有 ``ROLE_ADMIN`` 或者当前用户对象的
``isSuperAdmin()`` 方法返回 ``true``，则将授予访问权限
（注意：你的 ``User`` 对象可能并没有 ``isSuperAdmin()`` 方法，该方法是针对此示例发明的）。

这里使用了一个表达式，你可以了解有关表达式语言语法的更多信息，请参阅
:doc:`/components/expression_language/syntax`。

.. _security-expression-variables:

在表达式中，你可以访问许多变量：

``user``
    用户对象（如果未经过身份验证，则为 ``anon`` 字符串）。
``roles``
    用户拥有的角色数组，包括 :ref:`角色层级 <security-role-hierarchy>`
    但不包括 ``IS_AUTHENTICATED_*`` 属性（请参阅下面的函数）。
``object``
    被作为第二个参数传递到 ``isGranted()`` 的对象（如果有的话）。
``token``
    令牌对象。
``trust_resolver``
    :class:`Symfony\\Component\\Security\\Core\\Authentication\\AuthenticationTrustResolverInterface`，
    对象：你可能会使用下面的 ``is_*()`` 函数来代替。

此外，你可以访问表达式中的许多函数：

``is_authenticated``
    如果用户通过“记住我”或“完全”认证来完成了认证，则返回 ``true``，即如果用户“已经登录”，则返回true。
``is_anonymous``
    等同于在 ``isGranted()`` 函数中使用的 ``IS_AUTHENTICATED_ANONYMOUSLY``。
``is_remember_me``
    与 ``IS_AUTHENTICATED_REMEMBERED`` 相似，但不相等，见下文。
``is_fully_authenticated``
    与 ``IS_AUTHENTICATED_FULLY`` 相似，但不相等，见下文。
``is_granted``
    检查用户是否具有给定的权限。可选的接收已检查过权限的对象为第二个参数。
    它等同于使用授权检查器服务中的 :doc:`isGranted() 方法 </security/securing_services>`。

.. sidebar:: ``is_remember_me`` 不同于 ``IS_AUTHENTICATED_REMEMBERED`` 检查

    ``is_remember_me()`` 和 ``is_fully_authenticated()``
    函数与使用 ``isGranted()`` 函数时的 ``IS_AUTHENTICATED_REMEMBERED`` 和 ``IS_AUTHENTICATED_FULLY`` *相似*，
    但他们 *不* 一样。以下控制器代码段显示了该差异::

        use Symfony\Component\ExpressionLanguage\Expression;
        use Symfony\Component\Security\Core\Authorization\AuthorizationCheckerInterface;
        // ...

        public function index(AuthorizationCheckerInterface $authorizationChecker)
        {
            $access1 = $authorizationChecker->isGranted('IS_AUTHENTICATED_REMEMBERED');

            $access2 = $authorizationChecker->isGranted(new Expression(
                'is_remember_me() or is_fully_authenticated()'
            ));
        }

    在这里，``$access1`` and ``$access2`` 将是相同的值。
    与 ``IS_AUTHENTICATED_REMEMBERED``、``IS_AUTHENTICATED_FULLY`` 的行为不同，
    ``is_remember_me()`` 函数 *仅* 返回true(如果用户通过“记住我”的cookie进行认证的话)。
    ``is_fully_authenticated`` 也 *仅* 返回true(如果用户是在此会话期间实际登录的话)。

扩展阅读
----------

* :doc:`/service_container/expression_language`
* :doc:`/reference/constraints/Expression`
