控制器
===========

Symfony遵循\ *“瘦控制器和胖模型”*\的理念。
这意味着控制器应该只包含协调应用的不同部分所需的\ *胶水代码*\(glue-code)薄层。

你的控制器方法应该只调用其他服务，在需要时触发某些事件然后返回响应，
但它们不应包含任何实际的业务逻辑。如果这样做了，就将其重构出控制器并放置于一个服务中。

.. best-practice::

    确保你的控制器继承自Symfony提供的 ``AbstractController`` 基本控制器，
    并尽可能使用注释来配置路由、缓存和安全等内容。

将控制器藕合到框架内核，有助于你利用其功能并提高生产力。

由于你的控制器宜精简，除了\ *胶水代码*\之外再无其他，
若花费数个小时来尝试将其与框架核心解藕，并无益于持续开发。时间上的开销得不尝失。

除此之外，使用注释方式来处理路由、缓存、安全等层面可以简化配置。
你毋需浏览大量不同格式的文件（YAML、XML、PHP）：所有配置都在你需要的地方显示，并且只有一种格式。

总的来说，这意味着你应该积极地将业务逻辑与框架解藕，同时主动地将控制器和路由耦合 *到* 框架，以便充分利用它的特性。

控制器动作命名
------------------------

.. best-practice::

    不要将 ``Action`` 后缀添加到控制器操作的方法中。

第一个Symfony版本要求控制器方法名称以 ``Action`` 结尾（例如 ``newAction()``, ``showAction()``）。
但当为控制器引入注释时，此后缀变为可选。
在现代的Symfony应用中，既不要求也不建议使用此后缀，因此你可以安全地删除它。

路由配置
---------------------

为了在控制器中加载以注释方式定义的路由，添加下列配置到路由的主配置文件中：

.. code-block:: yaml

    # config/routes.yaml
    controllers:
        resource: '../src/Controller/'
        type:     annotation

此配置将从存储在 ``src/Controller/`` 目录甚至其子目录下的任何控制器中加载注释。
因此，如果你的应用定义了许多控制器，那么将它们重新组织到子目录中是完全可行的：

.. code-block:: text

    <your-project>/
    ├─ ...
    └─ src/
       ├─ ...
       └─ Controller/
          ├─ DefaultController.php
          ├─ ...
          ├─ Api/
          │  ├─ ...
          │  └─ ...
          └─ Backend/
             ├─ ...
             └─ ...

模板配置
----------------------

.. best-practice::

    不要使用 ``@Template`` 注释来配置控制器中的模板。

``@Template`` 注释十分有用，但它也带来了迷惑性。
我们不认为它的好处能够高过迷惑性，因此推荐不使用这种方式。

大多数情况下，使用 ``@Template`` 时没有任何参数，这使得更难以知道正在渲染哪个模板。
对于初学者来说，控制器应该总是返回一个Response对象（除非你使用的是视图层），这也是不太明显的。

控制器应该是什么样子
----------------------------------

综合考虑以上所有，下例呈现出我们程序首页的控制器应有的样子::

    namespace App\Controller;

    use App\Entity\Post;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Routing\Annotation\Route;

    class DefaultController extends AbstractController
    {
        /**
         * @Route("/", name="homepage")
         */
        public function index()
        {
            $posts = $this->getDoctrine()
                ->getRepository(Post::class)
                ->findLatest();

            return $this->render('default/index.html.twig', [
                'posts' => $posts,
            ]);
        }
    }

获取服务
-----------------

如果继承了  ``AbstractController`` 基类，则无法直接通过 ``$this->container->get()`` 或 ``$this->get()`` 从容器访问服务。
相反，你必须使用依赖注入来获取服务：
最容易通过 :ref:`类型约束动作方法的参数 <controller-accessing-services>`: 来完成。


.. best-practice::

    不要使用 ``$this->get()`` 或 ``$this->container->get()`` 来从容器中获取服务。相反，使用依赖注入。

通过不直接从容器中获取服务，你可以将你的服务设为 *私有* ，这有 :ref:`一些有利因素 <services-why-private>`。

.. _best-practices-paramconverter:

使用参数转换
------------------------

如果你正在使用Doctrine，那么你可以选择使用 `ParamConverter`_ 来自动查询实体并将其作为参数传递给控制器​​。

.. best-practice::

    使用 ParamConverter 来简单实用的自动查询 Doctrine 实体。

例如::

    use App\Entity\Post;
    use Symfony\Component\Routing\Annotation\Route;

    /**
     * @Route("/{id}", name="admin_post_show")
     */
    public function show(Post $post)
    {
        $deleteForm = $this->createDeleteForm($post);

        return $this->render('admin/post/show.html.twig', [
            'post' => $post,
            'delete_form' => $deleteForm->createView(),
        ]);
    }

通常你预期会有一个 ``$id`` 参数传入 ``show()``。
取而代之的是，创建一个新的参数（``$post``）并应用类型提示为 ``Post`` 类（这是个Doctrine实体），
此时 ParamConverter 将自动查询出一个 ``$id`` 属性与路由 ``{id}`` 值相匹配的对象。
如果查不到 ``Post`` 它也会显示404页面。

复杂情况
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

上面示例毋需额外的配置，因为通配符的名字 ``{id}`` 正好匹配实体的属性名。
如果不是这种情况，或者你有更复杂的逻辑，最好的选择就是手动查询实体。
在我们的程序中，我们在 ``CommentController`` 有这种情况::

    /**
     * @Route("/comment/{postSlug}/new", name="comment_new")
     */
    public function new(Request $request, $postSlug)
    {
        $post = $this->getDoctrine()
            ->getRepository(Post::class)
            ->findOneBy(['slug' => $postSlug]);

        if (!$post) {
            throw $this->createNotFoundException();
        }

        // ...
    }

你还是可以使用 ``@ParamConverter`` 配置，它具有无限灵活性::

    use App\Entity\Post;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\ParamConverter;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Routing\Annotation\Route;

    /**
     * @Route("/comment/{postSlug}/new", name="comment_new")
     * @ParamConverter("post", options={"mapping"={"postSlug"="slug"}})
     */
    public function new(Request $request, Post $post)
    {
        // ...
    }

重点在于：ParamConverter 更加适合于简单状况。然而你要记得，手动直接查询实体同样很简单。

前置/后置 钩子
------------------

如果需要在执行控制器之前或之后执行某些代码，
可以使用 EventDispatcher 组件 :doc:`设置前置/后置过滤器 </event_dispatcher/before_after_filters>`。

----

下一章: :doc:`/best_practices/templates`

.. _`ParamConverter`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
