.. index::
   single: Cache

HTTP缓存
==========

富Web应用的本质意味着它们是动态的。无论你的应用效率如何，每个请求总是会比提供静态文件包含更多的开销。
通常，这没关系。但是当你需要快速请求时，你需要HTTP缓存。

缓存在巨人的肩膀上
----------------------------------

使用HTTP缓存，你可以缓存页面的完整输出（即响应），并 *完全* 绕过后续请求。
对于高度动态的网站来说，缓存整个响应并不总是可行的，或者是这样吗？
使用 :doc:`Edge Side Includes (ESI) </http_cache/esi>`，你可以仅在站点的 *片段* 上使用HTTP缓存的强大功能。

Symfony缓存系统与众不同，因为它依靠的是 `RFC 7234 - Caching`_ 所定义的简单又强大的HTTP缓冲。
不同于重新发明一套缓存方法，Symfony强调的是定义了web基本通信的标准。
一旦理解了基本的HTTP验证和到期缓存模型，你就可以掌握Symfony缓存系统了。

由于使用HTTP进行缓存并不是Symfony独有的，因此该主题已经存在许多文章。
如果你刚接触HTTP缓存，*强烈推荐* Ryan Tomayko的文章 `Things Caches Do`_。
另一个深入的资源是 Mark Nottingham的 `缓存指南`_。

.. index::
   single: Cache; Proxy
   single: Cache; Reverse proxy

.. _gateway-caches:

使用网关缓存进行缓存
----------------------------

使用HTTP进行缓存时，*缓存* 将完全与应用分离，并位于应用和发出请求的客户端之间。

缓存的工作是接受来自客户端的请求并将它们传递回你的应用。
缓存还将从你的应用接收回复并将其转发给客户端。
缓存是客户端和应用之间的请求-响应通信的“中间人”。

在此过程中，缓存将存储被视为“可缓存”的每个响应（请参阅 :ref:`http-cache-introduction`）。
如果再次请求相同的资源，则缓存会将缓存的响应发送到客户端，完全忽略你的应用。

这种类型的缓存称为HTTP网关缓存，许多存在，如 `Varnish`_、`反向代理模式下的Squid`_，以及Symfony反向代理。

.. tip::

    网关缓存有时被称为反向代理缓存，代理(surrogate)缓存，甚至HTTP加速器。

.. index::
   single: Cache; Symfony reverse proxy

.. _`symfony-gateway-cache`:
.. _symfony2-reverse-proxy:

Symfony反向代理
~~~~~~~~~~~~~~~~~~~~~

Symfony带有一个用PHP编写的反向代理（即网关缓存）。
:ref:`它不像Varnish那样功能齐全的反向代理缓存 <http-cache-symfony-versus-varnish>`，但却是一个很好的开始。

.. tip::

    有关设置Varnish的详细信息，请参阅 :doc:`/http_cache/varnish`。

要启用代理，首先要创建一个缓存内核::

    // src/CacheKernel.php
    namespace App;

    use Symfony\Bundle\FrameworkBundle\HttpCache\HttpCache;

    class CacheKernel extends HttpCache
    {
    }

修改前端控制器的代码以将默认内核包装到缓存内核中：

.. code-block:: diff

    // public/index.php

    + use App\CacheKernel;
    use App\Kernel;

    // ...
    $kernel = new Kernel($_SERVER['APP_ENV'], (bool) $_SERVER['APP_DEBUG']);
    + // 在 'prod' 环境中使用 CacheKernel 封装默认内核
    + if ('prod' === $kernel->getEnvironment()) {
    +     $kernel = new CacheKernel($kernel);
    + }

    $request = Request::createFromGlobals();
    // ...

缓存内核将立即充当反向代理：缓存来自应用的响应并将其返回给客户端。

.. caution::

    如果你正在使用 :ref:`framework.http_method_override <configuration-framework-http_method_override>`
    选项从一个 ``_method`` 参数中读取HTTP方法，请参阅上面的链接以了解你需要进行的调整。

.. tip::

    缓存内核有一个特殊的 ``getLog()`` 方法，它返回字符串代表缓存层中发生的事件。
    在开发环境中，使用它来调试和验证你的缓存策略::

        error_log($kernel->getLog());

``CacheKernel`` 对象具有合理的默认配置，但可以通过重写
:method:`Symfony\\Bundle\\FrameworkBundle\\HttpCache\\HttpCache::getOptions` 方法设置的一组选项进行微调::

    // src/CacheKernel.php
    namespace App;

    use Symfony\Bundle\FrameworkBundle\HttpCache\HttpCache;

    class CacheKernel extends HttpCache
    {
        protected function getOptions()
        {
            return [
                'default_ttl' => 0,
                // ...
            ];
        }
    }

有关选项及其含义的完整列表，请参阅 :method:`HttpCache::__construct() 文档 <Symfony\\Component\\HttpKernel\\HttpCache\\HttpCache::__construct>`。

当你处于调试模式（``Kernel`` 前端控制器中构造函数的第二个参数是 ``true``）时，
Symfony会自动为响应添加 ``X-Symfony-Cache`` 标头。你还可以使用 ``trace_level``
配置选项，并将其设置为 ``none``、``short`` 或 ``full`` 来添加此信息。

``short`` 将只为主请求添加信息。它是以一种简洁的方式编写的，这样可以很容易地将信息记录到服务器日志文件中。
例如，可以在Apache的 ``LogFormat`` 格式语句中使用
``%{X-Symfony-Cache}o``。此信息可用于提取有关路由缓存效率的常规信息。

.. tip::

    你可以使用 ``trace_header`` 配置选项来更改用于跟踪信息的标头的名称。

.. versionadded:: 4.3

    Symfony 4.3中引入了 ``trace_level`` 和 ``trace_header`` 配置选项。

.. _http-cache-symfony-versus-varnish:

.. sidebar:: 从一个反向代理转换到另一个反向代理

    Symfony反向代理是在开发网站或将网站部署到共享主机时使用的一个很好的工具，在共享主机上你无法安装除PHP代码之外的任何内容。
    但是因为用PHP编写，它不能像用C语言编写的代理那样快速。

    幸运的是，由于所有反向代理实际上都是相同的，所以你应该能够无缝切换到更健壮的代理 - 比如Varnish。
    请参见 :doc:`如何使用 Varnish </http_cache/varnish>`

.. index::
   single: Cache; HTTP

.. _http-cache-introduction:

使你的响应HTTP可缓存
------------------------------------

一旦添加了反向代理缓存（例如Symfony反向代理或Varnish），你就可以缓存响应了。
为此，你需要与缓存 *沟通* *哪些* 响应可缓存以及缓存时间。
这是通过在响应上设置HTTP缓存头来完成的。

HTTP指定了可以设置为启用缓存的四个响应缓存标头：

* ``Cache-Control``
* ``Expires``
* ``ETag``
* ``Last-Modified``

这四个标头用于通过 *两种* 不同的模型来缓存你的响应：

.. _http-expiration-validation:
.. _http-expiration-and-validation:

#. :ref:`到期缓存 <http-cache-expiration-intro>`
   用于在特定时间内（例如24小时）缓存整个响应。很简单，但使缓存失效更加困难。

#. :ref:`验证缓存 <http-cache-validation-intro>`
   相对复杂一些：用于缓存你的响应，但允许你在内容更改后动态地使缓存失效。

.. sidebar:: 阅读HTTP规范

    你将阅读的所有HTTP标头都 *不是* Symfony发明的！它们是整个网络上使用的HTTP规范的一部分。
    要深入了解HTTP缓存，请查看文档 `RFC 7234 - Caching`_ 和 `RFC 7232 - Conditional Requests`_。

    作为Web开发人员，强烈建议你阅读该规范。它的清晰度和力量 - 甚至延续至创建后的十五年 - 是非常宝贵的。
    不要因为封面的外观而被推迟 - 它的内容比它的封面更漂亮！

.. index::
   single: Cache; Expiration

.. _http-cache-expiration-intro:

到期缓存
~~~~~~~~~~~~~~~~~~

缓存响应的 *最简单* 方法是将缓存缓存一段特定的时间::

    // src/Controller/BlogController.php
    use Symfony\Component\HttpFoundation\Response;
    // ...

    public function index()
    {
        // 以某种方式创建一个Response对象，就像渲染模板一样
        $response = $this->render('blog/index.html.twig', []);

        // 缓存3600秒
        $response->setSharedMaxAge(3600);

        // （可选）设置一个自定义Cache-Control指令
        $response->headers->addCacheControlDirective('must-revalidate', true);

        return $response;
    }

感谢这个新代码，你的HTTP响应将具有以下标头：

.. code-block:: text

    Cache-Control: public, s-maxage=3600, must-revalidate

这告诉你的HTTP反向代理将此响应缓存3600秒。如果 *有人* 在3600秒之前再次请求此URL，则你的应用将根本不会被命中。
如果你正在使用Symfony反向代理，请查看 ``X-Symfony-Cache`` 标头以获取有关缓存命中和未命中的调试信息。

.. tip::

    请求的URI被用作为缓存键（除非你做了 :doc:`改变 </http_cache/cache_vary>`）。

这提供了很好的性能并且易于使用。但是不支持缓存 *失效*。
如果你的内容发生了变化，你需要等到缓存过期才能更新页面。

.. tip::

    实际上，你 *可以* 手动使缓存无效，但它不是HTTP缓存规范的一部分。请参阅 :ref:`http-cache-invalidation`。


如果需要为许多不同的控制器动作设置缓存头，请查看 `FOSHttpCacheBundle`_。
它提供了一种基于URL模式和其他请求属性定义缓存头的方法。

最后，有关到期缓存的更多信息，请参阅 :doc:`/http_cache/expiration`。

.. _http-cache-validation-intro:

验证缓存
~~~~~~~~~~~~~~~~~~

.. index::
   single: Cache; Cache-Control header
   single: HTTP headers; Cache-Control

使用到期缓存，你只需说“缓存3600秒！”。但是，当有人更新已缓存的内容时，在缓存过期之前，你将无法在网站上看到该内容。

如果你需要立即查看更新的内容，则需要使缓存 *无效* *或* 使用验证缓存模型。

有关详细信息，请参阅 :doc:`/http_cache/validation`。

.. index::
   single: Cache; Safe methods

安全方法：仅缓存GET或HEAD请求
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

HTTP缓存仅适用于“安全”HTTP方法（如GET和HEAD）。这意味着两件事：

* 不要尝试缓存PUT或DELETE请求。它有充分的理由不起作用。
  这些方法旨在用于改变应用的状态（例如删除博客文章）。缓存它们会阻止某些请求命中并改变你的应用。

* POST请求通常被认为是不可缓存的，但是当它们包含显式新鲜度信息时 `可以缓存它们`_。
  但是，POST缓存并未广泛实现，因此你应该尽可能避免使用它。

* 在响应一个GET或HEAD请求时，你 *不应该* 在此时更改应用的状态（例如，更新博客文章）。
  如果这些请求被缓存，未来的请求可能实际上不会到达你的服务器。

.. index::
   pair: Cache; Configuration

更多响应方法
~~~~~~~~~~~~~~~~~~~~~

响应类提供了许多与缓存相关的方法。这是最有用的::

    // 将响应标记为过期
    $response->expire();

    // 强制该响应返回正确的304响应而没有内容
    $response->setNotModified();

此外，大多数与缓存相关的HTTP标头可以通过单一
:method:`Symfony\\Component\\HttpFoundation\\Response::setCache` 方法来设置::

    // sets cache settings in one call
    $response->setCache([
        'etag'          => $etag,
        'last_modified' => $date,
        'max_age'       => 10,
        's_maxage'      => 10,
        'public'        => true,
        // 'private'    => true,
    ]);

缓存失效
------------------

缓存失效 *不是* HTTP规范的一部分。但是，只要站点上的某些内容更新，删除各种HTTP缓存条目就非常迫切。

有关详细信息，请参阅 :doc:`/http_cache/cache_invalidation`。

使用Edge Side Includes
------------------------

当页面包含动态部分时，你可能无法缓存整个页面，而只能缓存部分页面。
阅读 :doc:`/http_cache/esi` 了解如何为页面的特定部分配置不同的缓存策略。

HTTP缓存和用户会话
------------------------------

每当在请求期间启动会话时，Symfony都会将响应转换为私有的非可缓存响应。
这是不缓存私人用户信息（例如购物车，个人资料等）并将其公开给其他访问者的最佳默认行为。

但是，即使使用会话的请求在某些情况下也可以被缓存。例如，可以为属于某个用户组的所有用户缓存与该组相关的信息。
处理这些高级缓存方案超出了Symfony的范围，但可以使用 `FOSHttpCacheBundle`_ 解决它们。

为了禁用“使用会话无法缓存请求”的默认Symfony行为，请将以下内部标头添加到你的响应中，Symfony将不会修改它::

    use Symfony\Component\HttpKernel\EventListener\AbstractSessionListener;

    $response->headers->set(AbstractSessionListener::NO_AUTO_CACHE_CONTROL_HEADER, 'true');

摘要
-------

Symfony旨在遵循经过验证的道路规则：HTTP。缓存也不例外。
掌握Symfony缓存系统意味着熟悉HTTP缓存模型并有效地使用它们。
这意味着，你可以跨入与HTTP缓存和网关缓存（如Varnish）相关的知识世界，而不仅仅依赖于Symfony文档和代码示例。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    http_cache/*

.. _`Things Caches Do`: http://2ndscale.com/writings/things-caches-do
.. _`缓存指南`: http://www.mnot.net/cache_docs/
.. _`Varnish`: https://www.varnish-cache.org/
.. _`反向代理模式下的Squid`: http://wiki.squid-cache.org/SquidFaq/ReverseProxy
.. _`HTTP Bis`: http://tools.ietf.org/wg/httpbis/
.. _`RFC 7234 - Caching`: https://tools.ietf.org/html/rfc7234
.. _`RFC 7232 - Conditional Requests`: https://tools.ietf.org/html/rfc7232
.. _`FOSHttpCacheBundle`: http://foshttpcachebundle.readthedocs.org/
.. _`可以缓存它们`: https://tools.ietf.org/html/draft-ietf-httpbis-p2-semantics-20#section-2.3.4
