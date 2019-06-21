.. index::
   single: Logging
   single: Logging; Exclude 404 Errors
   single: Monolog; Exclude 404 Errors

如何配置Monolog把404错误从日志中排除出去
===========================================================

.. tip::

    阅读 :doc:`/logging/monolog_exclude_http_codes`，以了解类似但更通用的功能，
    该功能允许排除任何HTTP状态码的日志，而不仅仅是404错误。

有时，你的日志会充斥着不需要的404 HTTP错误，例如，当攻击者扫描你的应用以寻找一些众所周知的应用路径时（例如 `/phpmyadmin`）。
使用一个 ``fingers_crossed`` 处理器时，你可以根据MonologBu​​ndle配置中的正则表达式来排除记录这些404错误：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                main:
                    # ...
                    type: fingers_crossed
                    handler: ...
                    excluded_404s:
                        - ^/phpmyadmin

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                https://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler type="fingers_crossed" name="main" handler="...">
                    <!-- ... -->
                    <monolog:excluded-404>^/phpmyadmin</monolog:excluded-404>
                </monolog:handler>
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', [
            'handlers' => [
                'main' => [
                    // ...
                    'type'          => 'fingers_crossed',
                    'handler'       => ...,
                    'excluded_404s' => [
                        '^/phpmyadmin',
                    ],
                ],
            ],
        ]);
