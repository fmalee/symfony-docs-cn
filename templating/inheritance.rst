.. index::
    single: Templating; Three-level inheritance pattern

如何通过继承来组织你的Twig模板
=====================================================

使用继承的一种常用方法是三级法。此方法以下三种不同类型的模板完美配合：

* 创建一个包含应用的主要布局的 ``templates/base.html.twig`` 文件（如上例所示）。
  在内部，用 ``base.html.twig`` 调用此模板;

* 为你网站的每个“部分”创建一个模板。
  例如，博客功能将具有一个仅包含博客部分特定元素的名为 ``blog/layout.html.twig`` 的模板;

  .. code-block:: html+twig

      {# templates/blog/layout.html.twig #}
      {% extends 'base.html.twig' %}

      {% block body %}
          <h1>Blog Application</h1>

          {% block content %}{% endblock %}
      {% endblock %}

* 为每个页面创建单独的模板，并使每个模板继承相应的部分模板。
  例如，该“首页”页面将调用比较相似的 ``blog/index.html.twig`` 并列出实际博客帖子的内容。

  .. code-block:: html+twig

      {# templates/blog/index.html.twig #}
      {% extends 'blog/layout.html.twig' %}

      {% block content %}
          {% for entry in blog_entries %}
              <h2>{{ entry.title }}</h2>
              <p>{{ entry.body }}</p>
          {% endfor %}
      {% endblock %}

请注意，此模板继承了部分模板（``blog/layout.html.twig``），
后者又继承了应用的基本布局（``base.html.twig``）。这是常见的三级继承模型。

在构建应用时，你可以选择遵循此方法，或者只是让每个页面模板直接继承应用的基本模板（即 ``{% extends 'base.html.twig' %}``）。
三模板模型是供应商bundle使用的最佳实践方法，因此可以覆盖bundle的基本模板，以正确继承应用的基本布局。
