.. index::
   single: Logging
   single: Logging; Exclude HTTP Codes
   single: Monolog; Exclude HTTP Codes

如何配置Monolog以从日志中排除特定的HTTP代码
====================================================================

.. versionadded:: 4.1
    Symfony 4.1和MonologBu​​ndle 3.3中引入了基于状态码排除日志消息的功能。

有时，你的日志会充斥着不需要的HTTP错误，例如403和404。
使用一个 ``fingers_crossed`` 处理器时，你可以根据MonologBu​​ndle配置排除记录这些HTTP代码：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/prod/monolog.yaml
        monolog:
            handlers:
                main:
                    # ...
                    type: fingers_crossed
                    handler: ...
                    excluded_http_codes: [403, 404, { 400: ['^/foo', '^/bar'] }]

    .. code-block:: xml

        <!-- config/packages/prod/monolog.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:monolog="http://symfony.com/schema/dic/monolog"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/monolog
                http://symfony.com/schema/dic/monolog/monolog-1.0.xsd">

            <monolog:config>
                <monolog:handler type="fingers_crossed" name="main" handler="...">
                    <!-- ... -->
                    <monolog:excluded-http-code code="403">
                        <monolog:url>^/foo</monolog:url>
                        <monolog:url>^/bar</monolog:url>
                    </monolog:excluded-http-code>
                    <monolog:excluded-http-code code="404" />
                </monolog:handler>
            </monolog:config>
        </container>

    .. code-block:: php

        // config/packages/prod/monolog.php
        $container->loadFromExtension('monolog', array(
            'handlers' => array(
                'main' => array(
                    // ...
                    'type'                => 'fingers_crossed',
                    'handler'             => ...,
                    'excluded_http_codes' => array(403, 404),
                ),
            ),
        ));
