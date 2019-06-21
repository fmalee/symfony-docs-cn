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

每个测试 - 无论是单元测试还是功能测试 - 都是一个PHP类，应该存在于应用的 ``tests/`` 目录中。
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
如果要测试应用的整体行为，请参阅有关功 :ref:`功能测试 <functional-tests>` 的章节。

编写Symfony单元测试与编写标准PHPUnit单元测试没有什么不同。
例如，假设你在app bundle的 `src/Util/`` 目录中有一个名为 ``Calculator`` 的非常简单的类::

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
（默认情况下在 ``phpunit.xml.dist`` 文件中配置）。

你还可以将测试运行限制在目录或特定测试文件：

.. code-block:: terminal

    # 运行应用的所有测试
    $ php bin/phpunit

    # 运行 Util/ 目录的所有测试
    $ php bin/phpunit tests/Util

    # 运行 Calculator 类的测试
    $ php bin/phpunit tests/Util/CalculatorTest.php

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
    内核类通常在 ``KERNEL_CLASS`` 环境变量中定义（包含在Symfony提供的默认 ``.env.test`` 文件中）：

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

现在，你可以将CSS选择器与Crawler一起使用。要断言短语“Hello World”至少在页面显示上一次，你可以使用此断言::

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
使用Crawler在DOM上进行断言::

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

        // 断言至少有一个带有“subtitle”类的h2标签
        $this->assertGreaterThan(
            0,
            $crawler->filter('h2.subtitle')->count()
        );

        // 断言页面上只有4个h2标签
        $this->assertCount(4, $crawler->filter('h2'));

        // 断言“Content-Type”标头是“application/json”
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
        return [
            ['/'],
            ['/blog'],
            ['/contact'],
            // ...
        ];
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

.. sidebar:: ``request()`` 方法的更多信息

    ``request()`` 方法的完整签名是::

        request(
            $method,
            $uri,
            array $parameters = [],
            array $files = [],
            array $server = [],
            $content = null,
            $changeHistory = true
        )

    ``server`` 数组是你通常在PHP `$_SERVER`_ 超全局中找到的原始值。
    例如，要设置 ``Content-Type`` 和 ``Referer`` HTTP标头，
    你将传递以下内容（请注意非标准标头的 ``HTTP_`` 前缀）::

        $client->request(
            'GET',
            '/post/hello-world',
            [],
            [],
            [
                'CONTENT_TYPE' => 'application/json',
                'HTTP_REFERER' => '/foo/bar',
            ]
        );

使用Crawler在响应中查找DOM元素。然后可以使用这些元素单击链接并提交表单::

    $crawler = $client->clickLink('Go elsewhere...');

    $crawler = $client->submitForm('validate', ['name' => 'Fabien']);

``clickLink()`` 和 ``submitForm()`` 方法都返回一个 ``Crawler`` 对象。
这些方法是浏览应用的最佳方式，因为它可以为你处理很多事情，
例如从表单中检测HTTP方法并为你提供良好的API用于上传文件。

``request()`` 方法还可用于直接模拟表单提交或执行更复杂的请求。一些有用的例子::

    // 直接提交表单（但使用Crawler更容易！）
    $client->request('POST', '/submit', ['name' => 'Fabien']);

    // 在请求正文中提交原始JSON字符串
    $client->request(
        'POST',
        '/submit',
        [],
        [],
        ['CONTENT_TYPE' => 'application/json'],
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
        ['name' => 'Fabien'],
        ['photo' => $photo]
    );

    // 执行DELETE请求并传递HTTP标头
    $client->request(
        'DELETE',
        '/post/12',
        [],
        [],
        ['PHP_AUTH_USER' => 'username', 'PHP_AUTH_PW' => 'pa$$word']
    );

最后但同样重要的是，你可以强制每个请求在其自己的PHP进程中执行，以避免在同一脚本中使用多个客户端时出现任何副作用::

    $client->insulate();

AJAX请求
~~~~~~~~~~~~~

客户端提供了一个 :method:`Symfony\\Component\\BrowserKit\\Client::xmlHttpRequest` 方法，
该方法与 ``request()`` 方法具有相同的参数，但它是生成AJAX请求的快捷方式::

    // 自动添加所需的HTTP_X_REQUESTED_WITH标头
    $client->xmlHttpRequest('POST', '/submit', ['name' => 'Fabien']);

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

    如果你需要检查的信息可以从分析器获得，请改用它。

访问分析器数据
~~~~~~~~~~~~~~~~~~~~~~~~~~~

在每个请求上，你都可以启用Symfony分析器来收集有关该请求的内部处理的数据。
例如，可以使用分析器来验证给定页面在加载时执行少于一定数量的数据库查询。

要获取最后一次请求的分析器，请执行以下操作::

    // 启用分析器以进行下一个请求
    $client->enableProfiler();

    $crawler = $client->request('GET', '/profiler');

    // 获取资料
    $profile = $client->getProfile();

有关在测试中使用分析器的具体详细信息，请参阅 :doc:`/testing/profiling` 文档。

重定向
~~~~~~~~~~~

当请求返回重定向响应时，客户端并不会自动跟踪它。
你可以使用 ``followRedirect()`` 方法检查响应并强制重定向::

    $crawler = $client->followRedirect();

如果你希望客户端自动遵循所有重定向，你可以在执行请求之前通过调用 ``followRedirects()`` 方法强制启动::

    $client->followRedirects();

如果将 ``false`` 传递给 ``followRedirects()`` 方法，则将不再遵循重定向::

    $client->followRedirects(false);

报告异常
~~~~~~~~~~~~~~~~~~~~

在功能测试中调试异常可能很困难，因为默认情况下它们会被捕获，你需要检查日志以查看抛出的异常。
禁用在测试客户端中捕获异常，以允许PHPUnit报告异常::

    $client->catchExceptions(false);

.. index::
   single: Tests; Crawler

.. _testing-crawler:

Crawler
-----------

每次向客户端发出请求时，都会返回一个Crawler实例。它允许你遍历HTML文档、选择节点、查找链接和表单。

遍历
~~~~~~~~~~

与jQuery一样，Crawler具有遍历HTML/XML文档的DOM的方法。
例如，以下内容查找所有 ``input[type=submit]`` 元素，选择页面上的最后一个元素，然后选择其直接父元素::

    $newCrawler = $crawler->filter('input[type=submit]')
        ->last()
        ->parents()
        ->first()
    ;

还有许多其他方法：

``filter('h1.title')``
    与CSS选择器匹配的节点
``filterXpath('h1')``
    与XPath表达式匹配的节点
``eq(1)``
    指定索引的节点
``first()``
    第一个节点
``last()``
    最后一个节点
``siblings()``
    相邻同胞
``nextAll()``
    后面的所有相邻同胞
``previousAll()``
    前面的所有相邻同胞
``parents()``
    返回父节点
``children()``
    返回子节点
``reduce($lambda)``
    可调用对象不返回false的节点

由于这些方法中的每一个都返回一个新的Crawler实例，因此你可以通过方法链调用来缩小节点的选择范围::

    $crawler
        ->filter('h1')
        ->reduce(function ($node, $i) {
            if (!$node->attr('class')) {
                return false;
            }
        })
        ->first()
    ;

.. tip::

    使用 ``count()`` 函数获取Crawler中存储的节点总数：``count($crawler)``

提取信息
~~~~~~~~~~~~~~~~~~~~~~

Crawler可以从节点中提取信息::

    // 返回第一个节点的属性值
    $crawler->attr('class');

    // 返回第一个节点的节点值
    $crawler->text();

    // 提取所有节点的属性数组
    // （_text返回节点值）
    // 为crawler中的每个元素返回一个数组，每个元素都带有值和href
    $info = $crawler->extract(['_text', 'href']);

    // 为每个节点执行一个lambda并返回一个结果数组
    $data = $crawler->each(function ($node, $i) {
        return $node->attr('href');
    });

链接
~~~~~

使用 ``clickLink（）`` 方法点击包含给定文本的第一个链接（或带有 ``alt`` 属性的第一个可点击图像）::

    $client = static::createClient();
    $client->request('GET', '/post/hello-world');

    $client->clickLink('Click here');

如果需要访问 :class:`Symfony\\Component\\DomCrawler\\Link` 对象，它提供了特定于链接的有用方法
（例如 ``getMethod()`` 和 ``getUri()``），请使用 ``selectLink()`` 方法::

    $client = static::createClient();
    $crawler = $client->request('GET', '/post/hello-world');

    $link = $crawler->selectLink('Click here')->link();
    $client->click($link);

表单
~~~~~

使用 ``submitForm()`` 方法提交包含给定按钮的表单::

    $client = static::createClient();
    $client->request('GET', '/post/hello-world');

    $crawler = $client->submitForm('Add comment', [
        'comment_form[content]' => '...',
    ]);

``submitForm()`` 的第一个参数是表单中任何 ``<button>`` 或 ``<input type="submit">`` 的
``id``、``value``、``name`` 等属性的值。
第二个可选参数用于覆盖表单默认的字段值。

.. note::

    请注意选择表单按钮而不是表单，因为表单可以有多个按钮;如果你使用遍历(traversing)API，请记住你必须查找按钮。

如果需要访问提供特定于表单的诸如 ``getUri()``、``getValues()``、``getFields()`` 等有用方法的
:class:`Symfony\\Component\\DomCrawler\\Form` 对象，请使用 ``selectButton()`` 方法::

    $client = static::createClient();
    $crawler = $client->request('GET', '/post/hello-world');

    $buttonCrawlerNode = $crawler->selectButton('submit');

    // 选择包含此按钮的表单
    $form = $buttonCrawlerNode->form();

    // 你还可以传递一组用来覆盖默认值的字段值
    $form = $buttonCrawlerNode->form([
        'my_form[name]'    => 'Fabien',
        'my_form[subject]' => 'Symfony rocks!',
    ]);

    // 你可以传递第二个参数来覆盖表单的HTTP方法
    $form = $buttonCrawlerNode->form([], 'DELETE');

    // 提交表单对象
    $client->submit($form);

字段值也可以作为 ``submit()`` 方法的第二个参数传递::

    $client->submit($form, [
        'my_form[name]'    => 'Fabien',
        'my_form[subject]' => 'Symfony rocks!',
    ]);

对于更复杂的情况，使用 ``Form`` 实例作为数组来单独设置每个字段的值::

    // 更改字段的值
    $form['my_form[name]'] = 'Fabien';
    $form['my_form[subject]'] = 'Symfony rocks!';

还有一个很好的API可以根据字段类型来操作字段的值::

    // 选择一个选项或 radio
    $form['country']->select('France');

    // 勾选一个复选框
    $form['like_symfony']->tick();

    // 上传文件
    $form['photo']->upload('/path/to/lucas.jpg');

    // 在一个多文件上传的情况下
    $form['my_form[field][O]']->upload('/path/to/lucas.jpg');
    $form['my_form[field][1]']->upload('/path/to/lisa.jpg');

.. tip::

    如果你特意要选择“无效”的选择框和单选框，请参阅 :ref:`components-dom-crawler-invalid`。

.. tip::

    你可以通过在 ``Form`` 对象上调用 ``getValues()`` 方法来获取将要提交的值。
    上传的文件在 ``getFiles()`` 方法返回的单独数组中。
    ``getPhpValues()`` 和 ``getPhpFiles()`` 方法也可返回提交的值，但是以PHP格式
    （它将带有方括号表示法的键 - 例如 ``my_form[subject]`` - 转换为PHP数组）返回。

.. tip::

    ``submit()`` 和 ``submitForm()`` 方法可以通过定义可选参数，在提交表单时添加自定义服务器参数和HTTP标头::

        $client->submit($form, [], ['HTTP_ACCEPT_LANGUAGE' => 'es']);
        $client->submitForm($button, [], 'POST', ['HTTP_ACCEPT_LANGUAGE' => 'es']);

添加/删​​除表单到一个集合
.........................................

如果你使用一个 :doc:`表单集合 </form/form_collections>`，则无法使用
``$form['task[tags][0][name]'] = 'foo';`` 将字段添加到现有表单中。
这会导致 ``Unreachable field "…"`` 错误，因为 ``$form`` 只能用于设置现有字段的值。
要添加新字段，你必须将值添加到原始数据数组::

    //获取表单
    $form = $crawler->filter('button')->form();

    // 获取原始值
    $values = $form->getPhpValues();

    // 将字段添加到原始值
    $values['task']['tags'][0]['name'] = 'foo';
    $values['task']['tags'][1]['name'] = 'bar';

    // 使用现有值和新值提交表单
    $crawler = $client->request($form->getMethod(), $form->getUri(), $values,
        $form->getPhpFiles());

    // 2个标签已添加到集合中
    $this->assertEquals(2, $crawler->filter('ul.tags > li')->count());

其中 ``task[tags][0][name]`` 是使用JavaScript创建的字段的名称。

你可以删除现有字段，例如标签::

    // 获取表单的值
    $values = $form->getPhpValues();

    // 删除第一个标签
    unset($values['task']['tags'][0]);

    // 提交数据
    $crawler = $client->request($form->getMethod(), $form->getUri(),
        $values, $form->getPhpFiles());

    // 标签已被删除
    $this->assertEquals(0, $crawler->filter('ul.tags > li')->count());

.. index::
   pair: Tests; Configuration

测试配置
---------------------

功能测试使用的客户端创建了一个在特定 ``test`` 环境中运行的内核。
由于Symfony在 ``test`` 环境中加载了 ``config/packages/test/*.yaml``，
因此你可以调整应用的任何设置，专门用于测试。

例如，默认情况下，Swift Mailer配置为 *不* 在 ``test`` 环境中实际传递电子邮件。
你可以在 ``swiftmailer`` 配置选项下看到这个：

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
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                https://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <!-- ... -->
            <swiftmailer:config disable-delivery="true"/>
        </container>

    .. code-block:: php

        // config/packages/test/swiftmailer.php

        // ...
        $container->loadFromExtension('swiftmailer', [
            'disable_delivery' => true,
        ]);

你当然完全可以使用不同的环境，或者通过将每个配置作为选项传递给 ``createClient()``
方法来覆盖默认的调试模式（``true``）::

    $client = static::createClient([
        'environment' => 'my_test_env',
        'debug'       => false,
    ]);

自定义数据库URL/环境变量
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你需要为测试自定义一些环境变量（例如Doctrine使用的 ``DATABASE_URL``），你可以通过重写
``.env.test`` 文件中任何需要的内容来实现：

.. code-block:: text

    # .env.test
    DATABASE_URL="mysql://db_user:db_password@127.0.0.1:3306/db_name_test"

    # use SQLITE
    # DATABASE_URL="sqlite:///%kernel.project_dir%/var/app.db"

该文件在 ``test`` 环境中自动读取：这里的任何键都覆盖 ``.env`` 中的默认值。

.. caution::

    在2018年11月之前创建的应用有一个稍微不同的系统，涉及一个 ``.env.dist`` 文件。
    有关升级的信息，请参阅 :doc:`configuration/dot-env-changes`。

发送自定义标头
~~~~~~~~~~~~~~~~~~~~~~

如果你的应用的行为根据某些HTTP标头，请将它们作为 ``createClient()`` 的第二个参数传递::

    $client = static::createClient([], [
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ]);

你还可以基于每个请求覆盖HTTP标头::

    $client->request('GET', '/', [], [], [
        'HTTP_HOST'       => 'en.example.com',
        'HTTP_USER_AGENT' => 'MySuperBrowser/1.0',
    ]);

.. tip::

    测试客户端可作为 ``test`` 环境中容器中的一个服务
    （或启用 :ref:`framework.test <reference-framework-test>` 选项来在任何位置生效）。
    这意味着如果需要，你可以完全覆盖该服务。

.. index::
   pair: PHPUnit; Configuration

PHPUnit配置
~~~~~~~~~~~~~~~~~~~~~

每个应用都有自己的PHPUnit配置，存储在 ``phpunit.xml.dist`` 文件中。
你可以编辑此文件以更改默认值或创建 ``phpunit.xml`` 文件以仅为本地计算机设置一个配置。

.. tip::

    将 ``phpunit.xml.dist`` 文件存储在代码仓库库中，并忽略 ``phpunit.xml`` 文件。

默认情况下，只有存储在 ``tests/`` 中的测试可以通过 ``phpunit`` 命令运行，
如 ``phpunit.xml.dist`` 文件中的配置：

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

但是你可以添加更多目录。例如，以下配置添加了一个自定义 ``lib/tests`` 目录到测试：

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

要在代码覆盖率中包含其他目录，还要编辑 ``<filter>`` 部分：

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

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    testing/*
    /best_practices/tests
    /components/dom_crawler
    /components/css_selector

.. _`PHPUnit`: https://phpunit.de/
.. _`文档`: https://phpunit.readthedocs.io/
.. _`PHPUnit Bridge组件`: https://symfony.com/components/PHPUnit%20Bridge
.. _`$_SERVER`: https://php.net/manual/en/reserved.variables.server.php
.. _`data providers`: https://phpunit.de/manual/current/en/writing-tests-for-phpunit.html#writing-tests-for-phpunit.data-providers
