.. index::
    single: Security; Creating a Custom Access Denied Handler

如何创建自定义的拒绝访问处理器
============================================

当你的应用抛出一个 ``AccessDeniedException`` 时，你可以使用一个服务来处理此异常以返回一个自定义响应。

首先，创建一个实现
:class:`Symfony\\Component\\Security\\Http\\Authorization\\AccessDeniedHandlerInterface`
的类。此接口定义了一个叫 ``handle()`` 的方法，你可以在其中实现拒绝当前用户访问时应执行的任何逻辑
（例如，发送邮件，记录消息或和往常一样返回自定义响应）::

    namespace App\Security;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;
    use Symfony\Component\Security\Http\Authorization\AccessDeniedHandlerInterface;

    class AccessDeniedHandler implements AccessDeniedHandlerInterface
    {
        public function handle(Request $request, AccessDeniedException $accessDeniedException)
        {
            // ...

            return new Response($content, 403);
        }
    }

如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么你已经完工了！Symfony会自动了解你的新服务。
然后，你可以在防火墙的下面配置它：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        firewalls:
            # ...

            main:
                # ...
                access_denied_handler: App\Security\AccessDeniedHandler

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <config>
            <firewall name="main">
                <access_denied_handler>App\Security\AccessDeniedHandler</access_denied_handler>
            </firewall>
        </config>

    .. code-block:: php

        // config/packages/security.php
        use App\Security\AccessDeniedHandler;

        $container->loadFromExtension('security', [
            'firewalls' => [
                'main' => [
                    // ...
                    'access_denied_handler' => AccessDeniedHandler::class,
                ],
            ],
        ]);

仅此而已！现在，你的服务将处理 ``main`` 防火墙下的代码抛出的任何 ``AccessDeniedException``。
