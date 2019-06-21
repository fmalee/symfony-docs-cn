.. index::
    single: Templating; Output escaping

如何转义模板中的输出
=================================

从模板生成HTML时，模板变量可能会输出非预期的HTML或危险的客户端代码。
结果是该动态内容可能会破坏结果页面的HTML或允许恶意用户执行 `跨站点脚本`_ （XSS）攻击。
思考这个典型的例子：

.. code-block:: twig

    Hello {{ name }}

想象一下，用户输入以下代码作为其名称：

.. code-block:: html

    <script>alert('hello!')</script>

在没有任何输出转义的情况下，生成的模板将弹出一个JavaScript警告框：

.. code-block:: html

    Hello <script>alert('hello!')</script>

虽然这似乎无害，但如果用户可以做到这一点，那么同一个用户也应该能够编写在不知情的合法用户的安全区域内执行恶意操作的JavaScript。

该问题的解决方案是输出转义。使用输出转义，相同的模板将无害地渲染，并按照字面将 ``script`` 标签打印到屏幕：

.. code-block:: html

    Hello &lt;script&gt;alert(&#39;hello!&#39;)&lt;/script&gt;

Twig和PHP模板系统以不同的方式解决这个问题。
如果你正在使用Twig，默认情况下输出转义处于启用状态，你已受到保护。
在PHP中，输出转义不是自动的，这意味着你需要在必要时手动转义。

Twig中的输出转义
-----------------------

如果你正在使用Twig模板，则默认情况下输出转义处于启用状态。
这意味着你开箱受到保护，以免受用户提交的代码的无意后果。
默认情况下，输出转义假定为HTML的输出转义内容。

在某些情况下，当你渲染受信任且包含不应转义的标签时，你需要禁用输出转义。
假设管理用户能够编写包含HTML代码的文章。但默认情况下，Twig将转义文章正文。

为了正常渲染，请添加 ``raw`` 过滤器：

.. code-block:: twig

    {{ article.body|raw }}

你还可以禁用 ``{% block %}`` 区域内或整个模板的输出转义。
有关更多信息，请参阅Twig文档中的 `输出转义`_。

PHP中的输出转义
----------------------

使用PHP模板时，输出转义不是自动的。这意味着除非你明确选择转义变量，否则你不受保护。
要使用输出转义，请使用特殊的 ``escape()`` 视图方法：

.. code-block:: html+php

    Hello <?= $view->escape($name) ?>

默认情况下，``escape()`` 方法假定该变量在HTML上下文中渲染（因此该变量针对HTML安全进行转义）。
第二个参数允许你更改上下文(context)。例如，要在一个脚本字符串中输出内容，请使用 ``js`` 上下文：

.. code-block:: html+php

    var myMsg = 'Hello <?= $view->escape($name, 'js') ?>';

.. _`跨站点脚本`: https://en.wikipedia.org/wiki/Cross-site_scripting
.. _`输出转义`: https://twig.symfony.com/doc/2.x/api.html#escaper-extension
