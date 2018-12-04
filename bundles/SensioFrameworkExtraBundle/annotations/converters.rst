@ParamConverter
===============

用法
-----

``@ParamConverter`` 注释调用 *转换器* 将请求参数转换为对象。
这些对象将被存储为请求属性，因此可以将它们注入为控制器方法参数::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\ParamConverter;

    /**
     * @Route("/blog/{id}")
     * @ParamConverter("post", class="SensioBlogBundle:Post")
     */
    public function showAction(Post $post)
    {
    }

这里发生了几件事：

* 转换器尝试从请求属性中获取一个 ``SensioBlogBundle:Post``
  对象，该请求属性来自路由占位符 - 此处为 ``id``;

* 如果没有找到 ``Post`` 对象，则生成一个 ``404`` 响应;

* 如果找到一个 ``Post`` 对象，则定义一个新的 ``post``
  请求属性（可通过 ``$request->attributes->get('post')`` 访问）;

* 对于其他请求属性，如果它存在于方法签名中时，则会被自动注入控制器。

如果你使用上面示例中的类型约束，你甚至可以完全省略 ``@ParamConverter`` 注释::

    // 使用方法签名来自动处理
    public function showAction(Post $post)
    {
    }

你可以通过将 ``auto_convert`` 选项设置为 ``false``，来禁用类型约束方法参数功能的自动转换：

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sensio_framework_extra:
            request:
                converters: true
                auto_convert: false

    .. code-block:: xml

        <sensio-framework-extra:config>
            <request converters="true" auto-convert="true" />
        </sensio-framework-extra:config>

你还可以按名称来明确禁用某些转换器：

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        sensio_framework_extra:
            request:
                converters: true
                disable: ['doctrine.orm', 'datetime']

    .. code-block:: xml

        <sensio-framework-extra:config>
            <request converters="true" disable="doctrine.orm,datetime" />
        </sensio-framework-extra:config>

为了检测需要在参数上运行哪个转换器，将执行以下过程：

* 如果是 ``@ParamConverter(converter="name")``
  这种显式标注了转换器的注释，则使用给定名称的转换器。

* 否则，所有已注册的参数转换器都按优先级进行迭代。
  一个参数转换器的 ``supports()`` 方法会被调用，以检查该转换器是否可以将该请求转换为所需参数。
  如果它返回 ``true``，则调用该转换器。

内置转换器
-------------------

该Bundle有两个内置转换器：Doctrine转换器和DateTime转换器。

Doctrine转换器
~~~~~~~~~~~~~~~~~~

转换器名称：``doctrine.orm``

Doctrine转换器尝试将请求属性转换为从数据库中抓取(fetch)的Doctrine实体。有几种不同的方法：

1) 自动抓取
......................

如果你的路由通配符与实体上的属性匹配，则转换器将自动抓取它们::

    /**
     * 通过主键抓取，因为路由中占位符的是{id}。
     *
     * @Route("/blog/{id}")
     */
    public function showByPkAction(Post $post)
    {
    }

    /**
     * 执行一个 findOneBy()，它的slug属性对应于 {slug} 占位符。
     *
     * @Route("/blog/{slug}")
     */
    public function showAction(Post $post)
    {
    }

在这些情况下会可以使用自动提取：

* 如果 ``{id}`` 存在于你的路由，那么将使用主键来调用 ``find()`` 方法来进行抓取。

* 转换器将尝试通过使用你的路由中的 *所有*
  通配符(对应你的实体中的实际属性，不是属性的将被忽略）来进行一个 ``findOneBy()`` 抓取。

你可以通过切实*增加* ``@ParamConverter`` 注释和使用 `@ParamConverter选项`_ 来控制这种行为。

2) 通过表达式获取
..........................

如果自动提取不起作用，另一个很好的选择是使用一个表达式::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Entity;

    /**
     * @Route("/blog/{post_id}")
     * @Entity("post", expr="repository.find(post_id)")
     */
    public function showAction(Post $post)
    {
    }

可以使用带有 ``expr`` 选项的 ``@Entity`` 特殊注释来通过调用仓库中的一个方法来获取对象。
``repository`` 方法将是你的实体的仓库类，并且任何路由通配符（例如 ``{post_id}``）都可用作变量。

.. tip::

    ``@Entity`` 注释是使用 ``expr`` 的一个快捷方式，并拥有所有和
    ``@ParamConverter`` 相同的选项。

这也可以用来解析多个参数::

    /**
     * @Route("/blog/{id}/comments/{comment_id}")
     * @Entity("comment", expr="repository.find(comment_id)")
     */
    public function showAction(Post $post, Comment $comment)
    {
    }

在上面的示例中，``$post`` 参数是自动处理的，但是 ``$comment``
需要使用注释进行配置，因为它们不能都遵循默认约定。

.. _`@ParamConverter选项`:

DoctrineConverter选项
.........................

``@ParamConverter`` (或 ``@Entity``) 注释上有许多用于控制行为的选项：

* ``id``: 如果配置了一个 ``id`` 选项并与一个路由参数匹配，那么转换器将通过主键来查找::

    /**
     * @Route("/blog/{post_id}")
     * @ParamConverter("post", options={"id" = "post_id"})
     */
    public function showPostAction(Post $post)
    {
    }

* ``mapping``: 配置要与 ``findOneBy()`` 方法一起使用的属性和值：
  键是路由占位符名称，值是Doctrine属性名称::

    /**
     * @Route("/blog/{date}/{slug}/comments/{comment_slug}")
     * @ParamConverter("post", options={"mapping": {"date": "date", "slug": "slug"}})
     * @ParamConverter("comment", options={"mapping": {"comment_slug": "slug"}})
     */
    public function showCommentAction(Post $post, Comment $comment)
    {
    }

* ``exclude`` 通过 *排除* 一个或多个属性来配置应在 ``findOneBy()``
  方法中使用的属性，以便不使用 *所有* 属性::

    /**
     * @Route("/blog/{date}/{slug}")
     * @ParamConverter("post", options={"exclude": {"date"}})
     */
    public function showAction(Post $post, \DateTime $date)
    {
    }

* ``strip_null`` 如果为 ``true``，则使用 ``findOneBy()``
  时，任何值为 ``null`` 的属性都不会被用于查询。

* ``entity_manager`` 默认情况下，Doctrine转换器使用 *默认* 实体管理器，但你可以指定配置::

    /**
     * @Route("/blog/{id}")
     * @ParamConverter("post", options={"entity_manager" = "foo"})
     */
    public function showAction(Post $post)
    {
    }

DateTime转换器
~~~~~~~~~~~~~~~~~~

转换器名称：``datetime``

datetime转换器将任何路由或请求的属性转换为一个 ``datetime`` 实例::

    /**
     * @Route("/blog/archive/{start}/{end}")
     */
    public function archiveAction(\DateTime $start, \DateTime $end)
    {
    }

默认情况下，该转换器接受任何 ``DateTime`` 构造函数可以解析的日期格式。
可以通过配置选项来执行更严格的输入::

    /**
     * @Route("/blog/archive/{start}/{end}")
     * @ParamConverter("start", options={"format": "Y-m-d"})
     * @ParamConverter("end", options={"format": "Y-m-d"})
     */
    public function archiveAction(\DateTime $start, \DateTime $end)
    {
    }

类似 ``2017-21-22`` 的错误格式的日期将返回一个 ``404``。

创建转换器
--------------------

所有转换器必须实现 ``ParamConverterInterface``::

    namespace Sensio\Bundle\FrameworkExtraBundle\Request\ParamConverter;

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\ParamConverter;
    use Symfony\Component\HttpFoundation\Request;

    interface ParamConverterInterface
    {
        function apply(Request $request, ParamConverter $configuration);

        function supports(ParamConverter $configuration);
    }

``supports()`` 方法必须在能够转换给定配置（一个 ``ParamConverter`` 实例）时返回 ``true``。

``ParamConverter`` 实例拥有三个关于注释的信息：

* ``name``: 属性名称;
* ``class``: 属性的类名称（可以是表示一个类名称的任何字符串）;
* ``options``: 一个选项数组。

只要一个配置能被支持，就会调用 ``apply()`` 方法。
根据请求属性，它应该设置一个名为 ``$configuration->getName()`` 的属性，该属性存储这一个
``$configuration->getClass()`` 类的对象。

如果你正在使用服务的 `自动注册和自动配置`_，那么你就已经完工了！你的转换器将自动调用。
如果没有，你必须给该服务添加一个标签：

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        services:
            my_converter:
                class:        MyBundle\Request\ParamConverter\MyConverter
                tags:
                    - { name: request.param_converter, priority: -2, converter: my_converter }

    .. code-block:: xml

        <service id="my_converter" class="MyBundle\Request\ParamConverter\MyConverter">
            <tag name="request.param_converter" priority="-2" converter="my_converter" />
        </service>

你可以按优先级、名称（属性“转换器”）或两者来注册一个转换器。
如果未指定一个优先级或一个名称，则该转换器将添加到优先级为 ``0`` 的转换器堆栈中。
要明确禁止优先级的注册，你必须在标签定义中设置 ``priority="false"``。

.. tip::

   如果要将服务或额外参数注入一个自定义参数转换器，则优先级不应高于 ``1``。
   否则，该服务将不会加载。

.. tip::

   使用 ``DoctrineParamConverter`` 类作为你自己的转换器的模板。

.. _自动注册和自动配置: http://symfony.com/doc/current/service_container/3.3-di-changes.html
