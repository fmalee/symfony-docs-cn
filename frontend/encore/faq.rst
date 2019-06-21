FAQ和常见错误
=====================

.. _how-do-i-deploy-my-encore-assets:

如何部署Encore资产？
---------------------------------

部署资产时需要记住两件重要事项。

**1) 为生产编译资产**

通过运行一下命令来优化你的资产以进行生产：

.. code-block:: terminal

    $ ./node_modules/.bin/encore production

这将压缩你的资产并进行其他性能优化。好极了！

但是，你应该在什么服务器上运行此命令？这取决于你的部署方式。
例如，你可以在本地（或在构建服务器上）执行此操作，并使用 `rsync`_ 或其他工具将生成的文件传输到生产服务器。
或者，你可以先将文件放在生产服务器上（例如通过 ``git pull``），然后在生产中运行此命令
（最好能在在流量到达代码之前）。在这种情况下，你需要在生产服务器上安装Node.js.

**2) 仅部署生成的资产**

需要部署到生产服务器的 *唯一* 文件是最终生成的资产（例如 ``public/build`` 目录）。
你不需要安装Node.js，部署 ``webpack.config.js``、``node_modules`` 目录，甚至你的源资源文件，
除非你打算在你的生产机器上运行 ``encore production``。
生成后的资产，才是生产服务器上 *唯一* 需要的东西。

我是否需要在我的生产服务器上安装Node.js？
-----------------------------------------------------

不用，除非你计划在生产服务器上生成生产资产，否则不建议这样做。请参阅 `如何部署Encore资产？`_。

哪些文件应该提交到git？又应该忽略哪些？
-------------------------------------------------------------

你应该将所有文件提交到git， ``node_modules/`` 目录和生成的文件除外。
你的 ``.gitignore`` 文件应包括：

.. code-block:: text

    /node_modules/
    # whatever path you're passing to Encore.setOutputPath()
    /public/build

你应该提交所有源资产文件、``package.json`` 以及 ``yarn.lock``。

我的应用位于子目录
---------------------------------

如果你的应用不在你的Web服务器的根目录下（即它位于子目录下，例如 ``/myAppSubdir``），
则需要在调用 ``Encore.setPublicPrefix()`` 时配置它：

.. code-block:: diff

    // webpack.config.js
    Encore
        // ...

        .setOutputPath('public/build/')

    -     .setPublicPath('/build')
    +     // 这里是你 *实际* 的 public 路径
    +     .setPublicPath('/myAppSubdir/build')

    +     // 现在需要这样，以便你的 manifest.json 键仍然是 `build/foo.js`
    +     // 这是Symfony资产功能使用的一个文件
    +     .setManifestKeyPrefix('build')
    ;

如果你正在使用 ``encore_entry_script_tags()`` 和 ``encore_entry_link_tags()``
Twig快捷方式（或者是以某种其他方式 :ref:`通过entrypoints.json处理你的资产 <load-manifest-files>`
），那么你已经完工了。这些快捷方法从
:ref:`entrypoints.json <encore-entrypointsjson-simple-description>`
文件中读取，该文件现在将包含子目录。

"jQuery is not defined" 或 "$ is not defined"
---------------------------------------------

当你的代码（或你正在使用的某个库）期望 ``$`` 或 ``jQuery`` 成为全局变量时，会发生此错误。
但是，当你使用Webpack和 ``require('jquery')``，不会有全局变量被设置。

此错误的修复取决于你的代码中或你正在使用的某些第三方代码中是否发生错误。
请参阅 :doc:`/frontend/encore/legacy-applications` 以获取修复方法。

Uncaught ReferenceError: webpackJsonp is not defined
----------------------------------------------------

如果你收到此错误，可能是因为你忘了为包含Webpack运行时的 ``runtime.js`` 文件添加一个 ``script`` 标签。
如果你正在使用 ``encore_entry_script_tags()`` Twig函数，则永远不会发生这种情况：该文件脚本标记会自动渲染。

This dependency was not found: some-module in ./path/to/file.js
---------------------------------------------------------------

通常，在通过yarn安装一个软件包后，你可以引入/导入它以便使用它。
例如，运行 ``yarn add respond.js`` 后，你尝试引入该模块：

.. code-block:: javascript

    require('respond.js');

但是，你看到了一个错误，而不是正常运行：

    This dependency was not found:

    * respond.js in ./assets/js/app.js

通常，软件包将通过向 ``package.json`` 添加一个 ``main`` 键来“宣告”它的“主”文件。
但有时候，旧的软件库不会有这个。相反，你需要专门引入你需要的文件。在这种情况下，你应该使用的文件位于 ``node_modules/respond.js/dest/respond.src.js``。
你可以通过以下方式引入：

.. code-block:: javascript

    // require a non-minified file whenever possible
    require('respond.js/dest/respond.src.js');

我需要在第三方模块上执行Babel
-----------------------------------------------

为了提高性能，Encore不会通过Babel处理 ``node_modules/`` 中的库。
但是，你可以通过 ``configureBabel()`` 方法改变它。
有关详细信息，请参阅 :doc:`/frontend/encore/babel`。

.. _`rsync`: https://rsync.samba.org/

如何将我的Encore配置与IDE集成？
-------------------------------------------------------

将 `Webpack集成到PhpStorm`_
和其他IDE中可以提高开发效率（例如通过解析别名）。但是，你可能会遇到此错误：

.. code-block:: text

    Encore.setOutputPath() cannot be called yet because the runtime environment
    doesn't appear to be configured. Make sure you're using the encore executable
    or call Encore.configureRuntimeEnvironment() first if you're purposely not
    calling Encore directly.

这个错误是因为Encore的运行时环境仅在你运行它时被配置（例如，在执行
``yarn encore dev`` 时）。可以调用 ``Encore.isRuntimeEnvironmentConfigured()`` 和
``Encore.configureRuntimeEnvironment()`` 方法来修复此问题：

.. code-block:: javascript

    // webpack.config.js
    const Encore = require('@symfony/webpack-encore')

    if (!Encore.isRuntimeEnvironmentConfigured()) {
        Encore.configureRuntimeEnvironment(process.env.NODE_ENV || 'dev');
    }

    // ... Encore配置的其余部分

.. _`Webpack集成到PhpStorm`: https://www.jetbrains.com/help/phpstorm/using-webpack.html
