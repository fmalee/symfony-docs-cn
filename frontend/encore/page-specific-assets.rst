创建特定于页面的CSS/JS
=============================

如果你正在创建单页面应用（SPA），那么你可能只需要在 ``webpack.config.js`` 中定义 *一个* 条目。
但是，如果你有多个页面，则可能需要特定于页面的CSS和JavaScript。

例如，假设你有一个具有自己的脚本的结帐页面。创建一个新 ``checkout`` 条目：

.. code-block:: diff

    // webpack.config.js

    Encore
        // 已存在的条目
        .addEntry('app', './assets/js/app.js')
        // 一个全局样式条目
        .addStyleEntry('global', './assets/css/global.scss')

    +     .addEntry('checkout', './assets/js/checkout.js')
    ;

在 ``checkout.js`` 里面，添加或引入你需要的JavaScript和CSS。
然后，在结帐页面里为 ``checkout.js`` 包含一个 ``script`` 标签
（如果你有导入CSS，则还需要在 ``link`` 标签里导入  ``checkout.css`` ）。

每页多个条目？
--------------------------

通常，每页只应包含 *一个* 脚本条目。
这意味着结帐页面将包括 ``checkout.js`` 但不包括在其他页面上使用的 ``app.js``。
将结帐页面视为自己的“应用”，其中 ``checkout.js`` 包含你需要的所有功能。

但是，如果有一些全局脚本要包含在每个页面里，则可以创建包含该代码的条目，并同时包含(include)该条目和特定于页面的条目。
例如，假设上面的 ``app`` 条目包含你希望在每个页面上使用的脚本。
在这种情况下，请在结帐页面上同时包含 ``app.js`` 和 ``checkout.js``。

.. tip::

    确保创建一个 :doc:`共享条目 </frontend/encore/shared-entry>` 以避免Webpack引导重复的逻辑和任何共享模块。
