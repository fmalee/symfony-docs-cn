.. index::
   single: EventDispatcher

如何在不使用继承的情况下自定义方法的行为(Behavior)
============================================================

在方法调用之前或之后做些操作
---------------------------------------------

如果你想在调用一个方法之前或者在之后执行某些操作，则可以分别在方法的开头或结尾处调度一个事件::

    class CustomMailer
    {
        // ...

        public function send($subject, $message)
        {
            // 在方法之前调度事件
            $event = new BeforeSendMailEvent($subject, $message);
            $this->dispatcher->dispatch('mailer.pre_send', $event);

            // 从事件中获取$foo和$bar，因为它们可能已被修改
            $subject = $event->getSubject();
            $message = $event->getMessage();

            // 真正的方法实现在这里
            $returnValue = ...;

            // 在方法之后做一些事情
            $event = new AfterSendMailEvent($returnValue);
            $this->dispatcher->dispatch('mailer.post_send', $event);

            return $event->getReturnValue();
        }
    }

在此示例中，抛出两个事件：在方法之前执行的 ``mailer.pre_send``，以及在方法执行之后的 ``mailer.post_send``。
每个都使用一个自定义事件类将信息传递给对应事件的监听器。
例如，``BeforeSendMailEvent`` 可能看起来像这样::

    // src/Event/BeforeSendMailEvent.php
    namespace App\Event;

    use Symfony\Component\EventDispatcher\Event;

    class BeforeSendMailEvent extends Event
    {
        private $subject;
        private $message;

        public function __construct($subject, $message)
        {
            $this->subject = $subject;
            $this->message = $message;
        }

        public function getSubject()
        {
            return $this->subject;
        }

        public function setSubject($subject)
        {
            $this->subject = $subject;
        }

        public function getMessage()
        {
            return $this->message;
        }

        public function setMessage($message)
        {
            $this->message = $message;
        }
    }

而 ``AfterSendMailEvent`` 是这样的::

    // src/Event/AfterSendMailEvent.php
    namespace App\Event;

    use Symfony\Component\EventDispatcher\Event;

    class AfterSendMailEvent extends Event
    {
        private $returnValue;

        public function __construct($returnValue)
        {
            $this->returnValue = $returnValue;
        }

        public function getReturnValue()
        {
            return $this->returnValue;
        }

        public function setReturnValue($returnValue)
        {
            $this->returnValue = $returnValue;
        }
    }

这两个事件都允许你获取一些信息（例如 ``getMessage()``）甚至更改该信息（例如 ``setMessage()``）。

现在，你可以创建一个事件订阅器来挂钩(hook)此事件。
例如，你可以监听 ``mailer.post_send`` 事件并更改该方法的返回值::

    // src/EventSubscriber/MailPostSendSubscriber.php
    namespace App\EventSubscriber;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use App\Event\AfterSendMailEvent;

    class MailPostSendSubscriber implements EventSubscriberInterface
    {
        public function onMailerPostSend(AfterSendMailEvent $event)
        {
            $returnValue = $event->getReturnValue();

            // 修改原本的 ``$returnValue`` 值

            $event->setReturnValue($returnValue);
        }

        public static function getSubscribedEvents()
        {
            return array(
                'mailer.post_send' => 'onMailerPostSend'
            );
        }
    }

仅此而已！你的订阅器会自动被调用（或阅读有关
:ref:`事件订阅器配置 <ref-event-subscriber-configuration>` 的更多信息 ）。
