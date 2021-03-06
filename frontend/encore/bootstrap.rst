使用Bootstrap的CSS和JS
========================

想在项目中使用Bootstrap（或类似的东西）？没问题！
首先，安装它。为了能够进一步定制，我们将安装 ``bootstrap``：

.. code-block:: terminal

    $ yarn add bootstrap --dev

导入Bootstrap样式
--------------------------

现在 ``bootstrap`` 位于你的 ``node_modules/`` 目录中，你可以从任何Sass或JavaScript文件中导入它。
例如，如果你已有 ``global.scss`` 文件，请从那里导入：

.. code-block:: scss

    // assets/css/global.scss

    // 自定义一些Bootstrap变量
    $primary: darken(#428bca, 20%);

    // ~ 允许你在 node_modules 引用一些东西
    @import "~bootstrap/scss/bootstrap";

仅此而已！这会将 ``node_modules/bootstrap/scss/bootstrap.scss`` 文件导入 ``global.scss``。
你甚至可以先自定义Bootstrap变量！

.. tip::

    如果你不需要Bootstrap的 *所有* 功能，则可以包含在 ``bootstrap`` 目录中的特定文件 - 例如 ``~bootstrap/scss/alert``。

导入Bootstrap的JavaScript
------------------------------

Bootstrap JavaScript需要jQuery和Popper.js，所以请确保安装了这些：

.. code-block:: terminal

    $ yarn add jquery popper.js --dev

现在，在任何你的脚本文件中引入bootstrap：

.. code-block:: javascript

    // app.js

    const $ = require('jquery');
    // 这会“修改”jquery模块：向其添加行为（behavior）
    // bootstrap 模块不会导出/返回任何内容
    require('bootstrap');

    // 或者你可以包含特定的部分
    // require('bootstrap/js/dist/tooltip');
    // require('bootstrap/js/dist/popover');

    $(document).ready(function() {
        $('[data-toggle="popover"]').popover();
    });

使用其他Bootstrap/jQuery插件
--------------------------------------

如果你需要使用与jQuery兼容的jQuery插件，你可能需要使用Encore的
:ref:`autoProvidejQuery() <encore-autoprovide-jquery>`
方法，以便这些插件知道在哪里可以找到jQuery。然后，你可以像往常一样包含所需的JavaScript和CSS：

.. code-block:: javascript

    // ...

    // require the JavaScript
    require('bootstrap-star-rating');
    // require 2 CSS files needed
    require('bootstrap-star-rating/css/star-rating.css');
    require('bootstrap-star-rating/themes/krajee-svg/theme.css');
