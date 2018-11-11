.. index::
    single: Templating; Debug
    single: Templating; Dump
    single: Twig; Debug
    single: Twig; Dump

如何在Twig模板中Dump调试信息
===============================================

使用PHP模板时，如果需要快速查找传递的变量值，可以使用
:ref:`VarDumper组件中的dump()函数 <components-var-dumper-dump>`。
首先，确保它已安装：

.. code-block:: terminal

    $ composer require symfony/var-dumper

该函数十分好用，例如在控制器内部::

    // src/Controller/ArticleController.php
    namespace App\Controller;

    // ...

    class ArticleController extends AbstractController
    {
        public function recentList()
        {
            $articles = ...;
            dump($articles);

            // ...
        }
    }

.. note::

    ``dump()`` 函数的输出结果会渲染到Web开发人员工具栏。

在Twig模板中，你可以将该 ``dump`` 实用工具用作函数或标记：

* ``{% dump foo.bar %}`` 是不会修改原始模板输出的方法：
  变量不是内联呈现(dumped)，而是在Web调试工具栏中;
* 相反，``{{ dump(foo.bar) }}`` 内联呈现，因此可能不一定不适合你的用例
  （例如，你不应在HTML属性或一个 ``<script>`` 标签中使用它）。

.. code-block:: html+twig

    {# templates/article/recent_list.html.twig #}
    {# 此变量的内容将发送到Web调试工具栏 #}
    {% dump articles %}

    {% for article in articles %}
        {# 该变量的内容显示在网页上 #}
        {{ dump(article) }}

        <a href="/article/{{ article.slug }}">
            {{ article.title }}
        </a>
    {% endfor %}

按照设计，该 ``dump()`` 函数仅在 ``dev`` 和 ``test`` 环境中使用，以避免在生产环境中泄漏敏感信息。
实际上，尝试在 ``prod`` 环境中使用 ``dump()`` 函数将导致PHP错误。

执行此命令以列出所有的Twig函数、过滤器、全局变量、测试和已注册的命名空间及其路径：

.. code-block:: terminal

    # 列出一般信息
    $ php bin/console debug:twig

    # 按任意关键字过滤输出
    $ php bin/console debug:twig --filter=date

    # 传递模板名称以显示将要加载的物理文件
    $ php bin/console debug:twig @Twig/Exception/error.html.twig
