.. index::
   single: Emails; Testing

如何测试在功能测试中发送的邮件
======================================================

由于SwiftmailerBundle利用了 `Swift Mailer`_ 库的强大功能，因此使用Symfony发送邮件非常简单。

要功能测试邮件的发送，甚至断言邮件主题、内容或任何其他标头，你可以使用 :doc:`Symfony分析器 </profiler>`。

从发送邮件的简单控制器动作开始::

    public function sendEmail($name, \Swift_Mailer $mailer)
    {
        $message = (new \Swift_Message('Hello Email'))
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody('You should see me from the profiler!')
        ;

        $mailer->send($message);

        return $this->render(...);
    }

在功能测试中，使用分析器上的 ``swiftmailer`` 收集器获取有关上一个请求中发送的消息的信息::

    // tests/Controller/MailControllerTest.php
    namespace App\Tests\Controller;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class MailControllerTest extends WebTestCase
    {
        public function testMailIsSentAndContentIsOk()
        {
            $client = static::createClient();

            // 为下一个请求启用分析器（如果分析器不可用，则不执行任何操作）
            $client->enableProfiler();

            $crawler = $client->request('POST', '/path/to/above/action');

            $mailCollector = $client->getProfile()->getCollector('swiftmailer');

            // 检索一个已发送的邮件
            $this->assertSame(1, $mailCollector->getMessageCount());

            $collectedMessages = $mailCollector->getMessages();
            $message = $collectedMessages[0];

            // 断言邮件数据
            $this->assertInstanceOf('Swift_Message', $message);
            $this->assertSame('Hello Email', $message->getSubject());
            $this->assertSame('send@example.com', key($message->getFrom()));
            $this->assertSame('recipient@example.com', key($message->getTo()));
            $this->assertSame(
                'You should see me from the profiler!',
                $message->getBody()
            );
        }
    }

故障排除
---------------

问题：The Collector Object Is ``null``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

邮件收集器仅在分析器已启用并收集信息时可用，请参阅 :doc:`/testing/profiling`。

问题：The Collector Doesn't Contain the Email
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果在发送邮件后执行重定向（例如，在处理表单之后和重定向到另一个页面之前发送电子邮件），
请确保测试客户端不遵循重定向，请参阅 :doc:`/testing`。
否则，该收集器将包含重定向页面的信息，从而无法访问该邮件。

.. _`Swift Mailer`: http://swiftmailer.org/
