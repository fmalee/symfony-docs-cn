CSS预处理器：Sass、LESS等
===================================

使用Sass
----------

要使用Sass预处理器，请安装该依赖：

.. code-block:: terminal

    $ yarn add --dev sass-loader node-sass

并在 ``webpack.config.js`` 中启用它：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .enableSassLoader()
    ;

仅此而已！所有以 ``.sass`` 或 ``.scss`` 结尾的文件将被预处理。
你还可以选项传递到 ``sass-loader``：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .enableSassLoader(function(options) {
            // https://github.com/sass/node-sass#options
            // options.includePaths = [...]
        });
    ;

使用LESS
----------

要使用LESS预处理器，请安装该依赖：

.. code-block:: terminal

    $ yarn add --dev less-loader less

并在 ``webpack.config.js`` 中启用它：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .enableLessLoader()
    ;

仅此而已！所有以 ``.less`` 结尾的文件将被预处理。
你还可以选项传递到 ``less-loader``：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .enableLessLoader(function(options) {
            // https://github.com/webpack-contrib/less-loader#examples
            // http://lesscss.org/usage/#command-line-usage-options
            // options.relativeUrls = false;
        });
    ;
