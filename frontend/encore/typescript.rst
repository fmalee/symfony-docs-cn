启用TypeScript (ts-loader)
===============================

想要使用 `TypeScript`_？没问题！首先，启用它：

.. code-block:: diff

    // webpack.config.js
    // ...

    Encore
        // ...
    +     .addEntry('main', './assets/main.ts')

    +     .enableTypeScriptLoader()

        // 可选择启用forked类型脚本以实现更快的构建
        // https://www.npmjs.com/package/fork-ts-checker-webpack-plugin
        // 要求你具有正确设置的tsconfig.json文件。
    +     //.enableForkedTypeScriptTypesChecking()
    ;

然后重启Encore。执行此操作时，它将为你提供一个命令，你可以运行该命令来安装任何缺少的依赖。
运行该命令并重新启动Encore后，你就完工了！

你引入的任何 ``.ts`` 文件都将被正确处理。
你还可以通过 ``enableTypeScriptLoader()`` 方法配置 `ts-loader选项`_。
有关详细文档，请参阅 `Encore的index.js文件`_。

如果启用了React（``.enableReactPreset()``），任何 ``.tsx`` 文件也将由 ``ts-loader`` 处理。

.. _`TypeScript`: https://www.typescriptlang.org/
.. _`ts-loader选项`: https://github.com/TypeStrong/ts-loader#options
.. _`fork-ts-checker-webpack-plugin`: https://www.npmjs.com/package/fork-ts-checker-webpack-plugin
.. _`Encore的index.js文件`: https://github.com/symfony/webpack-encore/blob/master/index.js
