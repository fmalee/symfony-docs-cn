.. index::
   single: EventDispatcher

如何设置前置/后置过滤器
======================================

在Web应用开发中，某些逻辑需要在你的控制器动作之前或之后执行，以充当过滤器或钩子，这是很常见的场景。

一些web框架定义了一些类似 ``preExecute()`` 和 ``postExecute()`` 的方法，但Symfony没有这样的事情。
好消息是有一种更好的方法，就是使用
:doc:`EventDispatcher组件 </components/event_dispatcher>` 干扰“请求->响应”的过程。

令牌验证示例
------------------------

想象一下，你需要开发一个API，其中一些控制器是公有的，但其他控制器仅限于一个或一些客户端。
对于这些私有功能，你可以向客户提供一个令牌以便识别它们。

因此，在执行控制器动作之前，你需要检查该动作是否受限制。如果受限制，则需要验证提供的令牌。

.. note::

    请注意，为了简化本示例，将在配置中定义令牌，并且不会使用数据库设置和通过安全组件进行认证。

使用 ``kernel.controller`` 事件的前置过滤器
---------------------------------------------------

首先，将一些令牌配置定义为参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            tokens:
                client1: pass1
                client2: pass2

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="tokens" type="collection">
                    <parameter key="client1">pass1</parameter>
                    <parameter key="client2">pass2</parameter>
                </parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('tokens', array(
            'client1' => 'pass1',
            'client2' => 'pass2',
        ));

标记控制器以便检查
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``kernel.controller``（又名 ``KernelEvents::CONTROLLER``）监听器在控制器执行之前就获得 *每个* 请求的通知。
因此，首先，你需要一些方法来确定与请求匹配的控制器是否需要令牌验证。

一种干净简单的方法是创建一个空接口并让控制器实现它::

    namespace App\Controller;

    interface TokenAuthenticatedController
    {
        // ...
    }

实现此接口的控制器如下所示::

    namespace App\Controller;

    use App\Controller\TokenAuthenticatedController;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class FooController extends AbstractController implements TokenAuthenticatedController
    {
        // 需要认证的动作
        public function bar()
        {
            // ...
        }
    }

创建事件订阅器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

接下来，你需要创建一个事件监听器，它将保存你希望在控制器之前执行的逻辑。
如果你不熟悉事件监听器，可以在 :doc:`/event_dispatcher` 中了解有关它们的更多信息::

    // src/EventSubscriber/TokenSubscriber.php
    namespace App\EventSubscriber;

    use App\Controller\TokenAuthenticatedController;
    use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;
    use Symfony\Component\HttpKernel\Event\FilterControllerEvent;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\HttpKernel\KernelEvents;

    class TokenSubscriber implements EventSubscriberInterface
    {
        private $tokens;

        public function __construct($tokens)
        {
            $this->tokens = $tokens;
        }

        public function onKernelController(FilterControllerEvent $event)
        {
            $controller = $event->getController();

            /*
             * 传递的 $controller 可以是类或闭包。
             * 这在Symfony中并不常见，但可能会发生。
             * 如果它是一个类，它以数组格式传递
             */
            if (!is_array($controller)) {
                return;
            }

            if ($controller[0] instanceof TokenAuthenticatedController) {
                $token = $event->getRequest()->query->get('token');
                if (!in_array($token, $this->tokens)) {
                    throw new AccessDeniedHttpException('This action needs a valid token!');
                }
            }
        }

        public static function getSubscribedEvents()
        {
            return array(
                KernelEvents::CONTROLLER => 'onKernelController',
            );
        }
    }

仅此而已！你的 ``services.yaml`` 文件应该已经设置为从 ``EventSubscriber`` 目录加载服务。
Symfony负责其余的工作。你的 ``TokenSubscriber`` 上的 ``onKernelController()`` 方法将在每个请求上执行。
如果即将执行的控制器实现了 ``TokenAuthenticatedController``，则应用令牌认证。
这使你可以在任何所需的控制器上使用“前置”过滤器。

.. tip::

    如果你的订阅器 *未* 在每个请求上调用，请仔细检查你是否从 ``EventSubscriber``
    目录 :ref:`加载服务 <service-container-services-load-example>`
    并启用了 :ref:`自动配置 <services-autoconfigure>`。
    你也可以手动添加 ``kernel.event_subscriber`` 标签。

使用 ``kernel.response`` 事件的后置过滤器
------------------------------------------------

除了在控制器 *之前* 执行“钩子”之外，还可以添加在控制器 *之后* 执行的钩子。
对于此示例，假设你要将一个sha1哈希（使用salt的令牌）添加到已通过此令牌认证的所有响应中。

另一个核心Symfony事件 - 名为``kernel.response``（又名 ``KernelEvents::RESPONSE``）
- 在每次请求时都会收到通知，但是在控制器返回一个响应对象之后。
要创建“后置”监听器，请创建一个监听器类，并将其注册为此事件上的服务。

例如， 从前面的示例中获取 ``TokenSubscriber`` 并先在该请求的属性中记录该认证令牌。
这将作为此请求已进行令牌认证的基本标识::

    public function onKernelController(FilterControllerEvent $event)
    {
        // ...

        if ($controller[0] instanceof TokenAuthenticatedController) {
            $token = $event->getRequest()->query->get('token');
            if (!in_array($token, $this->tokens)) {
                throw new AccessDeniedHttpException('This action needs a valid token!');
            }

            // 将请求标记为已通过令牌认证
            $event->getRequest()->attributes->set('auth_token', $token);
        }
    }

现在，将该订阅器配置为监听另一个事件并添加 ``onKernelResponse()``。
这将在请求对象上查找 ``auth_token`` 标识，如果找到该标识，则在响应中设置一个自定义标头::

    // 在文件顶部添加新的use语句
    use Symfony\Component\HttpKernel\Event\FilterResponseEvent;

    public function onKernelResponse(FilterResponseEvent $event)
    {
        // 检查 onKernelController 是否将此标记为一个令牌“auth'ed”请求
        if (!$token = $event->getRequest()->attributes->get('auth_token')) {
            return;
        }

        $response = $event->getResponse();

        // 创建一个哈希并将其设置为一个响应头
        $hash = sha1($response->getContent().$token);
        $response->headers->set('X-CONTENT-HASH', $hash);
    }

    public static function getSubscribedEvents()
    {
        return array(
            KernelEvents::CONTROLLER => 'onKernelController',
            KernelEvents::RESPONSE => 'onKernelResponse',
        );
    }

仅此而已！``TokenSubscriber`` 将在执行每个控制器之前（``onKernelController()``）被通知，
并在控制器执行之后返回一个响应（``onKernelResponse()``）。
通过使特定的控制器实现 ``TokenAuthenticatedController`` 接口，你的监听器知道应该在哪些控制器上采取行动。
通过在请求的“attributes”包中存储一个值，``onKernelResponse()`` 方法知道应不应该添加一个额外的标头。
玩得开心！
