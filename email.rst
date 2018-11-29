.. index::
   single: Emails

邮件
====================

Symfony通过 `SwiftMailerBundle`_ 提供基于流行的 `Swift Mailer`_ 库的邮件发送功能。
此邮件程序支持使用你自己的邮件服务器发送邮件，
同样支持 `Mandrill`_，`SendGrid`_ 和 `Amazon SES`_ 等流行的电子邮件提供商。

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令以在使用之前安装基于 Swift Mailer 的邮件服务：

.. code-block:: terminal

    $ composer require symfony/swiftmailer-bundle

如果你的应用程序未使用Symfony Flex，请按照 `SwiftMailerBundle`_ 上的安装说明进行操作。

.. _swift-mailer-configuration:

配置
-------------

安装邮件程序时创建的 ``config/packages/swiftmailer.yaml`` 文件提供了发送电子邮件所需的所有初始配置，
但邮件服务器连接信息除外。
这些参数在 ``.env`` 文件的 ``MAILER_URL`` 环境变量中定义：

.. code-block:: bash

    # .env (或覆盖 .env.local 中的 MAILER_URL 以避免提交更改)

    # 用此来禁用邮件发送
    MAILER_URL=null://localhost

    # 使用它来配置传统的SMTP服务器
    # 如果用户名和密码包含非字母数字(non-alphanumeric)字符，请确保对其进行URL编码
    # 例如 '+'，'@'，'：'和'*'，它们都是URL的保留字符
    MAILER_URL=smtp://localhost:25?encryption=ssl&auth_mode=login&username=&password=

有关所有可用配置选项的详细说明，请参阅 :doc:`SwiftMailer配置参考 </reference/configuration/swiftmailer>`。

发送邮件
--------------

Swift Mailer库通过创建、配置然后发送 ``Swift_Message`` 对象来工作。
“mailer”负责邮件的实际传递，可通过 ``Swift_Mailer`` 服务访问。
总的来说，发送电子邮件非常简单::

    public function index($name, \Swift_Mailer $mailer)
    {
        $message = (new \Swift_Message('Hello Email'))
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody(
                $this->renderView(
                    // templates/emails/registration.html.twig
                    'emails/registration.html.twig',
                    array('name' => $name)
                ),
                'text/html'
            )
            /*
             * 如果你还想要包含一个纯文本版本的信息
            ->addPart(
                $this->renderView(
                    'emails/registration.txt.twig',
                    array('name' => $name)
                ),
                'text/plain'
            )
            */
        ;

        $mailer->send($message);

        return $this->render(...);
    }

为了保持解耦，邮件正文已存储在模板中并使用 ``renderView()`` 方法渲染。
``registration.html.twig`` 模板可能像这样：

.. code-block:: html+jinja

    {# templates/emails/registration.html.twig #}
    <h3>You did it! You registered!</h3>

    Hi {{ name }}! You're successfully registered.

    {# 只是个例子, 假定你有一个名为 "login" 的路由 #}
    To login, go to: <a href="{{ url('login') }}">...</a>.

    Thanks!

    {# 为 /images/logo.png 文件生成一个绝对URL #}
    <img src="{{ absolute_url(asset('images/logo.png')) }}">

``$message`` 对象支持更多选项，例如包含附件，添加HTML内容等等。
有关更多详细信息，请参阅Swift Mailer文档的 `创建消息`_ 章节。

.. _email-using-gmail:

使用 Gmail 发送邮件
--------------------------

在开发过程中，你可能更愿意使用Gmail发送电子邮件，而不是设置常规的SMTP服务器。
为此，请将 ``.env`` 文件的 ``MAILER_URL`` 更新为：

.. code-block:: bash

    # 用户名是完整的 Gmail 或 Google Apps 邮件地址
    MAILER_URL=gmail://username:password@localhost

``gmail`` 传输只是一个使用 ``smtp`` 传输、``ssl`` 加密，``login`` 认证模式和 ``smtp.gmail.com`` 主机的快捷方式。
如果你的应用使用其他加密或认证模式，则必须覆盖这些值
（请参阅 :doc:`邮件程序配置参考 </reference/configuration/swiftmailer>`）。

.. code-block:: bash

    # 用户名是完整的 Gmail 或 Google Apps 邮件地址
    MAILER_URL=gmail://username:password@localhost?encryption=tls&auth_mode=oauth

如果你的Gmail帐户使用两步验证(2-Step-Verification)，则必须 `生成应用密码`_ 并将其用作邮件程序密码的值。
你还必须确保 `允许安全性较低的应用访问你的Gmail帐户`_。


使用云服务发送邮件
-----------------------------------

对于不想设置和维护自己的可靠邮件服务器的公司，云邮件服务是一种流行的选择。
要在Symfony应用中使用这些服务，更新 ``.env`` 文件中 ``MAILER_URL`` 的值。
例如，对于 `Amazon SES`_ （Simple Email Service）：

.. code-block:: bash

    # 主机会根据你的AWS区域而有所不同
    # 用户名/密码凭据是从Amazon SES控制台获取的
    MAILER_URL=smtp://email-smtp.us-east-1.amazonaws.com:587?encryption=tls&username=YOUR_SES_USERNAME&password=YOUR_SES_PASSWORD

对其他邮件服务使用相同的技巧，因为大多数情况下除了配置SMTP端点之外没有其他任何内容。

扩展阅读
----------

.. toctree::
    :maxdepth: 1

    email/dev_environment
    email/spool
    email/testing

.. _`Swift Mailer`: http://swiftmailer.org/
.. _`SwiftMailerBundle`: https://github.com/symfony/swiftmailer-bundle
.. _`创建消息`: https://swiftmailer.symfony.com/docs/messages.html
.. _`Mandrill`: https://mandrill.com/
.. _`SendGrid`: https://sendgrid.com/
.. _`Amazon SES`: http://aws.amazon.com/ses/
.. _`生成应用密码`: https://support.google.com/accounts/answer/185833
.. _`允许安全性较低的应用访问你的Gmail帐户`: https://support.google.com/accounts/answer/6010255
