Encore：设置你的项目
===============================

:doc:`安装Encore </frontend/encore/installation>` 后，你的应用的 ``assets/`` 目录已经有一个CSS和JS一个文件：

* ``assets/js/app.js``
* ``assets/css/app.css``

Encore将你的 ``app.js`` 文件视为独立的JavaScript应用：
它将 *引入* (require)它所需的所有依赖（例如jQuery），*包括* (including)任何CSS。
你的 ``app.js`` 文件已经使用特殊 ``require`` 功能执行此操作：

.. code-block:: javascript

    // assets/js/app.js
    // ...

    // var $ = require('jquery');

    require('../css/app.css');

    // ... the rest of your JavaScript...

Encore的工作很简单：读取 *所有* 的 ``require`` 语句，
并创建包含 *所有* 你的应用需求一个最终的 ``app.js``（和 ``app.css``）。
当然，Encore可以做更多的事情：压缩文件，预处理Sass/LESS，支持React、Vue.js和更多。

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

恭喜！你现在有两个新文件！接下来，在你的布局中，将 ``script`` 和 ``link`` 标签添加到新的编译资产
（例如 ``/build/app.css`` 和 ``/build/app.js``）。
在Symfony中，使用 ``asset()`` 帮助函数：

.. code-block:: twig

    {# templates/base.html.twig #}
    <!DOCTYPE html>
    <html>
        <head>
            <!-- ... -->
            <link rel="stylesheet" href="{{ asset('build/app.css') }}">
        </head>
        <body>
            <!-- ... -->
            <script src="{{ asset('build/app.js') }}"></script>
        </body>
    </html>

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

.. code-block:: javascript

    // assets/js/app.js

    // 从node_modules加载jquery
    var $ = require('jquery');

    // 从 greet.js 导入该函数 (.js 扩展名是可选的)
    // ./ (或 ../) 意味着查找一个本地文件
    var greet = require('./greet');

    $(document).ready(function() {
        $('body').prepend('<h1>'+greet('john')+'</h1>');
    });

仅此而已！在生成资源时，jQuery和 ``greet.js`` 会自动添加到输出文件（``app.js``）中。

导入和导出语句
--------------------------------

除了使用如上所示的 ``require`` 和 ``module.exports`` ，JavaScript还有一种替代语法，这是一种更为公认的标准。
你可以随你心意选择，它们的功能是相同的。

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
    - var $ = require('jquery');
    + import $ from 'jquery';

    - require('../css/app.css');
    + import '../css/app.css';

.. _multiple-javascript-entries:

特定于页面的JavaScript或CSS（多个条目）
--------------------------------------------------

到目前为止，你只有一个最终的JavaScript文件： ``app.js``。对于简单的应用或SPA（单页应用），这可能没问题！
但是，随着应用的增长，你可能希望拥有特定于页面的JavaScript或CSS（例如主页，博客，商店等）。
要处理此问题，请为需要自定义JavaScript或CSS的每个页面添加新的“entry”：

.. code-block:: diff

    Encore
        // ...
        .addEntry('app', './assets/js/app.js')
    +     .addEntry('homepage', './assets/js/homepage.js')
    +     .addEntry('blog', './assets/js/blog.js')
    +     .addEntry('store', './assets/js/store.js')
        // ...

Encore现在将渲染新的 ``homepage.js``、``blog.js`` 和 ``store.js`` 文件。
仅在需要它们的每个页面上为添加 ``script`` 标签。

.. tip::

    请记住，每次更新 ``webpack.config.js`` 文件时重新启动Encore。

如果任何条目需要CSS/Sass文件（例如 ``homepage.js`` 需要 ``assets/css/homepage.scss``），
则还将输出CSS文件（例如 ``build/homepage.css``）。添加 ``link`` 到需要CSS的页面。

要避免在不同的条目文件中复制重复的代码，请参阅 :doc:`创建共享条目 </frontend/encore/shared-entry>`。

使用Sass
----------

你也可以使用Sass代替使用纯CSS。
要使用Sass，请将 ``app.css`` 文件重命名为 ``app.scss``。更新 ``require`` 声明：

.. code-block:: diff

    // assets/js/app.js
    - require('../css/app.css');
    + require('../css/app.scss');

然后，告诉Encore启用Sass预处理器：

.. code-block:: diff

    // webpack.config.js
    Encore
        // ...

    +    .enableSassLoader()
    ;

使用 ``enableSassLoader()`` 需要安装其他软件包，但Encore会在运行时确切地告诉你哪些软件包。
Encore还支持LESS和Stylus。请参阅 :doc:`/frontend/encore/css-preprocessors`。

编译成一个CSS文件
-------------------------

要一起编译CSS，你通常应该遵循上面的模式：使用 ``addEntry()`` 指向JavaScript文件，然后从内部引入CSS。
但是，*如果* 你只想编译CSS文件，那么也可以使用 ``addStyleEntry()``：

.. code-block:: javascript

    // webpack/config.js
    Encore
        // ...

        .addStyleEntry('some_page', './assets/css/some_page.css')
    ;

这将输出一个新的 ``some_page.css``。

继续阅读！
-----------

返回到 :ref:`Encore文章列表 <encore-toc>` 以了解更多信息并添加新功能。
