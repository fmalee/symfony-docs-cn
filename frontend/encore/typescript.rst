启用TypeScript (ts-loader)
===============================

想要使用 `TypeScript`_？没问题！首先，安装该依赖：

.. code-block:: terminal

    $ yarn add --dev typescript ts-loader@^3.0

然后，在 ``webpack.config.js`` 中激活 ``ts-loader``：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .addEntry('main', './assets/main.ts')

        .enableTypeScriptLoader()
    ;

而已！.ts您需要的任何文件都将得到正确处理。您还可以通过回调配置ts-loader选项：
That's it! Any ``.ts`` files that you require will be processed correctly. You can
also configure the `ts-loader options`_ via a callback:

.. code-block:: javascript

    .enableTypeScriptLoader(function (typeScriptConfigOptions) {
        typeScriptConfigOptions.transpileOnly = true;
        typeScriptConfigOptions.configFile = '/path/to/tsconfig.json';
    });

如果已启用React资产（``.enableReactPreset()``），则任何 ``.tsx`` 文件都将由 ``ts-loader`` 处理。

有关 ``ts-loader`` 的更多信息，可以在他的 `README`_ 中找到。

使用fork-ts-checker-webpack-plugin更快生成
-------------------------------------------------

通过使用 `fork-ts-checker-webpack-plugin`_，你可以在单独的进程中运行类型检查，这样可以加快编译进程。
要启用它，请安装该插件：

.. code-block:: terminal

    $ yarn add --dev fork-ts-checker-webpack-plugin

然后通过调用启用它：

.. code-block:: javascript

    // webpack.config.js

    Encore
        // ...
        .enableForkedTypeScriptTypesChecking()
    ;

此插件要求你具有一个设置正确的 `tsconfig.json`_ 文件。

.. _`TypeScript`: https://www.typescriptlang.org/
.. _`ts-loader options`: https://github.com/TypeStrong/ts-loader#options
.. _`README`: https://github.com/TypeStrong/ts-loader#typescript-loader-for-webpack
.. _`fork-ts-checker-webpack-plugin`: https://www.npmjs.com/package/fork-ts-checker-webpack-plugin
.. _`tsconfig.json`: https://www.npmjs.com/package/fork-ts-checker-webpack-plugin#modules-resolution
