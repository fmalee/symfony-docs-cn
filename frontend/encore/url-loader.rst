使用Webpack URL Loader在CSS中内联文件
=============================================

提高Web应用性能的一种简单技术是在生成的CSS文件中内联(inlining)小文件为base64编码URL，从而减少HTTP请求的次数。

Webpack Encore通过Webpack的 `URL Loader`_ 插件提供此功能，但默认情况下已禁用。
首先，将URL loader添加到项目中：

.. code-block:: terminal

    $ yarn add --dev url-loader

然后在你的 ``webpack.config.js`` 中启用它：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .configureUrlLoader({
            fonts: { limit: 4096 },
            images: { limit: 4096 }
        })
    ;

``limit`` 选项定义内联文件的最大大小（以字节为单位）。
在前面的示例中，将内联小于或等于4KB的字体和图像文件，其余文件则照常处理。

你还可以使用 `URL Loader`_ 支持的所有其他选项。
如果要为图像或字体禁用此加载器，请从传递给 ``configureUrlLoader()`` 方法的对象中删除相应的键：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .configureUrlLoader({
            // 'fonts' 没有定义, 所以只有图像会内联
            images: { limit: 4096 }
        })
    ;

.. _`URL loader`: https://github.com/webpack-contrib/url-loader
