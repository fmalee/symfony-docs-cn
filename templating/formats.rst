.. index::
    single: Templating; Formats

如何在模板中使用不同的输出格式
======================================================

模板是以 *任何* 格式渲染内容的通用方式。
虽然在大多数情况下你将使用模板来渲染HTML内容，但一个模板可以生成JavaScript、CSS、XML或任何其他格式。

例如，相同的“资源”通常以多种格式渲染。
要以XML格式渲染文章索引页面，请在模板名称中包含以下格式：

* *XML模板名称*: ``article/show.xml.twig``
* *XML模板文件名*: ``show.xml.twig``

实际上，这只不过是命名约定，并且模板实际上不会根据其格式进行不同的渲染。

在许多情况下，你可能希望允许单个控制器根据“请求格式”渲染多种不同的格式。
因此，常见的模式是执行以下操作::

    // ...
    use Symfony\Component\Routing\Annotation\Route;

    class ArticleController extends AbstractController
    {
        /**
         * @Route("/{slug}")
         */
        public function show(Request $request, $slug)
        {
            // 基于 $slug 检索文章
            $article = ...;

            $format = $request->getRequestFormat();

            return $this->render('article/show.'.$format.'.twig', [
                'article' => $article,
            ]);
        }
    }

在 ``Request`` 对象中的 ``getRequestFormat()`` 默认为 ``html``，但也可以返回基于用户请求的任何其他格式。
请求格式通常通过路由来管理，一个路由被配置后，其中 ``/about-us`` 的请求格式为 ``html``，
而 ``/about-us.xml`` 的请求格式则是 ``xml``。
这可以通过在路由定义中使用特殊的 ``_format`` 占位符来实现::

    /**
     * @Route("/{slug}.{_format}", defaults={"_format"="html"}, requirements={"_format"="html|xml"}))
     */
    public function show(Request $request, $slug)
    {
        // ...
    }

现在，在为另一种格式生成路由时包含 ``_format`` 占位符：

.. code-block:: html+twig

    <a href="{{ path('article_show', {'slug': 'about-us', '_format': 'xml'}) }}">
        View as XML
    </a>

.. seealso::

    有关更多信息，请参阅 :ref:`高级路由示例 <advanced-routing-example>`。

.. tip::

    构建API时，使用文件扩展名通常不是最佳解决方案。FOSRestBundle提供使用内容协商的请求监听器。
    有关更多信息，请查看该bundle的 `Request Format Listener`_ 文档。

.. _Request Format Listener: http://symfony.com/doc/current/bundles/FOSRestBundle/3-listener-support.html#format-listener
