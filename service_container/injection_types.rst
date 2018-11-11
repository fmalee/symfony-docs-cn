.. index::
   single: DependencyInjection; Injection types

注入的类型
==================

把一个类的依赖进行显式地和必需地“注入”，是一个很好的实践，这可以令类更能复用、更易测试、相对于其他类的藕合度更低。

有几种方法可以注入依赖。每种注入方式各有优缺点，使用服务容器时，它们各自的执行方式也有所不同。

构造函数注入
---------------------

注入依赖的最常用方法是通过类的构造函数。为此，你需要添加一个参数到构造函数的签名以接受该依赖::

    namespace App\Mail;

    // ...
    class NewsletterManager
    {
        private $mailer;

        public function __construct(MailerInterface $mailer)
        {
            $this->mailer = $mailer;
        }

        // ...
    }

你可以在服务容器配置中指定要注入的服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Mail\NewsletterManager:
                arguments: ['@mailer']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="app.newsletter_manager" class="App\Mail\NewsletterManager">
                    <argument type="service" id="mailer"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mail\NewsletterManager;
        use Symfony\Component\DependencyInjection\Reference;

        // ...
        $container->register('app.newsletter_manager', NewsletterManager::class)
            ->addArgument(new Reference('mailer'));

.. tip::

    对注入的对象进行类型约束，意味着你可以确保一个合适的依赖被注入。
    如果注入了不合适的依赖项，你将立即得到明确的错误信息。
    类型约束一个接口而不是一个类，可以让你更灵活的选择依赖。
    如果你只使用接口中定义的方法，你可以在保证灵活性的同时仍可以安全地使用该对象。

使用构造函数注入有几个优点：

* 如果依赖是必需的，而没有它该类就无法工作，那么通过构造函数注入该依赖可以确保在该类使用它时它就存在，因为没有它就无法构造类。

* 构造函数只在创建对象时被调用一次，因此你可以确保在对象的生命周期内该依赖不会发生改变。

这些优点意味着构造函数注入不适合与“可选的依赖（非必须的依赖）”一起工作。
当类存在继承关系时，这种注入也很困难：如果一个类使用构造函数注入，那么继承它和覆写该构造函数时，会变得充满不确定性。

Setter注入
----------------

另一个可能的注入点是通过添加一个setter方法来接受依赖::

    // ...
    class NewsletterManager
    {
        private $mailer;

        public function setMailer(MailerInterface $mailer)
        {
            $this->mailer = $mailer;
        }

        // ...
    }

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
       services:
            # ...

            app.newsletter_manager:
                class: App\Mail\NewsletterManager
                calls:
                    - [setMailer, ['@mailer']]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="app.newsletter_manager" class="App\Mail\NewsletterManager">
                    <call method="setMailer">
                        <argument type="service" id="mailer" />
                    </call>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mail\NewsletterManager;
        use Symfony\Component\DependencyInjection\Reference;

        // ...
        $container->register('app.newsletter_manager', NewsletterManager::class)
            ->addMethodCall('setMailer', array(new Reference('mailer')));

setter注入的优点是：

* Setter注入适用于可选的依赖。如果你不需要该依赖，则不要调用对应的setter。

* 你可以多次调用setter。如果方法将依赖添加到一个集合，这将特别有用。然后，你可以拥有一个可变数量的依赖。

setter注入的缺点是：

* 相较于在构造时注入，setter可以被调用多次，所以你不能确定该依赖在对象的生命周期之内是否被替换
  （除非在setter方法中显式地添加判断来检查它是否已被调用）。

* 你无法确定setter是否被调用过，因此你需要添加检查以判断任何所需的依赖是否被注入。

属性注入
------------------

另一种可能是直接设置类的公共字段来进行注入::

    // ...
    class NewsletterManager
    {
        public $mailer;

        // ...
    }

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
       services:
            # ...

            app.newsletter_manager:
                class: App\Mail\NewsletterManager
                properties:
                    mailer: '@mailer'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="app.newsletter_manager" class="App\Mail\NewsletterManager">
                    <property name="mailer" type="service" id="mailer" />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mail\NewsletterManager;
        use Symfony\Component\DependencyInjection\Reference;

        // ...
        $container->register('newsletter_manager', NewsletterManager::class)
            ->setProperty('mailer', new Reference('mailer'));

使用属性注入基本只有缺点，它类似于setter注入，但有以下重要问题：

* 你完全不能控制何时依赖会被设置，它可以在对象的生命周期内的任何时间点被改变。

* 你无法使用类型约束，因此无法确定到底注入了什么依赖，除非在类的代码中写入显式的判断，以在使用该依赖之前，对其实例化的对象进行测试。

但是，在服务容器中知道有这样一种注入方式也是有用的，特别是如果你使用的是不受控制的代码，例如有第三方库使用公有属性设置其依赖。
