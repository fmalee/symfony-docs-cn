如何在JavaScript中生成路由URL
==========================================

如果你使用的是Twig模板，则可以使用相同的 ``path()`` 函数来设置JavaScript变量。
``escape()`` 函数有助于转义任何非安全JavaScript的值：

.. code-block:: html+twig

    <script>
    var route = "{{ path('blog_show', {'slug': 'my-blog-post'})|escape('js') }}";
    </script>

但是，如果你 *确实* 需要在纯JavaScript中生成路由，请考虑使用`FOSJsRoutingBundle`_。它使以下示例成为可能：

.. code-block:: html+twig

    <script>
    var url = Routing.generate('blog_show', {
        'slug': 'my-blog-post'
    });
    </script>

.. _`FOSJsRoutingBundle`: https://github.com/FriendsOfSymfony/FOSJsRoutingBundle
