.. index::
   single: MIME
   single: MIME Messages
   single: Components; MIME

Mime组件
==================

    Mime组件允许操作用于发送电子邮件的MIME消息，并提供与MIME类型相关的实用工具。

.. versionadded:: 4.3

    Mime组件是在Symfony 4.3中引入的，它仍然被认为是一个
    :doc:`实验性功能 </contributing/code/experimental>`。

安装
------------

.. code-block:: terminal

    $ composer require symfony/mime

.. include:: /components/require_autoload.rst.inc

介绍
------------

`MIME`_ （Multipurpose Internet Mail Extensions）是一种Internet标准，
它扩展了电子邮件的原始基本格式，以支持以下功能::

* 使用非ASCII字符的标头和文本内容;
* 具有多个部分的消息体（例如HTML和纯文本内容）;
* 非文本附件：音频、视频、图像、PDF等

整个MIME标准既复杂又庞大，但Symfony将所有这些复杂性都抽象化，以提供两种创建MIME消息的方法：

* 基于 :class:`Symfony\\Component\\Mime\\Email`
  类的高级API，可快速创建具有所有常用功能的电子邮件消息；
* 基于 :class:`Symfony\\Component\\Mime\\Message`
  类的低级API ，可以绝对控制电子邮件消息的每个部分。

用法
-----

使用 :class:`Symfony\\Component\\Mime\\Email` 类及其 *方法链* 来撰写整个电子邮件::

    use Symfony\Component\Mime\Email;

    $email = (new Email())
        ->from('fabien@symfony.com')
        ->to('foo@example.com')
        ->cc('bar@example.com')
        ->bcc('baz@example.com')
        ->replyTo('fabien@symfony.com')
        ->priority(Email::PRIORITY_HIGH)
        ->subject('Important Notification')
        ->text('Lorem ipsum...')
        ->html('<h1>Lorem ipsum</h1> <p>...</p>')
    ;

此组件的唯一目的是创建电子邮件消息。可使用 :doc:`Mailer组件 </components/mailer>`
实际发送它们。在Symfony应用中，使用:doc:`Mailer集成 </mailer>` 会更容易。

有关如何创建电子邮件对象的大部分详细信息（包括Twig集成）都可以在
:doc:`Mailer文档 </mailer>` 中找到。

Twig集成
----------------

Mime组件与Twig完美集成，允许你从Twig模板、嵌入图像、内联CSS等方面创建消息。
有关如何使用这些功能的详细信息，请参阅Mailer文档：:ref:`Twig: HTML & CSS <mailer-twig>`。

但是如果你在没有Symfony框架的情况下使用Mime组件，则需要处理一些设置细节。

Twig设置
~~~~~~~~~~

要与Twig集成，请使用 :class:`Symfony\\Bridge\\Twig\\Mime\\BodyRenderer`
类来渲染模板，并使用渲染结果更新电子邮件消息的内容::

    // ...
    use Symfony\Bridge\Twig\Mime\BodyRenderer;
    use Twig\Environment;
    use Twig\Loader\FilesystemLoader;

    // 当在一个全栈Symfony应用中使用Mime组件时，你不需要执行这个Twig设置。
    // 你只需要注入 'twig' 服务
    $loader = new FilesystemLoader(__DIR__.'/templates');
    $twig = new Environment($loader);

    $renderer = new BodyRenderer($twig);
    // 这将使用前面给定上下文中定义的模板的渲染结果来更新 $email 对象的内容。
    $renderer->render($email);

内联CSS样式（和其他扩展）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

要使用 :ref:`inline_css <mailer-inline-css>` 过滤器，请先安装Twig扩展：

.. code-block:: terminal

    $ composer require twig/cssinliner-extension

现在，启用该扩展::

    // ...
    use Twig\CssInliner\CssInlinerExtension;

    $loader = new FilesystemLoader(__DIR__.'/templates');
    $twig = new Environment($loader);
    $twig->addExtension(new CssInlinerExtension());

可以使用相同的过程来启用其他扩展，例如
:ref:`MarkdownExtension <mailer-markdown>` 和 :ref:`InkyExtension <mailer-inky>`。

创建原始电子邮件
---------------------------

这对需要对每个电子邮件的每部分进行绝对控制的高级应用很有用。
不推荐将它用于只有常规电子邮件要求的应用，因为它增加了复杂性又不增加实际收益。

在继续之前，重要的是要看一下电子邮件的低级结构。
考虑一条包含一些内容消息，其中包括文本、HTML、嵌入在这些内容中的单个PNG图像以及附加到其上的PDF文件。
MIME标准允许以不同的方式来构造此消息，但以下是适用于大多数电子邮件客户端的树结构：

.. code-block:: text

    multipart/mixed
    ├── multipart/related
    │   ├── multipart/alternative
    │   │   ├── text/plain
    │   │   └── text/html
    │   └── image/png
    └── application/pdf

这是MIME消息的每个部分的作用：

* ``multipart/alternative``：当两个或更多的部分是相同（或非常相似）内容的替代品时，使用它。
  首选格式必须最后添加。
* ``multipart/mixed``：用于在同一消息中发送不同的内容类型，例如附加文件时。
* ``multipart/related``：用于指示每个消息的部分是聚合整体的一个组件。
  最常见的用法是显示嵌入在消息内容中的图像。

使用低级的 :class:`Symfony\\Component\\Mime\\Message`
类创建电子邮件时，必须牢记以上所有内容，以便手动定义电子邮件的不同部分::

    use Symfony\Component\Mime\Header\Headers;
    use Symfony\Component\Mime\Message;
    use Symfony\Component\Mime\Part\Multipart\AlternativePart;
    use Symfony\Component\Mime\Part\TextPart;

    $headers = (new Headers())
        ->addMailboxListHeader('From', ['fabien@symfony.com'])
        ->addMailboxListHeader('To', ['foo@example.com'])
        ->addTextHeader('Subject', 'Important Notification')
    ;

    $textContent = new TextPart('Lorem ipsum...');
    $htmlContent = new TextPart('<h1>Lorem ipsum</h1> <p>...</p>', 'html');
    $body = new AlternativePart($textContent, $htmlContent);

    $email = new Message($headers, $body);

通过创建适当的电子邮件的Multipart，可以嵌入图像和附加文件::

    // ...
    use Symfony\Component\Mime\Part\DataPart;
    use Symfony\Component\Mime\Part\Multipart\MixedPart;
    use Symfony\Component\Mime\Part\Multipart\RelatedPart;

    // ...
    $embeddedImage = new DataPart(fopen('/path/to/images/logo.png', 'r'), null, 'image/png');
    $imageCid = $embeddedImage->getContentId();

    $attachedFile = new DataPart(fopen('/path/to/documents/terms-of-use.pdf', 'r'), null, 'application/pdf');

    $textContent = new TextPart('Lorem ipsum...');
    $htmlContent = new TextPart(sprintf(
        '<img src="cid:%s"/> <h1>Lorem ipsum</h1> <p>...</p>', $imageCid
    ), 'html');
    $bodyContent = new AlternativePart($textContent, $htmlContent);
    $body = new RelatedPart($bodyContent, $embeddedImage);

    $messageParts = new MixedPart($body, $attachedFile);

    $email = new Message($headers, $messageParts);

序列化邮件消息
--------------------------

使用 ``Email`` 或 ``Message`` 类创建的电子邮件消息可以序列化，因为它们是简单的数据对象::

    $email = (new Email())
        ->from('fabien@symfony.com')
        // ...
    ;

    $serializedEmail = serialize($email);

常见的用例是存储序列化的电子邮件消息，将其包含在与 :doc:`Messenger组件 </components/messenger>`
一起发送的消息中，并在以后发送时重新创建它们。使用
:class:`Symfony\\Component\\Mime\\RawMessage` 类从其序列化内容中重新创建电子邮件::

    use Symfony\Component\Mime\RawMessage;

    // ...
    $serializedEmail = serialize($email);

    // 稍后，重新创建原始邮件以实际发送它
    $message = new RawMessage(unserialize($serializedEmail));

MIME类型工具
--------------------

虽然MIME主要是为创建电子邮件而设计的，但MIME标准定义的内容类型（也称为 `MIME类型`_
和“媒体类型”）在电子邮件之外的通信协议（如HTTP）中也很重要。
这就是为什么这个组件还提供了使用MIME类型的实用工具。

:class:`Symfony\\Component\\Mime\\MimeTypes` 类用于转换MIME的类型和文件的扩展名::

    use Symfony\Component\Mime\MimeTypes;

    $mimeTypes = new MimeTypes();
    $exts = $mimeTypes->getExtensions('application/javascript');
    // $exts = ['js', 'jsm', 'mjs']
    $exts = $mimeTypes->getExtensions('image/jpeg');
    // $exts = ['jpeg', 'jpg', 'jpe']

    $mimeTypes = $mimeTypes->getMimeTypes('js');
    // $mimeTypes = ['application/javascript', 'application/x-javascript', 'text/javascript']
    $mimeTypes = $mimeTypes->getMimeTypes('apk');
    // $mimeTypes = ['application/vnd.android.package-archive']

这些方法返回带有一个或多个元素的数组。元素的位置表示其优先级，因此第一个返回的扩展名是首选扩展名。

猜测MIME类型
~~~~~~~~~~~~~~~~~~~~~~

另一个有用的实用工具允许猜测任何给定文件的MIME类型::

    use Symfony\Component\Mime\MimeTypes;

    $mimeTypes = new MimeTypes();
    $mimeType = $mimeTypes->guessMimeType('/some/path/to/image.gif');
    // 猜测不是基于文件名，因此只有给定的文件是
    // 真正的GIF图像时，$mimeType 才会是 'image/gif'。

猜测MIME类型是一个耗时的过程，因为需要检查文件的部分内容。
Symfony应用了多种猜测机制，其中一种基于PHP 
`fileinfo扩展`_。建议安装该扩展以提高猜测的性能。

添加MIME类型猜测器
..........................

你可以通过创建一个实现了 :class:`Symfony\\Component\\Mime\\MimeTypeGuesserInterface`
的类来注册自己的MIME类型猜测器::

    namespace App;

    use Symfony\Component\Mime\MimeTypeGuesserInterface;

    class SomeMimeTypeGuesser implements MimeTypeGuesserInterface
    {
        public function isGuesserSupported(): bool
        {
            // 当此猜测器受支持时返回 true（例如支不支持可能取决于操作系统）
            return true;
        }

        public function guessMimeType(string $path): ?string
        {
            // 检查存储在 $path 中的文件的内容以猜测其类型，并返回有效的MIME类型……
            // 如果结果未知，则为空

            return '...';
        }
    }

.. _`MIME`: https://en.wikipedia.org/wiki/MIME
.. _`MIME类型`: https://en.wikipedia.org/wiki/Media_type
.. _`fileinfo扩展`: https://php.net/fileinfo
