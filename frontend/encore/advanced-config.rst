高级Webpack配置
=======================

Encore使用你的 ``webpack.config.js`` 文件来生成Webpack配置。
Encore不支持添加所有Webpack的 `配置选项`_，因为你可以轻松添加很多自己的配置。

例如，假设你需要设置Webpack的 `watchOptions`_ 设置。为此，请在从Encore获取配置后修改它：

.. code-block:: javascript

    // webpack.config.js

    var Encore = require('@symfony/webpack-encore');

    // ... all Encore config here

    // 获取配置，然后修改它!
    var config = Encore.getWebpackConfig();
    config.watchOptions = { poll: true, ignored: /node_modules/ };

    // 其他示例: 添加一个别名或扩展
    // config.resolve.alias.local = path.resolve(__dirname, './resources/src');
    // config.resolve.extensions.push('json');

    // 导出最终配置
    module.exports = config;

但要注意不要意外覆盖Encore的任何配置：

.. code-block:: javascript

    // webpack.config.js
    // ...

    // GOOD - this modifies the config.resolve.extensions array
    config.resolve.extensions.push('json');

    // BAD - this replaces any extensions added by Encore
    // config.resolve.extensions = ['json'];

定义多个Webpack配置
----------------------------------------

Webpack支持传递一个 `配置数组`_，这些配置是并行处理的。
Webpack Encore包含一个 ``reset()`` 对象，允许重置当前配置的状态以生成新配置：

.. code-block:: javascript

    // define the first configuration
    Encore
        .setOutputPath('public/build/')
        .setPublicPath('/build')
        .addEntry('app', './assets/js/app.js')
        .addStyleEntry('global', './assets/css/global.scss')
        .enableSassLoader()
        .autoProvidejQuery()
        .enableSourceMaps(!Encore.isProduction())
    ;

    // build the first configuration
    const firstConfig = Encore.getWebpackConfig();

    // Set a unique name for the config (needed later!)
    firstConfig.name = 'firstConfig';

    // reset Encore to build the second config
    Encore.reset();

    // define the second configuration
    Encore
        .setOutputPath('public/build/')
        .setPublicPath('/build')
        .addEntry('mobile', './assets/js/mobile.js')
        .addStyleEntry('mobile', './assets/css/mobile.less')
        .enableLessLoader()
        .enableSourceMaps(!Encore.isProduction())
    ;

    // build the second configuration
    const secondConfig = Encore.getWebpackConfig();

    // Set a unique name for the config (needed later!)
    secondConfig.name = 'secondConfig';

    // export the final configuration as an array of multiple configurations
    module.exports = [firstConfig, secondConfig];

运行Encore时，两个配置将并行生成。如果你更喜欢单独生成配置，请传递 ``--config-name`` 选项：

.. code-block:: terminal

    $ yarn encore dev --config-name firstConfig

不使用命令行界面生成Webpack配置对象
----------------------------------------------------------------------------------

通常，你可以通过从命令行界面调用Encore来使用你的 ``webpack.config.js`` 文件。
但有时，不使用Encore的工具（例如像 `Karma`_ 这样的测试运行器）可能需要访问生成的Webpack配置。

问题是，如果你尝试在不使用 ``encore`` 命令的情况下生成Webpack配置对象，你将遇到以下错误：

.. code-block:: text

    Error: Encore.setOutputPath() cannot be called yet because the runtime environment doesn't appear to be configured. Make sure you're using the encore executable or call Encore.configureRuntimeEnvironment() first if you're purposely not calling Encore directly.

这条消息背后的原因是Encore在能够创建配置对象之前需要知道一些事情，最重要的是目标的环境。

要解决此问题，你可以使用 ``configureRuntimeEnvironment``。
但必须在要求 ``webpack.config.js`` **之前** 从JavaScript文件中调用此方法。

例如：

.. code-block:: javascript

    const Encore = require('@symfony/webpack-encore');

    // 设置运行时的环境
    Encore.configureRuntimeEnvironment('dev');

    // 获取 Webpack 的配置对象
    const webpackConfig = require('./webpack.config');

如果有需要，你还可以将通常在命令行界面中使用的所有选项传递给该方法：

.. code-block:: javascript

    Encore.configureRuntimeEnvironment('dev-server', {
        // 使用与CLI工具相同的选项，但它们的名称使用驼峰命名法。
        https: true,
        keepPublicPath: true,
    });

.. _`配置选项`: https://webpack.js.org/configuration/
.. _`watchOptions`: https://webpack.js.org/configuration/watch/#watchoptions
.. _`配置数组`: https://github.com/webpack/docs/wiki/configuration#multiple-configurations
.. _`Karma`: https://karma-runner.github.io
