启用React.js
=================

使用React？首先在 ``webpack.config.js`` 中启用对它的支持：

.. code-block:: terminal

    $ yarn add --dev @babel/preset-react
    $ yarn add react react-dom prop-types

在你的 ``webpack.config.js`` 中启用react：

.. code-block:: diff

    // webpack.config.js
    // ...

    Encore
        // ...
    +     .enableReactPreset()
    ;

然后重启Encore。执行此操作时，它将为你提供一个命令，你可以运行该命令来安装任何缺少的依赖。
运行该命令并重新启动Encore后，你就完工了！

你的 ``.js`` 和 ``.jsx`` 文件现在将通过 ``babel-preset-react`` 转换(transform)。

.. _`babel-preset-react`: https://babeljs.io/docs/plugins/preset-react/
