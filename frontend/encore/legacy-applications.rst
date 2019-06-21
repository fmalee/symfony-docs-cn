jQuery插件和旧应用
======================================

在Webpack内部，当你需要模块时，它（通常）*不会* 设置全局变量。相反，它只返回一个值：

.. code-block:: javascript

    // 这会加载jquery，但 *不会* 设置一个全局的 $ 或 jQuery 变量
    const $ = require('jquery');

在实践中，这将导致一些外部库出现问题。
有些库 *依赖* 于全局的jQuery，*或者* 如果 *你的* 脚本没有通过Webpack处理（例如你的模板中有一些脚本）并且你又需要jQuery。
这两种情况都会导致类似以下的错误：

.. code-block:: text

    Uncaught ReferenceError: $ is not defined at [...]
    Uncaught ReferenceError: jQuery is not defined at [...]

如何修复取决于导致该问题的代码。

.. _encore-autoprovide-jquery:

修复期望jQuery成为全局的jQuery插件
-----------------------------------------------------

jQuery插件往往期望jQuery已经是全局的 ``$`` 或 ``jQuery`` 变量。
要解决此问题，请从你的 ``webpack.config.js`` 文件中调用 ``autoProvidejQuery()``：

.. code-block:: diff

    Encore
        // ...
    +     .autoProvidejQuery()
    ;

重新启动Encore后，Webpack将查找所有未初始化的 ``$`` 和 ``jQuery`` 变量，并自动为你引入
``jquery`` 和设置这些变量。它会将那些“损坏”的代码“重写”成正确的代码。

在内部，此 ``autoProvidejQuery()`` 方法从Encore调用 ``autoProvideVariables()`` 方法。
在实践中，它相当于：

.. code-block:: javascript

    Encore
        // 你可以使用此方法来提供其他常见的全局变量，例如为'underscore'库提供 '_'
        .autoProvideVariables({
            $: 'jquery',
            jQuery: 'jquery',
            'window.jQuery': 'jquery',
        })
        // ...
    ;

从WebPack脚本文件之外访问jQuery
---------------------------------------------------------

如果 *你的* 代码需要访问 ``$`` 或 ``jQuery``，且你位于由Webpack/Encore处理的文件中，
你应该通过引入jQuery来移除任何 “$ is not defined” 错误：``var $ = require('jquery')``。

但是，如果你还需要在由Webpack处理的脚本文件之外的文件中访问 ``$`` 和 ``jQuery`` 变量
（例如仍存在于模板中的脚本），则需要在在旧代码之前加载的某个脚本文件中手动设置这些全局变量。

例如，在由Webpack处理并在每个页面上加载的 ``app.js`` 文件中，添加：

.. code-block:: diff

    // 正常引入 jQuery
    const $ = require('jquery');

    + // 创建全局的 $ 和 jQuery 变量
    + global.$ = global.jQuery = $;

该 ``global`` 变量是一种在 ``window`` 变量中设定东西的特殊方式。
在Web上下文中，使用 ``global`` 和 ``window`` 是等效的，除了在使用
``autoProvidejQuery()`` 时 ``window.jQuery`` 不起作用。换句话说，请使用 ``global``。
