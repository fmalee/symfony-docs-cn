创建共享的公共条目
===============================

.. caution::

    虽然此方法仍然有效，但请参阅 :doc:`/frontend/encore/split-chunks`，以获取在多个条目文件之间共享资产的首选解决方案。

假设你有多个条目文件，*每个* 文件都需要 ``jquery``。
在这种情况下，*每个* 输出文件都将包含jQuery，从而降低用户的体验。
要解决此问题，你可以将公共库 *提取* 到每个页面都包含的“共享”条目文件中。

假设你已经有一个包含在每个页面上的名为 ``app`` 的条目。
使用 ``createSharedEntry()`` 更新你的代码：

.. code-block:: diff

    Encore
        // ...
    -     .addEntry('app', './assets/js/app.js')
    +     .createSharedEntry('app', './assets/js/app.js')
        .addEntry('homepage', './assets/js/homepage.js')
        .addEntry('blog', './assets/js/blog.js')
        .addEntry('store', './assets/js/store.js')

在进行此更改之前，如果 ``app.js`` 和 ``store.js`` 两个都引入jquery，然后 ``jquery`` 就被打包在 *两个* 文件中，这是一种浪费。
通过创建 ``app.js`` “共享”条目，*任何* 引入到 ``app.js`` 的代码（如jQuery）将不再打包到任何其他文件中。
任何CSS也都是如此。

因为 ``app.js`` 包含其他条目文件所依赖的所有公共代码，所以很明显它的script (和link)标签必须包含在每个页面上。

.. tip::

    当文件内容 *很少* 更改并且你正在使用 :ref:`长期缓存 <encore-long-term-caching>` 时，
    使用 ``app.js`` 文件效果最佳。为什么？
    如果 ``app.js`` 包含 *经常* 更改的应用代码，那么（使用版本控制时）其文件名的哈希将经常变动。
    这意味着你的用户将无法享受此文件的长期缓存带来的好处（通常该文件非常大）。
