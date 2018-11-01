配置Babel
=================

通过具有合理的默认值（例如，使用 ``env`` 预设和已请求的 ``react``）的 ``babel-loader``，
`Babel`_ 自动配置所有的 ``.js`` 和 ``.jsx`` 文件。

需要进一步扩展Babel配置吗？最简单的方法是 ``configureBabel()``：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...

        // 首先，安装任何你需要的预设（如 yarn add babel-preset-es2017）
        // 然后，编辑默认的Babel配置
        .configureBabel(function(babelConfig) {
            // 添加额外预设
            babelConfig.presets.push('es2017');

            // 默认没有添加任何插件，但是你可以添加一些
            // babelConfig.plugins.push('styled-jsx/babel');
        })
    ;

创建 .babelrc 文件
------------------------

你可以在项目的根目录中创建 ``.babelrc`` 文件，而不是调用 ``configureBabel()``。
这是配置Babel的更“标准”的方式，但它有一个缺点：
只要 ``.babelrc`` 文件存在， **Encore就不能再为你添加任何Babel配置**。
例如，如果你调用 ``Encore.enableReactPreset()``，
``react`` 预设将 *不会* 自动添加到Babel：你必须在  ``.babelrc`` 中自己添加。

一个示例 ``.babelrc`` 文件可能如下所示：

.. code-block:: json

    {
        presets: [
            ['env', {
                modules: false,
                targets: {
                    browsers: '> 1%',
                    uglify: true
                },
                useBuiltIns: true
            }]
        ]
    }

.. _`Babel`: http://babeljs.io/
