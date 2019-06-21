PostCSS和autoprefixing(postcss-loader)
==========================================

PostCSS是CSS后期处理工具，可以以很多很酷的方式转换(transform)你的CSS，
像 `autoprefixing`_, `linting`_ 以及更多！

首先，下载 ``postcss-loader`` 和你想要的任何插件，如 ``autoprefixer``：

.. code-block:: terminal

    $ yarn add postcss-loader autoprefixer --dev

接下来，在项目的根目录下创建一个 ``postcss.config.js`` 文件：

.. code-block:: javascript

    module.exports = {
        plugins: {
            // include whatever plugins you want
            // but make sure you install these via yarn or npm!

            // add browserslist config to package.json (see below)
            autoprefixer: {}
        }
    }

然后，在Encore中启用该加载器！

.. code-block:: diff

    // webpack.config.js

    Encore
        // ...
    +     .enablePostCssLoader()
    ;

因为你刚刚修改了 ``webpack.config.js``，所以需要停止并重新启动Encore。

仅此而已！该 ``postcss-loader`` 现在将用于所有的CSS、Sass等文件。
你还可以通过传递回调将选项传递给 `postcss-loader`_：

.. code-block:: diff

    // webpack.config.js

    Encore
        // ...
    +     .enablePostCssLoader((options) => {
    +         options.config = {
    +             // the directory where the postcss.config.js file is stored
    +             path: 'path/to/config'
    +         };
    +     })
    ;

.. _browserslist_package_config:

添加browserslist到package.json
-----------------------------------

``autoprefixer`` （以及许多其他工具）需要知道你要支持哪些浏览器。
最佳做法是直接在你的 ``package.json`` 中配置（以便所有工具都能读取此内容）：

.. code-block:: diff

    {
    +  "browserslist": [
    +    "> 0.5%",
    +    "last 2 versions",
    +    "Firefox ESR",
    +    "not dead"
    +  ]
    }

有关语法的更多详细信息，请参阅 `browserslist`_ 。

.. note::

    Encore使用的是 `babel-preset-env`_，它 *同样* 需要知道你要支持哪些浏览器。
    但它不会读取 ``browserslist`` 配置键。
    所以你必须通过 :doc:`configureBabel() </frontend/encore/babel>` 单独配置浏览器。

.. _`PostCSS`: http://postcss.org/
.. _`autoprefixing`: https://github.com/postcss/autoprefixer
.. _`linting`: https://stylelint.io/
.. _`browserslist`: https://github.com/browserslist/browserslist
.. _`babel-preset-env`: https://github.com/babel/babel/tree/master/packages/babel-preset-env
.. _`postcss-loader`: https://github.com/postcss/postcss-loader
