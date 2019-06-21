.. index::
   single: DependencyInjection; Service configurators

如何使用配置器配置服务
==============================================

*服务配置器* 是服务容器的一个功能，它允许你在一个服务实例化后使用一个可调用对象来配置该服务。

例如，当你的服务需要基于来自不同源/服务的配置设置进行复杂设定时，可以使用一个服务配置器。
使用一个外部配置器，你可以干净地维护服务实现，并使其与提供所需配置的其他对象解耦。

另一个用例是当你拥有多个对象，却彼此共享一个通用配置或者应该在运行时中以相同方式进行配置时。

例如，假设你有一个可以向用户发送不同类型的电子邮件的应用。
该电子邮件被传递给不同的启用或未启用的格式化器，具体取决于某些动态的应用设置。
（Emails are passed through different formatters that could be enabled or not
depending on some dynamic application settings.）
首先像这样定义一个 ``NewsletterManager`` 类::

    // src/Mail/NewsletterManager.php
    namespace App\Mail;

    class NewsletterManager implements EmailFormatterAwareInterface
    {
        private $enabledFormatters;

        public function setEnabledFormatters(array $enabledFormatters)
        {
            $this->enabledFormatters = $enabledFormatters;
        }

        // ...
    }

还有一个 ``GreetingCardManager`` 类::

    // src/Mail/GreetingCardManager.php
    namespace App\Mail;

    class GreetingCardManager implements EmailFormatterAwareInterface
    {
        private $enabledFormatters;

        public function setEnabledFormatters(array $enabledFormatters)
        {
            $this->enabledFormatters = $enabledFormatters;
        }

        // ...
    }

如前所述，目标是根据应用设置以在运行时中设置格式化器。
为此，你还有一个 ``EmailFormatterManager`` 类，它负责在应用中加载和验证已启用的格式化器::

    // src/Mail/EmailFormatterManager.php
    namespace App\Mail;

    class EmailFormatterManager
    {
        // ...

        public function getEnabledFormatters()
        {
            // 用于配置要使用的格式化器的代码
            $enabledFormatters = [...];

            // ...

            return $enabledFormatters;
        }
    }

如果你的目标是避免将 ``NewsletterManager`` 和 ``GreetingCardManager`` 与
``EmailFormatterManager`` 耦合，那么你可能想创建一个配置器类来配置这些实例::

    // src/Mail/EmailConfigurator.php
    namespace App\Mail;

    class EmailConfigurator
    {
        private $formatterManager;

        public function __construct(EmailFormatterManager $formatterManager)
        {
            $this->formatterManager = $formatterManager;
        }

        public function configure(EmailFormatterAwareInterface $emailManager)
        {
            $emailManager->setEnabledFormatters(
                $this->formatterManager->getEnabledFormatters()
            );
        }

        // ...
    }

``EmailConfigurator`` 的工作是将已启用的格式化器注入到 ``NewsletterManager`` 和
``GreetingCardManager`` 中，因为它们不知道已启用的格式化器来自何处。
另一方面，``EmailFormatterManager`` 持有已启用的格式化器的信息以及知道如何加载它们，并保持单一责任原则。

.. tip::

    虽然此示例使用了一个PHP类方法，但配置器可以是任何有效的PHP可调用对象，包括函数、静态方法和服务方法。

使用配置器
----------------------

你可以使用 ``configurator`` 选项来配置服务配置器。
如果你使用的是
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，则所有的类都已作为服务加载。
你需要做的就是指定 ``configurator``：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            # 将所有的4个类注册为服务，包括 App\Mail\EmailConfigurator
            App\:
                resource: '../src/*'
                # ...

            # 重写服务以设置配置器
            App\Mail\NewsletterManager:
                configurator: ['@App\Mail\EmailConfigurator', 'configure']

            App\Mail\GreetingCardManager:
                configurator: ['@App\Mail\EmailConfigurator', 'configure']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <prototype namespace="App\" resource="../src/*"/>

                <service id="App\Mail\NewsletterManager">
                    <configurator service="App\Mail\EmailConfigurator" method="configure"/>
                </service>

                <service id="App\Mail\GreetingCardManager">
                    <configurator service="App\Mail\EmailConfigurator" method="configure"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mail\GreetingCardManager;
        use App\Mail\NewsletterManager;
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        // Same as before
        $definition = new Definition();

        $definition->setAutowired(true);

        $this->registerClasses($definition, 'App\\', '../src/*');

        $container->getDefinition(NewsletterManager::class)
            ->setConfigurator([new Reference(EmailConfigurator::class), 'configure']);

        $container->getDefinition(GreetingCardManager::class)
            ->setConfigurator([new Reference(EmailConfigurator::class), 'configure']);

仅此而已！在请求 ``App\Mail\NewsletterManager`` 或 ``App\Mail\GreetingCardManager``
服务时，已创建的实例将首先传递给 ``EmailConfigurator::configure()`` 方法。
