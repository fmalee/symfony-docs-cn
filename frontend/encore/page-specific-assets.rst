创建特定于页面的CSS/JS
=============================

如果你正在创建单页面应用（SPA），那么你可能只需要在 ``webpack.config.js`` 中定义 *一个* 条目。
但是，如果你有多个页面，则可能需要特定于页面的CSS和JavaScript。

要了解如何设置它，请参阅 :ref:`multiple-javascript-entries` 示例。

每页多个条目？
--------------------------

通常，每页只应包含 *一个* 脚本条目。
将结帐页面视为自己的“应用”，其中 ``checkout.js`` 包含你需要的所有功能。

但是，需要在每个页面上包含一些全局JavaScript和CSS是很常见的。
出于这个原因，通常有一个包含这个全局代码（包括JavaScript和CSS）的条目（例如 ``app``），并且将其插入(included)到每个页面上（即将它包含在你的应用的 *布局* 中）。
这意味着在每个页面上总是有一个全局条目（例如 ``app``），
而且 *会* 在一个特定于页面的条目（例如 ``checkout``）中配置一个特定于该页面的JavaScript和CSS文件。

.. tip::

    一定要使用 :doc:`split chunks </frontend/encore/split-chunks>`
    以避免重复并在你的条目文件之间共享代码。
