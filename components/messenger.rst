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

.. include:: /components/require_autoload.rst.inc

概念
--------

.. raw:: html

    <object data="../_images/components/messenger/overview.svg" type="image/svg+xml"></object>

**Sender**:
   负责序列化和发送消息到 *某些东西*。这些东西可以是消息代理(broker)或第三方API。

**Receiver**:
   负责检索、反序列化并将消息转发给处理器。例如，这可以是消息队列拉取器或API端点。

**Handler**:
   负责处理适用于该业务逻辑的消息。
   这些处理器将被 ``HandleMessageMiddleware`` 中间件调用。

**Middleware**:
  中间件可以在通过总线派遣时访问消息及其封装器（信封）。
  字面意思是“中间的软件”，它们与应用的核心问题（业务逻辑）无关。
  相反，它们是贯穿整个应用并影响整个消息总线的交叉(cross)关注点。
  例如：记录日志，验证一个消息，启动事务，...
  他们还负责调用链中的下一个中间件，这意味着他们可以调整信封，添加邮票或甚至替换它，以及打断中间件链。

**Envelope**
  特定于Messenger的概念，它通过将消息封装到消息总线内，从而在消息总线内部提供充分的灵活性，允许通过
  *envelope stamps* 在内部添加有用的信息。

**Envelope Stamps**
  需要附加到你的消息的信息片段：
  用于传输的序列化器上下文，标识接收到的消息的标记，或你的中间件或传输层可以使用的任何类型的元数据。

总线
-----

总线用于调度消息。总线的行为存在于其有序的中间件堆栈中。该组件附带了一组可以使用的中间件。

在Symfony的FrameworkBundle中使用消息总线时，会为你配置以下中间件：

#. :class:`Symfony\\Component\\Messenger\\Middleware\\LoggingMiddleware` (记录消息的处理)
#. :class:`Symfony\\Component\\Messenger\\Middleware\\SendMessageMiddleware` (启用异步处理)
#. :class:`Symfony\\Component\\Messenger\\Middleware\\HandleMessageMiddleware` (调用注册的处理器)

.. deprecated:: 4.3

    自Symfony 4.3起，``LoggingMiddleware`` 已被弃用，并将在5.0中删除。
    可以将日志器传递到 ``SendMessageMiddleware`` 来代替。

例如::

    use App\Message\MyMessage;
    use Symfony\Component\Messenger\Handler\HandlersLocator;
    use Symfony\Component\Messenger\MessageBus;
    use Symfony\Component\Messenger\Middleware\HandleMessageMiddleware;

    $bus = new MessageBus([
        new HandleMessageMiddleware(new HandlersLocator([
            MyMessage::class => ['dummy' => $handler],
        ])),
    ]);

    $bus->dispatch(new MyMessage(/* ... */));

.. note::

    每个中间件都需要实现 :class:`Symfony\\Component\\Messenger\\Middleware\\MiddlewareInterface`。

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

如果需要向消息添加元数据或某些配置，请使用 :class:`Symfony\\Component\\Messenger\\Envelope`
类将其封装并添加邮票(stamp)。例如，要设置消息通过传输层时使用的序列化组，请使用
``SerializerStamp`` 邮票::

    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\Stamp\SerializerStamp;

    $bus->dispatch(
        (new Envelope($message))->with(new SerializerStamp([
            'groups' => ['my_serialization_groups'],
        ]))
    );

目前，Symfony Messenger具有以下内置信封邮票：

#. :class:`Symfony\\Component\\Messenger\\Stamp\\SerializerStamp`，
   配置传输（transport）使用的序列化组。
#. :class:`Symfony\\Component\\Messenger\\Stamp\\ValidationStamp`，
   配置启用验证中间件时使用的验证组。
#. :class:`Symfony\\Component\\Messenger\\Stamp\\ReceivedStamp`，
   一个内部邮票，用于标记(mark)从传输接收的消息。
#. :class:`Symfony\\Component\\Messenger\\Stamp\\SentStamp`，
   一个用于标记由特定发送方发送的消息的邮票，允许从
   :class:`Symfony\\Component\\Messenger\\Transport\\Sender\\SendersLocator`
   访问该发送方的FQCN和别名（如果可用的话）。
#. :class:`Symfony\\Component\\Messenger\\Stamp\\HandledStamp`，
   一个用于标记由特定处理器处理的消息的邮票。允许从
   :class:`Symfony\\Component\\Messenger\\Handler\\HandlersLocator`
   访问该处理器的返回值、可调用名称及别名（如果可用的话）。

在你接收信封的中间件中，不直接处理消息。相反，你可以检查该信封内容及其邮票，或添加任何邮票::

    use App\Message\Stamp\AnotherStamp;
    use Symfony\Component\Messenger\Middleware\MiddlewareInterface;
    use Symfony\Component\Messenger\Middleware\StackInterface;
    use Symfony\Component\Messenger\Stamp\ReceivedStamp;

    class MyOwnMiddleware implements MiddlewareInterface
    {
        public function handle(Envelope $envelope, StackInterface $stack): Envelope
        {
            if (null !== $envelope->last(ReceivedStamp::class)) {
                // 刚刚收到消息...

                // 例如，你可以添加另一个邮票
                $envelope = $envelope->with(new AnotherStamp(/* ... */));
            }

            return $stack->next()->handle($envelope, $stack);
        }
    }

*如果* 刚收到消息（即至少有一个 ``ReceivedStamp``
邮票），上面的例子将把消息转发到下一个带有附加标记的中间件。你可以通过实现
:class:`Symfony\\Component\\Messenger\\Stamp\\StampInterface` 来创建自己的邮票。

如果要检查一个信封上的所有邮票，请使用 ``$envelope->all()``
方法，该方法返回按类型（FQCN）分组的所有邮票。
或者，你可以使用FQCN作为该方法的第一个参数来迭代所有特定类型的邮票（例如
``$envelope->all(ReceivedStamp::class)``）。

.. note::

    如果使用
    :class:`Symfony\\Component\\Messenger\\Transport\\Serialization\\Serializer`
    基础序列化器进行传输，则必须使用Symfony的Serializer组件对任何邮票进行序列化。

传输系统
----------

要发送和接收消息，你必须配置一个传输系统。该系统将负责与你的消息代理(broker)或第三方进行通信。

自定义发送方
~~~~~~~~~~~~~~~

想象一下，你已经有一个 ``ImportantAction`` 消息通过消息总线并由处理器处理。
现在，你还希望将此消息作为电子邮件发送（使用 :doc:`Mime </components/mime>`
和 :doc:`Mailer </components/mailer>` 组件)。

通过使用 :class:`Symfony\\Component\\Messenger\\Transport\\Sender\\SenderInterface`，
你可以创建自己的消息发件人::

    namespace App\MessageSender;

    use App\Message\ImportantAction;
    use Symfony\Component\Mailer\MailerInterface;
    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\Transport\Sender\SenderInterface;
    use Symfony\Component\Mime\Email;

    class ImportantActionToEmailSender implements SenderInterface
    {
        private $mailer;
        private $toEmail;

        public function __construct(MailerInterface $mailer, string $toEmail)
        {
            $this->mailer = $mailer;
            $this->toEmail = $toEmail;
        }

        public function send(Envelope $envelope): Envelope
        {
            $message = $envelope->getMessage();

            if (!$message instanceof ImportantAction) {
                throw new \InvalidArgumentException(sprintf('This transport only supports "%s" messages.', ImportantAction::class));
            }

            $this->mailer->send(
                (new Email())
                    ->to($this->toEmail)
                    ->subject('Important action made')
                    ->html('<h1>Important action</h1><p>Made by '.$message->getUsername().'</p>')
            );

            return $envelope;
        }
    }

自定义接收方
~~~~~~~~~~~~~~~~~

接收方负责从一个源获取消息并将它们调度给应用。

想象一下，你已经使用 ``NewOrder`` 消息在应用中处理了一些“订单” 。
现在，你希望与第三方或旧版应用集成，但不能使用API​​，而是需要使用带有新订单的共享CSV文件。

你将阅读此CSV文件并调度一个 ``NewOrder`` 消息。
你需要做的就是编写自定义CSV收件人::

    namespace App\MessageReceiver;

    use App\Message\NewOrder;
    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\Transport\Receiver\ReceiverInterface;
    use Symfony\Component\Serializer\SerializerInterface;

    class NewOrdersFromCsvFileReceiver implements ReceiverInterface
    {
        private $serializer;
        private $filePath;

        public function __construct(SerializerInterface $serializer, string $filePath)
        {
            $this->serializer = $serializer;
            $this->filePath = $filePath;
        }

        public function get(): void
        {
            $ordersFromCsv = $this->serializer->deserialize(file_get_contents($this->filePath), 'csv');

            foreach ($ordersFromCsv as $orderFromCsv) {
                $order = new NewOrder($orderFromCsv['id'], $orderFromCsv['account_id'], $orderFromCsv['amount']);

                $envelope = new Envelope($order);

                $handler($envelope);
            }

            return [$envelope];
        }

        public function ack(Envelope $envelope): void
        {
            // 添加有关已处理消息的信息
        }

        public function reject(Envelope $envelope): void
        {
            // 如果需要，可以拒绝该消息
        }
    }

.. versionadded:: 4.3

    如上例所示，在Symfony 4.3中，``ReceiverInterface``
    的方法已经改变了。如果你在之前的Symfony版本中使用了此接口，则可能需要更新代码。

同一总线上的收件人和发件人
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

为了允许在同一总线上发送和接收消息并防止无限循环，消息总线将添加一个
:class:`Symfony\\Component\\Messenger\\Stamp\\ReceivedStamp` 邮票到消息信封，而
:class:`Symfony\\Component\\Messenger\\Middleware\\SendMessageMiddleware`
中间件将知道它不应将这些消息再路由回一个传输系统。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /messenger
    /messenger/*

.. _关于命令总线的博客: https://matthiasnoback.nl/tags/command%20bus/
.. _SimpleBus项目: http://simplebus.io
