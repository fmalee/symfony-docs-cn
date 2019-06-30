.. index::
   single: Emails

Swift Mailer
============

.. note::

    在Symfony 4.3中，引入了 :doc:`Mailer </mailer>` 组件，可以用来替代Swift Mailer。

Symfony通过 `SwiftMailerBundle`_ 提供基于流行的 `Swift Mailer`_ 库的邮件发送功能。
此邮件程序支持使用你自己的邮件服务器发送邮件，
同样支持 `Mandrill`_，`SendGrid`_ 和 `Amazon SES`_ 等流行的电子邮件提供商。

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令以在使用之前安装基于 Swift Mailer 的邮件服务：

.. code-block:: terminal

    $ composer require symfony/swiftmailer-bundle

如果你的应用未使用Symfony Flex，请按照 `SwiftMailerBundle`_ 上的安装说明进行操作。

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
    MAILER_URL=smtp://localhost:25?encryption=ssl&auth_mode=login&username=&password=

.. caution::

    如果用户名、密码或主机包含一个在URI中被认为特殊的任何字符（例如
    ``+``、``@``、``$``、``#``、``/``、``:``、``*``、``!``），则必须对其进行编码。
    有关保留字符的完整列表，请参阅 `RFC 3986`_ 或使用 :phpfunction:`urlencode` 函数对其进行编码。

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
                    ['name' => $name]
                ),
                'text/html'
            )

            // 如果没有为电子邮件定义一个文本版本，则可以删除以下代码
            ->addPart(
                $this->renderView(
                    'emails/registration.txt.twig',
                    ['name' => $name]
                ),
                'text/plain'
            )
        ;

        $mailer->send($message);

        return $this->render(...);
    }

为了保持解耦，邮件正文已存储在模板中并使用 ``renderView()`` 方法渲染。
``registration.html.twig`` 模板可能像这样：

.. code-block:: html+twig

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

``gmail`` 传输是一个使用 ``smtp`` 传输、``ssl`` 加密，``login`` 认证模式和 ``smtp.gmail.com`` 主机的快捷方式。
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

如何在开发过程中处理邮件
------------------------------------------

在开发一个发送邮件的应用时，你通常不希望在开发期间将邮件实际发送给指定的收件人。
如果你将SwiftmailerBundle与Symfony一起使用，则可以通过配置设置实现此目的，而无需对应用的代码进行任何更改。
在开发过程中处理邮件有两个主要选择：（a）完全禁止发送邮件或（b）将所有邮件发送到指定地址（可配置可选的例外情况）。

禁用发送
~~~~~~~~~~~~~~~~~

你可以通过将 ``disable_delivery`` 选项设置为 ``true`` 来禁用邮件发送，这是Symfony在
``test`` 环境中使用的默认值（邮件将继续在其他环境中发送）：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/test/swiftmailer.yaml
        swiftmailer:
            disable_delivery: true

    .. code-block:: xml

        <!-- config/packages/test/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config disable-delivery="true"/>
        </container>

    .. code-block:: php

        // config/packages/test/swiftmailer.php
        $container->loadFromExtension('swiftmailer', [
            'disable_delivery' => "true",
        ]);

.. _sending-to-a-specified-address:

发送到特定地址
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以选择将所有邮件发送到一个特定地址或地址列表，而不是发送邮件时实际指定的地址。
这可以通过 ``delivery_addresses`` 选项来完成：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/dev/swiftmailer.yaml
        swiftmailer:
            delivery_addresses: ['dev@example.com']

    .. code-block:: xml

        <!-- config/packages/dev/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config>
                <swiftmailer:delivery-address>dev@example.com</swiftmailer:delivery-address>
            </swiftmailer:config>
        </container>

    .. code-block:: php

        // config/packages/dev/swiftmailer.php
        $container->loadFromExtension('swiftmailer', [
            'delivery_addresses' => ['dev@example.com'],
        ]);

现在，假设你在一个控制器向 ``recipient@example.com`` 发送邮件::

    public function index($name, \Swift_Mailer $mailer)
    {
        $message = (new \Swift_Message('Hello Email'))
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody(
                $this->renderView(
                    'HelloBundle:Hello:email.txt.twig',
                    ['name' => $name]
                )
            )
        ;
        $mailer->send($message);

        return $this->render(...);
    }

在 ``dev`` 环境中，该邮件将被发送到 ``dev@example.com``。
Swift Mailer会在邮件中添加一个额外的 ``X-Swift-To`` 标头，其中包含已被替换的地址，因此你仍然可以看到该邮件将被发送给谁。

.. note::

    除了 ``to`` 地址之外，这还将阻止邮件发送到为其设置的任何 ``CC`` 和 ``BCC`` 地址。
    Swift Mailer将在邮件中添加其他标头，并在其中包含已覆盖的地址。
    它们是分别对应 ``CC`` 和 ``BCC`` 的 ``X-Swift-Cc`` 和 ``X-Swift-Bcc``。

.. _sending-to-a-specified-address-but-with-exceptions:

发送到指定地址的白名单
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

假设你希望将所有邮件重定向到指定地址（如上所述的 ``dev@example.com``）。
但是，你可能希望发送到某些特定邮件地址的邮件能实际发送，而不是重定向（即使它是在开发环境中）。
这可以通过添加 ``delivery_whitelist`` 选项来完成：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/dev/swiftmailer.yaml
        swiftmailer:
            delivery_addresses: ['dev@example.com']
            delivery_whitelist:
                # all email addresses matching these regexes will be delivered
                # like normal, as well as being sent to dev@example.com
                - '/@specialdomain\.com$/'
                - '/^admin@mydomain\.com$/'

    .. code-block:: xml

        <!-- config/packages/dev/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config>
                <!-- all email addresses matching these regexes will be delivered
                     like normal, as well as being sent to dev@example.com -->
                <swiftmailer:delivery-whitelist-pattern>/@specialdomain\.com$/</swiftmailer:delivery-whitelist-pattern>
                <swiftmailer:delivery-whitelist-pattern>/^admin@mydomain\.com$/</swiftmailer:delivery-whitelist-pattern>
                <swiftmailer:delivery-address>dev@example.com</swiftmailer:delivery-address>
            </swiftmailer:config>
        </container>

    .. code-block:: php

        // config/packages/dev/swiftmailer.php
        $container->loadFromExtension('swiftmailer', [
            'delivery_addresses' => ["dev@example.com"],
            'delivery_whitelist' => [
                // all email addresses matching these regexes will be delivered
                // like normal, as well as being sent to dev@example.com
                '/@specialdomain\.com$/',
                '/^admin@mydomain\.com$/',
            ],
        ]);

在上面的示例中，所有邮件都将被重定向到 ``dev@example.com``，但是发送到 ``admin@mydomain.com``
地址或属于 ``specialdomain.com`` 域的任何邮件地址的邮件将照常传送。

.. caution::

    除非定义了 ``delivery_addresses`` 选项，否则将忽略 ``delivery_whitelist`` 选项。

从Web调试工具栏查看
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在 ``dev`` 环境中使用Web调试工具栏时，你可以查看在单个响应期间发送的任何邮件。
工具栏中的邮件图标将显示已发送的邮件数量。如果点击它，将打开一个显示已发送邮件的详细信息的报告。

如果你要发送邮件然后立即重定向到其他页面，则Web调试工具栏将不会在下一个页面上显示邮件图标或报告。

但是，你可以在 ``dev`` 环境中设置 ``intercept_redirects`` 选项为 ``true``
来停止重定向，然后允许你打开包含已发送邮件详细信息的报告。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/dev/web_profiler.yaml
        web_profiler:
            intercept_redirects: true

    .. code-block:: xml

        <!-- config/packages/dev/web_profiler.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:webprofiler="http://symfony.com/schema/dic/webprofiler"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/webprofiler
                https://symfony.com/schema/dic/webprofiler/webprofiler-1.0.xsd">

            <webprofiler:config
                intercept-redirects="true"
            />
        </container>

    .. code-block:: php

        // config/packages/dev/web_profiler.php
        $container->loadFromExtension('web_profiler', [
            'intercept_redirects' => 'true',
        ]);

.. tip::

    或者，你可以在重定向后打开分析器，并通过提交上一个请求中使用的URL来进行搜索（例如 ``/contact/handle``）。
    通过分析器的搜索功能，你可以加载任何过去请求的分析器信息。

.. tip::

    除了Symfony提供的功能外，还有一些应用可以帮助你在应用开发期间测试邮件，例如 `MailCatcher`_
    和 `MailHog`_。

如何假脱机邮件
-------------------

Symfony邮件程序的默认行为是立即发送电子邮件。
但是，你可能希望避免与邮件服务器通信的性能损失，因为这可能导致邮件发送时用户需要等待下一页的加载。
选择“假脱机(spool)”邮件而不是直接发送邮件可以避免这种情况。

这使邮件程序不会尝试发送邮件消息，而是将其保存在某个位置，例如文件。
然后，另一个进程可以负责在假脱机中读取并发送电子邮件。目前仅支持假脱机到文件或内存。

.. _email-spool-memory:

使用内存假脱机
~~~~~~~~~~~~~~~~~~

当你使用假脱机将邮件存储到内存时，它们将在内核终止之前发送。
这意味着只有在整个请求期间没有任何未处理的异常或错误的情况下才会发送邮件。
要配置此假脱机，请使用以下配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/swiftmailer.yaml
        swiftmailer:
            # ...
            spool: { type: memory }

    .. code-block:: xml

        <!-- config/packages/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config>
                <swiftmailer:spool type="memory"/>
            </swiftmailer:config>
        </container>

    .. code-block:: php

        // config/packages/swiftmailer.php
        $container->loadFromExtension('swiftmailer', [
            // ...
            'spool' => ['type' => 'memory'],
        ]);

.. _spool-using-a-file:

使用文件假脱机
~~~~~~~~~~~~~~~~~

当你使用文件系统进行假脱机时，Symfony会在每个邮件服务的给定路径中创建一个文件夹（例如，默认服务的“default”）。
此文件夹将包含假脱机中每封邮件的文件。因此，请确保Symfony（或你的 webserver/php）可以写入该目录！

要将假脱机与文件一起使用，请使用以下配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/swiftmailer.yaml
        swiftmailer:
            # ...
            spool:
                type: file
                path: /path/to/spooldir

    .. code-block:: xml

        <!-- config/packages/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config>
                <swiftmailer:spool
                    type="file"
                    path="/path/to/spooldir"
                />
            </swiftmailer:config>
        </container>

    .. code-block:: php

        // config/packages/swiftmailer.php
        $container->loadFromExtension('swiftmailer', [
            // ...

            'spool' => [
                'type' => 'file',
                'path' => '/path/to/spooldir',
            ],
        ]);

.. tip::

    如果要将假脱机存储在项目目录的某处，请记住可以使用 ``%kernel.project_dir%`` 参数来引用项目的根目录：

    .. code-block:: yaml

        path: '%kernel.project_dir%/var/spool'

现在，当你的应用发送邮件时，它实际上不会被发送，而是添加到假脱机中。
从假脱机发送消息是单独完成的。有一个控制台命令可以在假脱机中发送消息：

.. code-block:: terminal

    $ APP_ENV=prod php bin/console swiftmailer:spool:send

它有一个选项来限制要发送的消息数量：

.. code-block:: terminal

    $ APP_ENV=prod php bin/console swiftmailer:spool:send --message-limit=10

你还可以以秒为单位来设置时间限制：

.. code-block:: terminal

    $ APP_ENV=prod php bin/console swiftmailer:spool:send --time-limit=10

实际上，你不希望手动运行此操作。相反，控制台命令应由cron或计划任务触发，并以固定间隔运行。

.. caution::

    使用SwiftMailer创建一个消息时，它会生成一个 ``Swift_Message`` 类。
    如果swiftmailer服务是延迟加载的，它会生成一个名为 ``Swift_Message_<someRandomCharacters>`` 的代理类。

    如果使用内存假脱机，则此更改是透明的，不会产生任何影响。
    但是，在使用文件系统假脱机时，消息类将在具有随机类名的文件中序列化。
    问题是这个随机类名在每个缓存清除后都会改变。因此，如果你发送一个邮件然后清除了缓存，则该邮件将无法被反序列化。

    在下一次的 ``swiftmailer:spool:send`` 执行过程中会报错，因为该 ``Swift_Message_<someRandomCharacters>`` 类不(再)存在。

    解决方案是要么使用内存假脱机，要么在不带 ``lazy`` 选项的情况下加载 ``swiftmailer``
    服务（请参阅 :doc:`/service_container/lazy_services`）。

如何测试在功能测试中发送的邮件
------------------------------------------------------

由于SwiftmailerBundle利用了 `Swift Mailer`_ 库的强大功能，因此使用Symfony发送邮件非常简单。

要功能测试邮件的发送，甚至断言邮件主题、内容或任何其他标头，你可以使用 :doc:`Symfony分析器 </profiler>`。

从发送邮件的控制器动作开始::

    public function sendEmail($name, \Swift_Mailer $mailer)
    {
        $message = (new \Swift_Message('Hello Email'))
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody('You should see me from the profiler!')
        ;

        $mailer->send($message);

        // ...
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
~~~~~~~~~~~~~~~

问题：The Collector Object Is ``null``
.........................................

邮件收集器仅在分析器已启用并收集信息时可用，请参阅 :doc:`/testing/profiling`。

问题：The Collector Doesn't Contain the Email
................................................

如果在发送邮件后执行重定向（例如，在处理表单之后和重定向到另一个页面之前发送电子邮件），
请确保测试客户端不遵循重定向，请参阅 :doc:`/testing`。
否则，该收集器将包含重定向页面的信息，从而无法访问该邮件。

.. _`MailCatcher`: https://github.com/sj26/mailcatcher
.. _`MailHog`: https://github.com/mailhog/MailHog
.. _`Swift Mailer`: http://swiftmailer.org/
.. _`SwiftMailerBundle`: https://github.com/symfony/swiftmailer-bundle
.. _`创建消息`: https://swiftmailer.symfony.com/docs/messages.html
.. _`Mandrill`: https://mandrill.com/
.. _`SendGrid`: https://sendgrid.com/
.. _`Amazon SES`: http://aws.amazon.com/ses/
.. _`生成应用密码`: https://support.google.com/accounts/answer/185833
.. _`允许安全性较低的应用访问你的Gmail帐户`: https://support.google.com/accounts/answer/6010255
.. _`RFC 3986`: https://www.ietf.org/rfc/rfc3986.txt
