启用源映射
====================

`源映射`_ 允许浏览器访问与某些资产关联的原始代码（例如，编译为CSS的Sass代码或编译为JavaScript的TypeScript代码）。
源映射对于调试目标很有用，但不必要在生产中执行该应用。

Encore的默认 ``webpack.config.js`` 文件设置为在 ``dev`` 中构建时启用源映射：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...

        .enableSourceMaps(!Encore.isProduction())
    ;

.. _`源映射`: https://developer.mozilla.org/en-US/docs/Tools/Debugger/How_to/Use_a_source_map
