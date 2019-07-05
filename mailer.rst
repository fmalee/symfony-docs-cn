使用Mailer发送电子邮件
==========================

.. versionadded:: 4.3

    Symfony 4.3中添加了Mailer组件，它目前是实验性质的。之前的解决方案 -
    :doc:`Swift Mailer</email>` - 仍然有效。

安装
------------

.. caution::

    Mailer组件在Symfony 4.3中是实验性的：在4.4之前可能会出现一些向后兼容性中断。

Symfony的Mailer和 :doc:`Mime </components/mime>` 组件构成了一个用于创建和发送电子邮件的
*强大* 系统 - 完全支持多部分（multipart）消息、Twig集成、CSS内联、文件附件等等。安装它们::

.. code-block:: terminal

    $ composer require symfony/mailer

传输设置
---------------

电子邮件通过一个“传输”来投递。无需安装任何其他内容，你可以通过配置 ``.env``
文件来使用 ``smtp`` 发送电子邮件：

.. code-block:: bash

    # .env
    MAILER_DSN=smtp://user:pass@smtp.example.com

使用第三方传输
~~~~~~~~~~~~~~~~~~~~~~~~~~~

但更简单的选择是通过第三方提供器来发送电子邮件。Mailer支持众多提供器 - 安装你想要的任何一个：

==================  =============================================
Service             Install with
==================  =============================================
Amazon SES          ``composer require symfony/amazon-mailer``
Gmail               ``composer require symfony/google-mailer``
MailChimp           ``composer require symfony/mailchimp-mailer``
Mailgun             ``composer require symfony/mailgun-mailer``
Postmark            ``composer require symfony/postmark-mailer``
SendGrid            ``composer require symfony/sendgrid-mailer``
==================  =============================================

每个库都包含一个 :ref:`Flex指令 <flex-recipe>`，可以为你的 ``.env``
文件添加示例配置。例如，假设你要使用SendGrid。首先，安装它：

.. code-block:: terminal

    $ composer require symfony/sendgrid-mailer

你现在在你的 ``.env`` 文件中有了一个新行，你可以取消它的注释：

.. code-block:: bash

    # .env
    SENDGRID_KEY=
    MAILER_DSN=smtp://$SENDGRID_KEY@sendgrid

``MAILER_DSN`` 不是一个 *真正* 的SMTP地址：它是一种简单的格式，可以将大部分配置工作卸载到mailer。
地址中的的 ``@sendgrid`` 部分用于激活刚刚安装的SendGrid mailer库，它知道如何将消息传递给SendGrid。

你 *唯一* 需要更改的部分是将 ``SENDGRID_KEY`` 设置为你的密钥（在 ``.env`` 或 ``.env.local`` 中）。

每个传输都将具有不同的环境变量，该库将用于配置 *实际* 地址和用于投递的认证。有些选项还可以在
``MAILER_DSN``的末尾用查询参数进行配置，例如用于Amazon SES的 ``?region=``。一些传输支持通过
``http`` 或 ``smtp`` 发送 - 两者工作原理相同，但在可用时建议使用 ``http``。

创建和发送消息
---------------------------

要发送电子邮件，请使用 :class:`Symfony\\Component\\Mailer\\MailerInterface`（服务ID为
``mailer``）来自动装配mailer，并创建一个 :class:`Symfony\\Component\\Mime\\Email` 对象::

    // src/Controller/MailerController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Mailer\MailerInterface;
    use Symfony\Component\Mime\Email;

    class MailerController extends AbstractController
    {
        /**
         * @Route("/email")
         */
        public function sendEmail(MailerInterface $mailer)
        {
            $email = (new Email())
                ->from('hello@example.com')
                ->to('you@example.com')
                //->cc('cc@example.com')
                //->bcc('bcc@example.com')
                //->replyTo('fabien@example.com')
                //->priority(Email::PRIORITY_HIGH)
                ->subject('Time for Symfony Mailer!')
                ->text('Sending emails is fun again!')
                ->html('<p>See Twig integration for better HTML integration!</p>');

            $mailer->send($email);

            // ...
        }
    }

仅此而已！该消息将通过你配置的任何传输来发送。

电子邮件地址
~~~~~~~~~~~~~~~

所有需要电子邮件地址的方法（``from()``、``to()`` 等等）都接受字符串或地址对象::

    // ...
    use Symfony\Component\Mime\Address;
    use Symfony\Component\Mime\NamedAddress;

    $email = (new Email())
        // 作为简单字符串的电子邮件地址
        ->from('fabien@example.com')

        // 作为对象的电子邮件地址
        ->from(new Address('fabien@example.com'))

        // 作为对象的电子邮件地址（电子邮件客户端将显示名称而不是电子邮件地址）
        ->from(new NamedAddress('fabien@example.com', 'Fabien'))

        // ...
    ;

使用 ``addXXX()`` 方法定义多个地址::

    $email = (new Email())
        ->to('foo@example.com')
        ->addTo('bar@example.com')
        ->addTo('baz@example.com')

        // ...
    ;

或者，你可以将多个地址传递给每个方法::

    $toAddresses = ['foo@example.com', new Address('bar@example.com')];

    $email = (new Email())
        ->to(...$toAddresses)
        ->cc('cc1@example.com', 'cc2@example.com')

        // ...
    ;

消息内容
~~~~~~~~~~~~~~~~

电子邮件消息的文本和HTML内容可以是字符串（通常是渲染某些模板的结果）或PHP资源::

    $email = (new Email())
        // ...
        // 定义为字符串的简单内容
        ->text('Lorem ipsum...')
        ->html('<p>Lorem ipsum...</p>')

        // 附加一个文件流
        ->text(fopen('/path/to/emails/user_signup.txt', 'r'))
        ->html(fopen('/path/to/emails/user_signup.html', 'r'))
    ;

.. tip::

    你还可以使用Twig模板来渲染HTML和文本内容。阅读本文后面的 `Twig: HTML & CSS`_
    部分以了解更多信息。

文件附件
~~~~~~~~~~~~~~~~

使用 ``attachFromPath()`` 方法附加存在于文件系统上的文件::

    $email = (new Email())
        // ...
        ->attachFromPath('/path/to/documents/terms-of-use.pdf')
        // 或者，你可以告诉电子邮件客户端显示文件的自定义名称
        ->attachFromPath('/path/to/documents/privacy.pdf', 'Privacy Policy')
        // 或者，你可以提供一个显式的MIME类型（否则是猜测到的）
        ->attachFromPath('/path/to/documents/contract.doc', 'Contract', 'application/msword')
    ;

或者，你可以使用 ``attach()`` 方法从一个流中附加内容::

    $email = (new Email())
        // ...
        ->attach(fopen('/path/to/documents/contract.doc', 'r'))
    ;

嵌入图像
~~~~~~~~~~~~~~~~

如果要在电子邮件中显示图像，则必须嵌入它们，而不是将它们添加为附件。
使用Twig渲染电子邮件内容时，图像会如 `本文后面所述 <Embedding Images>`_
的自动嵌入。否则，你需要手动嵌入它们。

首先，使用 ``embed()`` 或 ``embedFromPath()`` 方法从文件或流中添加图像::

    $email = (new Email())
        // ...
        // 从PHP资源中获取图像内容
        ->embed(fopen('/path/to/images/logo.png', 'r'), 'logo')
        // 从现有文件中获取图像内容
        ->embedFromPath('/path/to/images/signature.gif', 'footer-signature')
    ;

两种方法的第二个可选参数是图像名称（MIME标准中的"Content-ID"）。
它的值是一个任意字符串，稍后用于引用HTML内容中的图像::

    $email = (new Email())
        // ...
        ->embed(fopen('/path/to/images/logo.png', 'r'), 'logo')
        ->embedFromPath('/path/to/images/signature.gif', 'footer-signature')
        // 使用 'cid:' + "image embed name" 语法引用图像
        ->html('<img src="cid:logo"> ... <img src="cid:footer-signature"> ...')
    ;

来自地址的全局
Global from Address
-------------------

你可以创建一个事件订阅器来自动设置地址，而不是 *每次* 创建新电子邮件时调用 ``->from()``::

    // src/EventListener/MailerFromListener.php
    namespace App\EventListener;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Mailer\Event\MessageEvent;
    use Symfony\Component\Mime\Email;

    class MailerFromListener implements EventSubscriberInterface
    {
        public function onMessageSend(MessageEvent $event)
        {
            $message = $event->getMessage();

            // 确保它是Email对象
            if (!$message instanceof Email) {
                return;
            }

            // 始终设置发件人地址
            $message->from('fabien@example.com');
        }

        public static function getSubscribedEvents()
        {
            return [MessageEvent::class => 'onMessageSend'];
        }
    }

.. _mailer-twig:

Twig: HTML & CSS
----------------

Mime组件与 :doc:`Twig模板引擎 </templating>` 集成，以提供CSS样式内联等高级功能，
并支持HTML/CSS框架以创建复杂的HTML电子邮件。首先，确保安装了Twig：

.. code-block:: terminal

    $ composer require symfony/twig-bundle

HTML内容
~~~~~~~~~~~~

要使用Twig定义电子邮件的内容，请使用 :class:`Symfony\\Bridge\\Twig\\Mime\\TemplatedEmail`
类。此类扩展了普通的 :class:`Symfony\\Component\\Mime\\Email` 类，但为Twig模板添加了一些新方法::

    use Symfony\Bridge\Twig\Mime\TemplatedEmail;

    $email = (new TemplatedEmail())
        ->from('fabien@example.com')
        ->to(new NamedAddress('ryan@example.com', 'Ryan'))
        ->subject('Thanks for signing up!')

        // 要渲染的Twig模板的路径
        ->htmlTemplate('emails/signup.html.twig')

        // 将变量（名称 => 值）传递到模板
        ->context([
            'expiration_date' => new \DateTime('+7 days'),
            'username' => 'foo',
        ])
    ;

然后，创建模板：

.. code-block:: html+twig

    {# templates/emails/signup.html.twig #}
    <h1>Welcome {{ email.toName }}!</h1>

    <p>
        You signed up as {{ username }} the following email:
    </p>
    <p><code>{{ email.to[0].address }}</code></p>

    <p>
        <a href="#">Click here to activate your account</a>
        (this link is valid until {{ expiration_date|date('F jS') }})
    </p>

Twig模板可以访问 ``TemplatedEmail`` 类的 ``context()``
方法中传递的任何参数，也可以访问一个名为 ``email`` 的特殊变量，它是一个
:class:`Symfony\\Bridge\\Twig\\Mime\\WrappedTemplatedEmail` 实例。

文本内容
~~~~~~~~~~~~

当未明确定义 ``TemplatedEmail`` 的文本内容时，mailer将通过将HTML内容转换为文本来自动生成它。
如果你在应用中安装了 `league/html-to-markdown`_
，则会使用它将HTML转换为Markdown（因此文本电子邮件具有一些视觉吸引力）。
否则，它会将 :phpfunction:`strip_tags` PHP函数应用于原始的HTML内容。

如果你想自己定义文本内容，请使用前面部分介绍的 ``text()`` 方法或 ``TemplatedEmail``
类提供的 ``textTemplate()`` 方法：

.. code-block:: diff

    + use Symfony\Bridge\Twig\Mime\TemplatedEmail;

    $email = (new TemplatedEmail())
        // ...

        ->htmlTemplate('emails/signup.html.twig')
    +     ->textTemplate('emails/signup.txt.twig')
        // ...
    ;

嵌入图像
~~~~~~~~~~~~~~~~

使用Twig渲染电子邮件内容时，你可以像往常一样引用图像文件，而不是使用前面部分解释的
``<img src="cid: ...">`` 语法来处理。首先，为了简化操作，定义一个名为``images``
的Twig命名空间，该命名空间指向存储图像的任何目录：

.. code-block:: yaml

    # config/packages/twig.yaml
    twig:
        # ...

        paths:
            # 把这个指向你的图像所在的地方
            '%kernel.project_dir%/assets/images': images

现在，使用特殊的 ``email.image()`` Twig辅助函数将图像嵌入到电子邮件内容中：

.. code-block:: html+twig

    {# '@images/' 引用前面定义的Twig命名空间 #}
    <img src="{{ email.image('@images/logo.png') }}" alt="Logo">

    <h1>Welcome {{ email.toName }}!</h1>
    {# ... #}

.. _mailer-inline-css:

内联CSS样式
~~~~~~~~~~~~~~~~~~~

设计电子邮件的HTML内容与设计普通HTML页面有很大不同。
首先，大多数电子邮件客户端仅支持所有CSS功能的子集。此外，Gmail等流行的电子邮件客户端不支持在
``<style> ... </style>`` 节点内部定义样式，所以你必须内联所有CSS样式。

CSS内联意味着每个HTML标签都必须用其所有的CSS样式定义一个 ``style``
属性。这会使你的CSS管理变得一团糟。这就是为什么Twig提供了一个
``CssInlinerExtension``，它可以自动为你完成所有事情。安装它：

.. code-block:: terminal

    $ composer require twig/cssinliner-extension

扩展会自动启用。要使用它，请使用 ``inline_css`` 过滤器封装整个模板：

.. code-block:: html+twig

    {% apply inline_css %}
        <style>
            {# 这里，像往常一样定义你的CSS样式 #}
            h1 {
                color: #333;
            }
        </style>

        <h1>Welcome {{ email.toName }}!</h1>
        {# ... #}
    {% endapply %}

使用外部CSS文件
........................

你还可以在外部文件中定义CSS样式，并将它们作为参数传递给过滤器：

.. code-block:: html+twig

    {% apply inline_css(source('@css/email.css')) %}
        <h1>Welcome {{ username }}!</h1>
        {# ... #}
    {% endapply %}

你可以给 ``inline_css()``
传递无限数量的参数来加载多个CSS文件。要使此示例起作用，你还需要定义一个名为 ``css``
的新Twig命名空间，该命名空间指向 ``email.css`` 所在的目录：

.. _mailer-css-namespace:

.. code-block:: yaml

    # config/packages/twig.yaml
    twig:
        # ...

        paths:
            # point this wherever your css files live
            '%kernel.project_dir%/assets/css': css

.. _mailer-markdown:

渲染Markdown内容
~~~~~~~~~~~~~~~~~~~~~~~~~~

Twig提供了另一个名为 ``MarkdownExtension`` 的扩展，它允许你使用 `Markdown语法`_
来定义电子邮件内容。要使用此功能，请安装该扩展和一个Markdown转换库（该扩展与多个常用库兼容）：

.. code-block:: terminal

    # 除了 league/commonmark，你还可以使用 erusev/parsedown 或 michelf/php-markdown
    $ composer require twig/markdown-extension league/commonmark

该扩展添加了一个 ``markdown`` 过滤器，你可以使用该过滤器将部分或整个电子邮件内容从Markdown转换为HTML：

.. code-block:: twig

    {% apply markdown %}
        Welcome {{ email.toName }}!
        ===========================

        You signed up to our site using the following email:
        `{{ email.to[0].address }}`

        [Click here to activate your account]({{ url('...') }})
    {% endapply %}

.. _mailer-inky:

Inky电子邮件模板语言
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在每个电子邮件客户端上创建设计精美的电子邮件非常复杂，因此有专门的HTML/CSS框架。
最受欢迎的框架之一叫做 `Inky`_。它基于一些简单标签定义了一种语法，这些标签稍后会转换为发送给用户的真实HTML代码：

.. code-block:: html

    <!-- Inky语法的简单示例 -->
    <container>
        <row>
            <columns>This is a column.</columns>
        </row>
    </container>

Twig通过 ``InkyExtension`` 扩展提供与Inky的集成。首先，在你的应用中安装该扩展：

.. code-block:: terminal

    $ composer require twig/inky-extension

该扩展添加了一个 ``inky`` 过滤器，可用于将部分或整个电子邮件内容从Inky转换为HTML：

.. code-block:: html+twig

    {% apply inky %}
        <container>
            <row class="header">
                <columns>
                    <spacer size="16"></spacer>
                    <h1 class="text-center">Welcome {{ email.toName }}!</h1>
                </columns>

                {# ... #}
            </row>
        </container>
    {% endapply %}

你可以组合使用所有过滤器以创建复杂的电子邮件：

.. code-block:: twig

    {% apply inky|inline_css(source('@css/foundation-emails.css')) %}
        {# ... #}
    {% endapply %}

这里使用了我们之前创建的 :ref:`css Twig命名空间 <mailer-css-namespace>`。
例如，你可以直接从GitHub `下载foundation-emails.css文件`_ 并将其保存在 ``assets/css``。

异步发送消息
----------------------

当你调用 ``$mailer->send($email)`` 时，电子邮件会立即发送到传输。要提高性能，你可以利用
:doc:`Messenger </messenger>` 以稍后通过Messenger传输来发送消息。

首先，遵循 :doc:`Messenger </messenger>` 文档并配置传输。设置完所有内容后，当你调用时
``$mailer->send()`` 时，:class:`Symfony\\Component\\Mailer\\Messenger\\SendEmailMessage`
消息将通过默认消息总线（``messenger.default_bus``）调度。假设你有一个称为
``async`` 的传输，那么你可以在那里路由消息：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    async: "%env(MESSENGER_TRANSPORT_DSN)%"

                routing:
                    'Symfony\Component\Mailer\Messenger\SendEmailMessage':  async

    .. code-block:: xml

        <!-- config/packages/messenger.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:messenger>
                    <framework:routing message-class="Symfony\Component\Mailer\Messenger\SendEmailMessage">
                        <framework:sender service="async"/>
                    </framework:routing>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'routing' => [
                    'Symfony\Component\Mailer\Messenger\SendEmailMessage' => 'async',
                ],
            ],
        ]);

得益于这一点，消息将被发送到传输以便稍后处理，而不是立即投递（请参阅 :ref:`messenger-worker`）。

开发和调试
-----------------------

禁用投递
~~~~~~~~~~~~~~~~~~

在开发（或测试）时，你可能希望完全禁用消息投递。你可以通过强制Mailer仅在 ``dev``
环境中使用 ``NullTransport`` 来实现此目的：

.. code-block:: yaml

    # config/packages/dev/mailer.yaml
    framework:
        mailer:
            dsn: 'smtp://null'

.. note::

    如果你使用Messenger并路由到了一个传输，则该消息 *仍会* 发送到该传输。

始终发送到同一地址
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

除了完全禁用投递，你可能希望始终将电子邮件发送到特定地址而不是 *真实*
地址。要做到这一点，你可以利用 ``EnvelopeListener`` 的优势，并只针对 ``dev`` 环境注册它：

.. code-block:: yaml

    # config/services_dev.yaml
    services:
        mailer.dev.set_recipients:
            class: Symfony\Component\Mailer\EventListener\EnvelopeListener
            tags: ['kernel.event_subscriber']
            arguments:
                $sender: null
                $recipients: ['youremail@example.com']

.. _`下载foundation-emails.css文件`: https://github.com/zurb/foundation-emails/blob/develop/dist/foundation-emails.css
.. _`league/html-to-markdown`: https://github.com/thephpleague/html-to-markdown
.. _`Markdown语法`: https://commonmark.org/
.. _`Inky`: https://foundation.zurb.com/emails.html
