.. index::
   single: Sessions

使用Symfony会话桥接遗留应用
=================================================

如果你将Symfony全栈框架集成到使用 ``session_start()``
来启动会话的遗留应用中，你仍可以使用PHP桥接会话来使用Symfony的会话管理。

如果应用有自己的PHP保存处理器，则可以将 ``handler_id`` 指定为 ``null``：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            session:
                storage_id: session.storage.php_bridge
                handler_id: ~

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:session storage-id="session.storage.php_bridge"
                    handler-id="null"
                />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            'session' => array(
                'storage_id' => 'session.storage.php_bridge',
                'handler_id' => null,
            ),
        ));

否则，如果问题仅在于你无法避免该应用使用 ``session_start()``
来启动会话，你仍然可以通过指定保存处理器来使用Symfony的基于会话的保存处理器，如下例所示：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            session:
                storage_id: session.storage.php_bridge
                handler_id: session.handler.native_file

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:session storage-id="session.storage.php_bridge"
                    handler-id="session.storage.native_file"
                />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            'session' => array(
                'storage_id' => 'session.storage.php_bridge',
                'handler_id' => 'session.storage.native_file',
            ),
        ));

.. note::

    如果遗留应用需要自己的会话保存处理器，请不要重写它，而是设置 ``handler_id: ~``。
    请注意，一旦启动会话后，就无法更换一个保存处理器。
    如果该应用在Symfony初始化之前就启动会话，则保存处理器已经被设置。
    在这种情况下，你就需要 ``handler_id: ~`` 设置。
    如果你确定遗留应用可以没有副作用的使用Symfony保存处理器，并且在Symfony初始化之前尚未启动会话，那么才可以重写保存处理器。

有关更多详细信息，请参阅 :doc:`/components/http_foundation/session_php_bridge`。
