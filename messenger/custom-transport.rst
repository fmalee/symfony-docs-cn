如何创建自己的信使传输
==========================================

一旦你编写了传输的发送方和接收方，就可以注册你的传输工厂，以便能够通过Symfony应用中的DSN使用它。

创建运输工厂
-----------------------------

你需要为FrameworkBundle提供从DSN创建传输的机会。你需要一个运输工厂::

    use Symfony\Component\Messenger\Transport\Receiver\ReceiverInterface;
    use Symfony\Component\Messenger\Transport\Sender\SenderInterface;
    use Symfony\Component\Messenger\Transport\TransportFactoryInterface;
    use Symfony\Component\Messenger\Transport\TransportInterface;

    class YourTransportFactory implements TransportFactoryInterface
    {
        public function createTransport(string $dsn, array $options): TransportInterface
        {
            return new YourTransport(/* ... */);
        }

        public function supports(string $dsn, array $options): bool
        {
            return 0 === strpos($dsn, 'my-transport://');
        }
    }

传输对象需要实现 ``TransportInterface``（简单地组合 ``SenderInterface`` 和
``ReceiverInterface``）。它看起来像这样::

    class YourTransport implements TransportInterface
    {
        public function send(Envelope $envelope): Envelope
        {
            // ...
        }

        public function receive(callable $handler): void
        {
            // ...
        }

        public function stop(): void
        {
            // ...
        }
    }

注册你的工厂
---------------------

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            Your\Transport\YourTransportFactory:
                tags: [messenger.transport_factory]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Your\Transport\YourTransportFactory">
                   <tag name="messenger.transport_factory"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Your\Transport\YourTransportFactory;

        $container->register(YourTransportFactory::class)
            ->setTags(['messenger.transport_factory']);

使用你的传输
------------------

在 ``framework.messenger.transports.*`` 配置中，使用你自己的DSN来创建自己命名的传输：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/messenger.yaml
        framework:
            messenger:
                transports:
                    yours: 'my-transport://...'

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
                    <framework:transport name="yours" dsn="my-transport://..."/>
                </framework:messenger>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/messenger.php
        $container->loadFromExtension('framework', [
            'messenger' => [
                'transports' => [
                    'yours' => 'my-transport://...',
                ],
            ],
        ]);

除了能够将你的消息路由到 ``yours`` 发送方之外，还可以访问以下服务：

#. ``messenger.sender.yours``: 发送方；
#. ``messenger.receiver.yours``: 接受方。
