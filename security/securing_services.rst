.. index::
   single: Security; Securing any service
   single: Security; Securing any method

如何保护应用中的任何服务或方法
=======================================================

在安全文档中，你学习了如何通过快捷方法 :ref:`保护控制器 <security-securing-controller>`。

但是，你可以通过在代码中注入 ``Security`` 服务来在 *任何* 位置验证访问。
例如，假设你有一个 ``SalesReportManager`` 服务，
但你希望仅为具有 ``ROLE_SALES_ADMIN`` 角色的用户提供额外的详细信息：

.. code-block:: diff

    // src/Newsletter/NewsletterManager.php

    // ...
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;
    + use Symfony\Component\Security\Core\Security;

    class SalesReportManager
    {
    +     private $security;

    +     public function __construct(Security $security)
    +     {
    +         $this->security = $security;
    +     }

        public function sendNewsletter()
        {
            $salesData = [];

    +         if ($this->security->isGranted('ROLE_SALES_ADMIN')) {
    +             $salesData['top_secret_numbers'] = rand();
    +         }

            // ...
        }

        // ...
    }

如果你使用的是
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，
得益于自动装配和 ``Security`` 类型约束，Symfony会自动将 ``security.helper`` 传递给你的服务。

你也可以使用一个较底层的
:class:`Symfony\\Component\\Security\\Core\\Authorization\\AuthorizationCheckerInterface`
服务。它的功能与 ``Security`` 相同，但允许你类型约束更具体的接口。
