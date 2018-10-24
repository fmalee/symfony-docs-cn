Flex: 管理你的应用
==============================

阅读本教程的第一部分后，您已经确定了 Symfony 值得再花费10分钟。 正确的选择！
在第二部分中，您将了解到 Symfony Flex：一个令人惊奇的工具，它使得添加新功能变得像运行一个命令一样简单。
这也是为什么 Symfony 会同时是小型微服务和大型应用的理想选择的原因。好奇吗？ 很好！


Symfony: 以精巧开局!
---------------------

除非你正在构建一个纯API（很快就会有更多内容！），否则你可能会需要渲染HTML。
要做到这一点，你可以使用 `Twig`_。 Twig是一款灵活，快速，安全的PHP模板引擎。
它使您的模板更具可读性和简洁性; 它也对网页设计师更友好。

Twig已经安装在我们的应用中了吗？ 其实还没有！这其实是一个好主意！当你开启一个新的 Symfony 项目时，它很*精巧*：
只在你的 ``composer.json`` 文件中包含了最关键的依赖项：

.. code-block:: text

    "require": {
        "...",
        "symfony/console": "^4.1",
        "symfony/flex": "^1.0",
        "symfony/framework-bundle": "^4.1",
        "symfony/yaml": "^4.1"
    }

这使得Symfony不同于任何其他PHP框架！ 它不是一个*巨大*的应用，带有你*可能*需要的*每个*功能，
一个 Symfony 应用小巧，简单，*快速*。 而且你可以完全控制需要添加的内容。

Flex 食谱(Recipes)和别名
------------------------

所以我们应该如何安装和配置 Twig？只要运行一个命令：

.. code-block:: terminal

    $ composer require twig

Symfony Flex在幕后做了两件*非常*有趣的事情：一个 Composer 插件已经安装到了我们的项目中。

首先，``twig`` 不是 Composer 包的名称：它是一个指向``symfony/twig-bundle`` 的 Flex *别名*
。 Flex 为 Composer 解析了该别名。

第二，Flex为 ``symfony/twig-bundle`` 安装了一个*食谱*。
什么是食谱？这是一种通过添加和修改文件来让一个仓库实现自动配置自身的方式。
得益于食谱，添加功能是无缝和自动化的：安装一个包，然后你就完成了所有工作！

你可以在 `https://flex.symfony.com`_ 上找到完整的食谱和别名。

这个食谱都做了什么？ 除了自动在 ``config/bundles.php`` 中启用该功能包外，它增加了三件事：

``config/packages/twig.yaml``
    一个用于设置具有合理默认值的 Twig 配置文件。

``config/routes/dev/twig.yaml``
    一个帮助你调试错误页面的路由。

``templates/``
    一个用于存放模板文件的目录。食谱同样添加了一个 ``base.html.twig`` 布局文件。

Twig: 渲染一个模板
--------------------------

感谢 Flex，一个命令，就能让你立刻使用 Twig：

.. code-block:: diff

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Symfony\Component\Routing\Annotation\Route;
    - use Symfony\Component\HttpFoundation\Response;
    + use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    -class DefaultController
    +class DefaultController extends AbstractController
     {
         /**
          * @Route("/hello/{name}")
          */
         public function index($name)
         {
    -        return new Response("Hello $name!");
    +        return $this->render('default/index.html.twig', [
    +            'name' => $name,
    +        ]);
         }

通过继承 ``AbstractController``，你可以访问许多的快捷方法和工具，比如 ``render()``。
创建新模板：

.. code-block:: twig

    {# templates/default/index.html.twig #}
    <h1>Hello {{ name }}</h1>

只是这样就足够！``{{name}}`` 语法将输出通过控制器传递过来的``name``变量。
如果你是 Twig 的新手，欢迎！ 你会在以后了解更多 Twig 的语法和能力。

但是，现在该页面*只*包含 ``h1`` 标签。 通过继承 ``base.html.twig`` 来给它一个HTML布局：

.. code-block:: twig

    {# templates/default/index.html.twig #}
    {% extends 'base.html.twig' %}

    {% block body %}
        <h1>Hello {{ name }}</h1>
    {% endblock %}

这称为模板继承：我们的页面现在从 ``base.html.twig`` 继承了HTML结构。

分析器: 调试的天堂
----------------------------

Symfony *最酷*的一个功能现在还没有安装，我们来解决这个问题：

.. code-block:: terminal

    $ composer require profiler

是的! 这是另一个别名！ Flex *同样*通过食谱自动化安装了Symfony的Profiler。
结果是什么？刷新一下！

看到底部的黑条了？ 那是网页调试工具栏，它是你最好的朋友。通过将鼠标悬停在每个图标上，您可以获得有关控制器的执行信息
，性能信息，缓存命中和未命中等等。点击任何图标进入 *profiler*，你就可以获取*更*详细的调试
和性能数据！

哦，当您安装更多库时，您将获得更多调试工具（如一个显示数据查询的工具栏图标）。

使用分析器非常简单，因为它配置了*自身*，这要归功于 Flex 的食谱。
还有什么可以轻松安装的吗？


富 API 支持
----------------

你在构建API吗？ 你可以从任何控制器轻松的返回JSON::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Symfony\Component\Routing\Annotation\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class DefaultController extends AbstractController
    {
        // ...

        /**
         * @Route("/api/hello/{name}")
         */
        public function apiExample($name)
        {
            return $this->json([
                'name' => $name,
                'symfony' => 'rocks',
            ]);
        }
    }

但是对于一个*真正*全功能(rich) API，请尝试安装 `Api Platform`_：

.. code-block:: terminal

    $ composer require api

这是一个 ``api-platform/api-pack`` 的别名，它依赖于几个其他软件包，
如 Symfony 的 Validator 和 Security 组件，以及 Doctrine ORM。
事实上，Flex安装了*5*个食谱！

但和往常一样，我们可以立即开始使用新库。 想要创建一个用于 ``product`` 表的丰富API？
创建一个 ``Product`` 实体并给它 ``@ApiResource()`` 注释::

    // src/Entity/Product.php
    namespace App\Entity;

    use ApiPlatform\Core\Annotation\ApiResource;
    use Doctrine\ORM\Mapping as ORM;

    /**
     * @ORM\Entity()
     * @ApiResource()
     */
    class Product
    {
        /**
         * @ORM\Id
         * @ORM\GeneratedValue(strategy="AUTO")
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string")
         */
        private $name;

        /**
         * @ORM\Column(type="int")
         */
        private $price;

        // ...
    }

完工！你现在拥有列出、添加、更新和删除产品的端点(endpoints)！不相信我？ 试试列出你的的路由：

.. code-block:: terminal

    $ php bin/console debug:router

    ------------------------------ -------- -------------------------------------
     Name                           Method   Path
    ------------------------------ -------- -------------------------------------
     api_products_get_collection    GET      /api/products.{_format}
     api_products_post_collection   POST     /api/products.{_format}
     api_products_get_item          GET      /api/products/{id}.{_format}
     api_products_put_item          PUT      /api/products/{id}.{_format}
     api_products_delete_item       DELETE   /api/products/{id}.{_format}
     ...
    ------------------------------ -------- -------------------------------------

轻松删除食谱
---------------------

还不确定吗？ 没问题：那现在就删除它：

.. code-block:: terminal

    $ composer remove api

Flex 将*卸载*该食谱：删除对应文件并取消更改对你的应用的更改，让其恢复原始状态。你可以随意实验！

更多功能，架构和速度
-------------------------------------

我希望你和我一样对 Flex 感到兴奋！ 但是我们还有*一个*章节，它是最重要的部分。
我想告诉你 Symfony 如何快速授权你构建功能而*不*牺牲代码质量或性能。
这全都和服务容器有关，它是Symfony的超能力。
请阅读 :doc:`/quick_tour/the_architecture` 以继续下去。

.. _`https://flex.symfony.com`: https://flex.symfony.com
.. _`Api Platform`: https://api-platform.com/
.. _`Twig`: https://twig.symfony.com/
