.. index::
   single: Events; Create listener
   single: Create subscriber

事件
==========================

在symfony程序执行期间，大量的事件通知会被触发。你的程序可以监听这些通知，并执行任意代码作为回应。

Symfony在处理HTTP请求时触发多个与 :doc:`内核相关的事件 </reference/events>`。
第三方Bundle也可以调度事件，甚你至可以从自己的代码中调度 :doc:`自定义事件 </components/event_dispatcher>` 。

本文展示的所有例子，考虑到一致性，使用了相同的 ``KernelEvents::EXCEPTION`` 事件。
在你自己的程序中，你可以使用任何事件，甚至在同一订阅器中混合若干事件。

创建一个事件监听器
--------------------------

监听一个事件最常用的方式是注册一个 **事件监听器**::

    // src/EventListener/ExceptionListener.php
    namespace App\EventListener;

    use Symfony\Component\HttpKernel\Event\GetResponseForExceptionEvent;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpKernel\Exception\HttpExceptionInterface;

    class ExceptionListener
    {
        public function onKernelException(GetResponseForExceptionEvent $event)
        {
            // 你可以从接收到的事件中，取得异常对象
            $exception = $event->getException();
            $message = sprintf(
                'My Error says: %s with code: %s',
                $exception->getMessage(),
                $exception->getCode()
            );

            // 自定义响应对象，来显示该异常的细节
            $response = new Response();
            $response->setContent($message);

            // HttpExceptionInterface 是一个特殊类型的异常，持有状态码和标头的细节
            if ($exception instanceof HttpExceptionInterface) {
                $response->setStatusCode($exception->getStatusCode());
                $response->headers->replace($exception->getHeaders());
            } else {
                $response->setStatusCode(Response::HTTP_INTERNAL_SERVER_ERROR);
            }

            // 发送修改后的响应对象到事件中
            $event->setResponse($response);
        }
    }

.. tip::

    每一个事件，都要接收“类型略有不同”的 ``$event`` 对象。
    对于 ``kernel.exception`` 事件，这个对象是
    :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseForExceptionEvent`。
    查看 :doc:`Symfony事件参考 </reference/events>`，了解每个事件提供的对象类型。

现在，类被创建了，你需要把它注册成服务，然后通过使用一个特殊的“标签”，
告诉Symfony这是一个针对 ``kernel.exception`` 事件的“监听器”：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\EventListener\ExceptionListener:
                tags:
                    - { name: kernel.event_listener, event: kernel.exception }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\EventListener\ExceptionListener">
                    <tag name="kernel.event_listener" event="kernel.exception" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\EventListener\ExceptionListener;

        $container
            ->autowire(ExceptionListener::class)
            ->addTag('kernel.event_listener', array('event' => 'kernel.exception'))
        ;

Symfony遵循以下逻辑来决定在事件监听器类中执行哪个方法：

#. 如果 ``kernel.event_listener`` 标签定义了 ``method`` 属性，那就执行该方法的名称;
#. 如果没有定义 ``method`` 属性，则尝试执行名称为 ``on`` + “驼峰式命名的事件名称”的方法
   （例如， ``kernel.exception`` 事件的 ``onKernelException()`` 方法）;
#. 如果该方法没有定义，请尝试执行 ``__invoke()`` 魔术方法（这使得事件监听器器可调用）;
#. 如果连 ``_invoke()`` 方法都没有定义，则抛出异常。

.. versionadded:: 4.1
    Symfony 4.1中引入了 ``__invoke()`` 方法对创建可调用事件监听器的支持。

.. note::

    ``kernel.event_listener`` 标签有一个名为 ``priority`` 的可选属性，它是一个正整数或负整数，默认值为 ``0``，
    它控制执行监听器的顺序（数字越大，监听器越早执行）。
    当你需要保证一个侦听器在另一个侦听器之前执行时，这非常有用。
    Symfony内部的监听器的优先级通常在 ``-255`` 到 ``255`` 之间，
    但你自己的监听器可以使用任何正整数或负整数。

.. _events-subscriber:

创建一个事件订阅器
----------------------------

另一种监听事件的方式是通过 **事件订阅器**，它是一个定义了一或多个方法的类，用于监听一或多个事件。
它同事件监听器的主要区别在于，订阅器始终知道它们正在监听的事件是哪一个。

在一个给定的订阅器中，不同的方法可以监听同一个事件。
方法被执行时的顺序，通过每一个方法中的 ``priority`` 参数来定义（数字越大，则方法越早被调用）。
要了解更多关于订阅器的内容，参考 :doc:`/components/event_dispatcher`。

下例展示了一个定义了若干方法的事件订阅器，它监听的是同一个 ``kernel.exception`` 事件::

    // src/EventSubscriber/ExceptionSubscriber.php
    namespace App\EventSubscriber;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\HttpKernel\Event\GetResponseForExceptionEvent;
    use Symfony\Component\HttpKernel\KernelEvents;

    class ExceptionSubscriber implements EventSubscriberInterface
    {
        public static function getSubscribedEvents()
        {
            // 返回被订阅的事件，以及它们的方法和属性
            return array(
               KernelEvents::EXCEPTION => array(
                   array('processException', 10),
                   array('logException', 0),
                   array('notifyException', -10),
               )
            );
        }

        public function processException(GetResponseForExceptionEvent $event)
        {
            // ...
        }

        public function logException(GetResponseForExceptionEvent $event)
        {
            // ...
        }

        public function notifyException(GetResponseForExceptionEvent $event)
        {
            // ...
        }
    }

仅此而已！你的 ``services.yaml`` 文件应该已经配置为从 ``EventSubscriber`` 目录中加载服务。
Symfony会负责其余的工作。

.. _ref-event-subscriber-configuration:

.. tip::

    如果在抛出异常时\ *没有*\调用你的方法，请仔细检查你是否启用了 :ref:`自动配置 <services-autoconfigure>`，
    并有从 ``EventSubscriber`` 目录 :ref:`加载服务 <service-container-services-load-example>`。
    你也可以手动添加 ``kernel.event_subscriber`` 标签。

请求事件, 检查类型
------------------------------

一个单一页面，可以产生若干次请求（一个主请求[master request]，然后是多个子请求[sub-requests]，
典型的像是 :doc:`/templating/embedding_controllers`）。
对于Symfony核心事件，你可能需要检查一下，看这个事件是一个“主”请求还是一个“子”请求::

    // src/EventListener/RequestListener.php
    namespace App\EventListener;

    use Symfony\Component\HttpKernel\Event\GetResponseEvent;
    use Symfony\Component\HttpKernel\HttpKernel;
    use Symfony\Component\HttpKernel\HttpKernelInterface;

    class RequestListener
    {
        public function onKernelRequest(GetResponseEvent $event)
        {
            if (!$event->isMasterRequest()) {
                // 如果不是主请求，就什么也不做
                return;
            }

            // ...
        }
    }

某些特定的行为，例如检查\ *真实*\请求的信息，可能不需要在子请求减去器上完成。

.. _events-or-subscribers:

监听器 VS 订阅器
------------------------

监听器和订阅器可以在同一个应用中模糊地使用。
决定使用哪一种，通常由个人口味决定。但是，每种都有各自的优点：

* **订阅器易于复用**，因为与事件有关的内容存在于类中，而不是存在于服务定义中。
  这就是Symfony内部使用订阅器的原因；
* **监听器更灵活**，因为Bundle可以基于配置中的某些“选项值”有条件地开启或关闭它们。

调试事件监听器
-------------------------

你可以使用控制台找出在事件调度器中注册的监听器。
要显示所有事件及其监听器，请运行：

.. code-block:: terminal

    $ php bin/console debug:event-dispatcher

通过指定事件名称，你可以得到针对此特定事件进行注册的监听器：

.. code-block:: terminal

    $ php bin/console debug:event-dispatcher kernel.exception

扩展阅读
----------

.. toctree::
    :maxdepth: 1

    event_dispatcher/before_after_filters
    event_dispatcher/method_behavior
