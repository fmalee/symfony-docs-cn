启用Vue.js (vue-loader)
============================

想要使用 `Vue.js`_？没问题！首先，安装Vue和一些依赖：

.. code-block:: terminal

    $ yarn add --dev vue vue-loader@^14 vue-template-compiler

然后，在你的 ``webpack.config.js`` 中激活 ``vue-loader``：

.. code-block:: diff

    // webpack.config.js
    // ...

    Encore
        // ...
        .addEntry('main', './assets/main.js')

    +     .enableVueLoader()
    ;

仅此而已！你引入的任何 ``.vue`` 文件都将得到正确处理。
你还可以通过回调配置 `vue-loader选项`_：

.. code-block:: javascript

    .enableVueLoader(function(options) {
        // https://vue-loader.vuejs.org/options.html

        options.preLoaders = {
            js: '/path/to/custom/loader'
        };
    });

热模块更换 (HMR)
----------------------------

``vue-loader`` 支持热模块更换：只需更新你的代码，*无需* 刷新浏览器就能监控Vue.js应用更新！
要激活它，请使用带 ``--hot`` 选项的 ``dev-server``：

.. code-block:: terminal

    $ ./node_modules/.bin/encore dev-server --hot

仅此而已！更改其中一个 ``.vue`` 文件并观看浏览器更新。
但请注意：该特性并 *不* 支持 ``.vue`` 文件中的 *样式* 变更。查看更新的样式仍需要刷新页面。

有关更多详细信息，请参阅 :doc:`/frontend/encore/dev-server`。

.. _`babel-preset-react`: https://babeljs.io/docs/plugins/preset-react/
.. _`Vue.js`: https://vuejs.org/
.. _`vue-loader选项`: https://vue-loader.vuejs.org/options.html
