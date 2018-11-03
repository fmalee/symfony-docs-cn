Symfony 基本面
===============

从零学习Symfony，只要10分钟！本文将带你贯穿框架中的一些重要概念，并通过简单的小项目来解释如何快速上手。

如果你以前用过web开发框架，你对symfony的感觉会像“在家里”一样自然。如果没有，欢迎来到web开发的全新方式。

Symfony *拥抱* 最佳实践，保持着向后兼容（是的，更新总是一个安全又简单的操作！）并提供官方的长期维护支持。

.. _installing-symfony2:

下载 Symfony
-------------------

首先，确保你已经安装了 `Composer`_ 和 PHP 7.1.3(或是更高版本)。

准备好了吗? 在终端中运行:

.. code-block:: terminal

    $ composer create-project symfony/skeleton quick_tour

该命令创建了一个  ``quick_tour/``  目录，并在里面安装好了精巧但强大的 Symfony 应用：

.. code-block:: text

    quick_tour/
    ├─ .env
    ├─ .env.dist
    ├─ bin/console
    ├─ composer.json
    ├─ composer.lock
    ├─ config/
    ├─ public/index.php
    ├─ src/
    ├─ symfony.lock
    ├─ var/
    └─ vendor/

我们现在可以在浏览器中运行项目了吗？当然！
你可以安装 :doc:`Nginx or Apache </setup/web_server_configuration>` 并配置他们的文档根目录到 ``public/`` 目录。
但是，为了便于开发，Symfony 配有自己的服务器。
安装和运行方式：

.. code-block:: terminal

    $ composer require server --dev
    $ php bin/console server:start

在浏览器中打开 ``http://localhost:8000`` 试试你的新应用吧！

.. image:: /_images/quick_tour/no_routes_page.png
   :align: center
   :class: with-browser

基本原理: 路由, 控制器, 响应
-----------------------------------------

我们的项目只有大约15个文件，但是已经可以当做一个优美的 API、一个强壮的web应用、甚至于一个微型服务器(microservice)。
Symfony 以小开局，但却可以成规模扩大。

但是在这之前，让我们通过构建第一个页面来挖掘基础知识。

从 ``config/routes.yaml`` 开始：*我们* 能在这里定义第一个页面的 URL。解开已经存在于文件中的例子：

.. code-block:: yaml

    # config/routes.yaml
    index:
        path: /
        controller: 'App\Controller\DefaultController::index'

这就是一个 *路由*：它定义了页面的URL(``/``)和 “controller”：
该 *函数* 会在任何人访问这个URL的时候被调用。不过现在这个函数还不存在，让我们创建它！

在 ``src/Controller`` 里，创建一个新的 ``DefaultController`` 类以及一个 ``index`` 方法::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;

    class DefaultController
    {
        public function index()
        {
            return new Response('Hello!');
        }
    }

就是这样！现在访问 ``http://localhost:8000/``。Symfony发现该URL和我们的路由匹配，
然后执行新增的 ``index()`` 方法。

一个路由器就是一个包含 *一个* 路由的普通函数：它必须返回一个Symfony ``Response`` 对象。
但是该响应可以包含任何东西：简单文本、JSON，或是一个完整的HTML页面。

但是路由系统已经 *足够* 强大。所以让我们创建一个更有趣的路由：

.. code-block:: diff

    # config/routes.yaml
    index:
    -     path: /
    +     path: /hello/{name}
        controller: 'App\Controller\DefaultController::index'

该 URL 已经改变：*现在* 是 ``/hello/*``：``{name}`` 字符就像一个匹配任何东西的通配符。
它变得更强了！同时我们也要更新一下控制器：

.. code-block:: diff

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;

    class DefaultController
    {
    -     public function index()
    +     public function index($name)
        {
    -         return new Response('Hello!');
    +         return new Response("Hello $name!");
        }
    }

现在访问 ``http://localhost:8000/hello/Symfony``，你会看见：Hello Symfony!
URL 中的 ``{name}`` 的值变成了控制器中的参数。

但是我们可以更简洁一些！所以让我们安装注释(annotations) 扩展：

.. code-block:: terminal

    $ composer require annotations

现在，使用 ``#`` 注释掉 YAML 里的路由：

.. code-block:: yaml

    # config/routes.yaml
    # index:
    #     path: /hello/{name}
    #     controller: 'App\Controller\DefaultController::index'

取而代之，我们在控制器的方法 *上方* 添加路由：

.. code-block:: diff

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;
    + use Symfony\Component\Routing\Annotation\Route;

    class DefaultController
    {
    +    /**
    +     * @Route("/hello/{name}")
    +     */
         public function index($name) {
             // ...
         }
    }

它会像之前一样工作！但是通过使用注释，路由和控制器就能放置在一起。
需要另一个页面？只要再在 ``DefaultController`` 里添加一个路由和方法::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing\Annotation\Route;

    class DefaultController
    {
        // ...

        /**
         * @Route("/simplicity")
         */
        public function simple()
        {
            return new Response('Simple! Easy! Great!');
        }
    }

路由还可以 *再* 继续添加，但是我们将在下次再进行！现在，我们的应用需要更多的功能！比如模板引擎、日志记录、调试工具以及其他。

请阅读 :doc:`/quick_tour/flex_recipes` 以继续下去。

.. _`Composer`: https://getcomposer.org/
