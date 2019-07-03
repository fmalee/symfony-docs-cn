.. index::
   single: HttpClient
   single: Components; HttpClient

HttpClient组件
========================

    HttpClient组件是一个低级HTTP客户端，同时支持PHP流封装器和cURL。
    它提供了实用的API​​，并支持同步和异步操作。

.. versionadded:: 4.3

    HttpClient组件是在Symfony 4.3中引入的，它仍然被认为是一个
    :doc:`实验性功能 </contributing/code/experimental>`。

安装
------------

.. code-block:: terminal

    $ composer require symfony/http-client

.. include:: /components/require_autoload.rst.inc

基本用法
-----------

使用 :class:`Symfony\\Component\\HttpClient\\HttpClient`
类创建发起请求的低级HTTP客户端，如下面的 ``GET`` 请求::

    use Symfony\Component\HttpClient\HttpClient;

    $httpClient = HttpClient::create();
    $response = $httpClient->request('GET', 'https://api.github.com/repos/symfony/symfony-docs');

    $statusCode = $response->getStatusCode();
    // $statusCode = 200
    $contentType = $response->getHeaders()['content-type'][0];
    // $contentType = 'application/json'
    $content = $response->getContent();
    // $content = '{"id":521583, "name":"symfony-docs", ...}'
    $content = $response->toArray();
    // $content = ['id' => 521583, 'name' => 'symfony-docs', ...]

性能
-----------

该组件是为最大化HTTP性能而构建的。根据设计，它兼容HTTP/2，也兼容并发异步流和多路复用的请求/响应。
即使在进行常规的同步调用时，此设计也允许在请求之间保持与远程主机的连接，并通过保存重复的DNS解析、SSL协商等来提高性能。
如果要利用所有这些设计优势，需要cURL扩展。

启用cURL支持
~~~~~~~~~~~~~~~~~~~~~

该组件支持用原生的PHP流和cURL发起HTTP请求。
虽然两者可互换使用并且提供包括并发请求在内的相同功能，但只有在使用cURL时才支持HTTP/2。

如果启用了 `cURL PHP扩展`_，则 ``HttpClient::create()``
选择cURL传输，否则回退到PHP流。如果你希望显式的选择传输，请使用以下类来创建客户端::

    use Symfony\Component\HttpClient\CurlHttpClient;
    use Symfony\Component\HttpClient\NativeHttpClient;

    // 使用原生的PHP流
    $httpClient = new NativeHttpClient();

    // 使用cURL PHP扩展
    $httpClient = new CurlHttpClient();

在全栈Symfony应用中使用此组件时，不可配置此行为。
如果安装并启用了cURL PHP扩展，则将自动使用cURL。否则，将使用原生的PHP流。

HTTP/2 支持
~~~~~~~~~~~~~~

请求 ``https`` URL时，如果使用了libcurl >= 7.36，则默认启用HTTP/2。要强制
``http`` URL为 HTTP/2，你需要通过 ``http_version`` 选项显式的启用它::

    $httpClient = HttpClient::create(['http_version' => '2.0']);

当libcurl >= 7.61与PHP >= 7.2.17/7.3.4一起使用时，HTTP/2 PUSH开箱即用：
推送的响应被放入临时缓存中，并在为相应的URL触发子请求时使用。

发起请求
---------------

使用 ``HttpClient`` 类创建的客户端提供了一种执行各种HTTP请求的 ``request()`` 方法::

    $response = $httpClient->request('GET', 'https://...');
    $response = $httpClient->request('POST', 'https://...');
    $response = $httpClient->request('PUT', 'https://...');
    // ...

响应始终是异步的，因此对该方法的调用会立即返回，而不是等待接收响应::

    // 代码执行立即并继续；它不会等待接收响应
    $response = $httpClient->request('GET', 'http://releases.ubuntu.com/18.04.2/ubuntu-18.04.2-desktop-amd64.iso');

    // 等待获取响应标头，直到它们到达为止
    $contentType = $response->getHeaders()['content-type'][0];

    // 尝试获取响应内容将中断执行，直到收到完整的响应内容为止。
    $contents = $response->getContent();

该组件还支持完全异步的应用的 :ref:`流式响应 <http-client-streaming-responses>`。

.. note::

    当PHP的运行时和远程服务器都支持HTTP压缩和分块传输编码时，它们会自动启用。

认证
~~~~~~~~~~~~~~

HTTP客户端支持不同的认证机制。
它们可以在创建客户端时全局定义（将其应用于所有请求），或每个请求单独定义（覆盖任何全局认证）::

    // 对所有请求使用相同的认证
    $httpClient = HttpClient::create([
        // 只使用用户名而不使用密码的HTTP Basic认证
        'auth_basic' => ['the-username'],

        // 使用用户名和密码进行HTTP Basic认证
        'auth_basic' => ['the-username', 'the-password'],

        // HTTP Bearer认证（也称为令牌认证）
        'auth_bearer' => 'the-bearer-token',
    ]);

    $response = $httpClient->request('GET', 'https://...', [
        // 仅对此请求使用不同的HTTP Basic认证
        'auth_basic' => ['the-username', 'the-password'],

        // ...
    ]);

查询字符串参数
~~~~~~~~~~~~~~~~~~~~~~~

你可以手动将它们附加到请求的URL，也可以通过 ``query``
选项将它们定义为关联数组，该选项将与URL合并::

    // 将向 https://httpbin.org/get?token=...&name=... 发起一个HTTP GET请求
    $response = $httpClient->request('GET', 'https://httpbin.org/get', [
        // 这些值在包含到URL中之前会自动编码
        'query' => [
            'token' => '...',
            'name' => '...',
        ],
    ]);

标头
~~~~~~~

使用 ``headers`` 选项可以定义添加到所有请求的默认标头和每个请求的特定标头::

    // 此标头将添加到此客户端所发起的所有请求中
    $httpClient = HttpClient::create(['headers' => [
        'User-Agent' => 'My Fancy App',
    ]]);

    // 此标头仅包含在此请求中，如果HTTP客户端全局定义了同一标头，将会被此标头的值重写。
    $response = $httpClient->request('POST', 'https://...', [
        'headers' => [
            'Content-Type' => 'text/plain',
        ],
    ]);

上传数据
~~~~~~~~~~~~~~

该组件提供了几种使用 ``body``
选项来上传数据的方法。 你可以使用常规字符串、闭包、迭代和资源，并在发起请求时自动处理它们::

    $response = $httpClient->request('POST', 'https://...', [
        // 使用常规字符串来定义数据
        'body' => 'raw data',

        // 使用参数数组来定义数据
        'body' => ['parameter1' => 'value1', '...'],

        // 使用闭包来生成上传的数据
        'body' => function (int $size): string {
            // ...
        },

        // 从资源中获取数据
        'body' => fopen('/path/to/file', 'r'),
    ]);

使用 ``POST`` 方法上传数据时，如果未明确定义``Content-Type``
标头，Symfony会假定你正在上传表单数据，并为你添加所需的
``'Content-Type: application/x-www-form-urlencoded'`` 标头。

当 ``body`` 选项设置为闭包时，它将被多次调用，直到它返回表示正文的结尾的空字符串。
每次，闭包应该返回一个小于作为参数请求的数量的字符串。

除了闭包，也可以使用生成器或任何 ``Traversable``。

.. tip::

    上载JSON有效负载时，请使用 ``json`` 选项而不是
    ``body``。给定的内容将自动进行JSON编码，该请求也会自动添加
    ``Content-Type: application/json``::

        $response = $httpClient->request('POST', 'https://...', [
            'json' => ['param1' => 'value1', '...'],
        ]);

        $decodedPayload = $response->toArray();

要提交包含文件上传的表单，你有必要根据 ``multipart/form-data`` 内容类型对正文进行编码。
:doc:`Symfony Mime </components/mime>` 组件可以几行代码就完成它::

    use Symfony\Component\Mime\Part\DataPart;
    use Symfony\Component\Mime\Part\Multipart\FormDataPart;

    $formFields = [
        'regular_field' => 'some value',
        'file_field' => DataPart::fromPath('/path/to/uploaded/file'),
    ];
    $formData = new FormDataPart($formFields);
    $client->request('POST', 'https://...', [
        'headers' => $formData->getPreparedHeaders()->toArray(),
        'body' => $formData->bodyToIterable(),
    ]);

Cookie
~~~~~~~

此组件提供的HTTP客户端是无状态的，但处理cookie需要有一个状态存储（因为响应可以更新cookie，并且它们必须用于后续请求）。
这就是这个组件不能自动处理cookie的原因。

你可以使用 ``Cookie`` HTTP标头自己处理cookie，也可以使用提供此功能的
:doc:`BrowserKit组件 </components/browser_kit>`，它可以与HttpClient组件无缝集成。

重定向
~~~~~~~~~

默认情况下，HTTP客户端在发起请求时会遵循重定向（最多20个）。使用 ``max_redirects``
设置来配置此行为（如果重定向的数量高于配置的值，你将获得一个
:class:`Symfony\\Component\\HttpClient\\Exception\\RedirectionException`）::

    $response = $httpClient->request('GET', 'https://...', [
        // 0 表示不遵循任何重定向
        'max_redirects' => 0,
    ]);

HTTP代理
~~~~~~~~~~~~

默认情况下，此组件遵循操作系统定义的标准环境变量，以引导HTTP流量通过本地代理。
这意味着，如果正确配置了这些环境变量，通常无需为任何客户端配置代理。

你仍然可以使用 ``proxy`` 和 ``no_proxy`` 选项来设置或重写这些设置::

* ``proxy`` 应该设置为要通过的 ``http://...`` URL代理

* ``no_proxy`` 为用逗号分隔的列表中的主机禁用代理。

进度回调
~~~~~~~~~~~~~~~~~

通过为 ``on_progress`` 选项提供一个回调，可以在上传/下载完成时跟踪它们。
保证在DNS解析、标头到达和完成时调用此回调；
此外，在上传或下载新数据时调用此回调，并且至少每秒调用一次::

    $response = $httpClient->request('GET', 'https://...', [
        'on_progress' => function (int $dlNow, int $dlSize, array $info): void {
            // $dlnow 是目前下载的字节数
            // $dlsize 是要下载的总大小，如果未知，则为-1
            // $info 是此时 $response->getInfo() 会返回的值。
        },
    ]);

从回调中抛出的任何异常都将封装在一个 ``TransportExceptionInterface`` 实例中，并将中止该请求。

高级选项
~~~~~~~~~~~~~~~~

:class:`Symfony\\Contracts\\HttpClient\\HttpClientInterface`
定义了所有完全控制请求的执行方式的选项，包括DNS预解析、SSL参数、公钥固定(HPKP)等。

处理响应
--------------------

所有HTTP客户端返回的响应都是
:class:`Symfony\\Contracts\\HttpClient\\ResponseInterface` 类型的对象，它提供了以下方法::

    $response = $httpClient->request('GET', 'https://...');

    //获取响应的HTTP状态代码
    $statusCode = $response->getStatusCode();

    // 以字符串[][]的形式获取HTTP头，其中标头名称都是小写
    $headers = $response->getHeaders();

    // 以字符串形式获取响应正文
    $content = $response->getContent();

    // 取消请求/响应
    $response->cancel();

    // 返回来自传输层的信息，如
    // "response_headers"、"redirect_count"、"start_time"、"redirect_url" 等。
    $httpInfo = $response->getInfo();
    // 你也可以获取单个信息
    $startTime = $response->getInfo('start_time');

.. note::

    ``$response->getInfo()`` 是非阻塞的：它返回有关响应的 *实时* 信息。
    当你调用它时，有些信息可能还是未知的（例如 ``http_code``）。

.. tip::

    调用 ``$response->getInfo('debug')`` 可以获取有关HTTP事务的详细日志。

.. _http-client-streaming-responses:

流式响应
~~~~~~~~~~~~~~~~~~~

调用HTTP客户端的 ``stream()`` 方法可以顺序获取响应的 *区块*，而不是等待整个响应::

    $url = 'https://releases.ubuntu.com/18.04.1/ubuntu-18.04.1-desktop-amd64.iso';
    $response = $httpClient->request('GET', $url, [
        // 可选：如果你不想在内存中缓冲响应
        'buffer' => false,
    ]);

    // 响应是惰性的：接收到标头后立即执行此代码
    if (200 !== $response->getStatusCode()) {
        throw new \Exception('...');
    }

    // 获取块中的响应内容并将其保存到文件中
    // 响应块实现了 Symfony\Contracts\HttpClient\ChunkInterface
    $fileHandler = fopen('/ubuntu.iso', 'w');
    foreach ($httpClient->stream($response) as $chunk) {
        fwrite($fileHandler, $chunk->getContent());
    }

取消回应
~~~~~~~~~~~~~~~~~~~

要中止一个请求（例如，因为它没有在适当的时候完成，或者你只想获取响应的第一个字节等等），你可以使用
``ResponseInterface`` 的 ``cancel()`` 方法::

    $response->cancel()

或者从进程回调中抛出一个异常::

    $response = $client->request('GET', 'https://...', [
        'on_progress' => function (int $dlNow, int $dlSize, array $info): void {
            // ...

            throw new \MyException();
        },
    ]);

该异常将封装在一个 ``TransportExceptionInterface`` 实例中，并将中止该请求。

处理异常
~~~~~~~~~~~~~~~~~~~

当响应的HTTP状态代码在300-599范围内（即3xx，4xx或5xx）时，你的代码应该处理它。
如果你不这样做，``getHeaders()`` 和 ``getContent()`` 方法会抛出一个适当的异常::

    // 此请求的响应将是403 HTTP错误
    $response = $httpClient->request('GET', 'https://httpbin.org/status/403');

    // 此代码将导致一个 Symfony\Component\HttpClient\Exception\ClientException
    // 因为它不检查响应的状态代码
    $content = $response->getContent();

    // 将 FALSE 作为可选参数传递，以不引发异常并返回原始响应内容（即使它是一个错误消息）
    $content = $response->getContent(false);

并发请求
-------------------

由于响应是惰性的，因此始终会同步的管理请求。
在足够快的网络中使用cURL时，以下代码将在不到半秒的时间内发起379个请求::

    use Symfony\Component\HttpClient\CurlHttpClient;

    $client = new CurlHttpClient();

    $responses = [];

    for ($i = 0; $i < 379; ++$i) {
        $uri = "https://http2.akamai.com/demo/tile-$i.png";
        $responses[] = $client->request('GET', $uri);
    }

    foreach ($responses as $response) {
        $content = $response->getContent();
        // ...
    }

正如你在第一个 “for” 循环中看到的那样，请求已经发起，但尚未被使用。
这是需要并发时的一个技巧：应首先发送请求，稍后再读取。
这将允许客户端在代码等待特定请求时监视所有挂起的请求，就像在上述 "foreach" 循环的每个迭代中所做的那样。

多路复用响应
~~~~~~~~~~~~~~~~~~~~~~

如果你再次查看上面的代码片段，会发现是按请求的顺序读取响应的。
但也许第二个响应在第一个响应之前就返回了了？完全异步操作需要能够以他们返回的任何顺序来处理响应。

为此，HTTP客户端的 ``stream()`` 方法接受要监视的响应列表。如
:ref:`之前 <http-client-streaming-responses>`
所提及的，此方法在响应从网络到达时产生(yield)响应块。
通过使用此代码替换代码片段中的 “foreach”，代码将变为完全异步::

    foreach ($client->stream($responses) as $response => $chunk) {
        if ($chunk->isFirst()) {
            // $response 的标头刚刚到达
            // $response->getHeaders() 现在是一个非阻塞调用
        } elseif ($chunk->isLast()) {
            // $response 的全部内容刚刚完成
            // $response->getContent() 现在是一个非阻塞调用
        } else {
            // $chunk->getContent() 将返回刚刚到达的响应主体的一部分
        }
    }

.. tip::

    使用 ``$response->getInfo('user_data')`` 结合 ``user_data``
    选项来跟踪foreach循环中响应的标识。

处理网络超时
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

该组件允许处理请求/响应超时。

例如，当DNS解析花费太多时间、无法在给定时间预算中打开TCP连接或者响应内容暂停时间过长时，都可能会发生超时。
这可以使用 ``timeout`` 请求选项进行配置::

    // 如果访问 $response 的2.5秒内未发生任何事件，
    // 则将抛出一个 TransportExceptionInterface。
    $response = $client->request('GET', 'https://...', ['timeout' => 2.5]);

如果未设置该选项，则使用PHP INI的 ``default_socket_timeout`` 设置。

可以使用 ``stream()`` 方法的第二个参数来重写该选项。这允许一次监视多个响应并将超时应用于组中的所有响应。
如果所有响应在给定的持续时间内变为非活动状态，则该方法将生成一个特殊的块，该块的
``isTimeout()`` 将返回 ``true``::

    foreach ($client->stream($responses, 1.5) as $response => $chunk) {
        if ($chunk->isTimeout()) {
            // $response 持续超过1.5秒
        }
    }

超时不一定是错误：你可以决定再次流式处理该响应，并获取可能在新的超时内返回的剩余内容，等等。

.. tip::

    传递 ``0`` 作为超时允许以非阻塞方式监视响应。

.. note::

    超时会控制 *HTTP事务空闲时* 愿意等待的时间。
    如果大响应在传输过程中保持活动状态，并且不会停留超过指定时间，则它们可以持续到完成响应所需的时间。

处理网络错误
~~~~~~~~~~~~~~~~~~~~~~~~~~~

网络错误（管道损坏，DNS解析失败等）将抛出一个
:class:`Symfony\\Contracts\\HttpClient\\Exception\\TransportExceptionInterface` 实例。

首先，你不是 *必须* 处理它们：让错误冒泡到你的通用异常处理堆栈中，可能在在大多数用例中真的没事。

如果你想处理它们，你需要了解以下内容::

要捕获错误，你需要封装对 ``$client->request()`` 的调用，还要调用返回响应的任何方法。 
这是因为响应是惰性的，因此在调用（如 ``getStatusCode()``）时可能会发生网络错误::

    try {
        // 两行都有可能抛出
        $response = $client->request(...);
        $headers = $response->getHeaders();
        // ...
    } catch (TransportExceptionInterface $e) {
        // ...
    }

.. note::

    因为 ``$response->getInfo()`` 是非阻塞的，所以它不应该被设计抛出。

在多路复用响应时，你可以通过在 foreach 循环中捕获 ``TransportExceptionInterface``
来处理各个流的错误::

    foreach ($client->stream($responses) as $response => $chunk) {
        try {
            if ($chunk->isLast()) {
                // ... 用 $response 做点什么
            }
        } catch (TransportExceptionInterface $e) {
            // ...
        }
    }

缓存请求和响应
------------------------------

该组件提供了一个 :class:`Symfony\\Component\\HttpClient\\CachingHttpClient`
装饰器，以允许缓存响应并从本地存储中为下一个请求提供响应。该实现在幕后利用了
:class:`Symfony\\Component\\HttpKernel\\HttpCache\\HttpCache`
类，因此需要在你的应用中安装 :doc:`HttpKernel组件 </components/http_kernel>`::

    use Symfony\Component\HttpClient\CachingHttpClient;
    use Symfony\Component\HttpClient\HttpClient;
    use Symfony\Component\HttpKernel\HttpCache\Store;

    $store = new Store('/path/to/cache/storage/');
    $client = HttpClient::create();
    $client = new CachingHttpClient($client, $store);

    // 如果资源已经在缓存中，则不会命中网络
    $response = $client->request('GET', 'https://example.com/cacheable-resource');

``CachingHttpClient`` 接受第三个参数来设置 ``HttpCache`` 的选项。

作用域客户端
--------------

通常，某些HTTP客户端的选项取决于请求的URL
（例如，你在向GitHub API发起请求时必须设置一些标头，但不能为其他主机设置同样的标头）。
如果是这种情况，该组件通过 :class:`Symfony\\Component\\HttpClient\\ScopingHttpClient`
类提供一个特殊的根据请求的URL自动配置的HTTP客户端::

    use Symfony\Component\HttpClient\HttpClient;
    use Symfony\Component\HttpClient\ScopingHttpClient;

    $client = HttpClient::create();
    $client = new ScopingHttpClient($client, [
        // 定义为值的选项仅适用于与定义为键的正则表达式相匹配的URL
        'https://api\.github\.com/' => [
            'headers' => [
                'Accept' => 'application/vnd.github.v3+json',
                'Authorization' => 'token '.$githubToken,
            ],
        ],
    ]);

如果请求的URL是相对的（因为你使用了 ``base_uri`` 选项），则作用域HTTP客户端无法匹配。
这就是为什么你可以在它的构造函数中定义第三个可选参数，它将被视为应用于相对URL的默认正则表达式::

    // ...

    $httpClient = new ScopingHttpClient($client, [
        'https://api\.github\.com/' => [
            'base_uri' => 'https://api.github.com/',
            // ...
        ],
    ],
        // 这是应用于所有相对URL的正则表达式
        'https://api\.github\.com/'
    );

互通性
----------------

该组件可与HTTP客户端的两种不同抽象进行互通：`Symfony契约`_ 和PSR-18。
该组件与两者兼容，你的应用可以使用其中任何一个库。当使用
:ref:`framework bundle <framework-bundle-configuration>`
时，它们也受益于 :ref:`自动装配别名 <service-autowiring-alias>`。

如果你正在编写或维护发起HTTP请求的库，则可以通过对Symfony Contracts（推荐）
或PSR-18进行编码，将其与任何特定HTTP客户端实现分离。

Symfony契约
~~~~~~~~~~~~~~~~~

``symfony/http-client-contracts`` 包中的接口定义了组件实现的主要抽象。
 它的切入点是 :class:`Symfony\\Contracts\\HttpClient\\HttpClientInterface`。
 这是需要客户端时要进行编码的接口::

    use Symfony\Contracts\HttpClient\HttpClientInterface;

    class MyApiLayer
    {
        private $client;

        public function __construct(HttpClientInterface $client)
        {
            $this->client = $client
        }

        // [...]
    }

上面提到的所有请求选项（例如超时管理）也在接口的措辞中定义，这样任何兼容的实现（比如这个组件）都可以保证提供它们。
这与PSR-18抽象有很大的不同，PSR-18抽象没有提供与传输本身相关的抽象。

Symfony Contracts涵盖的另一个主要功能是如前面部分所述的异步/多路复用。

PSR-18
~~~~~~

该组件通过 :class:`Symfony\\Component\\HttpClient\\Psr18Client` 类实现了
`PSR-18`_（HTTP客户端）规范，该类是将Symfony的 ``HttpClientInterface``
转换为PSR-18的 ``ClientInterface`` 的一个适配器。

要使用它，你需要 ``psr/http-client`` 包和一个 `PSR-17`_ 实现::

.. code-block:: terminal

    # 安装 PSR-18 ClientInterface
    $ composer require psr/http-client

    # 使用Symfony Flex提供的自动装配别名来安装响应和流工厂的有效实现
    $ composer require nyholm/psr7

现在你可以使用PSR-18客户端来发起HTTP请求了，如下所示::

    use Nyholm\Psr7\Factory\Psr17Factory;
    use Symfony\Component\HttpClient\Psr18Client;

    $psr17Factory = new Psr17Factory();
    $psr18Client = new Psr18Client();

    $url = 'https://symfony.com/versions.json';
    $request = $psr17Factory->createRequest('GET', $url);
    $response = $psr18Client->sendRequest($request);

    $content = json_decode($response->getBody()->getContents(), true);

Symfony框架集成
-----------------------------

在全栈Symfony应用中使用此组件时，你可以配置具有不同配置的多个客户端并将其注入到你的服务中。

配置
~~~~~~~~~~~~~

使用 ``framework.http_client`` 键配置应用中使用的默认HTTP客户端。
查看完整的 :ref:`http_client配置参考 <reference-http-client>` 以了解所有可用的配置选项::

.. code-block:: yaml

    # config/packages/framework.yaml
    framework:
        # ...
        http_client:
            max_host_connections: 10
            default_options:
                max_redirects: 7

如果要定义多个HTTP客户端，请使用其他扩展配置：

.. code-block:: yaml

    # config/packages/framework.yaml
    framework:
        # ...
        http_client:
            scoped_clients:
                crawler.client:
                    headers: { 'X-Powered-By': 'ACME App' }
                    http_version: '1.0'
                some_api.client:
                    max_redirects: 5

将HTTP客户端注入服务
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你的应用只需要一个HTTP客户端，可以通过使用
:class:`Symfony\\Contracts\\HttpClient\\HttpClientInterface`
对构造函数参数进行类型约束，以将默认值注入任何服务::

    use Symfony\Contracts\HttpClient\HttpClientInterface;

    class SomeService
    {
        private $httpClient;

        public function __construct(HttpClientInterface $httpClient)
        {
            $this->httpClient = $httpClient;
        }
    }

如果你有多个客户端，则必须使用Symfony定义的任何方法来
:ref:`选择一个特定服务 <services-wire-specific-service>`。
每个客户端都有一个用自身的配置命名的唯一服务。

每个作用域客户端还定义了相应的命名自动装配别名。如果你使用例如
``Symfony\Contracts\HttpClient\HttpClientInterface $myApiClient``
作为参数的类型和名称，则自动装配会将 ``my_api.client`` 服务注入你的自动装配的类中。

测试HTTP客户端和响应
----------------------------------

此组件包括 ``MockHttpClient`` 和 ``MockResponse``
类，以便在测试中使用这种不发起实际HTTP请求的HTTP客户端。

第一种使用 ``MockHttpClient`` 的方法是将一个响应列表传递给其构造函数。
这些响应将在发起请求时按顺序产生::

    use Symfony\Component\HttpClient\MockHttpClient;
    use Symfony\Component\HttpClient\Response\MockResponse;

    $responses = [
        new MockResponse($body1, $info1),
        new MockResponse($body2, $info2),
    ];

    $client = new MockHttpClient($responses);
    // 响应的返回顺序与传递给 MockHttpClient 的顺序相同
    $response1 = $client->request('...'); // 返回 $responses[0]
    $response2 = $client->request('...'); // 返回 $responses[1]

另一种使用 ``MockHttpClient`` 方法是传递一个在被调用时会动态生成响应的回调::

    use Symfony\Component\HttpClient\MockHttpClient;
    use Symfony\Component\HttpClient\Response\MockResponse;

    $callback = function ($method, $url, $options) {
        return new MockResponse('...');
    };

    $client = new MockHttpClient($callback);
    $response = $client->request('...'); // 调用 $callback 以获取响应

提供给模拟客户端的响应不必是 ``MockResponse`` 的实例。任何实现了 ``ResponseInterface``
的类都会生效（例如 ``$this->createMock(ResponseInterface::class)``）。

但是，使用 ``MockResponse`` 将允许模拟分块响应和超时::

    $body = function () {
        yield 'hello';
        // 空字符串将变为超时，以便易于测试
        yield '';
        yield 'world';
    };

    $mockResponse = new MockResponse($body());

.. _`cURL PHP扩展`: https://php.net/curl
.. _`PSR-17`: https://www.php-fig.org/psr/psr-17/
.. _`PSR-18`: https://www.php-fig.org/psr/psr-18/
.. _`Symfony契约`: https://github.com/symfony/contracts
