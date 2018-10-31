.. index::
   single: Create your First Page in Symfony

.. _creating-pages-in-symfony2:
.. _creating-pages-in-symfony:

在Symfony中创建第一个页面
=================================

创建一个新页面——无论是 HTML 还是 JSON 输出——都是一个简单的“两步”操作：

#. **创建一个路由**: 路由（route）是一个指向你的页面URL（比如/about），同时它映射到一个控制器；

#. **创建一个控制器**: 控制器（controller）是你为了构造页面而写的功能。
   你要拿到发送来的request请求信息，用它创建一个 Symfony 的 ``Response`` 对象，令其包含HTML内容，JSON字符串或是其他。

.. admonition:: Screencast
    :class: screencast

    更喜欢视频教程? 可以观看 `Stellar Development with Symfony`_ 系列录像.

.. seealso::

    Symfony*包含(embraces)*HTTP请求-响应生命周期。
    要了解更多信息，请参阅 :doc:`/introduction/http_fundamentals`。

.. index::
   single: Page creation; Example

创建一个页面：路由和控制器
-------------------------------------

.. tip::

    在开始之前，确保你已经阅读了 :doc:`/setup` 章节，同时已经可以在浏览器中访问Symfony程序。

假设你要新建一个 ``/lucky/number`` 页面，用于生成一个随机的幸运数字并且输出它。
那么，要先创建一个“控制器类”并添加一个"控制器"方法::

    <?php
    // src/Controller/LuckyController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;

    class LuckyController
    {
        public function number()
        {
            $number = random_int(0, 100);

            return new Response(
                '<html><body>Lucky number: '.$number.'</body></html>'
            );
        }
    }

现在，你需要将此控制器功能与一个公共URL（例如 ``/lucky/number``）相关联，
以便在用户浏览时执行 ``number()`` 方法。
通过在 ``config/routes.yaml`` 文件中创建一个 **路由** 来定义此关联：

.. code-block:: yaml

    # config/routes.yaml

    # "app_lucky_number" 这个路由名称现在还不是重点
    app_lucky_number:
        path: /lucky/number
        controller: App\Controller\LuckyController::number

完工！如果你使用的是Symfony Web服务器，请尝试访问：

    http://localhost:8000/lucky/number

如果你看到一个幸运数字被输出，那么恭喜！不过在玩转乐透之前，先要了解它是如何工作的。
还记得创建页面的两个步骤吗？

#. *创建路由*: 在 ``config/routes.yaml`` 中，路由定义了你的URL页面（``path``）和要调用的 ``controller``。
   你将在 :doc:`路由 </routing>` 章节了解有关信息，包括如何创建 *变量* URL;

#. *创建控制器*: 这是一个构建页面并最终返回 ``Response`` 对象的函数。
   你将在 :doc:`控制器 </controller>` 章节了解有关信息，包括如何返回JSON响应。

.. tip::

    要更快地创建控制器，让Symfony为你生成它：

    .. code-block:: terminal

        $ php bin/console make:controller

.. _annotation-routes:

注释路由
-----------------

Symfony还允许你使用注释路由，而不是在 YAML 中定义路由。为此，请安装 ``annotations`` 包：

.. code-block:: terminal

    $ composer require annotations

你现在可以直接在控制器上方添加路由了：

.. code-block:: diff

    // src/Controller/LuckyController.php

    // ...
    + use Symfony\Component\Routing\Annotation\Route;

    class LuckyController
    {
    +     /**
    +      * @Route("/lucky/number")
    +      */
        public function number()
        {
            // 这里似乎完全一样
        }
    }

仅此而已！该页面 -  ``http://localhost:8000/lucky/number`` 将像以前一样工作！
注释是配置路由的推荐方法。

.. _flex-quick-intro:

使用Symfony Flex自动安装食谱
-----------------------------------------

你可能没有注意到，但是当你运行 ``composer require annotations`` 时，发生了两件特别的事情，
这要归功于一个名为 :doc:`Flex </setup/flex>` 的强大的Composer插件。

首先，``annotations`` 不是真正的包名称：
它是Flex为 ``sensio/framework-extra-bundle`` 起的一个 *别名* （即快捷方式）。

其次，在下载该软件包之后，Flex执行了一个 *食谱*(recipe)，这是一组自动指令，告诉Symfony如何集成外部软件包。
`Flex 食谱`_ 适用于许多软件包，并且能够做很多事情，例如添加配置文件，创建目录，
更新 ``.gitignore`` 以及向 ``.env`` 文件添加新配置。
Flex 会 *自动* 安装软件包，以便你可以轻松编写代码。

你可以通过阅读 ":doc:`/setup/flex`"来了解有关Flex的更多信息。
但这不是必需的：当你添加依赖包时，Flex会在后台自动运行。

bin/console 命令
-----------------------

你的项目内部已经集成了一个强大的调试工具：``bin/console`` 命令。尝试运行它：

.. code-block:: terminal

    $ php bin/console

你应该看到一个命令列表，它可以为你提供调试信息，帮助生成代码，生成数据库迁移等等。
当你安装更多软件包时，将会看到更多命令。

要获取系统中*所有*路由的列表，可以使用 ``debug:router`` 命令：

.. code-block:: terminal

    $ php bin/console debug:router

你应该能在最顶层看到你的 ``app_lucky_number`` 路由：

================== ======== ======== ====== ===============
 Name               Method   Scheme   Host   Path
================== ======== ======== ====== ===============
 app_lucky_number   ANY      ANY      ANY    /lucky/number
================== ======== ======== ====== ===============

你还将在 ``app_lucky_number`` 下面看到调试路由 - 在下一节中有更多关于调试路由的信息。

你将继续学习更多命令！

Web调试工具栏：调试梦想
--------------------------------------

Symfony的 *杀手级* 功能之一是Web调试工具栏：
一个在开发过程中在页面底部显示 *大量* 调试信息的工具栏。
安装名为 ``symfony/profiler-pack`` 的软件包后，开箱即用。

你将在页面底部看到一个黑条。你将了解更多有关它所包含的所有信息，
但可以随意进行试验：将鼠标悬停在上面并单击不同的图标以获取有关路由，性能，日志记录等信息。

渲染模板
--------------------

如果你是从控制器返回HTML，则可能需要渲染一个模板。
幸运的是，Symfony提供 `Twig`_：一种简单，强大且非常有趣的模板语言。

确保 ``LuckyController`` 继承了Symfony的 :class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\AbstractController` 基类：

.. code-block:: diff

    // src/Controller/LuckyController.php

    // ...
    + use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    - class LuckyController
    + class LuckyController extends AbstractController
    {
        // ...
    }

现在，使用方便的 ``render()`` 函数来渲染模板。
传递一个 ``number`` 变量，以便你可以在Twig中使用它::

    // src/Controller/LuckyController.php

    // ...
    class LuckyController extends AbstractController
    {
        /**
         * @Route("/lucky/number")
         */
        public function number()
        {
            $number = random_int(0, 100);

            return $this->render('lucky/number.html.twig', [
                'number' => $number,
            ]);
        }
    }

模板文件存在于 ``templates/`` 目录中，该目录是在安装 Twig 时自动创建的。
创建一个新的 ``templates/lucky`` 目录，并在该目录下新建 ``number.html.twig`` 文件：

.. code-block:: twig

    {# templates/lucky/number.html.twig #}

    <h1>你的幸运数字是 {{ number }}</h1>

 ``{{ number }}`` 语法在Twig中用于打印变量。刷新浏览器以获取*新*的幸运号码！

    http://localhost:8000/lucky/number

现在你可能在想Web调试工具栏到底在哪里：那是因为当前模板中没有``</body>`` 标签。
你可以自己添加body元素，或者继承 ``base.html.twig``，它包含所有默认的HTML元素。

在 :doc:`/templating` 章节中，你将了解有关 Twig 的所有信息：如何实现循环，渲染其他模板以及利用其强大的布局继承系统。

浏览项目结构
----------------------------------

好消息！你早已经在项目中最重要的目录中开工了：

``config/``
    包含......当然是配置啦！你可以在这里配置路由，:doc:`services </service_container>` 和依赖包。

``src/``
    你的所有PHP代码都存在于此处。

``templates/``
    你的所有Twig模板都存在于此处。

大多数情况下，你将在 ``src/``，``templates/`` 或 ``config/`` 中工作。
当你继续阅读时，你将学习在每个目录中可以做些什么。

那么项目中的其他目录呢？

``bin/``
    著名的 ``bin/console`` 文件存在于此处（以及其他不太重要的可执行文件）。

``var/``
    这是存储系统自动创建的文件的位置，如缓存文件（``var/cache/``）和日志（``var/log/``）。

``vendor/``
    第三方（即“vendor”）库存放在这里！这些都是通过 `Composer`_ 包管理器下载的。

``public/``
    这是项目的文档根目录：你可以在此处放置任何可公开访问的文件。

当你安装新依赖包后，系统将在需要时自动创建新目录。

下一步是什么？
----------------

恭喜！你已经开始掌握Symfony并学习构建漂亮、功能强大、快速且可维护的应用的全新方式。

那么，是时候通过阅读这些文章来掌握基础知识了：

* :doc:`/routing`
* :doc:`/controller`
* :doc:`/templating`
* :doc:`/configuration`

然后，了解其他重要主题，如 :doc:`服务容器 </service_container>`、
:doc:`表单系统 </forms>`，使用 :doc:`Doctrine </doctrine>` （如果你需要查询数据库）等等！

玩得开心！

深入了解HTTP和框架基础知识
--------------------------------------------

.. toctree::
    :hidden:

    routing

.. toctree::
    :maxdepth: 1
    :glob:

    introduction/*

.. _`Twig`: https://twig.symfony.com
.. _`Composer`: https://getcomposer.org
.. _`Stellar Development with Symfony`: https://symfonycasts.com/screencast/symfony/setup
.. _`Flex 食谱`: https://flex.symfony.com
