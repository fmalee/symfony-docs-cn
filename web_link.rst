WebLink
===========================================================

  使用HTTP/2和WebLink进行资源(Asset)预加载和Resource Hint

Symfony提供了用于管理 ``Link`` HTTP标头的原生支持（通过 :doc:`WebLink组件 </components/web_link>`），
这是在使用现代Web浏览器的HTTP/2和预加载功能时提高应用性能的关键。

`HTTP/2 Server Push`_ 和W3C的 `Resource Hints`_ 中使用 ``Link`` 标头来将资源（例如CSS和JavaScript文件）
提前推送到客户端，直到他们知道他们需要它们。WebLink还支持其他适用于HTTP 1.x的优化:

* 要求浏览器在后台获取或渲染另一个网页;
* 进行早期DNS查询，TCP握手或TLS协商。

需要考虑的重要事项是，所有这些HTTP/2功能都需要安全的HTTPS连接，即使在本地计算机上工作也是如此。
主要的Web服务器（Apache，Nginx，Caddy等）支持这一点，
但你也可以使用来自Symfony社区的KévinDunglas创建的 `Docker installer and runtime for Symfony`_。

预加载资源
-----------------

想象一下，你的应用包含如下网页：

.. code-block:: twig

    {# templates/homepage.html.twig #}
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <title>My Application</title>
        <link rel="stylesheet" href="/app.css">
    </head>
    <body>
        <main role="main" class="container">
            {# ... some content here ... #}
        </main>
    </body>
    </html>

遵循传统的HTTP工作流程，当提供此页面时，浏览器将对HTML页面发出一个请求，并对链接的CSS文件发出另一个请求。
但是，得益于HTTP/2，你的应用甚至可以在浏览器请求之前开始发送CSS文件内容。

为此，首先安装WebLink组件：

.. code-block:: terminal

    $ composer req web-link

现在，更新模板以使用WebLink提供的 ``preload()`` Twig函数：

.. code:: twig

    <head>
       {# ... #}
        <link rel="stylesheet" href="{{ preload('/app.css') }}">
    </head>

如果重新加载页面，感知性能将得到改善，因为当浏览器仅请求HTML页面时，服务器同时响应HTML页面和CSS文件。

.. tip::

    Google Chrome提供了调试HTTP/2连接的界面。浏览 ``chrome://net-internals/#http2`` 以查看所有详细信息。

它是如何工作的？
~~~~~~~~~~~~~~~~~

WebLink组件管理添加到响应的 ``Link`` HTTP标头。
当使用上一示例中 ``preload()`` 函数时，响应中添加了以下标头：``Link </app.css>; rel="preload"``。

根据 `Preload规范`_，当HTTP/2服务器检测到原始（HTTP 1.x）响应包含此HTTP标头时，它将自动触发对同一HTTP/2连接中的相关文件的推送。

流行的代理服务和CDN（包括 `Cloudflare`_、`Fastly`_ 和 `Akamai`_） 也利用此功能。
这意味着你可以立即将资源推送到客户端并提高生产应用的性能。

如果要阻止推送但让浏览器通过发出早期单独的HTTP请求来预加载资源，请使用以下 ``nopush`` 选项：

.. code:: twig

    <head>
       {# ... #}
        <link rel="stylesheet" href="{{ preload('/app.css', { nopush: true }) }}">
    </head>

资源提示
--------------

应用使用 `Resource Hints`_ 来帮助浏览器决定应首先下载、预处理或连接哪些资源。

WebLink组件提供以下Twig函数来发送这些提示：

* ``dns_prefetch()``: “表示将用于获取所需资源的origin
  （例如 ``https://foo.cloudfront.net``），并且用户代理应尽快处理”。
* ``preconnect()``: “表示将用于获取所需资源的origin(例如 ``https://www.google-analytics.com``)。
  启动早期连接，包括DNS查找，TCP握手和可选的TLS协商，允许用户代理屏蔽建立连接的高延迟成本”。
* ``prefetch()``: “标识下一个导航可能需要的资源，以及用户代理 *应该* 获取的内容，
  以便用户代理可以在后续请求资源时提供更快的响应“。
* ``prerender()``: “标识下一个导航可能需要的资源，以及用户代理 *应该* 获取和执行的资源，
  以便用户代理可以在后续请求资源时提供更快的响应”。

该组件还支持发送与性能无关的HTTP Link以及实现 `PSR-13`_ 标准的任何Link。
例如，任何 `HTML规范中定义的Link`_：

.. code:: twig

    <head>
       {# ... #}
        <link rel="alternate" href="{{ link('/index.jsonld', 'alternate') }}">
        <link rel="stylesheet" href="{{ preload('/app.css', {nopush: true}) }}">
    </head>

上一个代码段将会产生此HTTP标头发送到客户端：``Link: </index.jsonld>; rel="alternate",</app.css>; rel="preload"; nopush``

你还可以直接从控制器和服务中添加指向HTTP响应的链接::

    // src/Controller/BlogController.php
    namespace App\Controller;

    use Fig\Link\GenericLinkProvider;
    use Fig\Link\Link;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class BlogController extends AbstractController
    {
        public function index(Request $request)
        {
            // 使用由 AbstractController 提供的 addLink() 快捷方式
            $this->addLink($request, new Link('preload', '/app.css'));

            // 如果你不想使用 addLink() 快捷方式的替代方法
            $linkProvider = $request->attributes->get('_links', new GenericLinkProvider());
            $request->attributes->set('_links', $linkProvider->withLink(new Link('preload', '/app.css')));

            return $this->render('...');
        }
    }

.. versionadded:: 4.2
    在Symfony 4.2中引入了 ``addLink()`` 快捷方式。

.. seealso::

    WebLink可以 :doc:`用作独立的PHP库 </components/web_link>`，而无需整个Symfony框架。

.. _`HTTP/2 Server Push`: https://tools.ietf.org/html/rfc7540#section-8.2
.. _`Resource Hints`: https://www.w3.org/TR/resource-hints/
.. _`Docker installer and runtime for Symfony`: https://github.com/dunglas/symfony-docker
.. _`preload`: https://developer.mozilla.org/en-US/docs/Web/HTML/Preloading_content
.. _`Preload规范`: https://www.w3.org/TR/preload/#server-push-(http/2)
.. _`Cloudflare`: https://blog.cloudflare.com/announcing-support-for-http-2-server-push-2/
.. _`Fastly`: https://docs.fastly.com/guides/performance-tuning/http2-server-push
.. _`Akamai`: https://blogs.akamai.com/2017/03/http2-server-push-the-what-how-and-why.html
.. _`this great article`: https://www.shimmercat.com/en/blog/articles/whats-push/
.. _`HTML规范中定义的Link`: https://html.spec.whatwg.org/dev/links.html#linkTypes
.. _`PSR-13`: http://www.php-fig.org/psr/psr-13/
