.. index::
    single: DependencyInjection; Advanced configuration

如何创建服务别名并将服务标记为私有
==========================================================

.. _container-private-services:

将服务标记为公有/私有
------------------------------------

定义服务时，可以将其设为 *公有* 或 *私有*。如果服务是 *公有* 的，则意味着你可以在运行时直接从容器访问它。
例如，``doctrine`` 服务是一项公有服务::

    // 只有共有服务才可以通过这种方式访问
    $doctrine = $container->get('doctrine');

但通常使用 :ref:`依赖注入 <services-constructor-injection>` 来访问服务。
在这种情况下，那些业务并 *不* 需要公开。

.. _inlined-private-services:

因此，除非你 *特别* 需要通过 ``$container->get()``
从容器直接访问服务，否则最佳做法是将你的服务设为 *私有*。
实际上，:ref:`默认的services.yaml配置 <container-public>` 默认将所有服务配置为私有。

你还可以逐个服务地控制 ``public`` 选项：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Service\Foo:
                public: false

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Service\Foo" public="false" />
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Service\Foo;

        $container->register(Foo::class)
            ->setPublic(false);

.. _services-why-private:

私有服务是特殊的，因为它们允许容器对其进行优化：要不要实例化以及如何实例化。这种行为增加了容器的性能。
它还为你提供了更好的错误处理：
如果你尝试引用不存在的服务，则在刷新 *任何* 页面时都会显示一个明显的错误，即在该页面上没有运行有问题的代码。

既然服务是私有的，你就 *不能* 直接从容器中获取相关服务：

    use App\Service\Foo;

    $container->get(Foo::class);

简单地说：如果你不想直接从代码中访问一个服务，则可以将该服务标记为私有。

但是，如果某个服务已被标记为私有，你仍可以对其设置别名（请参阅下文）以访问此服务（通过别名）。

.. _services-alias:

别名
--------

你有时可能希望使用快捷方式来访问某些服务。
你可以通过对它们设置别名来实现，此外，你甚至可以为非公有服务添加别名。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...
            App\Mail\PhpMailer:
                public: false

            app.mailer:
                alias: App\Mail\PhpMailer
                public: true

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Mail\PhpMailer" public="false" />

                <service id="app.mailer" alias="App\Mail\PhpMailer" />
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mail\PhpMailer;

        $container->register(PhpMailer::class)
            ->setPublic(false);

        $container->setAlias('app.mailer', PhpMailer::class);

这意味着当直接使用容器时，你可以像这样通过询问 ``app.mailer`` 服务来访问 ``PhpMailer`` 服务::

    $container->get('app.mailer'); // 将会返回一个 PhpMailer 实例

.. tip::

    在YAML中，你还可以使用快捷方式对一个服务设置别名：

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...
            app.mailer: '@App\Mail\PhpMailer'

匿名服务
------------------

.. note::

    只有XML和YAML配置格式支持匿名服务。

.. versionadded:: 3.3
    在Symfony 3.3中引入了在YAML中配置匿名服务的功能。

在某些情况下，你可能希望阻止将一个服务用作其他服务的依赖。这可以通过创建一个匿名服务来实现。
这些服务与常规服务类似，但它们不会定义一个ID，而是在它们被使用的地方才创建它们。

以下示例显示如何将一个匿名服务注入另一个服务：

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yaml
        services:
            App\Foo:
                arguments:
                    - !service
                        class: App\AnonymousBar

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="foo" class="App\Foo">
                    <argument type="service">
                        <service class="App\AnonymousBar" />
                    </argument>
                </service>
            </services>
        </container>

使用匿名服务作为一个工厂看起来像这样：

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yaml
        services:
            App\Foo:
                factory: [ !service { class: App\FooFactory }, 'constructFoo' ]

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="foo" class="App\Foo">
                    <factory method="constructFoo">
                        <service class="App\FooFactory"/>
                    </factory>
                </service>
            </services>
        </container>

弃用服务
--------------------

一旦你决定弃用一个服务（因为它已过时或你决定不再维护它），你可以增加一个弃用定义：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        App\Service\OldService:
            deprecated: The "%service_id%" service is deprecated since 2.8 and will be removed in 3.0.

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-Instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Service\OldService">
                    <deprecated>The "%service_id%" service is deprecated since 2.8 and will be removed in 3.0.</deprecated>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Service\OldService;

        $container
            ->register(OldService::class)
            ->setDeprecated(
                true,
                'The "%service_id%" service is deprecated since 2.8 and will be removed in 3.0.'
            )
        ;

现在，每次使用此服务时，都会触发一个弃用警告，建议你停止或更改对该服务的使用。

该消息实际上是一个消息模板，它用对应服务的id来替换出现的 ``%service_id%`` 占位符。
你的模板中 **必须** 至少出现一次 ``%service_id%`` 占位符。

.. note::

    弃用消息是可选的。如果未设置，Symfony将显示此默认消息：``The "%service_id%"
    service is deprecated. You should stop using it, as it will soon be removed.``

.. tip::

    强烈建议你定义一个自定义消息，因为默认消息太通用。
    一条好的消息会告知该服务何时被弃用，直到它将被维护以及使用了替代服务（如果有的话）。

对于服务装饰器（请参阅 :doc:`/service_container/service_decoration`），
如果该定义未改变已弃用的状态，则它将从装饰的定义继承状态。
