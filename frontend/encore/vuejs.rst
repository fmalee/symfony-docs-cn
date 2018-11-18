启用Vue.js (vue-loader)
============================

想要使用 `Vue.js`_？没问题！首先，在 ``webpack.config.js`` 中启用它：

.. code-block:: diff

    // webpack.config.js
    // ...

    Encore
        // ...
        .addEntry('main', './assets/main.js')

    +     .enableVueLoader()
    ;

然后重启Encore。执行此操作时，它将为你提供一个命令，你可以运行该命令来安装任何缺少的依赖。
运行该命令并重新启动Encore后，你就完工了！

你引入的任何 ``.vue`` 文件都将被正确处理。
你还可以通过将选项回调传递给 ``enableVueLoader()`` 来配置 `vue-loader选项`_。
有关详细文档，请参阅 `Encore的index.js文件`_。

热模块更换 (HMR)
----------------------------

``vue-loader`` 支持热模块更换：只需更新你的代码，*无需* 刷新浏览器就能监控Vue.js应用更新！
要激活它，请使用带 ``--hot`` 选项的 ``dev-server``：

.. code-block:: terminal

    $ yarn encore dev-server --hot

仅此而已！更改其中一个 ``.vue`` 文件并观看浏览器更新。
但请注意：该特性并 *不* 支持 ``.vue`` 文件中的 *样式* 变更。查看更新的样式仍需要刷新页面。

有关更多详细信息，请参阅 :doc:`/frontend/encore/dev-server`。

.. _`babel-preset-react`: https://babeljs.io/docs/plugins/preset-react/
.. _`Vue.js`: https://vuejs.org/
.. _`vue-loader选项`: https://vue-loader.vuejs.org/options.html
.. _`Encore的index.js文件`: https://github.com/symfony/webpack-encore/blob/master/index.js
