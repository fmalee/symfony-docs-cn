安装Encore
=================

首先，确保 `安装Node.js`_ 以及 `Yarn包管理器`_。
以下说明取决于你是否在Symfony应用中安装Encore。

在Symfony应用中安装Encore
-----------------------------------------

运行这些命令以在项目中安装PHP和JavaScript依赖：

.. code-block:: terminal

    $ composer require symfony/webpack-encore-bundle
    $ yarn install

如果你使用的是 :doc:`Symfony Flex </setup/flex>`，则会安装并启用
`WebpackEncoreBundle`_、创建 ``assets/`` 目录、添加 ``webpack.config.js``
文件并添加 ``node_modules/`` 到 ``.gitignore``。你可以跳过本文的其余部分，通过阅读
:doc:`/frontend/encore/simple-example` 来编写你的第一个JavaScript和CSS！

如果你不使用Symfony Flex，则需要按照下一节中展示的说明来自行创建所有这些目录和文件。

在非Symfony应用中安装Encore
---------------------------------------------

通过Yarn将Encore安装到你的项目中：

.. code-block:: terminal

    $ yarn add @symfony/webpack-encore --dev

    # 如果你更喜欢npm，请改为运行此命令：
    # npm install @symfony/webpack-encore --save-dev

此命令创建（或修改）``package.json`` 文件并将依赖下载到 ``node_modules/``
目录中。Yarn还可以创建/更新 ``yarn.lock``（如果使用npm，则名为 ``package-lock.json``）。

.. tip::

    你 *应该* 提交 ``package.json`` 和 ``yarn.lock``
    （如果使用npm，则是 ``package-lock.json``）到版本控制中，但要忽略 ``node_modules/``。

创建webpack.config.js文件
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

接下来，在项目的根目录下创建一个新的 ``webpack.config.js`` 文件。
这是Webpack和Webpack Encore的主配置文件：

.. code-block:: javascript

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

接下来，使用一些基本JavaScript创建一个新的 ``assets/js/app.js`` 文件
*并* 导入一些JavaScript：

.. code-block:: javascript

    // assets/js/app.js

    require('../css/app.css');

    console.log('Hello Webpack Encore');

还有新的 ``assets/css/app.css`` 文件：

.. code-block:: css

    /* assets/css/app.css */
    body {
        background-color: lightgray;
    }

你将在 :doc:`/frontend/encore/simple-example` 中自定义并了解有关这些文件的更多信息。

.. caution::

    某些文档将使用特定于Symfony或Symfony的 `WebpackEncoreBundle`_ 的功能。
    这些是可选的，是指向由Encore生成的资产路径的特殊方式，例如可以启用
    :doc:`版本控制 </frontend/encore/versioning>` 和
    :doc:`拆分区块 </frontend/encore/split-chunks>`等功能。

.. _`安装Node.js`: https://nodejs.org/en/download/
.. _`Yarn包管理器`: https://yarnpkg.com/lang/en/docs/install/
.. _`npm`: https://www.npmjs.com/
.. _`WebpackEncoreBundle`: https://github.com/symfony/webpack-encore-bundle
