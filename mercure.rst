.. index::
   single: Mercure

Mercure
==================================================

能够从服务器到客户端实时广播数据是许多现代Web和移动应用的要求。

创建实时响应其他用户所做更改的UI（例如，用户更改当前被其他几个用户浏览的数据，所有UI立即更新），在
:doc:`异步作业 </messenger>` 完成时通知用户，或者创建聊天应用等都是需要“推送”功能的典型用例之一。

Symfony提供了一个构建在 `Mercure协议`_ 之上的简单组件， 专为此类用例而设计。

Mercure是一个开放式协议，旨在从服务器发布更新到客户端。
它是基于计时器的轮询和WebSocket的现代高效替代方案。

由于它是基于 `服务器推送事件（SSE）`_ 构建的，因此大多数现代浏览器都支持Mercure（Edge和IE需要
`polyfill`_），并且在许多编程语言中都有 `高级实现`_。

Mercure提供授权机制，在网络出错的情况下自动重新连接并检索丢失的更新，智能手机的“无连接”推送和自动发现（受支持的客户端可以通过特定的HTTP标头自动发现和订阅给定资源的更新）。

在Symfony的集成中支持所有这些功能。

与仅与HTTP 1.x兼容的WebSocket不同，Mercure利用 HTTP/2 和
HTTP/3 提供的多路复用功能（但也支持旧版本的HTTP）。

`在此视频中`_，你可以看到Symfony Web API如何利用Mercure和 `API平台`_
来实时更新一个React应用，以及使用API​​平台客户端生成器来生成的一个移动应用（React Native）。

安装
------------

安装Symfony组件
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在使用 :doc:`Symfony Flex </setup/flex>`
的应用中，运行此命令以在使用之前安装Mercure支持：

.. code-block:: terminal

    $ composer require mercure

运行MercureHub
~~~~~~~~~~~~~~~~~~~~~

为了管理持久连接，Mercure依赖于一个Hub：一个专用服务器，用于处理与客户端的持久SSE连接。
Symfony应用将更新发布到该Hub，并将其广播给客户端。

.. image:: /_images/mercure/schema.png

可以从 `Mercure.rocks`_ 下载作为Hub的官方和开源（AGPL）实现的静态二进制文件。

运行以下命令来启动它：

.. code-block:: terminal

    $ JWT_KEY='aVerySecretKey' ADDR='localhost:3000' ALLOW_ANONYMOUS=1 CORS_ALLOWED_ORIGINS=* ./mercure

.. note::

    除了二进制文件之外，Mercure.rocks还提供了Docker映像、Kubernetes的Helm图和托管的高可用性Hub。

.. tip::

    该 `API平台发行版`_ 带有Docker Compose配置以及Kubernetes的Helm图表，这些图表与Symfony
    100%兼容，并包含一个Mercure集线器。即使你不使用API平台，也可以在项目中复制它们。

配置
-------------

配置MercureBundle的首选方法是使用 :doc:`环境变量 </configuration>`。

将Hub的URL设置为 ``MERCURE_PUBLISH_URL`` 环境变量的值。你的项目的 ``.env``
文件已经由Flex指令更新，以提供示例值。将其设置为Mercure Hub的URL（默认为
``http://localhost:3000/hub``）。

此外，Symfony应用必须携带一个
`JSON Web Token`_ （JWT）到Mercure Hub才有权发布更新。

此JWT应存储在 ``MERCURE_JWT_SECRET`` 环境变量中。

JWT必须使用与Hub用于验证JWT的密钥相同的密钥来进行签名（在我们的示例中为
``aVerySecretKey``）。其有效负载必须至少包含以下允许发布的结构：

.. code-block:: json

    {
        "mercure": {
            "publish": []
        }
    }

由于数组为空，因此Symfony应用仅被授权发布公共更新（有关详细信息，请参阅 授权_ 部分）。

.. tip::

    jwt.io网站是一个创建和签名JWT的便捷方式。查看此 `JWT示例`_，它为所有 *目标*
    授予发布权限（注意数组中的星号）。不要忘记在表单右侧面板的底部正确设置密钥！

.. caution::

    不要把密钥放入 ``MERCURE_JWT_SECRET``，它将无法正常工作！
    此环境变量必须包含一个使用密钥签名的JWT。

    Also, be sure to keep both the secret key and the JWTs... secrets!
    另外，一定要保密密钥和JWT ......秘密！

基本用法
-----------

发布
~~~~~~~~~~

Mercure组件提供一个表示要发布的更新的 ``Update``
值对象。它还提供了一个向Hub分发更新的 ``Publisher`` 服务。

``Publisher`` 服务可以使用 :doc:`自动装配 </service_container/autowiring>`
在任何其它服务中注入，包括控制器::

    // src/Controller/PublishController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Mercure\Publisher;
    use Symfony\Component\Mercure\Update;

    class PublishController
    {
        public function __invoke(Publisher $publisher): Response
        {
            $update = new Update(
                'http://example.com/books/1',
                json_encode(['status' => 'OutOfStock'])
            );

            // Publisher服务是一个可调用的对象
            $publisher($update);

            return new Response('published!');
        }
    }

传递给 ``Update`` 构造函数的第一个参数是要更新的 *主题*。该主题应该是
IRI_（国际化资源标识符，RFC 3987）：被调度的资源的唯一标识符。

通常，此参数包含传输到客户端的资源的原始URL，但它可以是任何有效的
IRI_，而不必是存在的URL（类似于XML命名空间）。

构造函数的第二个参数是更新的内容。它可以是任何东西，以任何格式存储。
但是，建议以超媒体格式（如JSON-LD、Atom、HTML或XML）序列化资源。

订阅
~~~~~~~~~~~

订阅JavaScript中的更新非常简单：

.. code-block:: javascript

    const es = new EventSource('http://localhost:3000/hub?topic=' + encodeURIComponent('http://example.com/books/1'));
    es.onmessage = e => {
        // 将在每次服务器发布更新时调用
        console.log(JSON.parse(e.data));
    }

Mercure还允许订阅多个主题，并使用URI模板作为模式：

.. code-block:: javascript

    // URL是一个内置的JavaScript类，用于操作URL
    const u = new URL('http://localhost:3000/hub');
    u.searchParams.append('topic', 'http://example.com/books/1');
    // 订阅多个图书资源的更新
    u.searchParams.append('topic', 'http://example.com/books/2');
    // 所有 Review 资源都将与此模式匹配
    u.searchParams.append('topic', 'http://example.com/reviews/{id}');

    const es = new EventSource(u);
    es.onmessage = e => {
        console.log(JSON.parse(e.data));
    }

.. tip::

    谷歌Chrome的开发者工具本地集成了显示收到的事件的 `实用界面`_：

    .. image:: /_images/mercure/chrome.png

    要使用它：

    * 打开开发者工具
    * 选择“网络”选项卡
    * 点击Mercure hub的请求
    * 单击“EventStream”子选项卡。

.. tip::

    可以使用 `在线调试器`_ 来测试URI模板是否与URL匹配

异步调度
-----------------

除了直接调用 ``Publisher`` 服务，你还可以让Symfony通过Messenger组件提供的集成来异步调度更新。

首先，确保 :doc:`安装了Messenger组件 </messenger>`
并正确配置传输（如果不这样，将同步调用该处理器）。

然后，将Mercure的 ``Update`` 调度到Messenger的消息总线，它将自动处理::

    // src/Controller/PublishController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Mercure\Update;
    use Symfony\Component\Messenger\MessageBusInterface;

    class PublishController
    {
        public function __invoke(MessageBusInterface $bus): Response
        {
            $update = new Update(
                'http://example.com/books/1',
                json_encode(['status' => 'OutOfStock'])
            );

            // 同步或异步 (RabbitMQ, Kafka...)
            $bus->dispatch($update);

            return new Response('published!');
        }
    }

发现
---------

Mercure协议附带一个发现机制。要利用它，Symfony应用必须在 ``Link``
HTTP标头中暴露Mercure Hub的URL。

.. image:: /_images/mercure/discovery.png

你可以使用 ``AbstractController::addLink`` 辅助方法来通过
:doc:`WebLink组件 </web_link>` 创建 ``Link`` 标头::

    // src/Controller/DiscoverController.php
    namespace App\Controller;

    use Fig\Link\Link;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\JsonResponse;
    use Symfony\Component\HttpFoundation\Request;

    class DiscoverController extends AbstractController
    {
        public function __invoke(Request $request): JsonResponse
        {
            // 此参数由MercureBundle自动创建
            $hubUrl = $this->getParameter('mercure.default_hub');

            // Link: <http://localhost:3000/hub>; rel="mercure"
            $this->addLink($request, new Link('mercure', $hubUrl));

            return $this->json([
                '@id' => '/books/1',
                'availability' => 'https://schema.org/InStock',
            ]);
        }
    }

然后，可以在客户端解析此标头以查找Hub的URL，并订阅它：

.. code-block:: javascript

    // 获取Symfony web API提供的原始资源
    fetch('/books/1') // Has Link: <http://localhost:3000/hub>; rel="mercure"
        .then(response => {
            // 从Link标头提取Hub的URL
            const hubUrl = response.headers.get('Link').match(/<([^>]+)>;\s+rel=(?:mercure|"[^"]*mercure[^"]*")/)[1];

            // 将要订阅的主题附加为查询参数
            const h = new URL(hubUrl);
            h.searchParams.append('topic', 'http://example.com/books/{id}');

            // 订阅更新
            const es = new EventSource(h);
            es.onmessage = e => console.log(e.data);
        });

授权
-------------

Mercure还允许仅向授权客户发送更新。为此，请将允许接收更新的 **目标** 列表设置为
``Update`` 构造函数的第三个参数::

    // src/Controller/Publish.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Mercure\Publisher;
    use Symfony\Component\Mercure\Update;

    class PublishController
    {
        public function __invoke(Publisher $publisher): Response
        {
            $update = new Update(
                'http://example.com/books/1',
                json_encode(['status' => 'OutOfStock']),
                ['http://example.com/user/kevin', 'http://example.com/groups/admin'] // 这是目标
            );

            // 发布器的JWT必须包含所有这些目标或在mercure.publish的 * 中，否则你将收到一个401
            // 订阅器的JWT必须至少包含其中一个目标或在Mercure的 * 中，以接收更新。
            $publisher($update);

            return new Response('published to the selected targets!');
        }
    }

要订阅私有更新，订阅器必须提供一个JWT，其中包含至少一个标记Hub更新的目标。

要提供此JWT，订阅器可以使用cookie或 ``Authorization`` HTTP标头。打开一个 ``EventSource``
连接时，浏览器会自动发送Cookie。当客户端是Web浏览器时，它们是最安全和首选的方式。
如果客户端不是Web浏览器，那么使用 ``Authorization`` 标头是可行的方法。

在以下示例控制器中，生成的cookie包含一个JWT，它本身包含适当的目标。
连接到Hub时，Web浏览器将自动发送此cookie。然后，Hub将验证所提供的JWT的有效性，并从中提取目标。

要生成JWT，我们将使用 ``lcobucci/jwt`` 库。先安装它：

.. code-block:: terminal

    $ composer require lcobucci/jwt

这是控制器::

    // src/Controller/DiscoverController.php
    namespace App\Controller;

    use Fig\Link\Link;
    use Lcobucci\JWT\Builder;
    use Lcobucci\JWT\Signer\Hmac\Sha256;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    class DiscoverController extends AbstractController
    {
        public function __invoke(Request $request): Response
        {
            $hubUrl = $this->getParameter('mercure.default_hub');
            $this->addLink($request, new Link('mercure', $hubUrl));

            $username = $this->getUser()->getUsername(); // 检索当前用户的用户名
            $token = (new Builder())
                // 设置其他适当的JWT声明，例如到期日期
                ->set('mercure', ['subscribe' => "http://example.com/user/$username"]) // 还可以包括安全角色或其他任何角色
                ->sign(new Sha256(), $this->getParameter('mercure_secret_key')) // 别忘了设置这个参数！测试值：AveryCretKey
                ->getToken();

            $response = $this->json(['@id' => '/demo/books/1', 'availability' => 'https://schema.org/InStock']);
            $response->headers->set(
                'set-cookie',
                sprintf('mercureAuthorization=%s; path=/hub; secure; httponly; SameSite=strict', $token)
            );

            return $response;
        }
    }

.. caution::

    要使用cookie认证方法，Symfony应用和Hub必须来自同一域（可以是不同的子域）。

以编程方式生成用于发布的JWT
---------------------------------------------------

你可以创建一个服务来返回 ``Publisher`` 对象使用的令牌，而不是直接将JWT存储在配置中::

    // src/Mercure/MyJwtProvider.php
    namespace App\Mercure;

    final class MyJwtProvider
    {
        public function __invoke(): string
        {
            return 'the-JWT';
        }
    }

然后，在bundle配置中引用此服务：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/mercure.yaml
        mercure:
            hubs:
                default:
                    url: https://mercure-hub.example.com/hub
                    jwt_provider: App\Mercure\MyJwtProvider

    .. code-block:: xml

        <!-- config/packages/mercure.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <config>
            <hub
                name="default"
                url="https://mercure-hub.example.com/hub"
                jwt-provider="App\Mercure\MyJwtProvider"
            />
        </config>

    .. code-block:: php

        // config/packages/mercure.php
        use App\Mercure\MyJwtProvider;

        $container->loadFromExtension('mercure', [
            'hubs' => [
                'default' => [
                    'url' => 'https://mercure-hub.example.com/hub',
                    'jwt_provider' => MyJwtProvider::class,
                ],
            ],
        ]);

当使用具有到期日期的令牌时，此方法特别方便，可以通过编程方式刷新。

Web API
--------

创建Web API时，能够立即将新版本的资源推送到所有已连接的设备并更新其视图。

每次创建、修改或删除API资源时，API平台都可以使用Mercure组件自动发送更新。

首先使用官方指令来安装该库：

.. code-block:: terminal

    $ composer require api

然后，创建以下实体就足以获得一个功能齐全的超媒体API，并通过Mercure hub自动更新广播::

    // src/Entity/Book.php
    namespace App\Entity;

    use ApiPlatform\Core\Annotation\ApiResource;
    use Doctrine\ORM\Mapping as ORM;

    /**
    * @ApiResource(mercure=true)
    * @ORM\Entity
    */
    class Book
    {
        /**
         * @ORM\Id
         * @ORM\Column
         */
        public $name;

        /**
         * @ORM\Column
         */
        public $status;
    }

正如 `在此视频中`_ 所展示的那样，API平台的客户端生成器还允许从该API构建完整的React和React Native应用。
这些应用将实时呈现Mercure更新的内容。

查看 `专用的API平台文档`_，了解有关Mercure支持的更多信息。

.. _`Mercure协议`: https://github.com/dunglas/mercure#protocol-specification
.. _`服务器推送事件（SSE）`: https://developer.mozilla.org/docs/Server-sent_events
.. _`polyfill`: https://github.com/Yaffle/EventSource
.. _`高级实现`: https://github.com/dunglas/mercure#tools
.. _`在此视频中`: https://www.youtube.com/watch?v=UI1l0JOjLeI
.. _`API平台`: https://api-platform.com
.. _`Mercure.rocks`: https://mercure.rocks
.. _`API平台发行版`: https://api-platform.com/docs/distribution/
.. _`JSON Web Token`: https://tools.ietf.org/html/rfc7519
.. _`JWT示例`: https://jwt.io/#debugger-io?token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJtZXJjdXJlIjp7InB1Ymxpc2giOlsiKiJdfX0.iHLdpAEjX4BqCsHJEegxRmO-Y6sMxXwNATrQyRNt3GY
.. _`IRI`: https://tools.ietf.org/html/rfc3987
.. _`实用界面`: https://twitter.com/chromedevtools/status/562324683194785792
.. _`专用的API平台文档`: https://api-platform.com/docs/core/mercure/
.. _`在线调试器`: https://uri-template-tester.mercure.rocks
