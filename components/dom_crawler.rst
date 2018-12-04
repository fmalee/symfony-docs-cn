.. index::
   single: DomCrawler
   single: Components; DomCrawler

DomCrawler组件
========================

    DomCrawler组件简化了HTML和XML文档的DOM导航。

.. note::

    尽管可以做到，但DomCrawler组件并非设计来用于操纵DOM或重新转储(re-dumping)HTML/XML。

安装
------------

.. code-block:: terminal

    $ composer require symfony/dom-crawler

或者，你也可以克隆 `<https://github.com/symfony/dom-crawler>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

用法
-----

.. seealso::

    本文介绍如何在任何PHP应用中将DomCrawler功能用作独立组件。
    阅读 :ref:`Symfony功能测试 <functional-tests>` 文档，以了解如何在创建Symfony测试时使用它。

:class:`Symfony\\Component\\DomCrawler\\Crawler` 类提供了查询和操作HTML和XML文档的方法。

Crawler的一个实例代表一组 :phpclass:`DOMElement` 对象，它们是可以轻松遍历的基本节点::

    use Symfony\Component\DomCrawler\Crawler;

    $html = <<<'HTML'
    <!DOCTYPE html>
    <html>
        <body>
            <p class="message">Hello World!</p>
            <p>Hello Crawler!</p>
        </body>
    </html>
    HTML;

    $crawler = new Crawler($html);

    foreach ($crawler as $domElement) {
        var_dump($domElement->nodeName);
    }

:class:`Symfony\\Component\\DomCrawler\\Link`、
:class:`Symfony\\Component\\DomCrawler\\Image` 和
:class:`Symfony\\Component\\DomCrawler\\Form` 类专门用于通过HTML树来遍历html链接、图片、表单。

.. note::

    DomCrawler将尝试自动修复你的HTML以符合官方规范。
    例如，如果将 ``<p>`` 标签嵌套在另一个 ``<p>`` 标签内，它将被移动为父标签的兄弟(sibling)。
    这是预期的，也是HTML5规范的一部分。但如果你获得意料之外的行为，这可能是一个原因。
    而且，尽管DomCrawler并不意味着转储(dump)的内容，但你可以通过
    :ref:`转储它 <component-dom-crawler-dumping>` 以看到你的HTML的“修复”版本。

节点过滤
~~~~~~~~~~~~~~

使用XPath表达式，你可以选择文档中的特定节点::

    $crawler = $crawler->filterXPath('descendant-or-self::body/p');

.. tip::

    ``DOMXPath::query`` 在内部用于实际执行一个XPath查询。

如果你更喜欢CSS选择器而不是XPath，请安装CssSelector组件。
该组件允许你使用类似jQuery的选择器来进行遍历::

    $crawler = $crawler->filter('body > p');

匿名函数可用于过滤更复杂的条件::

    use Symfony\Component\DomCrawler\Crawler;
    // ...

    $crawler = $crawler
        ->filter('body > p')
        ->reduce(function (Crawler $node, $i) {
            // 过滤每个节点
            return ($i % 2) == 0;
        });

要删除一个节点，该匿名函数必须返回 ``false``。

.. note::

    所有的过滤器方法都返回一个带有过滤内容的新的
    :class:`Symfony\\Component\\DomCrawler\\Crawler` 实例。

无论是
:method:`Symfony\\Component\\DomCrawler\\Crawler::filterXPath`
还是 :method:`Symfony\\Component\\DomCrawler\\Crawler::filter`
方法都可以自动发现或明确注册XML命令空间。

思考下面的XML：

.. code-block:: xml

    <?xml version="1.0" encoding="UTF-8"?>
    <entry
        xmlns="http://www.w3.org/2005/Atom"
        xmlns:media="http://search.yahoo.com/mrss/"
        xmlns:yt="http://gdata.youtube.com/schemas/2007"
    >
        <id>tag:youtube.com,2008:video:kgZRZmEc9j4</id>
        <yt:accessControl action="comment" permission="allowed"/>
        <yt:accessControl action="videoRespond" permission="moderated"/>
        <media:group>
            <media:title type="plain">Chordates - CrashCourse Biology #24</media:title>
            <yt:aspectRatio>widescreen</yt:aspectRatio>
        </media:group>
    </entry>

该XML可以使用 ``Crawler`` 的
:method:`Symfony\\Component\\DomCrawler\\Crawler::filterXPath`
方法进行过滤，而无需注册命名空间别名::

    $crawler = $crawler->filterXPath('//default:entry/media:group//yt:aspectRatio');

以及 :method:`Symfony\\Component\\DomCrawler\\Crawler::filter` 方法::

    $crawler = $crawler->filter('default|entry media|group yt|aspectRatio');

.. note::

    默认命名空间使用一个“default”前缀进行注册。它可以通过使用
    :method:`Symfony\\Component\\DomCrawler\\Crawler::setDefaultNamespacePrefix`
    方法来改变。

    如果内容是文档中唯一的命名空间，则在加载该内容时将删除默认命名空间。这样做是为了简化xpath查询。

可以使用 :method:`Symfony\\Component\\DomCrawler\\Crawler::registerNamespace`
方法显式注册命名空间::

    $crawler->registerNamespace('m', 'http://search.yahoo.com/mrss/');
    $crawler = $crawler->filterXPath('//m:group//yt:aspectRatio');

节点遍历
~~~~~~~~~~~~~~~

按节点在列表中的位置访问::

    $crawler->filter('body > p')->eq(0);

获取当前选择的第一个或最后一个节点::

    $crawler->filter('body > p')->first();
    $crawler->filter('body > p')->last();

获取与当前选择的同级别的节点::

    $crawler->filter('body > p')->siblings();

获取在当前选择之前或之后的同级别的节点::

    $crawler->filter('body > p')->nextAll();
    $crawler->filter('body > p')->previousAll();

获取所有子节点或父节点::

    $crawler->filter('body')->children();
    $crawler->filter('body > p')->parents();

获取与一个CSS选择器匹配的所有直接子节点::

    $crawler->filter('body')->children('p.lorem');

.. versionadded:: 4.2
    ``children($selector)`` 方法中的可选选择器是在Symfony 4.2中引入的。

.. note::

    所有遍历方法都返回一个新的 :class:`Symfony\\Component\\DomCrawler\\Crawler` 实例。

访问节点值
~~~~~~~~~~~~~~~~~~~~~

访问当前选择的第一个节点的节点名称（HTML标记名称，例如“p”或“div”）::

    // 返回 <body> 之下的第一个子元素的节点名称（HTML标记名称）
    $tag = $crawler->filterXPath('//body/*')->nodeName();

访问当前选择的第一个节点的值::

    $message = $crawler->filterXPath('//body/p')->text();

访问当前选择的第一个节点的属性值::

    $class = $crawler->filterXPath('//body/p')->attr('class');

从节点列表中提取属性值和节点值::

    $attributes = $crawler
        ->filterXpath('//body/p')
        ->extract(array('_name', '_text', 'class'))
    ;

.. note::

    ``_text`` 特殊属性代表一个节点值。而 ``_name`` 表示元素名称（HTML标签名称）。

.. versionadded:: 4.3
    在Symfony4.3中引入了 ``_name`` 特殊属性。

在列表的每个节点上调用一个匿名函数::

    use Symfony\Component\DomCrawler\Crawler;
    // ...

    $nodeValues = $crawler->filter('p')->each(function (Crawler $node, $i) {
        return $node->text();
    });

匿名函数接收节点（作为Crawler）和节点位置作为参数。结果是调用该匿名函数返回的数组形式的值。

添加内容
~~~~~~~~~~~~~~~~~~

Crawler支持多种添加内容的方式::

    $crawler = new Crawler('<html><body /></html>');

    $crawler->addHtmlContent('<html><body /></html>');
    $crawler->addXmlContent('<root><node /></root>');

    $crawler->addContent('<html><body /></html>');
    $crawler->addContent('<root><node /></root>', 'text/xml');

    $crawler->add('<html><body /></html>');
    $crawler->add('<root><node /></root>');

.. note::

    :method:`Symfony\\Component\\DomCrawler\\Crawler::addHtmlContent` 和
    :method:`Symfony\\Component\\DomCrawler\\Crawler::addXmlContent`
    方法默认为UTF-8编码，但你可以通过它们的第二个可选参数改变此行为。

    :method:`Symfony\\Component\\DomCrawler\\Crawler::addContent`
    方法根据给定的内容猜测最佳字符集，如果不能猜出字符集则默认设置为 ``ISO-8859-1``。

由于Crawler是基于DOM扩展实现的，所以它也能与原生(native)的 :phpclass:`DOMDocument`、
:phpclass:`DOMNodeList` 和 :phpclass:`DOMNode` 对象交互::

    $domDocument = new \DOMDocument();
    $domDocument->loadXml('<root><node /><node /></root>');
    $nodeList = $domDocument->getElementsByTagName('node');
    $node = $domDocument->getElementsByTagName('node')->item(0);

    $crawler->addDocument($domDocument);
    $crawler->addNodeList($nodeList);
    $crawler->addNodes(array($node));
    $crawler->addNode($node);
    $crawler->add($domDocument);

.. _component-dom-crawler-dumping:

.. sidebar:: Manipulating and Dumping a ``Crawler``

    ``Crawler`` 上的这些方法最初是为了填充你的 ``Crawler``，而不是用来进一步操作DOM（虽然这是可行的）。
    但是，由于Crawler是一组 :phpclass:`DOMElement` 对象，你可以使用
    :phpclass:`DOMElement`、:phpclass:`DOMNode` 以及 :phpclass:`DOMDocument`
    上的任何可用的方法或属性。
    例如，你可以使用以下内容来获取一个 ``Crawler`` 的HTML::

        $html = '';

        foreach ($crawler as $domElement) {
            $html .= $domElement->ownerDocument->saveHTML($domElement);
        }

    或者你可以使用 :method:`Symfony\\Component\\DomCrawler\\Crawler::html`
    方法获取第一个节点的HTML::

        $html = $crawler->html();

表达式求值
~~~~~~~~~~~~~~~~~~~~~

``evaluate()`` 方法对给定的XPath表达式进行求值。返回值取决于该XPath表达式。
如果对一个标量值（例如HTML属性）进行表达式求值，则结果将返回一个数组。
如果对一个DOM文档进行表达式求值，则将返回一个新的 ``Crawler`` 实例。

以下示例最好地说明了此行为::

    use Symfony\Component\DomCrawler\Crawler;

    $html = '<html>
    <body>
        <span id="article-100" class="article">Article 1</span>
        <span id="article-101" class="article">Article 2</span>
        <span id="article-102" class="article">Article 3</span>
    </body>
    </html>';

    $crawler = new Crawler();
    $crawler->addHtmlContent($html);

    $crawler->filterXPath('//span[contains(@id, "article-")]')->evaluate('substring-after(@id, "-")');
    /* array:3 [
         0 => "100"
         1 => "101"
         2 => "102"
       ]
     */

    $crawler->evaluate('substring-after(//span[contains(@id, "article-")]/@id, "-")');
    /* array:1 [
         0 => "100"
       ]
     */

    $crawler->filterXPath('//span[@class="article"]')->evaluate('count(@id)');
    /* array:3 [
         0 => 1.0
         1 => 1.0
         2 => 1.0
       ]
     */

    $crawler->evaluate('count(//span[@class="article"])');
    /* array:1 [
         0 => 3.0
       ]
     */

    $crawler->evaluate('//span[1]');
    // A Symfony\Component\DomCrawler\Crawler instance

链接
~~~~~

要按名称查找链接（或按其 ``alt`` 属性搜索可点击的图像），请在现有Crawler上使用 ``selectLink()`` 方法。
这将返回一个仅包含所选链接的 ``Crawler`` 实例。
调用 ``link()`` 将为你提供一个特殊的 :class:`Symfony\\Component\\DomCrawler\\Link` 对象::

    $linksCrawler = $crawler->selectLink('Go elsewhere...');
    $link = $linksCrawler->link();

    // 或者一次完成这一切
    $link = $crawler->selectLink('Go elsewhere...')->link();

:class:`Symfony\\Component\\DomCrawler\\Link` 对象有几种有用的方法，可以用来获取有关所选链接本身的更多信息::

    // 返回可用于创建其他请求的正确URI
    $uri = $link->getUri();

.. note::

    ``getUri（）`` 特别有用，因为它会清理 ``href`` 值并将其转换为实际需要的样子。
    例如，对于带有 ``href="#foo"`` 的链接，这将返回附带 ``#foo`` 后缀的当前页面的完整URI。
    ``getUri()`` 返回的始终是你可以执行的完整URI。

图片
~~~~~~

要按其 ``alt`` 属性查找一个图像，请在现有Crawler上使用 ``selectImage()`` 方法。
这将返回一个仅包含所选图像的 ``Crawler`` 实例。
调用 ``image()`` 将为你提供一个特殊的 :class:`Symfony\\Component\\DomCrawler\\Image` 对象::

    $imagesCrawler = $crawler->selectImage('Kitten');
    $image = $imagesCrawler->image();

    // 或者一次完成这一切
    $image = $crawler->selectImage('Kitten')->image();

:class:`Symfony\\Component\\DomCrawler\\Image` 对象具有与
:class:`Symfony\\Component\\DomCrawler\\Link` 相同的 ``getUri()`` 方法。

表单
~~~~~

表单也有特殊待遇。Crawler上有一个 ``selectButton()`` 方法，它返回另一个匹配
``<button>``、``<input type="submit">`` 或 ``<input type="button">``
元素（或它们中的 ``<img>`` 元素）的Crawler。
作为参数的给定字符串会被用于查找 ``id``、``alt``、``name`` 和 ``value`` 等属性以及这些元素的文本内容。

此方法特别有用，因为你可以使用它来返回一个代表按钮所在表单的
:class:`Symfony\\Component\\DomCrawler\\Form` 对象::

    // 实例按钮: <button id="my-super-button" type="submit">My super button</button>

    // 你可以通过它的标签来获得按钮
    $form = $crawler->selectButton('My super button')->form();

    // 如果该按钮没有标签，则可以依据该按钮的ID来查找
    $form = $crawler->selectButton('my-super-button')->form();

    // 或者你可以过滤整个表单，例如该表单具有一个类属性：<form class =“form-vertical”method =“POST”>
    $crawler->filter('.form-vertical')->form();

    // 或者用数据“填充”表单字段
    $form = $crawler->selectButton('my-super-button')->form(array(
        'name' => 'Ryan',
    ));

:class:`Symfony\\Component\\DomCrawler\\Form` 对象提供许多非常有用的方法来处理表单::

    $uri = $form->getUri();

    $method = $form->getMethod();

:method:`Symfony\\Component\\DomCrawler\\Form::getUri` 方法不仅仅返回表单的 ``action`` 属性。
如果表单方法是 ``GET``，那么它模仿浏览器的行为，然后返回 ``action`` 属性，并跟随一个附带所有表单值的查询字符串。

.. note::

    支持可选的 ``formaction`` 和 ``formmethod`` 按钮属性。
    ``getUri()`` 和 ``getMethod()`` 方法会确保这些属性总是返回正确的动作和方法，具体取决于用于获取表单的按钮。

你可以实质性的设置和获取表单上的值::

    // 在表单内部设置值
    $form->setValues(array(
        'registration[username]' => 'symfonyfan',
        'registration[terms]'    => 1,
    ));

    // 获取到一个数组形式的值 - 在像上面那样的“扁平”数组中
    $values = $form->getValues();

    // 返回PHP会看到的值，其中“registration”是它自己的数组
    $values = $form->getPhpValues();

处理多维的字段::

    <form>
        <input name="multi[]" />
        <input name="multi[]" />
        <input name="multi[dimensional]" />
    </form>

传递一个数组形式的值::

    // 设置单个字段
    $form->setValues(array('multi' => array('value')));

    // 一次设置多个字段
    $form->setValues(array('multi' => array(
        1             => 'value',
        'dimensional' => 'an other value',
    )));

这很棒，但它会变得更好！``Form`` 对象允许你像浏览器一样与表单进行交互，选择单选框的值，勾选复选框和上传文件::

    $form['registration[username]']->setValue('symfonyfan');

    // 选中或取消选中一个复选框
    $form['registration[terms]']->tick();
    $form['registration[terms]']->untick();

    // 选择一个选项
    $form['registration[birthday][year]']->select(1984);

    // 在选择框中选择多个选项
    $form['registration[interests]']->select(array('symfony', 'cookies'));

    // 伪造文件上传
    $form['registration[photo]']->upload('/path/to/lucas.jpg');

使用表单数据
...................

做这一切有什么意义呢？如果你在内部进行测试，则可以从表单中获取信息，就像刚刚使用PHP值提交的那样::

    $values = $form->getPhpValues();
    $files = $form->getPhpFiles();

如果你使用的是外部HTTP客户端，则可以使用该表单来获取“为该表单创建POST请求而所需的所有信息”::

    $uri = $form->getUri();
    $method = $form->getMethod();
    $values = $form->getValues();
    $files = $form->getFiles();

    // 现在使用一些HTTP客户端并使用此信息进行提交

使用所有这些功能的集成系统的一个很好的例子就是`Goutte`_
Goutte理解Symfony Crawler对象，可以使用它来直接提交表单::

    use Goutte\Client;

    // 向外部网站发出一个真实请求
    $client = new Client();
    $crawler = $client->request('GET', 'https://github.com/login');

    // 选中表单，并填充一些值
    $form = $crawler->selectButton('Sign in')->form();
    $form['login'] = 'symfonyfan';
    $form['password'] = 'anypass';

    // 提交给定的表单
    $crawler = $client->submit($form);

.. _components-dom-crawler-invalid:

选择无效的选择框值
...............................

默认情况下，选择字段（select, radio）已激活内部验证，以防止你设置无效值。
如果你希望能够设置无效值，则可以在整个表单或特定字段上使用 ``disableValidation()`` 方法::

    // 禁用特定字段的验证
    $form['country']->disableValidation()->select('Invalid value');

    // 禁用整个表单的验证
    $form->disableValidation();
    $form['country']->select('Invalid value');

.. _`Goutte`: https://github.com/FriendsOfPHP/Goutte
.. _Packagist: https://packagist.org/packages/symfony/dom-crawler

扩展阅读
----------

* :doc:`/testing`
* :doc:`/components/css_selector`
