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

JSX支持
-----------

你可以通过配置 ``.enableVueLoader()`` 方法的第二个参数来启用 `JSX with Vue.js`_：

.. code-block:: diff

    // webpack.config.js
    // ...

    Encore
        // ...
        .addEntry('main', './assets/main.js')

    -     .enableVueLoader()
    +     .enableVueLoader(() => {}, {
    +         useJsx: true
    +     })
    ;

接下来，运行或重新启动Encore。执行此操作时，你将看到一条错误消息，它会帮助你安装任何缺少的依赖。
运行该命令并重新启动Encore后，也就完工了！

现在将通过 ``@vue/babel-preset-jsx`` 来转换你的 ``.jsx`` 文件。

使用样式
~~~~~~~~~~~~

你不能在 ``.jsx`` 文件中使用 ``<style>``。作为一种变通方法，你可以手动导入
``.css``、``.scss`` 等手动文件：

.. code-block:: javascript

    // App.jsx

    import './App.css'

    export default {
        name: 'App',
        render() {
            return (
                <div>
                    ...
                </div>
            )
        }
    }

.. note::

    以这种方式导入样式会使它们成为全局。请参阅下一节，了解它们与你的组件的范围。

使用范围样式
~~~~~~~~~~~~~~~~~~~

你不能在 ``.jsx`` 文件中使用 `范围样式`_
(``<style scoped>``) 。作为一种变通方法，你可以在使用 `CSS Modules`_ 时后缀
``?module`` 来导入路径：

.. code-block:: javascript

    // Component.jsx

    import styles from './Component.css?module' // 使用 "?module" 后缀

    export default {
        name: 'Component',
        render() {
            return (
                <div>
                    <h1 class={styles.title}>
                        Hello World
                    </h1>
                </div>
            )
        }
    }

.. code-block:: css

    /* Component.css */

    .title {
        color: red
    }

输出将是类似于 ``<h1 class="title_a3dKp">Hello World</h1>``。

使用图像
~~~~~~~~~~~~

你不能在 ``.jsx`` 文件中使用 ``<img src="./image.png">``。作为一种变通方法，你可以使用
``require()`` 函数来导入它们：

.. code-block:: javascript

    export default {
        name: 'Component',
        render() {
            return (
                <div>
                    <img src={require("./image.png")}/>
                </div>
            )
        }
    }

.. _`babel-preset-react`: https://babeljs.io/docs/plugins/preset-react/
.. _`Vue.js`: https://vuejs.org/
.. _`vue-loader选项`: https://vue-loader.vuejs.org/options.html
.. _`Encore的index.js文件`: https://github.com/symfony/webpack-encore/blob/master/index.js
.. _`JSX with Vue.js`: https://github.com/vuejs/jsx
.. _`范围样式`: https://vue-loader.vuejs.org/guide/scoped-css.html
.. _`CSS Modules`: https://github.com/css-modules/css-modules
