jQuery和旧应用
==============================

在Webpack内部，当你需要模块时，它（通常）*不会* 设置全局变量。相反，它只返回一个值：

.. code-block:: javascript

    // this loads jquery, but does *not* set a global $ or jQuery variable
    const $ = require('jquery');

实际上，这会导致一些 *依赖* jQuery的外部库存在问题。
如果你的某些脚本没有通过Webpack处理（例如你的模板中有一些脚本），那将是一个问题。

使用期望jQuery成为全局的库
-----------------------------------------------

一些遗留的脚本应用使用的编程实践与Webpack提供的新实践不相符。
最常见的问题是你的代码（例如jQuery插件）认为jQuery已经是 ``$`` 或 ``jQuery`` 全局变量。
如果未定义这些变量，你将收到以下错误：

.. code-block:: text

    Uncaught ReferenceError: $ is not defined at [...]
    Uncaught ReferenceError: jQuery is not defined at [...]

Encore不是重写所有内容，而是允许使用不同的解决方案。
多亏了 ``autoProvidejQuery()`` 方法，只要JavaScript文件使用 ``$`` 或 ``jQuery`` 变量，
Webpack就会自动引入 ``jquery`` 并创建这些变量。

因此，在使用旧版应用时，你可能需要将以下内容添加到 ``webpack.config.js``：

.. code-block:: diff

    Encore
        // ...
    +     .autoProvidejQuery()
    ;

在内部，该 ``autoProvidejQuery()`` 方法从Encore调用 ``autoProvideVariables()`` 方法。
在实践中，它相当于：

.. code-block:: javascript

    Encore
        // 你可以使用这个方法来提供其他常用全局变量
        // 例如 'underscore' 库的 '_'
        .autoProvideVariables({
            $: 'jquery',
            jQuery: 'jquery',
            'window.jQuery': 'jquery',
        })
        // ...
    ;

从WebPack脚本文件之外访问jQuery
---------------------------------------------------------

如果你还需要提供对Webpack处理的脚本文件之外访问 ``$`` 和 ``jQuery`` 变量
（例如仍存在于模板中的脚本），则需要在旧代码之前加载的某个脚本文件中手动设置这些全局变量。

例如，你可以定义一个由Webpack处理并在每个页面上加载的 ``common.js`` 文件，其中包含以下内容：

.. code-block:: javascript

    // 手动引入 jQuery
    const $ = require('jquery');

    // 创建全局的 $ 和 jQuery 变量
    global.$ = global.jQuery = $;

.. tip::

    该 ``global`` 变量是在 ``window`` 变量里设定东西的一种特殊方式。
    在Web上下文中，使用 ``global`` 和 ``window`` 等效，
    除了在使用 ``autoProvidejQuery()`` 时 ``window.jQuery`` 不起作用。换句话说，请使用 ``global``。
