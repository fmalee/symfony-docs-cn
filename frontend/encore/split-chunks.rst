通过将共享代码“拆分”为单独的文件来防止重复
=====================================================================

假设你有多个条目文件，*每个* 文件都引入 ``jquery``。
在这种情况下，*每个* 输出文件都将包含jQuery，这使你的文件比必要的大得多。
要解决此问题，你可以要求webpack分析你的文件，并将这些文件 *拆分* 到可能包含“共享”代码的额外文件中。

要启用此功能，请调用 ``splitEntryChunks()``：

.. code-block:: diff

    Encore
        // ...

        // 可能导入相同代码的多个 entry 文件
        .addEntry('app', './assets/js/app.js')
        .addEntry('homepage', './assets/js/homepage.js')
        .addEntry('blog', './assets/js/blog.js')
        .addEntry('store', './assets/js/store.js')

    +     .splitEntryChunks()

现在，每个输出文件（例如 ``homepage.js``） *可* 被拆分成多个文件（例如 ``homepage.js``、``vendor~homepage.js``）。
这意味着你 *可能* 需要在模板中包含 *多个* ``script`` 标签（或CSS的 ``link`` 标签）。
Encore创建了一个 :ref:`entrypoints.json <encore-entrypointsjson-simple-description>`
文件，该文件准确列出了每个条目所需的CSS和JavaScript文件。

如果你正在使用WebpackEncoreBundle中的 ``encore_entry_link_tags()`` 和 ``encore_entry_script_tags()``
Twig函数，则无需执行任何其他操作！这些函数会自动读取文件并根据需要渲染多个 ``script`` 或 ``link`` 标签：

.. code-block:: twig

    {#
        现在可能渲染多个 script 标签:
            <script src="/build/runtime.js"></script>
            <script src="/build/homepage.js"></script>
            <script src="/build/vendor~homepage.js"></script>
    #}
    {{ encore_entry_script_tags('homepage') }}

控制如何拆分资产
--------------------------------

*何时* 以及 *如何* 拆分文件的逻辑由 `Webpack中的SplitChunksPlugin`_ 控制。
你可以使用 ``configureSplitChunks()`` 函数来控制传递给此插件的配置：

.. code-block:: diff

    Encore
        // ...

        .splitEntryChunks()
    +     .configureSplitChunks(function(splitChunks) {
    +         // 修改配置
    +         splitChunks.minSize = 0;
    +     }

.. _`Webpack中的SplitChunksPlugin`: https://webpack.js.org/plugins/split-chunks-plugin/
