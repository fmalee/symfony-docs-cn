.. index::
    single: Templating; Embedding action

如何在模板中嵌入控制器
======================================

.. note::

    渲染嵌入式控制器比引入模板或调用自定义Twig函数“繁重”。
    除非你计划 :doc:`缓存该片段 </http_cache/esi>`，否则请避免嵌入许多控制器。

:ref:`引入模板片段 <including-other-templates>` 对于在多个页面上重用相同内容很有用。
但是，在某些情况下，这种技术不是最佳解决方案。

设想在一个网站的侧边栏上显示最近发布的文章。
这篇文章列表是动态的，它可能是数据库查询的结果。
换句话说，显示该侧边栏的任何页面的控制器必须进行相同的数据库查询，并将文章列表传递给引入的模板片段。

Symfony提出的替代解决方案是创建一个控制器，该控制器仅显示最近文章的列表，然后从需要显示该内容的任何模板中调用该控制器。

首先，创建一个渲染一定数量的最近文章的控制器::

    // src/Controller/ArticleController.php
    namespace App\Controller;

    // ...

    class ArticleController extends AbstractController
    {
        public function recentArticles($max = 3)
        {
            // 进行数据库调用或其他逻辑以获取 “$max” 数量的最新文章
            $articles = ...;

            return $this->render(
                'article/recent_list.html.twig',
                ['articles' => $articles]
            );
        }
    }

然后，创建一个 ``recent_list`` 模板片段以列出该控制器给出的文章：

.. code-block:: html+twig

    {# templates/article/recent_list.html.twig #}
    {% for article in articles %}
        <a href="{{ path('article_show', {slug: article.slug}) }}">
            {{ article.title }}
        </a>
    {% endfor %}

最后，使用 ``render()`` 函数和控制器的标准字符串语法
（即 **controllerNamespace**::**action**）从任何模板调用该控制器：

.. code-block:: html+twig

    {# templates/base.html.twig #}

    {# ... #}
    <div id="sidebar">
        {{ render(controller(
            'App\\Controller\\ArticleController::recentArticles',
            { 'max': 3 }
        )) }}
    </div>
