.. index::
   single: Security; Restrict Security Firewalls to a Request

如何将防火墙限制到一个请求
======================================

使用安全组件时，防火墙将根据请求匹配器的结果决定是否处理请求：匹配该请求的第一个防火墙将处理它。

最后一个防火墙可以配置为不使用任何匹配器来处理每个传入请求。

按配置限制
----------------------------

大多数情况下，你不需要自己创建匹配器，因为Symfony可以根据防火墙配置为你执行此操作。

.. note::

    你可以单独使用以下任何限制，也可以将它们混合在一起以获得所需的防火墙配置。

按路径限制
~~~~~~~~~~~~~~~~~~~

这是默认限制，并限制仅在请求路径与 ``pattern`` 配置的路径匹配时才初始化防火墙。

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
在此示例中，仅当一个路径以 ``/admin`` 开头时（由于 ``^`` 正则表达式字符），才会激活该防火墙。
如果路径与此模式不匹配，则不会激活防火墙，并且后续防火墙将有机会匹配此请求。

按主机限制
~~~~~~~~~~~~~~~~~~~

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

按HTTP方法限制
~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

按服务限制
----------------------

如果上面的选项不符合你的需求，你可以将实现
:class:`Symfony\\Component\\HttpFoundation\\RequestMatcherInterface`
的任何服务配置为 ``request_matcher``。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml

        # ...
        security:
            firewalls:
                secured_area:
                    request_matcher: app.firewall.secured_area.request_matcher
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
                <firewall name="secured_area" request-matcher="app.firewall.secured_area.request_matcher">
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
                    'request_matcher' => 'app.firewall.secured_area.request_matcher',
                    // ...
                ],
            ],
        ]);
