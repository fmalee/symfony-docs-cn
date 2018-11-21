Encore：设置你的项目
===============================

:doc:`安装Encore </frontend/encore/installation>` 后，你的应用的
``assets/`` 目录已经有一个CSS和一个JS文件：

* ``assets/js/app.js``
* ``assets/css/app.css``

使用Encore，将你的 ``app.js`` 文件视为独立的JavaScript应用：
它将 *引入(require)* 它所需的所有依赖（例如jQuery或React），*包含(including)* 任何CSS。
你的 ``app.js`` 文件已经使用特殊 ``require`` 函数执行此操作：

.. code-block:: javascript

    // assets/js/app.js
    // ...

    require('../css/app.css');

    // var $ = require('jquery');

Encore的工作（通过WebPack）很简单：读取并跟进 *所有* 的 ``require``
语句，并创建包含你的应用 *所有* 需求的一个最终的 ``app.js`` （和 ``app.css``）。
当然，Encore可以做更多的事情：压缩文件，预处理Sass/LESS，支持React、Vue.js等等。

配置Encore/Webpack
--------------------------

Encore中的所有内容都是通过项目根目录下的 ``webpack.config.js`` 文件进行配置的。
它已经拥有你需要的基本配置：

.. code-block:: javascript

    // webpack.config.js
    var Encore = require('@symfony/webpack-encore');

    Encore
        // directory where compiled assets will be stored
        .setOutputPath('public/build/')
        // public path used by the web server to access the output path
        .setPublicPath('/build')

        .addEntry('app', './assets/js/app.js')

        // ...
    ;

    // ...

他们 *关键* 的部分是 ``addEntry()``：它告诉Encore加载 ``assets/js/app.js`` 文件，并关注所有的的 ``require`` 语句。
然后它将所有内容打包在一起 - 感谢第一个 ``app`` 参数 -
将最终的 ``app.js`` 和 ``app.css`` 文件输出到 ``public/build`` 目录中。

.. _encore-build-assets:

要生成资产，请运行：

.. code-block:: terminal

    # 一次性编译资产
    $ yarn encore dev

    # 或者在文件变更时自动重新编译
    $ yarn encore dev --watch

    # 部署时，生成一个生产版本
    $ yarn encore production

.. note::

    每次更新 ``webpack.config.js`` 文件时停止并重新启动 ``encore``。

恭喜！你现在有三个新文件：

* ``public/build/app.js`` (包含“app”条目的所有JavaScript)
* ``public/build/app.css`` (包含“app”条目的所有CSS)
* ``public/build/runtime.js`` (一个帮助Webpack完成工作的文件)

接下来，将它们包含在基本布局文件中。
来自WebpackEncoreBundle的两个Twig助手可以为你完成大部分工作：

.. code-block:: twig

    {# templates/base.html.twig #}
    <!DOCTYPE html>
    <html>
        <head>
            <!-- ... -->

            {% block stylesheets %}
                {# 'app' 必须与 webpack.config.js 中 addEntry() 的第一个参数匹配 #}
                {{ encore_entry_link_tags('app') }}

                <!-- 渲染一个link标签（如果你的模块引入了任何CSS）
                     <link rel="stylesheet" src="/build/app.css"> -->
            {% endblock %}
        </head>
        <body>
            <!-- ... -->

            {% block javascripts %}
                {{ encore_entry_script_tags('app') }}

                <!-- 渲染 app.js & webpack runtime.js 文件
                    <script src="/build/runtime.js"></script>
                    <script src="/build/app.js"></script> -->
            {% endblock %}
        </body>
    </html>

.. _encore-entrypointsjson-simple-description:

仅此而已！刷新页面时，``assets/js/app.js`` 中的所有脚本 - 以及其引入的其他所有脚本文件 - 都将被执行。
同时还将显示引入的所有CSS文件。

``encore_entry_link_tags()`` 和 ``encore_entry_script_tags()`` 函数读取一个由Encore生成的
``entrypoints.json`` 文件，以获取确切的文件名称来进行渲染。
此文件 *特别* 有用，因为你可以 :doc:`启用版本控制</frontend/encore/versioning>`
或 :doc:`将资源指向CDN</frontend/encore/cdn>` 而无需对模板进行任何更改：
在 ``entrypoints.json`` 中的路径将始终是最终的正确路径。

如果你不使用Symfony，则可以忽略 ``entrypoints.json`` 文件并直接指向最终的构建文件。
只有某些可选功能才需要 ``entrypoints.json``。

.. versionadded:: 0.21.0

    来自WebpackEncoreBundle的 ``encore_entry_link_tags()`` 和依赖于Encore一项功能，首次在0.21.0版本中引入。
    在之前是使用 ``asset()`` 函数直接指向对应文件。

引入JavaScript模块
----------------------------

Webpack是一个模块bundler，这意味着你可以 ``require`` 其他JavaScript文件。
首先，创建一个导出一个函数的文件：

.. code-block:: javascript

    // assets/js/greet.js
    module.exports = function(name) {
        return `Yo yo ${name} - welcome to Encore!`;
    };

我们将使用jQuery在页面上打印此消息。通过以下方式安装：

.. code-block:: terminal

    $ yarn add jquery --dev

很好！使用 ``require()`` 来导入 ``jquery`` and ``greet.js``：

.. code-block:: diff

    // assets/js/app.js
    // ...

    + // 从 node_modules 加载 jquery 包
    + var $ = require('jquery');

    + // 从 greet.js 导入函数（.js扩展名是可选的）
    + // ./ (或 ../) 意味着查找一个本地文件
    + var greet = require('./greet');

    + $(document).ready(function() {
    +     $('body').prepend('<h1>'+greet('jill')+'</h1>');
    + });

仅此而已！如果你之前运行过 ``encore dev --watch``，你的最终构建文件已经更新：
jQuery和 ``greet.js`` 已自动添加到输出的文件（``app.js``）中。
请刷新以查看该消息！

导入和导出语句
--------------------------------

除了使用如上所示的 ``require`` 和 ``module.exports``，JavaScript还有一种替代语法，这是一种更为公认的标准。
你可以随你心意选择，它们都做同一件事情。

要使用替代语法导出值，请使用 ``exports``：

.. code-block:: diff

    // assets/js/greet.js
    - module.exports = function(name) {
    + export default function(name) {
        return `Yo yo ${name} - welcome to Encore!`;
    };

要导入值，请使用 ``import``：

.. code-block:: diff

    // assets/js/app.js
    - require('../css/app.css');
    + import '../css/app.css';

    - var $ = require('jquery');
    + import $ from 'jquery';

    - var greet = require('./greet');
    + import greet from './greet';

.. _multiple-javascript-entries:

特定于页面的JavaScript或CSS（多个条目）
--------------------------------------------------

到目前为止，你只有一个最终的JavaScript文件：``app.js``。对于简单的应用或SPA（单页应用），这可能没问题！
但是，随着应用的增长，你可能希望拥有特定于页面的JavaScript或CSS（例如结帐，帐户等）。
要处理此问题，请为每个页面创建一个新的“entry”脚本文件：

.. code-block:: javascript

    // assets/js/checkout.js
    // 结帐页面的自定义代码

.. code-block:: javascript

    // assets/js/account.js
    // 账户页面的自定义代码

接下来，使用 ``addEntry()`` 来告诉Webpack在构建时读取这两个新文件：

.. code-block:: diff

    // webpack.config.js
    Encore
        // ...
        .addEntry('app', './assets/js/app.js')
    +     .addEntry('checkout', './assets/js/checkout.js')
    +     .addEntry('account', './assets/js/account.js')
        // ...

因为你刚刚更改了 ``webpack.config.js`` 文件，请确保停止并重新启动Encore：

.. code-block:: terminal

    $ yarn run encore dev --watch

Webpack现在将在你的构建目录中输出新的 ``checkout.js`` 和 ``account.js`` 文件。
而且，如果这些文件中的任何一个 引入/导入了 CSS，Webpack *也* 将输出 ``checkout.css`` 和 ``account.css`` 文件。

最后，在你需要的对应页面上包含 ``script`` 和 ``link`` 标签：

.. code-block:: diff

    {# templates/.../checkout.html.twig #}
    {% extends 'base.html.twig' %}

    + {% block stylesheets %}
    +     {{ parent() }}
    +     {{ encore_entry_link_tags('checkout') }}
    + {% endblock %}

    + {% block javascripts %}
    +     {{ parent() }}
    +     {{ encore_entry_script_tags('checkout') }}
    + {% endblock %}

现在，结帐页面将包含 ``app`` 条目的所有JavaScript和CSS（因为它包含在
``base.html.twig`` 内）*以及* 你的 ``checkout`` 条目。

有关更多详细信息，请参阅 :doc:`/frontend/encore/page-specific-assets`。
要避免在不同的条目文件中复制相同的代码，请参阅 :doc:`/frontend/encore/split-chunks`。

使用Sass/LESS/Stylus
----------------------

你已经掌握了Encore的基础知识。太好了！但是，还有 *更多* 的功能，如果你需要的话，可以选择加入。
例如，你也可以使用Sass、LESS或Stylus代替纯CSS。
要使用Sass，请将 ``app.css`` 文件重命名为 ``app.scss`` 并更新 ``import`` 语句：

.. code-block:: diff

    // assets/js/app.js
    - import '../css/app.css';
    + import '../css/app.scss';

然后，告诉Encore启用Sass预处理器：

.. code-block:: diff

    // webpack.config.js
    Encore
        // ...

    +    .enableSassLoader()
    ;

因为你刚刚更改了 ``webpack.config.js`` 文件，所以需要重新启动Encore。
当你这样做时，你会看到一个错误！

>   Error: Install sass-loader & node-sass to use enableSassLoader()
>     yarn add sass-loader@^7.0.1 node-sass --dev

Encore支持许多功能。但是，Encore不会强制安装所有这些功能，只有当你需要一项功能时，Encore才会告诉你需要安装的内容。
运行：

.. code-block:: terminal

    $ yarn add sass-loader@^7.0.1 node-sass --dev
    $ yarn encore dev --watch

你的应用现在支持Sass了。Encore还支持LESS和Stylus。请参阅 :doc:`/frontend/encore/css-preprocessors`。

仅编译CSS文件
-------------------------

.. caution::

    更好的选择是使用上面的模式：使用 ``addEntry()`` 指向一个JavaScript文件，然后在该文件内部引入需要的CSS。

如果你只想编译CSS文件，可以通过 ``addStyleEntry()`` 方式实现：

.. code-block:: javascript

    // webpack/config.js
    Encore
        // ...

        .addStyleEntry('some_page', './assets/css/some_page.css')
    ;

这将输出一个新的 ``some_page.css`` 文件。

继续阅读！
-----------

Encore支持更多功能！有关你可以执行的操作的完整列表，请参阅 `Encore的index.js文件`_。
或者，返回 :ref:`Encore文档列表 <encore-toc>`。

.. _`Encore的index.js文件`: https://github.com/symfony/webpack-encore/blob/master/index.js
