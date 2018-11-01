启用源映射
====================

`源映射`_ 允许浏览器访问与某些资产关联的原始代码（例如，编译为CSS的Sass代码或编译为JavaScript的TypeScript代码）。
源映射对于调试目标很有用，但不必要在生产中执行该应用。

Encore仅在开发环境的已编译资产中内联源映射，但你可以使用 ``enableSourceMaps()`` 方法控制该行为：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...

        // this is the default behavior...
        .enableSourceMaps(!Encore.isProduction())
        // ... but you can override it by passing a boolean value
        .enableSourceMaps(true)
    ;

.. _`源映射`: https://developer.mozilla.org/en-US/docs/Tools/Debugger/How_to/Use_a_source_map
