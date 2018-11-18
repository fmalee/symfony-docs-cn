CSS预处理器：Sass、LESS、Stylus等
===========================================

要使用Sass、LESS 或 Stylus预处理器，请在 ``webpack.config.js`` 中启用你需要的预处理器：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...

        // 只启用你想要的那个

        // 处理以.scss或.sass结尾的文件
        .enableSassLoader()

        // 处理以.less结尾的文件
        .enableLessLoader()

        // 处理以.styl结尾的文件
        .enableStylusLoader()
    ;

然后重启Encore。执行此操作时，它将为你提供一个命令，你可以运行该命令来安装任何缺少的依赖。
运行该命令并重新启动Encore后，你就完工了！

你还可以将配置选项传递给每个加载器。
有关详细文档，请参阅 `Encore的index.js文件`_。

.. _`Encore的index.js文件`: https://github.com/symfony/webpack-encore/blob/master/index.js
