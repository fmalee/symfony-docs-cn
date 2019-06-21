.. index::
    single: DependencyInjection; Method Calls

服务方法调用和Setter注入
=========================================

.. tip::

    如果你使用自动装配，可以使用 ``@required`` 来 :ref:`自动的配置方法调用 <autowiring-calls>`。

通常，你需要通过构造函数注入依赖。
但有时，特别是如果依赖是可选的，你可能需要使用“setter注入”。例如::

    namespace App\Service;

    use Psr\Log\LoggerInterface;

    class MessageGenerator
    {
        private $logger;

        public function setLogger(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }

        // ...
    }

要配置容器以能调用 ``setLogger`` 方法，请使用 ``calls`` 键：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Service\MessageGenerator:
                # ...
                calls:
                    - method: setLogger
                      arguments:
                          - '@logger'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Service\MessageGenerator">
                    <!-- ... -->
                    <call method="setLogger">
                        <argument type="service" id="logger"/>
                    </call>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Service\MessageGenerator;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register(MessageGenerator::class)
            ->addMethodCall('setLogger', [new Reference('logger')]);
