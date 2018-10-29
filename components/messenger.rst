.. index::
   single: Messenger
   single: Components; Messenger

Messenger组件
=======================

    Messenger组件可帮助应用向/从其他应用或通过消息队列发送和接收消息。

    该组件受 Matthias Noback 一系列 `关于命令总线的博客`_ 和 `SimpleBus项目`_ 的启发。

.. seealso::

    本文介绍如何在任何PHP应用中将Messenger功能用作独立组件。
    阅读 :doc:`/messenger` 文档，了解如何在Symfony应用中使用它。

安装
------------

.. code-block:: terminal

    $ composer require symfony/messenger

或者，你可以克隆 `<https://github.com/symfony/messenger>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

概念
--------

.. image:: /_images/components/messenger/overview.png

**Sender**:
   负责序列化和发送消息到 *某些东西*。这些东西可以是消息代理(broker)或第三方API。

**Receiver**:
   负责反序列化并将消息转发给处理器。例如，这可以是消息队列拉取器或API端点。

**Handler**:
   责处理适用于该业务逻辑的消息。

总线
-----

总线用于派遣消息。总线的行为在其有序的中间件堆栈中。该组件附带了一组可以使用的中间件。

在Symfony的FrameworkBundle中使用消息总线时，会为你配置以下中间件：

#. ``LoggingMiddleware`` (记录消息的处理)
#. ``SendMessageMiddleware`` (启用异步处理)
#. ``HandleMessageMiddleware`` (调用注册的处理器)

例如::

    use App\Message\MyMessage;
    use Symfony\Component\Messenger\MessageBus;
    use Symfony\Component\Messenger\Handler\Locator\HandlerLocator;
    use Symfony\Component\Messenger\Middleware\HandleMessageMiddleware;

    $bus = new MessageBus([
        new HandleMessageMiddleware(new HandlerLocator([
            MyMessage::class => $handler,
        ])),
    ]);

    $bus->dispatch(new MyMessage(/* ... */));

.. note::

    每个中间件都需要实现 ``MiddlewareInterface``。

处理器
--------

一旦发送到总线，消息将由“消息处理器”处理。
消息处理器是可调用的PHP（即函数或类的实例），它将对你的消息执行所需的处理::

    namespace App\MessageHandler;

    use App\Message\MyMessage;

    class MyMessageHandler
    {
       public function __invoke(MyMessage $message)
       {
           // 消息处理...
       }
    }

向消息添加元数据（信封）
---------------------------------------

如果需要向消息添加元数据或某些配置，请将其与 :class:`Symfony\\Component\\Messenger\\Envelope` 类包装在一起。
例如，要设置消息通过传输层时使用的序列化组，请使用 ``SerializerConfiguration`` 信封::

    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\Transport\Serialization\SerializerConfiguration;

    $bus->dispatch(
        (new Envelope($message))->with(new SerializerConfiguration([
            'groups' => ['my_serialization_groups'],
        ]))
    );

目前，Symfony Messenger具有以下内置信封：

#. :class:`Symfony\\Component\\Messenger\\Transport\\Serialization\\SerializerConfiguration`,
   配置传输（transport）使用的序列化组。
#. :class:`Symfony\\Component\\Messenger\\Middleware\\Configuration\\ValidationConfiguration`,
   配置启用验证中间件时使用的验证组。
#. :class:`Symfony\\Component\\Messenger\\Asynchronous\\Transport\\ReceivedMessage`,
   一个内部项，用于标记(mark)从传输接收的消息。

你可以通过实现 :class:`Symfony\\Component\\Messenger\\EnvelopeAwareInterface` 标记来接收信封，
而不是直接处理中间件中的消息，如下所示::

    use Symfony\Component\Messenger\Asynchronous\Transport\ReceivedMessage;
    use Symfony\Component\Messenger\Middleware\MiddlewareInterface;
    use Symfony\Component\Messenger\EnvelopeAwareInterface;

    class MyOwnMiddleware implements MiddlewareInterface, EnvelopeAwareInterface
    {
        public function handle($envelope, callable $next)
        {
            // $envelope 在这是一个 `Envelope` 对象，因为这个中间件实现了 EnvelopeAwareInterface 接口。

            if (null !== $envelope->get(ReceivedMessage::class)) {
                // 刚刚收到消息...

                // 例如，你可以添加另一个项目
                $envelope = $envelope->with(new AnotherEnvelopeItem(/* ... */));
            }

            return $next($envelope);
        }
    }

如果刚刚收到消息（即具有 `ReceivedMessage`  项），上面的示例将使用额外的信封将消息转发到下一个中​​间件。
你可以通过实现 :class:`Symfony\\Component\\Messenger\\EnvelopeAwareInterface` 来创建自己的项目。

传输系统
----------

要发送和接收消息，你必须配置一个传输系统。该系统将负责与你的消息代理(broker)或第三方进行通信。

自定义发件人
~~~~~~~~~~~~~~~

使用 ``SenderInterface``，你可以创建自己的消息发件人。
想象一下，你已经有一个 ``ImportantAction`` 消息通过消息总线并由处理器处理。
现在，你还希望将此消息作为电子邮件发送。

首先，创建发件人::

    namespace App\MessageSender;

    use App\Message\ImportantAction;
    use Symfony\Component\Messenger\Transport\SenderInterface;
    use Symfony\Component\Messenger\Envelope;

    class ImportantActionToEmailSender implements SenderInterface
    {
       private $mailer;
       private $toEmail;

       public function __construct(\Swift_Mailer $mailer, string $toEmail)
       {
           $this->mailer = $mailer;
           $this->toEmail = $toEmail;
       }

       public function send(Envelope $envelope)
       {
           $message = $envelope->getMessage();

           if (!$message instanceof ImportantAction) {
               throw new \InvalidArgumentException(sprintf('This transport only supports "%s" messages.', ImportantAction::class));
           }

           $this->mailer->send(
               (new \Swift_Message('Important action made'))
                   ->setTo($this->toEmail)
                   ->setBody(
                       '<h1>Important action</h1><p>Made by '.$message->getUsername().'</p>',
                       'text/html'
                   )
           );
       }
    }

自定义收件人
~~~~~~~~~~~~~~~~~

收件人负责从一个源接收消息并将它们调度给应用。

想象一下，你已经使用 ``NewOrder`` 消息在应用中处理了一些“订单” 。
现在，你希望与第三方或旧版应用集成，但不能使用API​​，而是需要使用带有新订单的共享CSV文件。

你将阅读此CSV文件并调度一个 ``NewOrder`` 消息。
你需要做的就是编写自定义CSV收件人，Symfony将完成剩下的工作。

首先，创建收件人::

    namespace App\MessageReceiver;

    use App\Message\NewOrder;
    use Symfony\Component\Messenger\Transport\ReceiverInterface;
    use Symfony\Component\Serializer\SerializerInterface;
    use Symfony\Component\Messenger\Envelope;

    class NewOrdersFromCsvFile implements ReceiverInterface
    {
       private $serializer;
       private $filePath;

       public function __construct(SerializerInterface $serializer, string $filePath)
       {
           $this->serializer = $serializer;
           $this->filePath = $filePath;
       }

       public function receive(callable $handler): void
       {
           $ordersFromCsv = $this->serializer->deserialize(file_get_contents($this->filePath), 'csv');

           foreach ($ordersFromCsv as $orderFromCsv) {
               $order = new NewOrder($orderFromCsv['id'], $orderFromCsv['account_id'], $orderFromCsv['amount']);

               $handler(new Envelope($order));
           }
       }

       public function stop(): void
       {
           // noop
       }
    }

同一总线上的收件人和发件人
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

为了允许在同一总线上发送和接收消息并防止无限循环，消息总线将添加一个
:class:`Symfony\\Component\\Messenger\\Asynchronous\\Transport\\ReceivedMessage` 信封到消息信封项，
:class:`Symfony\\Component\\Messenger\\Asynchronous\\Middleware\\SendMessageMiddleware`
中间件将知道它不应将这些消息再路由回一个传输系统。

.. _关于命令总线的博客: https://matthiasnoback.nl/tags/command%20bus/
.. _SimpleBus项目: http://simplebus.io
