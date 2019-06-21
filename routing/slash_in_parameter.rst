.. index::
   single: Routing; Allow / in route parameter

.. _routing/slash_in_parameter:

如何允许在路由参数中使用“/”字符
=================================================

有时，你需要使用包含一个斜杠 ``/`` 的参数来组合URL。
例如，想象 ``/share/{token}`` 路由。如果 ``token`` 值包含 ``/`` 字符，则此路由将不匹配。
这是因为Symfony使用该字符作为路由区块之间的分隔符。

本文介绍如何修改路由定义，以便占位符也可以包含 ``/`` 字符。

配置路由
-------------------

默认情况下，Symfony Routing组件要求参数与以下正则表达式匹配：``[^/]+``。
这意味着允许除了 ``/`` 之外的所有字符。

你必须通过为占位符指定更宽松的正则表达式来明确允许 ``/`` 成为它的一部分：

.. configuration-block::

    .. code-block:: php-annotations

        use Symfony\Component\Routing\Annotation\Route;

        class DefaultController
        {
            /**
             * @Route("/share/{token}", name="share", requirements={"token"=".+"})
             */
            public function share($token)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        share:
            path:       /share/{token}
            controller: App\Controller\DefaultController::share
            requirements:
                token: .+

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="share" path="/share/{token}" controller="App\Controller\DefaultController::share">
                <requirement key="token">.+</requirement>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\DefaultController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('share', '/share/{token}')
                ->controller([DefaultController::class, 'share'])
                ->requirements([
                    'token' => '.+',
                ])
            ;
        };

仅此而已！现在 ``{token}`` 参数可以包含 ``/`` 字符了。

.. note::

    如果路由包含特殊的 ``{_format}`` 占位符，则不应对参数使用 ``.+`` 要求以允许斜杠。
    例如，如果模式是 ``/share/{token}.{_format}`` 并且 ``{token}`` 允许任何字符，
    则 ``/share/foo/bar.json`` URL中 ``foo/bar.json`` 会被视为令牌，而格式部分将为空。
    这可以通过将 ``.+`` 要求替换为 ``[^.]+`` 以允许除点之外的任何字符来解决 。

.. note::

    如果路由定义了多个占位符，然后你将该正则表达式应用于所有占位符，那么结果将不是你所预期的那样。
    例如，如果路由定义是 ``/share/{path}/{token}``，并且
    ``path`` 和 ``token`` 都接受 ``/``，那么 ``path`` 将包含自身的内容和令牌，并且 ``token`` 将是空的。
