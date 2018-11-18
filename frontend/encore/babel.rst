配置Babel
=================

通过具有合理的默认值（例如，使用 ``@babel/preset-env`` 和已要求的
``@babel/preset-react``）的 ``babel-loader``，
`Babel`_ 自动配置所有的 ``.js`` 和 ``.jsx`` 文件。

需要进一步扩展Babel配置吗？最简单的方法是 ``configureBabel()``：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...

        .configureBabel(function(babelConfig) {
            // 添加额外预设
            // babelConfig.presets.push('@babel/preset-flow');

            // 默认情况下不添加任何插件，但你可以添加一些插件
            // babelConfig.plugins.push('styled-jsx/babel');
        }, {
            // 默认情况下，不通过Babel处理node_modules
            // 但你可以将特定模块列入白名单进行处理
            // include_node_modules: ['foundation-sites']

            // 或者完全控制 exclude
            // exclude: /bower_components/
        })
    ;

配置目标浏览器
---------------------------

``@ babel / preset-env`` 预设会重写你的JavaScript，以便最终语法可以在你任何想要的浏览器中使用。
要配置需要支持的浏览器，请参阅 :ref:`browserslist_package_config`。

更改我们的“browerslist”配置后，你需要手动删除babel缓存目录：

.. code-block:: terminal

    # 在Unix上运行此命令。在Windows上，手动清除此目录
    $ rm -rf node_modules/.cache/babel-loader/

创建 .babelrc 文件
------------------------

你可以在项目的根目录中创建 ``.babelrc`` 文件，而不是调用 ``configureBabel()``。
这是配置Babel的更“标准”的方式，但它有一个缺点：
只要 ``.babelrc`` 文件存在， **Encore就不能再为你添加任何Babel配置**。
例如，如果你调用 ``Encore.enableReactPreset()``，
``react`` 预设将 *不会* 自动添加到Babel：你必须在  ``.babelrc`` 中自己添加。

只要存在一个 ``.babelrc`` 文件，它将优先于Encore添加的Babel配置。

.. _`Babel`: http://babeljs.io/
