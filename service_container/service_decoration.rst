.. index::
    single: Service Container; Decoration

如何装饰服务
========================

重写一个现有定义时，原始服务将会丢失：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Mailer: ~

            # 这将旧的 App\Mailer 定义替换为新的定义，旧的定义将丢失
            App\Mailer:
                class: App\NewMailer

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance"
            xsd:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Mailer"/>

                <!-- this replaces the old App\Mailer definition with the new
                     one, the old definition is lost -->
                <service id="App\Mailer" class="App\NewMailer"/>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mailer;
        use App\NewMailer;

        $container->register(Mailer::class);

        // this replaces the old App\Mailer definition with the new one, the
        // old definition is lost
        $container->register(Mailer::class, NewMailer::class);

大多数时候，这正是你想要做的。但有时，你可能只是想要装饰旧的服务（即应用 `装饰器模式`_）。
在这种情况下，应保留旧服务以便能够在新服务中引用它。
以下配置替换 ``App\Mailer`` 为一个新定义，但保留了旧定义的一个引用（``App\DecoratingMailer.inner``）：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Mailer: ~

            App\DecoratingMailer:
                # 重写 App\Mailer 服务
                # 但该服务仍可作为 App\DecoratingMailer.inner 来使用
                decorates: App\Mailer

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance"
            xsd:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Mailer"/>

                <service id="App\DecoratingMailer"
                    decorates="App\Mailer"
                />

            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\DecoratingMailer;
        use App\Mailer;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register(Mailer::class);

        $container->register(DecoratingMailer::class)
            ->setDecoratedService(Mailer::class)
        ;

``decorates`` 选项告诉容器使用 ``App\DecoratingMailer`` 服务替换 ``App\Mailer`` 服务。
如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么当装饰服务的构造函数使用一个被装饰的服务类的类型约束作为参数时，将自动注入被装饰的服务。

如果你没有使用自动装配，或该装饰的服务拥有多个带有被装饰服务类的类型约束的构造函数参数，
那么就必须显式的注入被装饰的服务（被装饰的服务的ID自动更改为 ``decorating_service_id + '.inner'``）：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Mailer: ~

            App\DecoratingMailer:
                decorates: App\Mailer
                # 传递旧服务为一个参数
                arguments: ['@App\DecoratingMailer.inner']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance"
            xsd:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Mailer"/>

                <service id="App\DecoratingMailer"
                    decorates="App\Mailer"
                >
                    <argument type="service" id="App\DecoratingMailer.inner"/>
                </service>

            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\DecoratingMailer;
        use App\Mailer;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register(Mailer::class);

        $container->register(DecoratingMailer::class)
            ->setDecoratedService(Mailer::class)
            ->addArgument(new Reference(DecoratingMailer::class.'.inner'))
        ;

.. tip::

    被装饰的 ``App\Mailer`` 服务（它是新服务的一个别名）的可见性仍将与原始 ``App\Mailer`` 的可见性相同。

.. note::

    生成的内部id基于装饰器服务的id（此处为
    ``App\DecoratingMailer``），而不是基于被装饰的服务（此处为 ``App\Mailer``）。
    你可以通过 ``decoration_inner_name`` 选项来控制该内部服务的名称：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            services:
                App\DecoratingMailer:
                    # ...
                    decoration_inner_name: App\DecoratingMailer.wooz
                    arguments: ['@App\DecoratingMailer.wooz']

        .. code-block:: xml

            <!-- config/services.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance"
                xsd:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

                <services>
                    <!-- ... -->

                    <service
                        id="App\DecoratingMailer"
                        decorates="App\Mailer"
                        decoration-inner-name="App\DecoratingMailer.wooz"
                        public="false"
                    >
                        <argument type="service" id="App\DecoratingMailer.wooz"/>
                    </service>

                </services>
            </container>

        .. code-block:: php

            // config/services.php
            use App\DecoratingMailer;
            use Symfony\Component\DependencyInjection\Reference;

            $container->register(DecoratingMailer::class)
                ->setDecoratedService(App\Mailer, DecoratingMailer::class.'.wooz')
                ->addArgument(new Reference(DecoratingMailer::class.'.wooz'))
                // ...
            ;

装饰优先级
-------------------

将多个装饰器应用于服务时，你可以使用 ``decoration_priority`` 选项控制其顺序。
它的值是一个默认为 ``0`` 的整数，更高的优先级意味着该装饰器将更早的被应用。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        Foo: ~

        Bar:
            public: false
            decorates: Foo
            decoration_priority: 5
            arguments: ['@Bar.inner']

        Baz:
            public: false
            decorates: Foo
            decoration_priority: 1
            arguments: ['@Baz.inner']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Foo"/>

                <service id="Bar" decorates="Foo" decoration-priority="5" public="false">
                    <argument type="service" id="Bar.inner"/>
                </service>

                <service id="Baz" decorates="Foo" decoration-priority="1" public="false">
                    <argument type="service" id="Baz.inner"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\DependencyInjection\Reference;

        $container->register(Foo::class)

        $container->register(Bar::class)
            ->addArgument(new Reference(Bar::class.'.inner'))
            ->setPublic(false)
            ->setDecoratedService(Foo::class, null, 5);

        $container->register(Baz::class)
            ->addArgument(new Reference(Baz::class.'.inner'))
            ->setPublic(false)
            ->setDecoratedService(Foo::class, null, 1);

生成的代码如下::

    $this->services[Foo::class] = new Baz(new Bar(new Foo()));

.. _装饰器模式: https://en.wikipedia.org/wiki/Decorator_pattern
