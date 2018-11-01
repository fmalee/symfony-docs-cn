添加自定义加载器&插件
===============================

添加自定义加载器
---------------------

Encore已经开箱支持了各种不同的加载器，但是如果你想使用一个当前不支持的特定加载器，你可以通过 ``addLoader`` 函数添加自己的加载器。在 该 ``addLoader`` 支持(takes)任何有效的WebPack规则配置。

例如，如果要添加 `handlebars-loader`_，请使用你的loader配置调用 ``addLoader``：

.. code-block:: javascript

    Encore
        // ...
        .addLoader({ test: /\.handlebars$/, loader: 'handlebars-loader' })
    ;

由于加载器配置接受任何有效的Webpack规则对象，因此你可以传递所需的任何额外信息给加载器：

.. code-block:: javascript

    Encore
        // ...
        .addLoader({
            test: /\.handlebars$/,
            loader: 'handlebars-loader',
            options: {
                helperDirs: [
                    __dirname + '/helpers1',
                    __dirname + '/helpers2',
                ],
                partialDirs: [
                    path.join(__dirname, 'templates', 'partials')
                ]
            }
        })
    ;

添加自定义插件
---------------------

Encore在内部使用各种不同的 `插件`_。
但是，你可以通过 ``addPlugin()`` 方法添加自己的插件。
例如，如果你使用 `Moment.js`_，则可能需要使用 `IgnorePlugin`_ （请参阅 `moment/moment#2373`_）：

.. code-block:: diff

    // webpack.config.js
    + var webpack = require('webpack');

    Encore
        // ...

    +     .addPlugin(new webpack.IgnorePlugin(/^\.\/locale$/, /moment$/))
    ;

.. _`handlebars-loader`: https://github.com/pcardune/handlebars-loader
.. _`插件`: https://webpack.js.org/plugins/
.. _`Moment.js`: https://momentjs.com/
.. _`IgnorePlugin`: https://webpack.js.org/plugins/ignore-plugin/
.. _`moment/moment#2373`: https://github.com/moment/moment/issues/2373
