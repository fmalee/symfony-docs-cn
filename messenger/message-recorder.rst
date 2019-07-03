.. index::
    single: Messenger; Record messages; Transaction messages

事务性消息：处理完成后才处理新消息
==================================================================

消息处理器可以在执行期间将新消息 ``dispatch`` 到相同或不同的总线（如果应用具有
`多个总线 </messenger/multiple_buses>`_）。
在此过程中发生的任何错误或异常都可能产生意想不到的后果，例如：

- 如果使用 ``DoctrineTransactionMiddleware``，然后调度的消息抛出异常，
  则将回滚原始处理器中的所有数据库事务。
- 如果将消息调度到不同的总线，则即使稍后当前处理器中的某些代码抛出异常，也将继续处理已调度的消息。

``RegisterUser`` 进程示例
-----------------------------------

让我们以一个同时带有 *命令* 和 *事件* 总线的应用为例。应用调度一个名为 ``RegisterUser``
的命令到命令总线。这个由 ``RegisterUserHandler`` 处理的命令会创建一个 ``User``
对象并将该对象存储到数据库，然后调度一个 ``UserRegistered`` 消息给事件总线。

``UserRegistered`` 消息会有许多处理器，其中一个处理器可以向新用户发送欢迎电子邮件。
 我们使用 ``DoctrineTransactionMiddleware`` 来在一个数据库事务中封装所有数据库查询。

**问题1：** 如果在发送欢迎电子邮件时抛出异常，则该用户不会被创建，因为
``DoctrineTransactionMiddleware`` 将回滚已创建用户的Doctrine事务。

**问题2：** 如果在将用户保存到数据库时抛出异常，仍会发送欢迎电子邮件，因为它是异步处理的。

DispatchAfterCurrentBusMiddleware中间件
--------------------------------------------

对于许多应用，所需的行为是 *仅* 处理由一个处理器完全处理完后再调度的消息。
这可以通过使用 ``DispatchAfterCurrentBusMiddleware`` 和添加一个 ``DispatchAfterCurrentBusStamp``
邮票到 `消息信封 </components/messenger#adding-metadata-to-messages-envelopes>`_ 来完成::

    namespace App\Messenger\CommandHandler;

    use App\Entity\User;
    use App\Messenger\Command\RegisterUser;
    use App\Messenger\Event\UserRegistered;
    use Doctrine\ORM\EntityManagerInterface;
    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\MessageBusInterface;
    use Symfony\Component\Messenger\Stamp\DispatchAfterCurrentBusStamp;

    class RegisterUserHandler
    {
        private $eventBus;
        private $em;

        public function __construct(MessageBusInterface $eventBus, EntityManagerInterface $em)
        {
            $this->eventBus = $eventBus;
            $this->em = $em;
        }

        public function __invoke(RegisterUser $command)
        {
            $user = new User($command->getUuid(), $command->getName(), $command->getEmail());
            $this->em->persist($user);

            // DispatchAfterCurrentBusStamp 仅在该处理器不引发异常时标记要处理的事件消息。

            $event = new UserRegistered($command->getUuid());
            $this->eventBus->dispatch(
                (new Envelope($event))
                    ->with(new DispatchAfterCurrentBusStamp())
            );

            // ...
        }
    }

.. code-block:: php

    namespace App\Messenger\EventSubscriber;

    use App\Entity\User;
    use App\Messenger\Event\UserRegistered;
    use Doctrine\ORM\EntityManagerInterface;
    use Symfony\Component\Mailer\MailerInterface;
    use Symfony\Component\Mime\RawMessage;

    class WhenUserRegisteredThenSendWelcomeEmail
    {
        private $mailer;
        private $em;

        public function __construct(MailerInterface $mailer, EntityManagerInterface $em)
        {
            $this->mailer = $mailer;
            $this->em = $em;
        }

        public function __invoke(UserRegistered $event)
        {
            $user = $this->em->getRepository(User::class)->find(new User($event->getUuid()));

            $this->mailer->send(new RawMessage('Welcome '.$user->getFirstName()));
        }
    }

这意味着只有在 ``RegisterUserHandler`` 完成并且新的 ``User`` 持久存储到数据库 *之后* 才会处理
``UserRegistered`` 消息。如果 ``RegisterUserHandler`` 遇到异常，则永远不会处理
``UserRegistered`` 事件。如果在发送欢迎电子邮件时抛出异常，则不会回滚Doctrine事务。

.. note::

    如果 ``WhenUserRegisteredThenSendWelcomeEmail`` 抛出异常，该异常将被封装为
    ``DelayedMessageHandlingException``。使用
    ``DelayedMessageHandlingException::getExceptions`` 将为你提供在使用
    ``DispatchAfterCurrentBusStamp`` 处理消息时抛出的所有异常。

``dispatch_after_current_bus`` 中间件是默认启用的。如果你手动配置中间件，请务必在中间件链的
``doctrine_transaction`` 之前注册 ``dispatch_after_current_bus``。此外，必须为 *所有*
正在使用的总线加载 ``dispatch_after_current_bus`` 中间件。
