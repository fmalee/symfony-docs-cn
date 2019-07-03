.. index::
   single: Controller

控制器
==========

控制器是你创建的一个PHP函数，它从 ``Request`` 对象读取信息并创建和返回一个 ``Response`` 对象。
响应可能是HTML页面、JSON、XML、文件下载、重定向、404错误或任何其他内容。
控制器负责实施你的应用渲染页面内容所需的任意逻辑。

.. tip::

    如果你尚未创建第一个页面，请查看 :doc:`/page_creation`，然后再回来！

.. index::
   single: Controller; Simple example

一个简单的控制器
-------------------

虽然控制器可以是任何可调用的PHP（函数，对象上的方法或 ``Closure``），但一个控制器通常是控制器类中的一个方法::

    // src/Controller/LuckyController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing\Annotation\Route;

    class LuckyController
    {
        /**
         * @Route("/lucky/number/{max}", name="app_lucky_number")
         */
        public function number($max)
        {
            $number = random_int(0, $max);

            return new Response(
                '<html><body>Lucky number: '.$number.'</body></html>'
            );
        }
    }

控制器就是该 ``number()`` 方法，它位于 ``LuckyController`` 控制器类中。

这个控制器非常简单：

* *line 2*: Symfony利用PHP的命名空间函数来命名整个控制器类。

* *line 4*: Symfony再次利用PHP的命名空间函数：使用 ``use`` 关键字导入了控制器必须返回的 ``Response`` 类。

* *line 7*: 从技术上讲，该类可以被命名为任何东西，但它按照惯例以 ``Controller`` 为后缀。

* *line 12*: 得益于 :doc:`路由通配符 </routing>` ``{max}`` ，该动作(action)方法可以增加一个 ``$max`` 参数。

* *line 16*: 控制器创建并返回一个 ``Response`` 对象。

.. index::
   single: Controller; Routes and controllers

将URI映射到控制器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

为了能 *浏览* 此控制器的输出，你需要通过路由将一个URL映射到该控制器。
这是通过 ``@Route("/lucky/number/{max}")`` :ref:`路由注释 <annotation-routes>` 完成的。

要查看页面，请在浏览器中打开此URL:

    http://localhost:8000/lucky/number/100

有关路由的更多信息，请参阅 :doc:`/routing`。

.. index::
   single: Controller; Base controller class

.. _the-base-controller-class-services:
.. _the-base-controller-classes-services:

控制器基类 & 服务
------------------------------------

为了帮助开发，symfony提供了一个可选的控制器基类，名为
:class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\AbstractController`。
你可以继承通过它以访问辅助方法。

在控制器类的顶部添加 ``use`` 语句，然后修改 ``LuckyController`` 以继承它：

.. code-block:: diff

    // src/Controller/LuckyController.php
    namespace App\Controller;

    + use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    - class LuckyController
    + class LuckyController extends AbstractController
    {
        // ...
    }

仅此而已！你现在可以访问诸如 :ref:`$this->render() <controller-rendering-templates>` 之类的方法以及你接下来将学习的更多其他方法。

.. index::
   single: Controller; Redirecting

生成URL
~~~~~~~~~~~~~~~

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\AbstractController::generateUrl` 方法只是一个生成给定路由的URL的辅助方法::

    $url = $this->generateUrl('app_lucky_number', ['max' => 10]);

重定向
~~~~~~~~~~~

如果要将用户重定向到另一个页面，请使用 ``redirectToRoute()`` 和  ``redirect()`` 方法::

    use Symfony\Component\HttpFoundation\RedirectResponse;

    // ...
    public function index()
    {
        // 重定向到 "homepage" 路由
        return $this->redirectToRoute('homepage');

        // redirectToRoute 只是一个快捷方式:
        // return new RedirectResponse($this->generateUrl('homepage'));

        // 永久性 - 301 重定向
        return $this->redirectToRoute('homepage', [], 301);

        // 重定向到带参数的路由
        return $this->redirectToRoute('app_lucky_number', ['max' => 10]);

        // 重定向到路由并维持原有的查询字符串参数
        return $this->redirectToRoute('blog_show', $request->query->all());

        // 重定向到外部
        return $this->redirect('http://symfony.com/doc');
    }

.. caution::

    ``redirect()`` 方法不以任何方式检查其目的地URL。
    如果你重定向到最终用户提供的URL，你的应用会遭遇 `未经验证的重定向安全漏洞`_。

.. index::
   single: Controller; Rendering templates

.. _controller-rendering-templates:

渲染模板
~~~~~~~~~~~~~~~~~~~

如果你要提供HTML服务，则需要渲染一个模板。
``render()`` 方法会渲染模板 **并** 将该内容放入到一个 ``Response`` 对象中::

    // 渲染 templates/lucky/number.html.twig
    return $this->render('lucky/number.html.twig', ['number' => $number]);

可以在 :doc:`创建和使用模板 </templating>` 章节中了解更多关于模板和Twig的内容。

.. index::
   single: Controller; Accessing services

.. _controller-accessing-services:
.. _accessing-other-services:

获取服务
~~~~~~~~~~~~~~~~~

Symfony开箱即拥有许多有用的对象，称为 :doc:`服务 </service_container>`。
它们用于渲染模板，发送电子邮件，查询数据库以及你可以想到的任何其他“工作”。

如果你在控制器中的需要一个服务，只需用该服务的类（或接口）作为一个带类型约束(type-hint)的参数。
Symfony会自动给你传递所需的服务::

    use Psr\Log\LoggerInterface;
    // ...

    /**
     * @Route("/lucky/number/{max}")
     */
    public function number($max, LoggerInterface $logger)
    {
        $logger->info('We are logging!');
        // ...
    }

真是神奇!

你可以获取到哪些其他服务？要查看它们，请使用 ``debug:autowiring`` console命令：

.. code-block:: terminal

    $ php bin/console debug:autowiring

如果需要控制一个参数的 *确切* 值，可以使用其名称来 :ref:`绑定 <services-binding>` 该参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            # 显式的配置服务
            App\Controller\LuckyController:
                public: true
                bind:
                    # 对于任何 $logger 参数，传递此特定服务
                    $logger: '@monolog.logger.doctrine'
                    # 对于任何 $projectDir 参数(argument)，传递此参数(parameter)值
                    $projectDir: '%kernel.project_dir%'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <!-- Explicitly configure the service -->
                <service id="App\Controller\LuckyController" public="true">
                    <bind key="$logger"
                        type="service"
                        id="monolog.logger.doctrine"
                    />
                    <bind key="$projectDir">%kernel.project_dir%</bind>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Controller\LuckyController;
        use Symfony\Component\DependencyInjection\Reference;

        $container->register(LuckyController::class)
            ->setPublic(true)
            ->setBindings([
                '$logger' => new Reference('monolog.logger.doctrine'),
                '$projectDir' => '%kernel.project_dir%'
            ])
        ;

与所有服务一样，你也可以在控制器中使用常规的 :ref:`构造函数注入 <services-constructor-injection>`。

有关服务的更多信息，请参阅 :doc:`/service_container` 章节。

生成控制器
----------------------

为了节省时间，你可以安装 `Symfony Maker`_ 并让Symfony生成一个新的控制器类：

.. code-block:: terminal

    $ php bin/console make:controller BrandNewController

    created: src/Controller/BrandNewController.php

如果要从一个 Doctrine :doc:`实体 </doctrine>` 中生成整个CRUD，请使用：

.. code-block:: terminal

    $ php bin/console make:crud Product

.. versionadded:: 1.2

    ``make:crud`` 命令是在MakerBundle 1.2中引入的。

.. index::
   single: Controller; Managing errors
   single: Controller; 404 pages

管理错误和404页面
-----------------------------

如果找不到内容，你应该返回404响应。要达到目的，需要抛出一种特殊类型的异常::

    use Symfony\Component\HttpKernel\Exception\NotFoundHttpException;

    // ...
    public function index()
    {
        // 从数据库中检索对象
        $product = ...;
        if (!$product) {
            throw $this->createNotFoundException('该产品不存在');

            // 上面只是一个快捷方式:
            // throw new NotFoundHttpException('该产品不存在');
        }

        return $this->render(...);
    }

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\AbstractController::createNotFoundException` 方法只是创建一个特定的 :class:`Symfony\\Component\\HttpKernel\\Exception\\NotFoundHttpException` 对象的快捷方式，它最终会在Symfony中触发一个404 HTTP响应。

如果抛出的异常继承自 :class:`Symfony\\Component\\HttpKernel\\Exception\\HttpException` 或是其实例，Symfony将使用适当的HTTP状态代码。否则，响应将会使用500 HTTP状态代码::

    // 此异常最终会生成 500 错误
    throw new \Exception('Something went wrong!');

无论什么案例，都应该向最终用户显示错误页面，而向开发人员显示完整的调试错误页面（比如当你处于“调试”模式时 - 请参阅 :ref:`page-creation-environments`）。

要自定义向用户展示的错误页面，请参阅 :doc:`/controller/error_pages` 章节。

.. _controller-request-argument:

将Request对象作为控制器参数
-------------------------------------------

如果你需要读取查询参数、获取请求标头或访问上传的文件，该怎么办？
所有这些信息都存储在Symfony的 ``Request`` 对象中。
要在控制器中访问它，只需将Request添加为参数并对其该类进行类类型约束::

    use Symfony\Component\HttpFoundation\Request;

    public function index(Request $request, $firstName, $lastName)
    {
        $page = $request->query->get('page', 1);

        // ...
    }

:ref:`继续阅读 <request-object-info>` 有关使用 Request 对象的更多信息。

.. index::
   single: Controller; The session
   single: Session

.. _session-intro:

管理会话
--------------------

Symfony支持会话服务，你可以使用该服务在各请求之间存储有关用户的信息。会话默认启用，但只有在你读取或写入时才会启动。

会话存储和其他配置可以在 ``config/packages/framework.yaml`` 文件中的
:ref:`framework.session 配置 <config-framework-session>` 下进行控制。

要获取会话，请添加一个参数并使用 :class:`Symfony\\Component\\HttpFoundation\\Session\\SessionInterface`
对其进行类型约束::

    use Symfony\Component\HttpFoundation\Session\SessionInterface;

    public function index(SessionInterface $session)
    {
        // 存储一个属性，以便在以后的用户请求中重用
        $session->set('foo', 'bar');

        // 获取另一个请求中另一个控制器设置的属性
        $foobar = $session->get('foobar');

        // 如果该属性不存在，则使用默认值
        $filters = $session->get('filters', []);
    }

这些储存的属性将会在用户会话的有效期内保留。

有关的详细信息，请参阅 :doc:`/session`。

.. index::
   single: Session; Flash messages

.. _flash-messages:

Flash消息
~~~~~~~~~~~~~~

你还可以在用户的​​会话中存储称为“flash”消息的特殊消息。
根据设计，闪存消息只能使用一次：一旦你取出(retrieve)它们，它们就会自动从会话中消失。
此功能使“闪存”消息特别适合存储用户通知。

例如，假设你正在处理一个 :doc:`表单 </forms>` 提交::

    use Symfony\Component\HttpFoundation\Request;

    public function update(Request $request)
    {
        // ...

        if ($form->isSubmitted() && $form->isValid()) {
            // 做某种处理

            $this->addFlash(
                'notice',
                '你的更改已保存!'
            );
            // $this->addFlash() 等同于 $request->getSession()->getFlashBag()->add()

            return $this->redirectToRoute(...);
        }

        return $this->render(...);
    }

处理完请求后，控制器在会话中保存一个闪存消息，然后重定向。
消息的键（在此示例中为 ``notice``）可以是任何内容：你将使用此键来检索消息。

在下一个页面的的模板中（可以使用基本布局模板做到更好），使用 ``app.flashes()`` 从会话中读取任何闪存消息：

.. code-block:: html+twig

    {# templates/base.html.twig #}

    {# 你可以只读取并显示一种闪存消息类型... #}
    {% for message in app.flashes('notice') %}
        <div class="flash-notice">
            {{ message }}
        </div>
    {% endfor %}

    {# 读取并显示多种类型的闪存消息 #}
    {% for label, messages in app.flashes(['success', 'warning']) %}
        {% for message in messages %}
            <div class="flash-{{ label }}">
                {{ message }}
            </div>
        {% endfor %}
    {% endfor %}

    {# 读取并显示所有闪存消息 #}
    {% for label, messages in app.flashes %}
        {% for message in messages %}
            <div class="flash-{{ label }}">
                {{ message }}
            </div>
        {% endfor %}
    {% endfor %}

将 ``notice``，``warning`` 和 ``error`` 用作不同类型的闪存消息的键是很常见的，
但你可以使用任何符合你需求的键。

.. tip::

    你可以使用 :method:`Symfony\\Component\\HttpFoundation\\Session\\Flash\\FlashBagInterface::peek` 方法来取出消息，同时将其保留在包(bag)中。

.. index::
   single: Controller; Response object

.. _request-object-info:

请求和响应对象
-------------------------------

如 :ref:`前面 <controller-request-argument>` 所述，
Symfony会将 ``Request`` 对象传递给任何使用 ``Request`` 类进行类型约束的控制器参数::

    use Symfony\Component\HttpFoundation\Request;

    public function index(Request $request)
    {
        $request->isXmlHttpRequest(); // 这是一个Ajax请求吗？

        $request->getPreferredLanguage(['en', 'fr']);

        // 分别检索 GET 和 POST 变量
        $request->query->get('page');
        $request->request->get('page');

        // 检索 SERVER 变量
        $request->server->get('HTTP_HOST');

        // 检索一个有 foo 标识的 UploadedFile 实例
        $request->files->get('foo');

        // 检索一个 COOKIE 值
        $request->cookies->get('PHPSESSID');

        // 使用规范化的小写键来检索HTTP请求标头
        $request->headers->get('host');
        $request->headers->get('content-type');
    }

``Request`` 类有几个公共属性和方法，可返回有关请求的所有信息。

与 ``Request`` 类似，``Response`` 对象有一个公共 ``headers`` 属性。此对象属于
:class:`Symfony\\Component\\HttpFoundation\\ResponseHeaderBag`
类型，并提供获取和设置响应标头的方法。该标头的名称已经被规范化。因此，``Content-Type``
的名称等同于 ``content-type`` 或 ``content_type``的名称。

在Symfony中，控制器需要返回一个 ``Response`` 对象::

    use Symfony\Component\HttpFoundation\Response;

    // 使用200状态代码（默认值）创建一个简单的响应
    $response = new Response('Hello '.$name, Response::HTTP_OK);

    // 使用200状态代码创建一个CSS响应
    $response = new Response('<style> ... </style>');
    $response->headers->set('Content-Type', 'text/css');

为了方便这一点，内置了不同的响应对象来处理不同的响应类型。其中一些会在下面提到。
要了解有关 ``Request`` 和 ``Response``（以及不同的 ``Response`` 类）的更多信息 ，请参阅
:ref:`HttpFoundation组件文档 <component-http-foundation-request>`。

返回JSON响应
~~~~~~~~~~~~~~~~~~~~~~~

要从控制器返回JSON，请使用 ``json()`` 辅助方法。
这将返回一个 ``JsonResponse`` 对象，该对象自动对数据进行编码::

    // ...
    public function index()
    {
        // 返回 '{"username":"jane.doe"}' 并设置合适的 Content-Type 标头
        return $this->json(['username' => 'jane.doe']);

        // 该快捷方式同时定义了三个可选参数
        // return $this->json($data, $status = 200, $headers = [], $context = []);
    }

如果你的应用启用了 :doc:`serializer 服务 </serializer>`，它会被用于将该数据序列化为JSON。
否则将使用 :phpfunction:`json_encode` 函数。

流文件响应
~~~~~~~~~~~~~~~~~~~~~~~~

你可以使用 :method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\AbstractController::file` 辅助方法
来在控制器内部提供(serve)一个文件::

    public function download()
    {
        // 发送文件内容并强制浏览器下载
        return $this->file('/path/to/some_file.pdf');
    }

``file()`` 辅助方法提供了一些参数来配置其行为::

    use Symfony\Component\HttpFoundation\File\File;
    use Symfony\Component\HttpFoundation\ResponseHeaderBag;

    public function download()
    {
        // 从文件系统加载一个文件
        $file = new File('/path/to/some_file.pdf');

        return $this->file($file);

        // 对下载的文件进行重命名
        return $this->file($file, 'custom_name.pdf');

        // 在浏览器中显示文件内容而不是下载它
        return $this->file('invoice_3241.pdf', 'my_invoice.pdf', ResponseHeaderBag::DISPOSITION_INLINE);
    }

总结
--------------

在Symfony中，控制器通常是一个类方法，用于接受请求并返回一个 ``Response``
对象。使用一个URL进行映射时，控制器变为可访问状态，并且可以查看其响应。

为了方便控制器的开发，Symfony提供了一个
``AbstractController``。它可用于扩展控制器类，以允许访问一些常用的实用工具，如
``render()`` 和 ``redirectToRoute()``。``AbstractController`` 还提供了
``createNotFoundException()`` 工具，用于一个返回页面未找到的响应。

在其他文章中，你将学习如何使用控制器内部的特定服务，这些服务将帮助你持久化并从数据库中获取对象，处理表单提交，处理缓存等。

继续阅读
-----------

接下来，了解有关 :doc:`使用 Twig 渲染模板 </templating>` 的所有信息。

扩展阅读
----------------------------

.. toctree::
    :maxdepth: 1
    :glob:

    controller/*

.. _`Symfony Maker`: https://symfony.com/doc/current/bundles/SymfonyMakerBundle/index.html
.. _`未经验证的重定向安全漏洞`: https://www.owasp.org/index.php/Open_redirect
