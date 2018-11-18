安装Encore（不使用Symfony Flex）
==========================================

.. tip::

    如果你的项目使用Symfony Flex，请阅读 :doc:`/frontend/encore/installation` 以获得更简单的说明。

安装Encore
-----------------

通过Yarn将Encore安装到你的项目中：

.. code-block:: terminal

    $ yarn add @symfony/webpack-encore --dev

.. note::

    如果你更喜欢使用 `npm`_，没问题！运行 ``npm install @symfony/webpack-encore --save-dev``。

此命令创建（或修改） ``package.json`` 文件并将依赖下载到 ``node_modules/`` 目录中。
Yarn也会创建/更新 ``yarn.lock``（如果你使用npm5+，则调用 ``package-lock.json``）。

.. tip::

    你应该提交 ``package.json`` 和 ``yarn.lock``
    （或者 ``package-lock.json``，如果使用npm5的话）到版本控制，但忽略 ``node_modules/``。

创建webpack.config.js文件
-----------------------------------

接下来，在项目的根目录下创建一个新文件 ``webpack.config.js``：

.. code-block:: js

    var Encore = require('@symfony/webpack-encore');

    Encore
        // directory where compiled assets will be stored
        .setOutputPath('public/build/')
        // public path used by the web server to access the output path
        .setPublicPath('/build')
        // only needed for CDN's or sub-directory deploy
        //.setManifestKeyPrefix('build/')

        /*
         * ENTRY CONFIG
         *
         * Add 1 entry for each "page" of your app
         * (including one that's included on every page - e.g. "app")
         *
         * Each entry will result in one JavaScript file (e.g. app.js)
         * and one CSS file (e.g. app.css) if you JavaScript imports CSS.
         */
        .addEntry('app', './assets/js/app.js')
        //.addEntry('page1', './assets/js/page1.js')
        //.addEntry('page2', './assets/js/page2.js')

        // will require an extra script tag for runtime.js
        // but, you probably want this, unless you're building a single-page app
        .enableSingleRuntimeChunk()

        .cleanupOutputBeforeBuild()
        .enableSourceMaps(!Encore.isProduction())
        // enables hashed filenames (e.g. app.abc123.css)
        .enableVersioning(Encore.isProduction())

        // uncomment if you use TypeScript
        //.enableTypeScriptLoader()

        // uncomment if you use Sass/SCSS files
        //.enableSassLoader()

        // uncomment if you're having problems with a jQuery plugin
        //.autoProvidejQuery()
    ;

    module.exports = Encore.getWebpackConfig();

接下来，使用一些基本JavaScript创建一个新的 ``assets/js/app.js`` 文件 *并* 导入一些JavaScript：

.. code-block:: javascript

    // assets/js/app.js

    require('../css/app.css');

    console.log('Hello Webpack Encore');

以及新的 ``assets/css/app.css`` 文件：

.. code-block:: css

    // assets/css/app.css
    body {
        background-color: lightgray;
    }

你将在 :doc:`/frontend/encore/simple-example` 中自定义并了解有关这些文件的更多信息。

.. caution::

    一些文档将使用特定于Symfony或Symfony的 `WebpackEncoreBundle`_ 的功能。
    这些是可选的，并且是指向Encore生成的资产路径的特殊方式，它们启用一些功能：
    :doc:`版本控制 </frontend/encore/versioning>`
    和 :doc:`split chunks </frontend/encore/split-chunks>`。

.. _`npm`: https://www.npmjs.com/
.. _WebpackEncoreBundle: https://github.com/symfony/webpack-encore-bundle
