构架
================

你是我的英雄！谁会想到你会在经历两个章节之后仍然在这里？你的努力很快就会得到很好的回报。
前两个章节还没有太深入讲解框架的架构。因为是它让Symfony从众多框架中脱颖而出，现在，让我们现在深入了解架构。

添加日志记录
--------------

一个新的Symfony应用是精巧的：它基本上只是一个路由&控制器系统。 但感谢Flex，安装更多功能很简单。

想要一个日志系统？ 没问题：

.. code-block:: terminal

    $ composer require logger

这将安装和配置（通过食谱）功能强大的 `Monolog`_ 库。
要在控制器中使用记录器，使用 ``LoggerInterface`` 添加一个新的类型提示(type-hinted)参数::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Psr\Log\LoggerInterface;
    use Symfony\Component\Routing\Annotation\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class DefaultController extends AbstractController
    {
        /**
         * @Route("/hello/{name}")
         */
        public function index($name, LoggerInterface $logger)
        {
            $logger->info("Saying hello to $name!");

            // ...
        }
    }

完工！新的日志消息将会被写入 ``var/log/dev.log``。
当然，指令自动配置了很多文件，你可以通过更新其中一个配置文件来更新日志消息的记录路径。

服务和自动装配
---------------------

可是等等！ 发生了一件 *非常* 酷的事情。
Symfony读取了 ``LoggerInterface`` 类型约束并自动计算出它应该传递给我们一个Logger对象！
这称为 *自动装配* (autowiring)。

在Symfony应用中，每项工作都是由一个 *对象* 完成的：Logger对象记录事物，而Twig对象渲染模板。
这些对象被称为 *服务*，它们是帮助我们构建丰富功能的有效工具。


为了让生活更加美好，你可以让Symfony通过使用类型提示向你传递一个服务。
还可以使用哪些其他可能的类或接口？ 通过运行找出：

.. code-block:: terminal

    $ php bin/console debug:autowiring

=============================================================== =====================================
类/接口 类型                                                       服务ID的别名
=============================================================== =====================================
``Psr\Cache\CacheItemPoolInterface``                            alias for "cache.app.recorder"
``Psr\Log\LoggerInterface``                                     alias for "monolog.logger"
``Symfony\Component\EventDispatcher\EventDispatcherInterface``  alias for "debug.event_dispatcher"
``Symfony\Component\HttpFoundation\RequestStack``               alias for "request_stack"
``Symfony\Component\HttpFoundation\Session\SessionInterface``   alias for "session"
``Symfony\Component\Routing\RouterInterface``                   alias for "router.default"
=============================================================== =====================================

这只是完整列表的简短摘要！ 当你添加更多包时，这个工具列表将会增长。

创建服务
-----------------

为了保持代码的有序性，您甚至可以创建自己的服务！假设你想要生成随机问候语（例如“Hello”，“Yo”等）。
我们可以创建一个新类，而不是将这段代码直接放在你的控制器中::

    // src/GreetingGenerator.php
    namespace App;

    class GreetingGenerator
    {
        public function getRandomGreeting()
        {
            $greetings = ['Hey', 'Yo', 'Aloha'];
            $greeting = $greetings[array_rand($greetings)];

            return $greeting;
        }
    }

很好！可以立即在控制器中使用它::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use App\GreetingGenerator;
    use Psr\Log\LoggerInterface;
    use Symfony\Component\Routing\Annotation\Route;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class DefaultController extends AbstractController
    {
        /**
         * @Route("/hello/{name}")
         */
        public function index($name, LoggerInterface $logger, GreetingGenerator $generator)
        {
            $greeting = $generator->getRandomGreeting();

            $logger->info("Saying $greeting to $name!");

            // ...
        }
    }

完工！Symfony将自动实例化 ``GreetingGenerator`` 并将它作为一个参数传递过去。
但是，我们 *可以* 将记录器逻辑移动到 ``GreetingGenerator`` 吗？可以!
你可以在服务中使用自动装配来访问 *其他* 服务。 唯一的区别在于它是在构造函数中完成的:

.. code-block:: diff

    // src/GreetingGenerator.php
    + use Psr\Log\LoggerInterface;

    class GreetingGenerator
    {
    +     private $logger;
    +
    +     public function __construct(LoggerInterface $logger)
    +     {
    +         $this->logger = $logger;
    +     }

        public function getRandomGreeting()
        {
            // ...

     +        $this->logger->info('Using the greeting: '.$greeting);

             return $greeting;
        }
    }

是的!这样也有效：没有配置，没有时间浪费。
那么继续下去吧！

Twig 扩展 & 自动配置
----------------------------------

感谢 Symfony 的服务处理，您可以通过多种方式 *扩展* Symfony，例如通过创建一个事件订阅器或一个安全表决器
来构建复杂的授权规则。让我们为Twig添加一个名为 ``greet`` 的新过滤器。 怎么做？
只需创建一个继承 ``AbstractExtension`` 的类::

    // src/Twig/GreetExtension.php
    namespace App\Twig;

    use App\GreetingGenerator;
    use Twig\Extension\AbstractExtension;
    use Twig\TwigFilter;

    class GreetExtension extends AbstractExtension
    {
        private $greetingGenerator;

        public function __construct(GreetingGenerator $greetingGenerator)
        {
            $this->greetingGenerator = $greetingGenerator;
        }

        public function getFilters()
        {
            return [
                new TwigFilter('greet', [$this, 'greetUser']),
            ];
        }

        public function greetUser($name)
        {
            $greeting =  $this->greetingGenerator->getRandomGreeting();

            return "$greeting $name!";
        }
    }

只需创建 *一个* 文件，你就可以立即使用:

.. code-block:: twig

    {# templates/default/index.html.twig #}
    {# Will print something like "Hey Symfony!" #}
    <h1>{{ name|greet }}</h1>

这是如何运作的？Symfony 注意到你的类继承自 ``AbstractExtension``，
所以 *自动* 将其注册为Twig扩展。这称为自动配置(autoconfiguration)，它适用于 *许多* 许多事情。
只需创建一个类，然后继承一个基类（或实现一个接口），Symfony 负责其余的工作。

快如闪电: 缓存容器
-----------------------------------

在看到 Symfony 这么多的自动处理机制后，你可能会想：“不会这伤害了性能？“事实上并不会！Symfony快如闪电。

这怎么可能？服务系统由一个非常重要的叫“容器”的对象来管理。大多数框架都有一个容器，
但 Symfony 是独一无二的，因为它具有 *缓存性(cached)* 。当你加载第一个页面时，所有服务信息都是编译并保存。
这意味着自动装配和自动配置功能添加 *没有* 开销的！
这也意味着你会得到 *很棒* 的错误信息：Symfony会在构建容器时检查和验证 *所有东西*。

现在你可能会担心你更新了一个文件应该怎么办？缓存会重建吗？我喜欢你的想法！它很聪明，会在下一个页面加载时重建。
但这确实是下一节的主题。

开发 & 生成: 环境
-------------------------------------------

框架的主要工作之一是使调试变得容易！
我们的应用 *提供* 了很棒的工具来应对：Web调试工具栏显示在页面底部，
错误信息会以显眼、美观、明确的方式展现，并在需要的时候自动重建配置缓存。

但是当你部署到生产时呢？我们需要隐藏这些工具和优化速度！

这是由 Symfony 的 *环境* 系统解决的，它们有三个：``dev``，``prod`` 和 ``test``。
根据环境，Symfony会加载在 ``config/`` 目录中不同的文件：

.. code-block:: text

    config/
    ├─ services.yaml
    ├─ ...
    └─ packages/
        ├─ framework.yaml
        ├─ ...
        ├─ **dev/**
            ├─ monolog.yaml
            └─ ...
        ├─ **prod/**
            └─ monolog.yaml
        └─ **test/**
            ├─ framework.yaml
            └─ ...
    └─ routes/
        ├─ annotations.yaml
        └─ **dev/**
            ├─ twig.yaml
            └─ web_profiler.yaml

这是一个 *伟大* 的想法：通过改变一个配置（环境），你的应用从调试友好的体验转变为速度而优化的体验了。

哦，怎么改变环境？更改 ``APP_ENV`` 环境变量的值 ``dev`` 为 ``prod``：

.. code-block:: diff

    # .env
    - APP_ENV=dev
    + APP_ENV=prod

但我接下来想谈谈环境变量。将值改回 ``dev``：当你在本地工作时，可以很容易使用调试工具。

环境变量
---------------------

每个应用包含的配置在每个服务器上都有所不同 - 比如数据库连接信息或密码。
那配置应该如何存储？在文件中？或者一些另一种方式？

Symfony 遵循行业最佳实践，将基于服务器的配置存储为 *environment* 变量。
这意味着 Symfony 可以与平台即服务（PaaS）部署系统以及Docker完美配合。

但是在开发过程中设置环境变量可能会很痛苦。
这就是为什么在 ``APP_ENV`` 环境变量在当前环境中没有配置的情况下，你的应用会自动加载一个 ``.env`` 文件。
然后，此文件中的键会成为环境变量，并由你的应用读取：

.. code-block:: bash

    # .env
    ###> symfony/framework-bundle ###
    APP_ENV=dev
    APP_SECRET=cc86c7ca937636d5ddf1b754beb22a10
    ###< symfony/framework-bundle ###

起初，该文件不包含太多内容。但随着你的应用的增长，你将根据需要添加更多配置。
但是，实际上，它变得更有趣！假设你的应用需要数据库ORM。 让我们安装Doctrine ORM：

.. code-block:: terminal

    $ composer require doctrine

感谢 Flex 安装的新食谱，再次查看 ``.env`` 文件：

.. code-block:: diff

    ###> symfony/framework-bundle ###
    APP_ENV=dev
    APP_SECRET=cc86c7ca937636d5ddf1b754beb22a10
    ###< symfony/framework-bundle ###

    + ###> doctrine/doctrine-bundle ###
    + # ...
    + DATABASE_URL=mysql://db_user:db_password@127.0.0.1:3306/db_name
    + ###< doctrine/doctrine-bundle ###

新的 ``DATABASE_URL`` 环境变量 *自动添加* 并且已被新的 ``doctrine.yaml`` 配置文件引用。
通过结合环境变量和Flex，你可以毫不费力地使用行业最佳实践。

继续阅读!
-----------

请叫我疯子，但在阅读完这篇文章之后，你应该对 Symfony 最 *重要* 的部分感到满意。
Symfony 中的所有一切都旨在让你不受限制，因此你可以继续写代码和添加功能，所有这些都可以满足您的速度和质量要求。

这就是快速上手的全部内容。从身份验证到表单再到缓存，还有很多东西要探寻。现在准备好深入研究这些主题了吗？
不要再犹豫了 - 去官方：:doc:`/index` 并选择你想要的任何指南。

.. _`Monolog`: https://github.com/Seldaek/monolog
