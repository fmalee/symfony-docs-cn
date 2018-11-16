.. index::
   single: Emails; In development

如何在开发过程中处理邮件
==========================================

在开发一个发送邮件的应用时，你通常不希望在开发期间将邮件实际发送给指定的收件人。
如果你使用默认的Symfony邮件程序，则可以通过配置设置实现此目的，而无需对应用的代码进行任何更改。
在开发过程中处理邮件有两个主要选择：（a）完全禁止发送邮件或（b）将所有邮件发送到指定地址（可配置可选的例外情况）。

禁用发送
-----------------

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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config disable-delivery="true" />
        </container>

    .. code-block:: php

        // config/packages/test/swiftmailer.php
        $container->loadFromExtension('swiftmailer', array(
            'disable_delivery' => "true",
        ));

.. _sending-to-a-specified-address:

发送到特定地址
----------------------------------

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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <swiftmailer:config>
                <swiftmailer:delivery-address>dev@example.com</swiftmailer:delivery-address>
            </swiftmailer:config>
        </container>

    .. code-block:: php

        // config/packages/dev/swiftmailer.php
        $container->loadFromExtension('swiftmailer', array(
            'delivery_addresses' => array("dev@example.com"),
        ));

现在，假设你在一个控制器向 ``recipient@example.com`` 发送邮件::

    public function index($name, \Swift_Mailer $mailer)
    {
        $message = (new \Swift_Message('Hello Email'))
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody(
                $this->renderView(
                    'HelloBundle:Hello:email.txt.twig',
                    array('name' => $name)
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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

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
        $container->loadFromExtension('swiftmailer', array(
            'delivery_addresses' => array("dev@example.com"),
            'delivery_whitelist' => array(
                // all email addresses matching these regexes will be delivered
                // like normal, as well as being sent to dev@example.com
                '/@specialdomain\.com$/',
                '/^admin@mydomain\.com$/',
            ),
        ));

在上面的示例中，所有邮件都将被重定向到 ``dev@example.com``，但是发送到 ``admin@mydomain.com``
地址或属于 ``specialdomain.com`` 域的任何邮件地址的邮件将照常传送。

.. caution::

    除非定义了 ``delivery_addresses`` 选项，否则将忽略 ``delivery_whitelist`` 选项。

从Web调试工具栏查看
----------------------------------

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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/webprofiler
                http://symfony.com/schema/dic/webprofiler/webprofiler-1.0.xsd">

            <webprofiler:config
                intercept-redirects="true"
            />
        </container>

    .. code-block:: php

        // config/packages/dev/web_profiler.php
        $container->loadFromExtension('web_profiler', array(
            'intercept_redirects' => 'true',
        ));

.. tip::

    或者，你可以在重定向后打开分析器，并通过提交上一个请求中使用的URL来进行搜索（例如 ``/contact/handle``）。
    通过分析器的搜索功能，你可以加载任何过去请求的分析器信息。

.. tip::

    除了Symfony提供的功能外，还有一些应用可以帮助你在应用开发期间测试邮件，例如 `MailCatcher`_
    和 `MailHog`_。

.. _`MailCatcher`: https://github.com/sj26/mailcatcher
.. _`MailHog`: https://github.com/mailhog/MailHog
