.. index::
    single: DependencyInjection; ExpressionLanguage
    single: DependencyInjection; Expressions
    single: Service Container; ExpressionLanguage
    single: Service Container; Expressions

如何基于复杂表达式来注入值
=================================================

服务容器还支持一个“表达式”，允许你将非常特别的值注入一个服务。

例如，假设你有一个名为 ``App\Mail\MailerConfiguration``
的服务（此处未显示），该服务上有一个 ``getMailerMethod()`` 方法。
它会返回一个字符串 - 就像基于某些配置的 ``sendmail`` 一样。

假设你要将此方法的结果作为构造函数参数传递给另一个服务：``App\Mailer``。一种方法是使用表达式：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Mail\MailerConfiguration: ~

            App\Mailer:
                arguments: ["@=service('App\\\\Mail\\\\MailerConfiguration').getMailerMethod()"]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="App\Mail\MailerConfiguration"></service>

                <service id="App\Mailer">
                    <argument type="expression">service('App\\Mail\\MailerConfiguration').getMailerMethod()</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mail\MailerConfiguration;
        use App\Mailer;
        use Symfony\Component\ExpressionLanguage\Expression;

        $container->autowire(MailerConfiguration::class);

        $container->autowire(Mailer::class)
            ->addArgument(new Expression('service("App\\\\Mail\\\\MailerConfiguration").getMailerMethod()'));

要了解有关表达式语言语法的更多信息，请参阅 :doc:`/components/expression_language/syntax`。

在此上下文中，你可以访问2个函数：

``service``
    返回一个给定的服务（请参阅上面的示例）。
``parameter``
    返回一个特定的参数值（语法就像 ``service``）。

你还可以通过一个 ``container`` 变量来访问
:class:`Symfony\\Component\\DependencyInjection\\Container`。这是另一个例子：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Mailer:
                arguments: ["@=container.hasParameter('some_param') ? parameter('some_param') : 'default_value'"]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Mailer">
                    <argument type="expression">container.hasParameter('some_param') ? parameter('some_param') : 'default_value'</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mailer;
        use Symfony\Component\ExpressionLanguage\Expression;

        $container->autowire(Mailer::class)
            ->addArgument(new Expression(
                "container.hasParameter('some_param') ? parameter('some_param') : 'default_value'"
            ));

表达式可以在 ``arguments``、``properties`` 中使用，作为
``configurator`` 的参数和 ``calls`` （方法调用）的参数。
