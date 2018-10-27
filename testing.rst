.. index::
   single: Tests

测试
=======

每当你多写一行代码，你都在增加潜在的新bug。
为了打造更好、更好可靠的程序，你需要对代码使用功能测试和单元测试。

PHPUnit测试框架
-----------------------------

Symfony与一个名为 `PHPUnit`_ 的独立库集成，为你提供丰富的测试框架。
本文不会介绍PHPUnit本身，它有自己的优秀 `文档`_。

在创建第一个测试之前，先安装 `PHPUnit Bridge组件`_，该组件包装原始PHPUnit二进制文件以提供额外功能：

.. code-block:: terminal

    $ composer require --dev symfony/phpunit-bridge

每个测试 - 无论是单元测试还是功能测试 - 都是一个PHP类，应该存在于应用程序的 ``tests/`` 目录中。
如果遵循此规则，则可以使用以下命令运行应用的所有测试：

.. code-block:: terminal

    $ ./bin/phpunit

.. note::

    安装 ``phpunit-bridge`` 软件包时，:doc:`Symfony Flex </setup/flex>` 会创建 ``./bin/phpunit`` 命令。
    如果缺少该命令，则可以删除该包（``composer remove symfony/phpunit-bridge``）并重新安装。
    另一种解决方案是删除项目的 ``symfony.lock`` 文件并运行 ``composer install`` 以强制执行Symfony Flex的所有指令。

PHPUnit由Symfony应用的根目录中的 ``phpunit.xml.dist`` 文件配置。

.. tip::

    可以使用 ``--coverage-*`` 选项生成代码覆盖率，有关详细信息，请参阅使用 ``--help`` 时显示的帮助信息。

.. index::
   single: Tests; Unit tests

单元测试
----------

单元测试是针对单个PHP类的测试，也称为 *单元*。
如果要测试应用程序的整体行为，请参阅有关功 :ref:`功能测试 <functional-tests>` 的章节。

编写Symfony单元测试与编写标准PHPUnit单元测试没有什么不同。
例如，假设你在app bundle的 ``Util/`` 目录中有一个名为 ``Calculator`` 的非常简单的类::

    // src/Util/Calculator.php
    namespace App\Util;

    class Calculator
    {
        public function add($a, $b)
        {
            return $a + $b;
        }
    }

为了测试它，创建一个 ``CalculatorTest`` 文件到应用的 ``tests/Util`` 目录下::

    // tests/Util/CalculatorTest.php
    namespace App\Tests\Util;

    use App\Util\Calculator;
    use PHPUnit\Framework\TestCase;

    class CalculatorTest extends TestCase
    {
        public function testAdd()
        {
            $calculator = new Calculator();
            $result = $calculator->add(30, 12);

            // 断言你的计算器正确添加了数字！
            $this->assertEquals(42, $result);
        }
    }

.. note::

    按照惯例，``tests/`` 目录应该复制bundle的目录以进行单元测试。
    因此，如果你正在测试 ``src/Util/`` 目录中的类，请将测试放在 ``tests/Util/`` 目录中。

就像在你的实际应用中一样 - 通过 ``vendor/autoload.php`` 文件自动启用自动加载
（默认情况下在``phpunit.xml.dist`` 文件中配置）。

针对指定文件或目录的测试也很简单：

.. code-block:: terminal

    # 运行应用的所有测试
    $ ./bin/phpunit

    # 运行 Util/ 目录的所有测试
    $ ./bin/phpunit tests/Util

    # 运行 Calculator 类的测试
    $ ./bin/phpunit tests/Util/CalculatorTest.php

.. index::
   single: Tests; Functional tests

.. _functional-tests:

功能测试
----------------

功能测试检查应用的不同层的集成（从路由到视图）。
就PHPUnit而言，它们与单元测试没有什么不同，但它们具有非常特定的工作流程：

* 发出请求；
* 点击链接或提交表单；
* 测试响应；
* 清洗并重复。

在创建第一个测试之前，请安装这些包，这些包提供了功能测试中使用的一些实用工具：

.. code-block:: terminal

    $ composer require --dev symfony/browser-kit symfony/css-selector

你的第一个功能测试
~~~~~~~~~~~~~~~~~~~~~~~~~~

首先，在项目中安装BrowserKit组件：

.. code-block:: terminal

    $ composer require --dev symfony/browser-kit

功能测试是PHP文件，通常位于bundle的 ``tests/Controller`` 目录中。
如果要测试由 ``PostController`` 类处理的页面，
首先要创建一个继承特殊 ``WebTestCase`` 类的新 ``PostControllerTest.php`` 文件。

例如，一个测试可能如下所示::

    // tests/Controller/PostControllerTest.php
    namespace App\Tests\Controller;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class PostControllerTest extends WebTestCase
    {
        public function testShowPost()
        {
            $client = static::createClient();

            $client->request('GET', '/post/hello-world');

            $this->assertEquals(200, $client->getResponse()->getStatusCode());
        }
    }

.. tip::

    要运行功能测试，``WebTestCase`` 类需要知道引导它的应用内核。
    内核类通常在 ``KERNEL_CLASS`` 环境变量中定义（包含在Symfony提供的默认 ``phpunit.xml.dist`` 文件中）：

    .. code-block:: xml

        <?xml version="1.0" charset="utf-8" ?>
        <phpunit>
            <php>
                <!-- the value is the FQCN of the application kernel -->
                <env name="KERNEL_CLASS" value="App\Kernel" />
            </php>
            <!-- ... -->
        </phpunit>

    如果你的用例更复杂，你还可以覆盖功能测试的 ``createKernel()``
    或 ``getKernelClass()`` 方法，这些方法优先于 ``KERNEL_CLASS`` 环境变量。

在上面的示例中，你验证了HTTP响应是否成功。下一步是验证页面实际上是否包含预期的内容。
``createClient()`` 方法返回一个客户端，就像将用于抓取你的网站的浏览器::

    $crawler = $client->request('GET', '/post/hello-world');

``request()`` 方法（阅读 :ref:`有关请求方法的更多信息 <testing-request-method-sidebar>`）返回一个
:class:`Symfony\\Component\\DomCrawler\\Crawler` 对象，该对象可用于选择响应中的元素，单击链接并提交表单。

.. tip::

    只有当响应是XML或HTML文档时，``Crawler`` 才有效。
    要获取原始内容响应，请调用 ``$client->getResponse()->getContent()``。

Crawler与 ``symfony/css-selector`` 组件集成，为你提供CSS选择器的功能，以查找页面中的内容。
要安装CSS选择器组件，请运行：

.. code-block:: terminal

    $ composer require --dev symfony/css-selector

现在，你可以将CSS选择器与Crawler一起使用。要断言短语“Hello World”至少在页面显示上一次，你可以使用此断言：

    $this->assertGreaterThan(
        0,
        $crawler->filter('html:contains("Hello World")')->count()
    );

Crawler也可用于与页面交互。首先使用Crawler配合XPath表达式或CSS选择器选择链接，然后使用客户端点击它::

    $link = $crawler
        ->filter('a:contains("Greet")') // 查找所有带有"Greet"文本的链接
        ->eq(1) // 选择列表的第二个链接
        ->link()
    ;

    // 然后点击它
    $crawler = $client->click($link);

提交表单非常相似：选择一个表单按钮，可选择覆盖某些表单值并提交相应的表单::

    $form = $crawler->selectButton('submit')->form();

    // 设置一些值
    $form['name'] = 'Lucas';
    $form['form_name[subject]'] = 'Hey there!';

    // 提交表单
    $crawler = $client->submit($form);

.. tip::

    表单还可以处理上传，并包含填写不同类型的表单字段的方法（例如 ``select()`` 和 ``tick()``）。
    有关详细信息，请参阅下面的 `表单`_ 章节。

现在你可以轻松浏览一个应用，使用断言来测试它实际上是否符合你的预期。
使用Crawler在DOM上进行断言：

    // 断言响应与给定的CSS选择器匹配。
    $this->assertGreaterThan(0, $crawler->filter('h1')->count());

如果你只想断言内容包含某些文本，或者如果响应不是XML/HTML文档，则直接对响应内容进行测试::

    $this->assertContains(
        'Hello World',
        $client->getResponse()->getContent()
    );

.. tip::

    你可以使用Symfony Test Pack立即安装所有这些依赖项，而不是单独安装每个测试依赖项：

    .. code-block:: terminal

        $ composer require --dev symfony/test-pack

.. index::
   single: Tests; Assertions

.. sidebar:: 有用的断言

    为了更快地开始，这里列出了最常见和最有用的测试断言::

        use Symfony\Component\HttpFoundation\Response;

        // ...

        // 断言至少有一个带有“副标题”类的h2标签
        $this->assertGreaterThan(
            0,
            $crawler->filter('h2.subtitle')->count()
        );

        // 断言页面上只有4个h2标签
        $this->assertCount(4, $crawler->filter('h2'));

        // 断言“Content-Type”标头是“application / json”
        $this->assertTrue(
            $client->getResponse()->headers->contains(
                'Content-Type',
                'application/json'
            ),
            'the "Content-Type" header is "application/json"' // 失败时显示的可选消息
        );

        // 断言响应内容包含一个字符串
        $this->assertContains('foo', $client->getResponse()->getContent());
        // ...或匹配一个表达式
        $this->assertRegExp('/foo(bar)?/', $client->getResponse()->getContent());

        // 断言响应状态代码是2xx
        $this->assertTrue($client->getResponse()->isSuccessful(), 'response status is 2xx');
        // 断言响应状态代码是404
        $this->assertTrue($client->getResponse()->isNotFound());
        // 断言特定的200状态代码
        $this->assertEquals(
            200, // 或 Symfony\Component\HttpFoundation\Response::HTTP_OK
            $client->getResponse()->getStatusCode()
        );

        // 断言响应是重定向到 /demo/contact
        $this->assertTrue(
            $client->getResponse()->isRedirect('/demo/contact')
            // 如果重定向URL是作为绝对URL生成的
            // $client->getResponse()->isRedirect('http://localhost/demo/contact')
        );
        // ...或者只是检查响应是否重定向到任何URL
        $this->assertTrue($client->getResponse()->isRedirect());

.. _testing-data-providers:

针对不同数据集的测试
--------------------------------------

必须针对不同的数据集执行相同的测试以检查代码必须处理的多个条件。
这可以通过PHPUnit的 `data providers`_ 解决，它们可用于单元和功能测试。

首先，在测试方法中添加一个或多个参数，并在测试代码中使用它们。
然后，定义另一个方法，该方法返回一个嵌套数组，其中包含在每次测试运行时使用的参数。
最后，添加 ``@dataProvider`` 注释以关联两个方法::

    /**
     * @dataProvider provideUrls
     */
    public function testPageIsSuccessful($url)
    {
        $client = self::createClient();
        $client->request('GET', $url);

        $this->assertTrue($client->getResponse()->isSuccessful());
    }

    public function provideUrls()
    {
        return array(
            array('/'),
            array('/blog'),
            array('/contact'),
            // ...
        );
    }

.. index::
   single: Tests; Client

使用测试客户端
----------------------------

测试客户端像浏览器一样模拟HTTP客户端，并向Symfony应用发出请求::

    $crawler = $client->request('GET', '/post/hello-world');

``request()`` 方法将HTTP方法和URL作为参数，并返回一个 ``Crawler`` 实例。

.. tip::

    对请求URL进行硬编码是功能测试的最佳实践。
    如果测试使用Symfony路由生成的URL，则不会检测到可能影响最终用户的应用URL所做的任何更改。

.. _testing-request-method-sidebar:

.. sidebar:: ``request()`` 方法的更多信息：

    ``request()`` 方法的完整签名是::

        request(
            $method,
            $uri,
            array $parameters = array(),
            array $files = array(),
            array $server = array(),
            $content = null,
            $changeHistory = true
        )

    ``server`` 数组是你通常在PHP $_SERVER`_ 超全局中找到的原始值。
    例如，要设置 ``Content-Type`` 和 ``Referer`` HTTP标头，
    你将传递以下内容（请注意非标准标头的 ``HTTP_`` 前缀）::


        $client->request(
            'GET',
            '/post/hello-world',
            array(),
            array(),
            array(
                'CONTENT_TYPE' => 'application/json',
                'HTTP_REFERER' => '/foo/bar',
            )
        );

使用Crawler在响应中查找DOM元素。然后可以使用这些元素单击链接并提交表单::

    $crawler = $client->clickLink('Go elsewhere...');

    $crawler = $client->submitForm('validate', array('name' => 'Fabien'));

``clickLink()`` 和 ``submitForm()`` 方法都返回一个 ``Crawler`` 对象。
这些方法是浏览应用的最佳方式，因为它可以为你处理很多事情，
例如从表单中检测HTTP方法并为你提供良好的API用于上传文件。

.. versionadded:: 4.2
    ``clickLink()`` 和 ``submitForm()`` 方法是在Symfony 4.2中引入的。

``request()`` 方法还可用于直接模拟表单提交或执行更复杂的请求。一些有用的例子::

    // 直接提交表单（但使用Crawler更容易！）
    $client->request('POST', '/submit', array('name' => 'Fabien'));

    // 在请求正文中提交原始JSON字符串
    $client->request(
        'POST',
        '/submit',
        array(),
        array(),
        array('CONTENT_TYPE' => 'application/json'),
        '{"name":"Fabien"}'
    );

    // 带有文件上传的表单提交
    use Symfony\Component\HttpFoundation\File\UploadedFile;

    $photo = new UploadedFile(
        '/path/to/photo.jpg',
        'photo.jpg',
        'image/jpeg',
        null
    );
    $client->request(
        'POST',
        '/submit',
        array('name' => 'Fabien'),
        array('photo' => $photo)
    );

    // 执行DELETE请求并传递HTTP标头
    $client->request(
        'DELETE',
        '/post/12',
        array(),
        array(),
        array('PHP_AUTH_USER' => 'username', 'PHP_AUTH_PW' => 'pa$$word')
    );

最后但同样重要的是，你可以强制每个请求在其自己的PHP进程中执行，以避免在同一脚本中使用多个客户端时出现任何副作用::

    $client->insulate();

AJAX请求
~~~~~~~~~~~~~

客户端提供了一个 :method:`Symfony\\Component\\BrowserKit\\Client::xmlHttpRequest`方法，
该方法与 ``request()`` 方法具有相同的参数，但它是生成AJAX请求的快捷方式::

    // the required HTTP_X_REQUESTED_WITH header is added automatically
    $client->xmlHttpRequest('POST', '/submit', array('name' => 'Fabien'));

.. versionadded:: 4.1
    ``xmlHttpRequest()`` 方法是在Symfony 4.2中引入的。

浏览
~~~~~~~~

客户端支持许多可以在真实浏览器中完成的操作::

    $client->back();
    $client->forward();
    $client->reload();

    // 清除所有cookie和历史记录
    $client->restart();

.. note::

    ``back()`` 和 ``forward()`` 方法就像普通浏览器那样，跳过请求URL时可能发生的重定向。

访问内部对象
~~~~~~~~~~~~~~~~~~~~~~~~~~

如果使用客户端测试应用，则可能需要访问客户端的内部对象::

    $history = $client->getHistory();
    $cookieJar = $client->getCookieJar();

你还可以获取与最后一个请求相关的对象::

    // HttpKernel 请求实例
    $request = $client->getRequest();

    // BrowserKit 请求实例
    $request = $client->getInternalRequest();

    // HttpKernel 响应实例
    $response = $client->getResponse();

    // BrowserKit 响应实例
    $response = $client->getInternalResponse();

    $crawler = $client->getCrawler();

访问容器
~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 4.1
    ``self::$container`` 属性是在Symfony 4.1中引入的。

强烈建议功能测试仅测试响应。但在某些极少数情况下，你可能希望访问某些服务来编写断言。
鉴于默认情况下服务是私有的，测试类定义了一个属性，该属性存储着由Symfony创建的特殊容器，该容器允许获取公共和所有未删除的私有服务::

    // 要访问测试中的同一个的服务，除非你使用 $client->insulate()
    // 或使用真实的HTTP请求来测试你的应用
    $container = self::$container;

有关应用中可用服务的列表，请使用 ``debug:container`` 命令。

.. tip::

    提供对私有服务的访问的特殊容器仅存在于 ``test`` 环境中，并且本身是一个可以使用 ``test.service_container`` id
    从真实容器获取的服务。

.. tip::

    If the information you need to check is available from the profiler, use
    it instead.
    如果你需要检查的信息可以从分析器获得，请改用它。

访问Profiler数据
~~~~~~~~~~~~~~~~~~~~~~~~~~~

在每个请求上，您都可以启用Symfony探查器来收集有关该请求的内部处理的数据。例如，可以使用分析器来验证给定页面在加载时执行少于一定数量的数据库查询。
On each request, you can enable the Symfony profiler to collect data about the
internal handling of that request. For example, the profiler could be used to
verify that a given page executes less than a certain number of database
queries when loading.

要获取上次请求的Profiler，请执行以下操作：
To get the Profiler for the last request, do the following::

    // enables the profiler for the very next request
    // 启用探查器以进行下一个请求
    $client->enableProfiler();

    $crawler = $client->request('GET', '/profiler');

    // gets the profile
    $profile = $client->getProfile();

有关在测试中使用探查器的具体详细信息，请参阅“功能测试”中的“如何使用探查器”一文。
For specific details on using the profiler inside a test, see the
:doc:`/testing/profiling` article.

重定向
~~~~~~~~~~~

当请求返回重定向响应时，客户端不会自动跟踪它。您可以使用followRedirect（）方法检查响应并强制重定向：
When a request returns a redirect response, the client does not follow
it automatically. You can examine the response and force a redirection
afterwards with the ``followRedirect()`` method::

    $crawler = $client->followRedirect();

如果您希望客户端自动遵循所有重定向，您可以在执行请求之前通过调用followRedirects（）方法强制它们：
If you want the client to automatically follow all redirects, you can
force them by calling the ``followRedirects()`` method before performing the request::

    $client->followRedirects();

如果将false传递给followRedirects（）方法，则将不再遵循重定向：
If you pass ``false`` to the ``followRedirects()`` method, the redirects
will no longer be followed::

    $client->followRedirects(false);

报告异常
~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 3.4
    ``catchExceptions()`` 方法是在Symfony 3.4中引入的。

在功能测试中调试异常可能很困难，因为默认情况下它们被捕获，您需要查看日志以查看抛出的异常。
禁用在测试客户端中捕获异常允许PHPUnit报告异常：
Debugging exceptions in functional tests may be difficult because by default
they are caught and you need to look at the logs to see which exception was
thrown. Disabling catching of exceptions in the test client allows the exception
to be reported by PHPUnit::

    $client->catchExceptions(false);

.. index::
   single: Tests; Crawler

.. _testing-crawler:

Crawler
-----------

每次向客户发出请求时，都会返回一个Crawler实例。它允许您遍历HTML文档，选择节点，查找链接和表单。
A Crawler instance is returned each time you make a request with the Client.
It allows you to traverse HTML documents, select nodes, find links and forms.

遍历
~~~~~~~~~~

与jQuery一样，Crawler具有遍历HTML / XML文档的DOM的方法。例如，以下内容查找所有input [type = submit]元素，选择页面上的最后一个元素，然后选择其直接父元素：
Like jQuery, the Crawler has methods to traverse the DOM of an HTML/XML
document. For example, the following finds all ``input[type=submit]`` elements,
selects the last one on the page, and then selects its immediate parent element::

    $newCrawler = $crawler->filter('input[type=submit]')
        ->last()
        ->parents()
        ->first()
    ;

还有许多其他方法：

``filter('h1.title')``
    与CSS选择器匹配的节点。
``filterXpath('h1')``
    与XPath表达式匹配的节点。
``eq(1)``
    指定索引的节点。
``first()``
    第一个节点。
``last()``
    最后一个节点。
``siblings()``
    Siblings.
``nextAll()``
    All following siblings.
``previousAll()``
    All preceding siblings.
``parents()``
    Returns the parent nodes.
``children()``
    Returns children nodes.
``reduce($lambda)``
    Nodes for which the callable does not return false.

Since each of these methods returns a new ``Crawler`` instance, you can
narrow down your node selection by chaining the method calls::

    $crawler
        ->filter('h1')
        ->reduce(function ($node, $i) {
            if (!$node->getAttribute('class')) {
                return false;
            }
        })
        ->first()
    ;

.. tip::

    Use the ``count()`` function to get the number of nodes stored in a Crawler:
    ``count($crawler)``

Extracting Information
~~~~~~~~~~~~~~~~~~~~~~

The Crawler can extract information from the nodes::

    // returns the attribute value for the first node
    $crawler->attr('class');

    // returns the node value for the first node
    $crawler->text();

    // extracts an array of attributes for all nodes
    // (_text returns the node value)
    // returns an array for each element in crawler,
    // each with the value and href
    $info = $crawler->extract(array('_text', 'href'));

    // executes a lambda for each node and return an array of results
    $data = $crawler->each(function ($node, $i) {
        return $node->attr('href');
    });

Links
~~~~~

Use the ``clickLink()`` method to click on the first link that contains the
given text (or the first clickable image with that ``alt`` attribute)::

    $client = static::createClient();
    $client->request('GET', '/post/hello-world');

    $client->clickLink('Click here');

If you need access to the :class:`Symfony\\Component\\DomCrawler\\Link` object
that provides helpful methods specific to links (such as ``getMethod()`` and
``getUri()``), use the ``selectLink()`` method instead:

    $client = static::createClient();
    $crawler = $client->request('GET', '/post/hello-world');

    $link = $crawler->selectLink('Click here')->link();
    $client->click($link);

Forms
~~~~~

Use the ``submitForm()`` method to submit the form that contains the given button::

    $client = static::createClient();
    $client->request('GET', '/post/hello-world');

    $crawler = $client->submitForm('Add comment', array(
       'comment_form[content]' => '...',
    ));

The first argument of ``submitForm()`` is the text content, ``id``, ``value`` or
``name`` of any ``<button>`` or ``<input type="submit">`` included in the form.
The second optional argument is used to override the default form field values.

.. note::

    Notice that you select form buttons and not forms as a form can have several
    buttons; if you use the traversing API, keep in mind that you must look for a
    button.

If you need access to the :class:`Symfony\\Component\\DomCrawler\\Form` object
that provides helpful methods specific to forms (such as ``getUri()``,
``getValues()`` and ``getFields()``) use the ``selectButton()`` method instead::

    $client = static::createClient();
    $crawler = $client->request('GET', '/post/hello-world');

    $buttonCrawlerNode = $crawler->selectButton('submit');

    // select the form that contains this button
    $form = $buttonCrawlerNode->form();

    // you can also pass an array of field values that overrides the default ones
    $form = $buttonCrawlerNode->form(array(
        'my_form[name]'    => 'Fabien',
        'my_form[subject]' => 'Symfony rocks!',
    ));

    // you can pass a second argument to override the form HTTP method
    $form = $buttonCrawlerNode->form(array(), 'DELETE');

    // submit the Form object
    $client->submit($form);

The field values can also be passed as a second argument of the ``submit()``
method::

    $client->submit($form, array(
        'my_form[name]'    => 'Fabien',
        'my_form[subject]' => 'Symfony rocks!',
    ));

For more complex situations, use the ``Form`` instance as an array to set the
value of each field individually::

    // changes the value of a field
    $form['my_form[name]'] = 'Fabien';
    $form['my_form[subject]'] = 'Symfony rocks!';

There is also a nice API to manipulate the values of the fields according to
their type::

    // selects an option or a radio
    $form['country']->select('France');

    // ticks a checkbox
    $form['like_symfony']->tick();

    // uploads a file
    $form['photo']->upload('/path/to/lucas.jpg');

.. tip::

    If you purposefully want to select "invalid" select/radio values, see
    :ref:`components-dom-crawler-invalid`.

.. tip::

    You can get the values that will be submitted by calling the ``getValues()``
    method on the ``Form`` object. The uploaded files are available in a
    separate array returned by ``getFiles()``. The ``getPhpValues()`` and
    ``getPhpFiles()`` methods also return the submitted values, but in the
    PHP format (it converts the keys with square brackets notation - e.g.
    ``my_form[subject]`` - to PHP arrays).

.. tip::

    The ``submit()`` and ``submitForm()`` methods define optional arguments to
    add custom server parameters and HTTP headers when submitting the form::

        $client->submit($form, array(), array('HTTP_ACCEPT_LANGUAGE' => 'es'));
        $client->submitForm($button, array(), 'POST', array('HTTP_ACCEPT_LANGUAGE' => 'es'));

    .. versionadded:: 4.1
        The feature to add custom HTTP headers was introduced in Symfony 4.1.

Adding and Removing Forms to a Collection
.........................................

If you use a :doc:`Collection of Forms </form/form_collections>`,
you can't add fields to an existing form with
``$form['task[tags][0][name]'] = 'foo';``. This results in an error
``Unreachable field "…"`` because ``$form`` can only be used in order to
set values of existing fields. In order to add new fields, you have to
add the values to the raw data array::

    // gets the form
    $form = $crawler->filter('button')->form();

    // gets the raw values
    $values = $form->getPhpValues();

    // adds fields to the raw values
    $values['task']['tags'][0]['name'] = 'foo';
    $values['task']['tags'][1]['name'] = 'bar';

    // submits the form with the existing and new values
    $crawler = $client->request($form->getMethod(), $form->getUri(), $values,
        $form->getPhpFiles());

    // the 2 tags have been added to the collection
    $this->assertEquals(2, $crawler->filter('ul.tags > li')->count());

Where ``task[tags][0][name]`` is the name of a field created
with JavaScript.

You can remove an existing field, e.g. a tag::

    // gets the values of the form
    $values = $form->getPhpValues();

    // removes the first tag
    unset($values['task']['tags'][0]);

    // submits the data
    $crawler = $client->request($form->getMethod(), $form->getUri(),
        $values, $form->getPhpFiles());

    // the tag has been removed
    $this->assertEquals(0, $crawler->filter('ul.tags > li')->count());

.. index::
   pair: Tests; Configuration

Testing Configuration
---------------------

The Client used by functional tests creates a Kernel that runs in a special
``test`` environment. Since Symfony loads the ``config/packages/test/*.yaml``
in the ``test`` environment, you can tweak any of your application's settings
specifically for testing.

For example, by default, the Swift Mailer is configured to *not* actually
deliver emails in the ``test`` environment. You can see this under the ``swiftmailer``
configuration option:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/test/swiftmailer.yaml

        # ...
        swiftmailer:
            disable_delivery: true

    .. code-block:: xml

        <!-- config/packages/test/swiftmailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <!-- ... -->
            <swiftmailer:config disable-delivery="true" />
        </container>

    .. code-block:: php

        // config/packages/test/swiftmailer.php

        // ...
        $container->loadFromExtension('swiftmailer', array(
            'disable_delivery' => true,
        ));

You can also use a different environment entirely, or override the default
debug mode (``true``) by passing each as options to the ``createClient()``
method::

    $client = static::createClient(array(
        'environment' => 'my_test_env',
        'debug'       => false,
    ));

If your application behaves according to some HTTP headers, pass them as the
second argument of ``createClient()``::

    $client = static::createClient(array(), array(
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ));

You can also override HTTP headers on a per request basis::

    $client->request('GET', '/', array(), array(), array(
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ));

.. tip::

    The test client is available as a service in the container in the ``test``
    environment (or wherever the :ref:`framework.test <reference-framework-test>`
    option is enabled). This means you can override the service entirely
    if you need to.

.. index::
   pair: PHPUnit; Configuration

PHPUnit Configuration
~~~~~~~~~~~~~~~~~~~~~

Each application has its own PHPUnit configuration, stored in the
``phpunit.xml.dist`` file. You can edit this file to change the defaults or
create a ``phpunit.xml`` file to set up a configuration for your local machine
only.

.. tip::

    Store the ``phpunit.xml.dist`` file in your code repository and ignore
    the ``phpunit.xml`` file.

By default, only the tests stored in ``tests/`` are run via the ``phpunit`` command,
as configured in the ``phpunit.xml.dist`` file:

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <phpunit>
        <!-- ... -->
        <testsuites>
            <testsuite name="Project Test Suite">
                <directory>tests</directory>
            </testsuite>
        </testsuites>
        <!-- ... -->
    </phpunit>

But you can easily add more directories. For instance, the following
configuration adds tests from a custom ``lib/tests`` directory:

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <phpunit>
        <!-- ... -->
        <testsuites>
            <testsuite name="Project Test Suite">
                <!-- ... --->
                <directory>lib/tests</directory>
            </testsuite>
        </testsuites>
        <!-- ... -->
    </phpunit>

To include other directories in the code coverage, also edit the ``<filter>``
section:

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <phpunit>
        <!-- ... -->
        <filter>
            <whitelist>
                <!-- ... -->
                <directory>lib</directory>
                <exclude>
                    <!-- ... -->
                    <directory>lib/tests</directory>
                </exclude>
            </whitelist>
        </filter>
        <!-- ... -->
    </phpunit>

Learn more
----------

.. toctree::
    :maxdepth: 1
    :glob:

    testing/*

* :ref:`Testing a console command <console-testing-commands>`
* :doc:`The chapter about tests in the Symfony Framework Best Practices </best_practices/tests>`
* :doc:`/components/dom_crawler`
* :doc:`/components/css_selector`

.. _`PHPUnit`: https://phpunit.de/
.. _`文档`: https://phpunit.readthedocs.io/
.. _`PHPUnit Bridge组件`: https://symfony.com/components/PHPUnit%20Bridge
.. _`$_SERVER`: https://php.net/manual/en/reserved.variables.server.php
.. _`data providers`: https://phpunit.de/manual/current/en/writing-tests-for-phpunit.html#writing-tests-for-phpunit.data-providers
