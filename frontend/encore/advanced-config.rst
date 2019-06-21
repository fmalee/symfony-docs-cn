高级Webpack配置
=======================

总结一下，Encore使用你的 ``webpack.config.js`` 文件来生成Webpack配置。
Encore不支持添加所有Webpack的 `配置选项`_，因为你可以添加很多自己的配置。

例如，假设你需要自动解析一个新扩展。为此，请在从Encore获取配置后修改配置：

.. code-block:: javascript

    // webpack.config.js

    var Encore = require('@symfony/webpack-encore');

    // ... 所有的Encore配置

    // 获取配置，然后修改它!
    var config = Encore.getWebpackConfig();

    // 添加一个扩展
    config.resolve.extensions.push('json');

    // 导出最终配置
    module.exports = config;

但要注意不要意外覆盖Encore的任何配置：

.. code-block:: javascript

    // webpack.config.js
    // ...

    // 最佳 - 这会修改 config.resolve.extensions 数组
    config.resolve.extensions.push('json');

    // 糟糕 - 它取代了Encore添加的任何扩展
    // config.resolve.extensions = ['json'];

配置监视选项和轮询
----------------------------------------

Encore提供了 ``configureWatchOptions()`` 方法来配置在运行
``encore dev --watch`` 或 ``encore dev-server`` 时的 `监视选项`_：

.. code-block:: javascript

    Encore.configureWatchOptions(function(watchOptions) {
        // 启用轮询并每250ms检查一次更改
        // 在虚拟机中运行Encore时，轮询很有用
        watchOptions.poll = 250;
    });

定义多个Webpack配置
----------------------------------------

Webpack支持传递一个 `配置数组`_，这些配置是并行处理的。
Webpack Encore包含一个 ``reset()`` 对象，允许重置当前配置的状态以生成新配置：

.. code-block:: javascript

    // 定义第一个配置
    Encore
        .setOutputPath('public/build/first_build/')
        .setPublicPath('/build/first_build')
        .addEntry('app', './assets/js/app.js')
        .addStyleEntry('global', './assets/css/global.scss')
        .enableSassLoader()
        .autoProvidejQuery()
        .enableSourceMaps(!Encore.isProduction())
    ;

    // 构建第一个配置
    const firstConfig = Encore.getWebpackConfig();

    // 为配置设置一个唯一的名称（稍后需要！）
    firstConfig.name = 'firstConfig';

    // 重置Encore以构建第二个配置
    Encore.reset();

    // 定义第二个配置
    Encore
        .setOutputPath('public/build/second_build/')
        .setPublicPath('/build/second_build')
        .addEntry('mobile', './assets/js/mobile.js')
        .addStyleEntry('mobile', './assets/css/mobile.less')
        .enableLessLoader()
        .enableSourceMaps(!Encore.isProduction())
    ;

    // 构建第二个配置
    const secondConfig = Encore.getWebpackConfig();

    // 为配置设置一个唯一的名称（稍后需要！）
    secondConfig.name = 'secondConfig';

    // 将最终配置导出为一个多个配置的数组
    module.exports = [firstConfig, secondConfig];

运行Encore时，两个配置将并行生成。如果你更喜欢单独生成配置，请传递 ``--config-name`` 选项：

.. code-block:: terminal

    $ yarn encore dev --config-name firstConfig

接下来，定义每个构建的输出目录：

.. code-block:: yaml

    # config/packages/webpack_encore.yaml
    webpack_encore:
        output_path: '%kernel.project_dir%/public/default_build'
        builds:
            firstConfig: '%kernel.project_dir%/public/first_build'
            secondConfig: '%kernel.project_dir%/public/second_build'

最后，使用 ``encore_entry_*_tags()`` 函数的第三个可选参数来指定要使用的构建：

.. code-block:: twig

    {# 使用位于 ./public/first_build 中的 entrypoints.json 文件 #}
    {{ encore_entry_script_tags('app', null, 'firstConfig') }}
    {{ encore_entry_link_tags('global', null, 'firstConfig') }}

    {# 使用位于 ./public/second_build 中的 entrypoints.json文件 #}
    {{ encore_entry_script_tags('mobile', null, 'secondConfig') }}
    {{ encore_entry_link_tags('mobile', null, 'secondConfig') }}

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

对加载器规则进行完全控制
----------------------------------------

``configureLoaderRule()`` 方法提供了一种配置Webpack加载器规则（``module.rules``，请参阅
`配置 <https://webpack.js.org/concepts/loaders/#configuration>`_）的简洁方法。

这是一种低级方法。你的所有修改将在加载器规则推送到Webpack之前被应用。
这意味着你可以覆盖Encore提供的默认配置，这可能会破坏一些东西。使用时要小心。

一种用途可能是配置 ``eslint-loader`` 来优化（lint）Vue文件。以下代码是等效的：

.. code-block:: javascript

    // 手动
    const webpackConfig = Encore.getWebpackConfig();

    const eslintLoader = webpackConfig.module.rules.find(rule => rule.loader === 'eslint-loader');
    eslintLoader.test = /\.(jsx?|vue)$/;

    return webpackConfig;

    // 使用 Encore.configureLoaderRule()
    Encore.configureLoaderRule('eslint', loaderRule => {
        loaderRule.test = /\.(jsx?|vue)$/
    });

    return Encore.getWebpackConfig();

以下加载器可以用 ``configureLoaderRule()`` 进行配置：
  - ``javascript`` (alias ``js``)
  - ``css``
  - ``images``
  - ``fonts``
  - ``sass`` (alias ``scss``)
  - ``less``
  - ``stylus``
  - ``vue``
  - ``eslint``
  - ``typescript`` (alias ``ts``)
  - ``handlebars``

.. _`配置选项`: https://webpack.js.org/configuration/
.. _`配置数组`: https://github.com/webpack/docs/wiki/configuration#multiple-configurations
.. _`Karma`: https://karma-runner.github.io
.. _`监视选项`: https://webpack.js.org/configuration/watch/#watchoptions
