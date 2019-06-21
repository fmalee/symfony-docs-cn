.. index::
   single: Security; Restrict Security Firewalls to a Request

如何将防火墙限制到一个请求
======================================

使用安全组件时，你可以创建与某些请求选项匹配的防火墙。
在大多数情况下，匹配URL就足够了，但在特殊情况下，你可以针对请求的其他选项进一步限制防火墙的初始化。

.. note::

    你可以单独使用这些限制，也可以将它们混合在一起以获得所需的防火墙配置。

模式限制
----------------------

这是默认限制，并限制仅在请求URL与配置的 ``pattern`` 匹配时才初始化防火墙。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml

        # ...
        security:
            firewalls:
                secured_area:
                    pattern: ^/admin
                    # ...

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" pattern="^/admin">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php

        // ...
        $container->loadFromExtension('security', [
            'firewalls' => [
                'secured_area' => [
                    'pattern' => '^/admin',
                    // ...
                ],
            ],
        ]);

``pattern`` 是一个正则表达式。
在此示例中，仅当一个URL以 ``/admin`` 开头时（由于 ``^`` 正则表达式字符），才会激活该防火墙。
如果URL与此模式不匹配，则不会激活防火墙，并且后续防火墙将有机会匹配此请求。

主机限制
-------------------

如果仅匹配 ``pattern`` 不够满足用例，则还可以匹配请求的 ``host``。
设置 ``host`` 配置选项后，仅当请求中的主机与该配置匹配时，该防火墙将才会初始化。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml

        # ...
        security:
            firewalls:
                secured_area:
                    host: ^admin\.example\.com$
                    # ...

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" host="^admin\.example\.com$">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php

        // ...
        $container->loadFromExtension('security', [
            'firewalls' => [
                'secured_area' => [
                    'host' => '^admin\.example\.com$',
                    // ...
                ],
            ],
        ]);

``host`` 是一个正则表达式（类似于 ``pattern``）。
在此示例中，仅当该主机与主机名 ``admin.example.com``
完全相同（由于 ``^`` 和 ``$`` 正则表达式字符）时，才会激活该防火墙。
如果主机名与此模式不匹配，则不会激活防火墙，并且后续防火墙将有机会匹配此请求。

HTTP方法限制
---------------------------

配置选项 ``methods`` 将防火墙的初始化限制为该选项提供的HTTP方法。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml

        # ...
        security:
            firewalls:
                secured_area:
                    methods: [GET, POST]
                    # ...

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <firewall name="secured_area" methods="GET,POST">
                    <!-- ... -->
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php

        // ...
        $container->loadFromExtension('security', [
            'firewalls' => [
                'secured_area' => [
                    'methods' => ['GET', 'POST'],
                    // ...
                ],
            ],
        ]);

在此示例中，仅当请求的HTTP方法为 ``GET`` 或 ``POST`` 时，才会激活该防火墙。
如果该方法不在允许方法的数组中，则不会激活防火墙，并且后续防火墙将再次有机会匹配此请求。
