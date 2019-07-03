.. index::
   single: Templating

模板
============================

如同  :doc:`前文 </controller>` 所述，
控制器负责处理每一个进入symfony程序的请求，通常以渲染一个模板来生成响应内容作为结束。

现实中，控制器把大部分的繁重工作都委托给了其他地方，以令代码能够被测试和复用。
当一个控制器需要生成HTML、CSS或者其他内容时，它把这些工作给了一个模板引擎。

在本文中，你将学习如何编写功能强大的模板，用于把内容返回给用户、填充email等等。
你还将学会快捷方法，用聪明的方式来扩展模板，以及如何复用模板代码。

.. index::
   single: Templating; What is a template?

模板
---------

模板就是生成基于文本格式（HTML, XML, CSV, LaTeX ...）的任何文本文件。
我们最熟悉的模板类型就是 *PHP* 模板——一个包含文字和PHP代码的被PHP引擎解析的文本文件：

.. code-block:: html+php

    <!DOCTYPE html>
    <html>
        <head>
            <title>Welcome to Symfony!</title>
        </head>
        <body>
            <h1><?= $page_title ?></h1>

            <ul id="navigation">
                <?php foreach ($navigation as $item): ?>
                    <li>
                        <a href="<?= $item->getHref() ?>">
                            <?= $item->getCaption() ?>
                        </a>
                    </li>
                <?php endforeach ?>
            </ul>
        </body>
    </html>

.. index:: Twig; Introduction

但是，Symfony 框架中有一个更强大的模板语言叫作 `Twig`_。
Twig可令你写出简明易读且对设计师友好的模板，在几个方面比PHP模板强大许多：

.. code-block:: html+twig

    <!DOCTYPE html>
    <html>
        <head>
            <title>Welcome to Symfony!</title>
        </head>
        <body>
            <h1>{{ page_title }}</h1>

            <ul id="navigation">
                {% for item in navigation %}
                    <li><a href="{{ item.href }}">{{ item.caption }}</a></li>
                {% endfor %}
            </ul>
        </body>
    </html>

Twig定义了三种特殊的语法：

``{{ ... }}``
    “说些什么”：输出一个变量值或者一个表达式的结果到模板。

``{% ... %}``
    “做些什么”：控制模板逻辑的一个 **标签**，用于执行声明，如for循环语句等。

``{# ... #}``
    “注释些什么”：它相当于PHP的 ``/* comment */`` 语法。它用于注释单行和多行。注释的内容不作为页面输出。

Twig也包含 **过滤器**，它可以在模板输出之前改变输出内容。下例让 ``title`` 变量在被输出之前全部大写：

.. code-block:: twig

    {{ title|upper }}

Twig 内置了大量的 `标签`_、`过滤器`_ 和 `函数`_ ，默认就可以使用。
你甚至可以利用 :doc:`Twig 扩展 </templating/twig_extension>` 来 *自定义*
你自己的过滤器和函数（乃至更多）。运行以下命令将它们全部列出：

.. code-block:: terminal

    $ php bin/console debug:twig

Twig代码很像PHP代码，但两者有微妙的区别。
下例使用了一个标准的 ``for`` 标签和 ``cycle()`` 函数来输出10个div标签，并用 ``odd``、``even`` css类交替显示。

.. code-block:: html+twig

    {% for i in 1..10 %}
        <div class="{{ cycle(['even', 'odd'], i) }}">
            <!-- some HTML here -->
        </div>
    {% endfor %}

本章的模板示例，将同时使用Twig和PHP来展示。

.. sidebar:: 为什么选择Twig?

    Twig模板就是为了简单而不去处理PHP标签。设计上即是如此：Twig模板只负责渲染，而不去考虑逻辑。
    你用Twig越多，你就越会欣赏它，并从它的特性中受益。当然，你也会被普天下的网页设计者喜欢。

    还有很多Twig可以做但是PHP不可以的事，如空格控制、沙盒、自动转码HTML、手动上下文输出转义，
    以及包容“只会影响模板”的自定义函数和过滤器。
    Twig包含了大量特性，使得写模板时更加方便快捷。请看下例，它结合了循环和 ``if`` 逻辑语句：

    .. code-block:: html+twig

        <ul>
            {% for user in users if user.active %}
                <li>{{ user.username }}</li>
            {% else %}
                <li>No users found</li>
            {% endfor %}
        </ul>

.. index::
   pair: Twig; Cache

Twig 模板缓存
~~~~~~~~~~~~~~~~~~~~~

Twig超快的原因是，Twig模板被编译成原生PHP类并且缓存起来。
不用担心，这一切自动完成，毋须你做任何事。
而且在开发时，当你做出任何修改时，Twig足够智能地重新编译你的模板。
这意味着，Twig在生产环境下极快，而在开发时很方便使用。

.. index::
   single: Templating; Inheritance

.. _twig-inheritance:

模板继承和布局
--------------------------------

大多数的时候，模板在项目中都有通用的元素，比如页眉、页脚、侧边栏等等。
在Symfony中，我们将采用不同的思考角度来对待这个问题：一个模板可以被另外的模板装饰(decorated)。
这个的工作原理跟PHP类非常像，模板继承让你可以创建一个基础“布局”模板，
它包含你的站点所有通用元素，并被定义成 **区块(blocks)** （如同一个“包含基础方法的PHP基类”）。
一个子模板可以继承基础布局并覆写它的任何一个区块（就像“PHP子类覆写父类中的特定方法”）。

首先创建一个基础布局文件：

.. code-block:: html+twig

    {# templates/base.html.twig #}
    <!DOCTYPE html>
    <html>
        <head>
            <meta charset="UTF-8">
            <title>{% block title %}Test Application{% endblock %}</title>
        </head>
        <body>
            <div id="sidebar">
                {% block sidebar %}
                    <ul>
                        <li><a href="/">Home</a></li>
                        <li><a href="/blog">Blog</a></li>
                    </ul>
                {% endblock %}
            </div>

            <div id="content">
                {% block body %}{% endblock %}
            </div>
        </body>
    </html>

.. note::

    虽然讨论的是关于Twig的模板继承，但在思维方式上Twig和PHP模板之间是相同的。

该模板定义了一个两列式HTML框架页面。
在本例中，三处 ``{% block %}`` 区域被定义了（``title``、``sidebar`` 和 ``body``）。
每个区块都可以被继承它的子模板覆写，或者保留现在这种默认实现。这些模板也能被直接渲染。
只不过此时只是显示基础模板所定义的内容。
在该示例中，该模板都将使用 ``title``、 ``sidebar`` 和 ``body`` 等区块的默认值。

一个子模板看起来是这样的：

.. code-block:: html+twig

    {# templates/blog/index.html.twig #}
    {% extends 'base.html.twig' %}

    {% block title %}My cool blog posts{% endblock %}

    {% block body %}
        {% for entry in blog_entries %}
            <h2>{{ entry.title }}</h2>
            <p>{{ entry.body }}</p>
        {% endfor %}
    {% endblock %}

.. note::

    父模板被存放在 ``templates/`` 目录，因此其路径是 ``base.html.twig``。模板命名约定可参考下文的 :ref:`template-naming-locations`。

模板继承的关键是 ``{% extends %}`` 标签。
该标签告诉模板引擎首先评估父模板，它设置了布局并定义了若干区块。
然后子模板被渲染，上例中父模板中定义的 ``title`` 和 ``body`` 两个区块将会被子模板中的同名区块内容所取代。
根据 ``blog_entries`` 的取值，输出的内容可能像下面这样：

.. code-block:: html

    <!DOCTYPE html>
    <html>
        <head>
            <meta charset="UTF-8">
            <title>My cool blog posts</title>
        </head>
        <body>
            <div id="sidebar">
                <ul>
                    <li><a href="/">Home</a></li>
                    <li><a href="/blog">Blog</a></li>
                </ul>
            </div>

            <div id="content">
                <h2>My first post</h2>
                <p>The body of the first post.</p>

                <h2>Another post</h2>
                <p>The body of the second post.</p>
            </div>
        </body>
    </html>

注意，由于子模板中没有定义 ``sidebar`` 这个区块，来自父模板的内容将被显示出来。
父模板中的 ``{% block %}`` 标签内的内容，始终作为默认值来用。

.. tip::

    你可以进行任意多个层级的模板继承。请参考 :doc:`/templating/inheritance`。

使用模板继承时，需要注意：

* 如果在模板中使用 ``{% extends %}`` ，它必须是模板中的第一个标签；

* 你的基础模板中的 ``{% block %}`` 标签越多越好。记住，子模板不需要定义父模板中的所有区块。因此可以根据需要在基础模板中创建多个区块，并为每个区块提供合理的默认值。基础模板中的区块定义得愈多，你的布局就愈灵活；

* 如果你发现在多个模板中有重复的内容，这可能意味着你需要为该内容在父模板中定义一个 ``{% block %}`` 了。某些情况下，更好的解决方案可能是把这些内容放到一个新模板中，然后在该模板中 ``include`` 它(查看下文的：:ref:`including-templates`)；

* 如果你需要从父模板中获取一个区块的内容，可以使用 ``{{ parent() }}`` 函数。如果你只是想在父级块上添加新内容，而不是完全覆盖它，这很有用：

  .. code-block:: html+twig

      {% block sidebar %}
          <h3>Table of Contents</h3>

          {# ... #}

          {{ parent() }}
      {% endblock %}

.. index::
   single: Templating; Naming conventions
   single: Templating; File locations

.. _template-naming-locations:

模板的命名和存放位置
-----------------------------

默认情况下，模板可以存放在两个不同的位置：

``templates/``
    程序级的 ``views`` 目录可以存放整个程序的基础模板（程序布局和bundle模板），以及那些 :ref:`覆盖第三方软件包模板 <override-templates>` 的模板。

``vendor/path/to/CoolBundle/Resources/views/``
    每个第三方bundle的模板都会存放于它自己的 ``Resources/views/`` 目录（或者子目录）下。当你打算共享你的bundle时，你应该把它放在bundle中，而不是 ``templates/`` 目录。

更多时候你要用到的模板是在 ``templates/`` 目录下。你需要的模板路径是相对于这个目录的。
例如，去输出/继承 ``templates/base.html.twig``，你需要使用 ``base.html.twig`` 路径，
而要输出 ``templates/blog/index.html.twig`` 时，你需要使用 ``blog/index.html.twig`` 路径。

.. _template-referencing-in-bundle:

引用Bundle中的模板
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*如果* 你需要引入一个位于bundle中的模板，
Symfony使用Twig的命名空间语法（``@BundleName/directory/filename.html.twig``）来表示。
这可以表示多种类型的模板，每种都存放在一个特定路径下：

* ``@AcmeBlog/Blog/index.html.twig``: 用于指定一个特定页面的模板。字符串分为三个部分，每个部分由斜杆（``/``）隔开，含义如下：

  * ``@AcmeBlog``: 就是去掉了 ``Bundle`` 后缀的bundle名称。该模板位于 AcmeBlogBundle (即 ``src/Acme/BlogBundle``) 中;

  * ``Blog``: (*目录*) 指示了模板位于 ``Resources/views/`` 的 ``Blog`` 子目录中;

  * ``index.html.twig``: (*文件名*) 文件的真实名称是 ``index.html.twig``。

  假设 AcmeBlogBundle 位于 ``src/Acme/BlogBundle``, 最终的路径将是： ``src/Acme/BlogBundle/Resources/views/Blog/index.html.twig``。

* ``@AcmeBlog/layout.html.twig``: 这种语法指向了 AcmeBlogBundle 的父模板。由于没有了中间的“目录”部分（即 ``Blog``），模板应该位于 AcmeBlogBundle 的 ``Resources/views/layout.html.twig``。

使用此命名空间语法而不是真实文件路径，是因为该语法允许应用 :ref:`覆盖任何bundle内的模板 <override-templates>`。

模版后缀
~~~~~~~~~~~~~~~

每个模版都有两个扩展名，用来指定模板的 *格式* 和 *渲染引擎*。

========================  ======  ======
文件名称                    格式    引擎
========================  ======  ======
``blog/index.html.twig``  HTML    Twig
``blog/index.html.php``   HTML    PHP
``blog/index.css.twig``   CSS     Twig
========================  ======  ======

默认情况下，任何Symfony模板都可以用Twig或PHP编写，而后缀的最后一部分
（``.twig`` 或 ``.php``）指明了应该使用这两个 *引擎* 中的哪一个。
后缀的第一部分（``.html``, ``.css``）是模板最终将生成的格式。
与决定Symfony如何解析模板的引擎不同，这只是一种组织策略，
用于需要将相同的资源渲染为HTML（``index.html.twig``），XML（``index.xml.twig``）或任何其他格式的情况。
参考 :doc:`/templating/formats` 以了解更多。

.. index::
   single: Templating; Tags and helpers
   single: Templating; Helpers

标签和辅助工具
----------------

你已经了解了模板基础，它们是如何命名的，以及如何使用模板继承等基础知识。最难的部分已经过去。
接下来，我们将了解大量的可用工具，来帮我们完成常见的模板任务，比如包含其他模板，链接到一个页面或者引入图片。

Symfony内置了几个特殊的Twig标签和函数，来帮助模板设计者简化工作。
在PHP中，模板系统提供了一个可扩展的 *辅助(helper)* 系统用于在模板上下文中提供有用的功能。

你已经看到了一些内置的Twig标签，比如 ``{% block %}`` 和 ``{% extends %}`` 等，现在，你将会学到更多。

.. index::
   single: Templating; Including other templates

.. _including-templates:
.. _including-other-templates:

引用其他模版
~~~~~~~~~~~~~~~~~~~~~~~~~

你经常需要在多个不同的页面中包含同一个模板或者代码片段。
比如在一个“新闻文章”程序中，
用于显示文章的模板代码可能会被用到正文页，或者用到一个显示“人气文章”的页面，乃至一个“最新文章”的列表页等。

当你需要复用一些PHP代码块时，通常都是把这些代码放到一个PHP类或者函数中。
同样在模板中你也可以这么做。通过把可复用的代码放到一个它自己的模板中，然后从其他模板中引用这个模板。
首先，创建一个可复用模板如下。

.. code-block:: html+twig

    {# templates/article/article_details.html.twig #}
    <h2>{{ article.title }}</h2>
    <h3 class="byline">by {{ article.authorName }}</h3>

    <p>
        {{ article.body }}
    </p>

使用 ``{{ include() }}`` 函数实现从任何其他模板中引用此模板：

.. code-block:: html+twig

    {# templates/article/list.html.twig #}
    {% extends 'layout.html.twig' %}

    {% block body %}
        <h1>Recent Articles<h1>

        {% for article in articles %}
            {{ include('article/article_details.html.twig', { 'article': article }) }}
        {% endfor %}
    {% endblock %}

注意，模板命名要遵循相同的典型约定。
在 ``article_details.html.twig`` 模板中使用了 ``article`` 变量，这是我们传入模板的。
本例中，你也可以完全不这样做，
因为在 ``list.html.twig`` 模板中所有可用的变量也都可以在 ``article_details.html.twig`` 中使用
（除非你设置了 `with_context`_ 为 ``false``）。

.. tip::

    ``{'article': article}`` 语法是Twig哈希映射（hash maps）的标准写法（即是一个键值对数组）。
    如果你需要传递多个元素，可以写成 ``{'foo': foo, 'bar': bar}``。

.. index::
   single: Templating; Linking to pages

.. _templating-pages:

链接到页面
~~~~~~~~~~~~~~~~

在你的程序中创建一个链接到其他页面，对于模板来说是再普通不过的事情了。
使用 ``path`` Twig函数（或者PHP中的 ``router`` 辅助函数）基于路由配置来生成URL而非在模板中写死URL。
以后，如果你想修改一个特定页面的链接，你只需要改变路由配置即可；模板将自动生成新的URL。

比如我们打算链接到“welcome”页面，首先定义其路由配置：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/WelcomeController.php

        // ...
        use Symfony\Component\Routing\Annotation\Route;

        class WelcomeController extends AbstractController
        {
            /**
             * @Route("/", name="welcome", methods={"GET"})
             */
            public function index()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        welcome:
            path:     /
            controller: App\Controller\WelcomeController::index
            methods: GET

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="welcome" path="/" controller="App\Controller\WelcomeController::index" methods="GET"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\WelcomeController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('welcome', '/')
                ->controller([WelcomeController::class, 'index'])
                ->methods(['GET'])
            ;
        };

要链到该页面，需使用Twig的 ``path()`` 函数来指定这个路由即可：

.. code-block:: html+twig

    <a href="{{ path('welcome') }}">Home</a>

正如预期的那样，它生成了URL ``/``。现在，处理一个更复杂的路由：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/ArticleController.php

        // ...
        use Symfony\Component\Routing\Annotation\Route;

        class ArticleController extends AbstractController
        {
            /**
             * @Route("/article/{slug}", name="article_show", methods={"GET"})
             */
            public function show($slug)
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        article_show:
            path:       /article/{slug}
            controller: App\Controller\ArticleController::show
            methods: GET

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="article_show"
                path="/article/{slug}"
                controller="App\Controller\ArticleController::show"
                methods="GET"/>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\ArticleController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('article_show', '/articles/{slug}')
                ->controller([ArticleController::class, 'show'])
                ->methods(['GET'])
            ;
        };

这种情况下，你需要同时指定路由名称(``article_show``)和一个 ``{slug}`` 参数的值。
使用这个路由重新访问前面提到的 ``recent_list.html.twig`` 模板，即可正确地链接到该文章。

.. code-block:: html+twig

    {# templates/article/recent_list.html.twig #}
    {% for article in articles %}
        <a href="{{ path('article_show', {'slug': article.slug}) }}">
            {{ article.title }}
        </a>
    {% endfor %}

.. tip::

    你可以通过Twig的 ``url()`` 函数来生成绝对URL：

.. code-block:: html+twig

    <a href="{{ url('welcome') }}">Home</a>

.. index::
   single: Templating; Linking to assets

.. _templating-assets:

链接到资源文件
~~~~~~~~~~~~~~~~~

模板通常也需要一些图片、Javascript、样式文件和其他web资源。
你可以写死它们的路径，比如 ``/images/logo.png``。
但是Symfony通过Twig函数 ``asset()`` ，提供了一个更加动态的选择。

要使用该函数，先安装 *asset* 包：

.. code-block:: terminal

    $ composer require symfony/asset

现在你可以使用 ``asset()`` 函数了:

.. code-block:: html+twig

    <img src="{{ asset('images/logo.png') }}" alt="Symfony!"/>

    <link href="{{ asset('css/blog.css') }}" rel="stylesheet"/>

``asset()`` 函数的主要目的，是让你的程序更加portable（可移动）。
如果你的程序在主机根目录下(如 ``http://example.com``)，生成的路径应该是 ``/images/logo.png``。
但是如果你的程序位于一个子目录中(如 ``http://example.com/my_app``），
那么每个资源路径在生成时应该带有子目录（如 ``/my_app/images/logo.png``）。
``asset()`` 函数负责打点这些，它根据你的程序 “是如何使用的” 而生成相应的正确路径。

.. tip::

    ``asset()`` 函数通过
    :ref:`version <reference-framework-assets-version>`，
    :ref:`version_format <reference-assets-version-format>` 和
    :ref:`json_manifest_path <reference-assets-json-manifest-path>`
    配置选项来支持各种缓存清除策略。

如果你需要资源的绝对URL，可以使用 ``absolute_url()`` Twig函数：

.. code-block:: html+twig

    <img src="{{ absolute_url(asset('images/logo.png')) }}" alt="Symfony!"/>

.. index::
   single: Templating; Including stylesheets and JavaScripts
   single: Stylesheets; Including stylesheets
   single: JavaScript; Including JavaScripts

在Twig中引用样式表和Javascript
---------------------------------------------

每个网站中都不能完全没有样式表和javascript文件。
在Symfony中，这些内容可以利用模板继承来优雅地处理。

.. tip::

    本节将教你如何在Symfony中引用样式表和JavaScript资源。如果你有兴趣编译和创建这些资源，请查看 :doc:`Webpack Encore 文档 </frontend>`，该工具可将 Webpack 和其他现代JavaScript工具无缝集成到Symfony应用中。

首先在你的基础布局模板中添加两个区块来保存你的资源：
一个叫 ``stylesheets`` ，放在 ``head`` 标签里，
另一个叫 ``javascripts``，放在 ``body`` 结束标签上面一行。
这些区块将包含你整个站点所需的全部CSS样式和Javascript脚本：

.. code-block:: html+twig

    {# templates/base.html.twig #}
    <html>
        <head>
            {# ... #}

            {% block stylesheets %}
                <link href="{{ asset('css/main.css') }}" rel="stylesheet"/>
            {% endblock %}
        </head>
        <body>
            {# ... #}

            {% block javascripts %}
                <script src="{{ asset('js/main.js') }}"></script>
            {% endblock %}
        </body>
    </html>

这看起来几乎像普通的HTML，但添加了 ``{% block %}``。
当你需要在子模板中包含额外的样式表或JavaScript时，这些非常有用。
比如，假设你有一个联系页面需要应用一个 ``contact.css`` 样式，*仅* 用在该页面上。
在联系人页面的模板中，你可以这样实现：

.. code-block:: html+twig

    {# templates/contact/contact.html.twig #}
    {% extends 'base.html.twig' %}

    {% block stylesheets %}
        {{ parent() }}

        <link href="{{ asset('css/contact.css') }}" rel="stylesheet"/>
    {% endblock %}

    {# ... #}

在子模板中，你需要覆写 ``stylesheets`` 区块并把你新样式表标签放到该区块里。
由于你只是想把它添加到父区块的内容中（而不是真的 *替代* 它们），
所以你需要先用 ``parent()`` 函数来获取基础模板中的所有 ``stylesheets`` 区块中的内容。

你也可以引用位于你bundle的 ``Resources/public/`` 文件夹下的资源。
你需要运行 ``php bin/console assets:install target [--symlink]`` 命令，
它会把文件复制（或符号链接）到正确的位置（默认情况下，目标是应用的 "public/" 目录）。

.. code-block:: html+twig

    <link href="{{ asset('bundles/acmedemo/css/contact.css') }}" rel="stylesheet"/>

最终结果是，该页面同时引用了 ``main.js`` 以及 ``main.css`` 和 ``contact.css`` 两个样式表。

引用请求、用户或会话
----------------------------------------

Symfony同样在Twig中给了你一个全局的 ``app`` 变量，可以用于访问当前用户、请求以及更多对象。

参考 :doc:`/templating/app_variable` 以了解细节。

输出转义
---------------

在输出任意内容时，Twig自动进行“输出转义（output escaping）”，为的是保护你免受跨站攻击(XSS)。

假设 ``description`` 是 ``I <3 this product``：

.. code-block:: twig

    {# 输出转义自动启用 #}
    {{ description }} {# I &lt;3 this product #}

    {# 使用 raw 过滤器禁用输出转义 #}
    {{ description|raw }} {# I <3 this product #}

.. caution::

    PHP模板不对内容进行自动转义。

更多细节，参考 :doc:`/templating/escaping`。

总结
--------------

模板系统仅仅是Symfony诸多工具中的 *一个*。
它的工作很简单：方便我们渲染动态的、复杂的HTML输出，以便最终将其返回给用户，通过电子邮件或其他方式发送。

继续阅读
-----------

在深入了解Symfony的其他部分之前，请查看 :doc:`配置系统 </configuration>`。

扩展阅读
----------------------

.. toctree::
    :maxdepth: 1
    :glob:

    /templating/*

.. _`Twig`: https://twig.symfony.com
.. _`标签`: https://twig.symfony.com/doc/2.x/tags/index.html
.. _`过滤器`: https://twig.symfony.com/doc/2.x/filters/index.html
.. _`函数`: https://twig.symfony.com/doc/2.x/functions/index.html
.. _`add your own extensions`: https://twig.symfony.com/doc/2.x/advanced.html#creating-an-extension
.. _`with_context`: https://twig.symfony.com/doc/2.x/functions/include.html
.. _`include() function`: https://twig.symfony.com/doc/2.x/functions/include.html
.. _`{% include %} tag`: https://twig.symfony.com/doc/2.x/tags/include.html
