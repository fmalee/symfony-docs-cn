.. index::
   single: BrowserKit
   single: Components; BrowserKit

BrowserKit组件
========================

    BrowserKit组件模拟Web浏览器的行为，允许你以编程方式创建请求、单击链接和提交表单。

.. note::

    在4.3之前的Symfony版本中，BrowserKit组件只能向应用发起内部请求。从Symfony 4.3开始，当与
    :doc:`HttpClient组件 </components/http_client>` 结合使用时，该组件还可以
    :ref:`向任何公共站点发起HTTP请求 <component-browserkit-external-requests>`。

安装
------------

.. code-block:: terminal

    $ composer require symfony/browser-kit

.. include:: /components/require_autoload.rst.inc

基本用法
-----------

.. seealso::

    本文介绍如何在任何PHP应用中将BrowserKit功能用作独立组件。
    阅读 :ref:`Symfony功能测试 <functional-tests>` 文档，了解如何在Symfony应用中使用它。

获取客户端
~~~~~~~~~~~~~~~~~

该组件仅提供一个抽象客户端，并且不提供准备用于HTTP层的任何后端。

要创建自己的客户端，必须扩展抽象 ``Client`` 类并实现
:method:`Symfony\\Component\\BrowserKit\\Client::doRequest`
方法。此方法接受一个请求并应返回一个响应::

    namespace Acme;

    use Symfony\Component\BrowserKit\Client as BaseClient;
    use Symfony\Component\BrowserKit\Response;

    class Client extends BaseClient
    {
        protected function doRequest($request)
        {
            // ... 将请求转换为响应

            return new Response($content, $status, $headers);
        }
    }

对于基于HTTP层的浏览器的简单实现，请查看 `Goutte`_。
对于基于 ``HttpKernelInterface`` 的实现，请查看
:doc:`HttpKernel组件 </components/http_kernel>` 提供的
:class:`Symfony\\Component\\HttpKernel\\Client`。

创建请求
~~~~~~~~~~~~~~~

使用 :method:`Symfony\\Component\\BrowserKit\\Client::request`
方法创建HTTP请求。前两个参数分别是HTTP方法和请求的URL::

    use Acme\Client;

    $client = new Client();
    $crawler = $client->request('GET', '/');

``request()`` 方法返回的值是 :doc:`DomCrawler组件 </components/dom_crawler>` 提供的 :class:`Symfony\\Component\\DomCrawler\\Crawler`
类的一个实例，它允许以编程的方式访问和遍历HTML元素。

:method:`Symfony\\Component\\BrowserKit\\Client::xmlHttpRequest`
方法与 ``request()`` 方法定义了相同的参数，是一个生成AJAX请求的快捷方式::

    use Acme\Client;

    $client = new Client();
    // 自动添加所需的 HTTP_X_REQUESTED_WITH 标头
    $crawler = $client->xmlHttpRequest('GET', '/');

点击链接
~~~~~~~~~~~~~~

``Client`` 对象能够模拟链接点击。传递链接的文本内容，客户端将执行所需的HTTP GET请求以模拟链接点击::

    use Acme\Client;

    $client = new Client();
    $client->request('GET', '/product/123');

    $crawler = $client->clickLink('Go elsewhere...');

如果你需要一个提供访问链接属性（例如 ``$link->getMethod()``、``$link->getUri()``）的方法的
:class:`Symfony\\Component\\DomCrawler\\Link` 对象，请使用另一种方法::

    // ...
    $crawler = $client->request('GET', '/product/123');
    $link = $crawler->selectLink('Go elsewhere...')->link();
    $client->click($link);

提交表单
~~~~~~~~~~~~~~~~

``Client`` 对象还能够提交表单。
首先，使用其任何按钮选择表单，然后在提交之前重写其任何属性（方法、字段值等）::

    use Acme\Client;

    $client = new Client();
    $crawler = $client->request('GET', 'https://github.com/login');

    // 使用“Log in”按钮找到表单并提交
    // “Log in”可以是 <button> 或 <input type =“submit”> 的文本内容、id、值或名称
    $client->submitForm('Log in');

    // 第二个可选参数允许你重写默认的表单字段值
    $client->submitForm('Log in', [
        'login' => 'my_user',
        'password' => 'my_pass',
        // 要上传文件，该值必须是绝对文件路径
        'file' => __FILE__,
    ]);

    // 你也可以重写其他表单选项
    $client->submitForm(
        'Log in',
        ['login' => 'my_user', 'password' => 'my_pass'],
        // 重写默认表单HTTP方法
        'PUT',
        // 重写一些 $_SERVER 参数（例如HTTP标头）
        ['HTTP_ACCEPT_LANGUAGE' => 'es']
    );

如果你需要一个提供访问表单属性（例如 ``$form->getUri()``、``$form->getValues()``
``$form->getFields()``）的方法的 :class:`Symfony\\Component\\DomCrawler\\Form`
对象，请使用另一种方法::

    // ...

    // 选择表单并填写一些值
    $form = $crawler->selectButton('Log in')->form();
    $form['login'] = 'symfonyfan';
    $form['password'] = 'anypass';

    // 提交该表单
    $crawler = $client->submit($form);

Cookies
-------

检索Cookie
~~~~~~~~~~~~~~~~~~

``Client`` 实现通过一个 :class:`Symfony\\Component\\BrowserKit\\CookieJar`
来暴露cookie（如果有的话），它允许你在使用客户端创建请求时存储和检索任何cookie::

    use Acme\Client;

    // 创建一个请求
    $client = new Client();
    $crawler = $client->request('GET', '/');

    // 获取 cookie Jar
    $cookieJar = $client->getCookieJar();

    // 根据名称获取 cookie
    $cookie = $cookieJar->get('name_of_the_cookie');

    // 获取cookie数据
    $name       = $cookie->getName();
    $value      = $cookie->getValue();
    $rawValue   = $cookie->getRawValue();
    $isSecure   = $cookie->isSecure();
    $isHttpOnly = $cookie->isHttpOnly();
    $isExpired  = $cookie->isExpired();
    $expires    = $cookie->getExpiresTime();
    $path       = $cookie->getPath();
    $domain     = $cookie->getDomain();
    $sameSite   = $cookie->getSameSite();

.. note::

    这些方法仅返回尚未过期的cookie。

循环使用Cookie
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: php

    use Acme\Client;

    // 创建一个请求
    $client = new Client();
    $crawler = $client->request('GET', '/');

    // 获取 cookie Jar
    $cookieJar = $client->getCookieJar();

    // 获取包含所有cookie的数组
    $cookies = $cookieJar->all();
    foreach ($cookies as $cookie) {
        // ...
    }

    // 获取所有值
    $values = $cookieJar->allValues('http://symfony.com');
    foreach ($values as $value) {
        // ...
    }

    // 获取所有原始值
    $rawValues = $cookieJar->allRawValues('http://symfony.com');
    foreach ($rawValues as $rawValue) {
        // ...
    }

设置Cookie
~~~~~~~~~~~~~~~

你还可以创建cookie并将其添加到可以注入客户端构造函数的cookie jar中::

    use Acme\Client;

    // 创建 cookies 并添加到 cookie jar
    $cookie = new Cookie('flavor', 'chocolate', strtotime('+1 day'));
    $cookieJar = new CookieJar();
    $cookieJar->set($cookie);

    // 创建一个客户端并设置cookies
    $client = new Client([], null, $cookieJar);
    // ...

历史记录
----------

客户端存储着你的所有请求，允许你在历史记录中前后移动::

    use Acme\Client;

    $client = new Client();
    $client->request('GET', '/');

    // 选择并点击一个连接
    $link = $crawler->selectLink('Documentation')->link();
    $client->click($link);

    // 回退到主页
    $crawler = $client->back();

    // 前进到文档页
    $crawler = $client->forward();

你可以使用 ``restart()`` 方法来删除客户端的历史记录。这也将删除所有cookie::

    use Acme\Client;

    $client = new Client();
    $client->request('GET', '/');

    // 重置客户端 (历史和cookie也被清除)
    $client->restart();

.. _component-browserkit-external-requests:

发起外部HTTP请求
-----------------------------

到目前为止，本文中的所有示例都假定你正在对自己的应用发起内部请求。
但是，在向外部网站和应用发起HTTP请求时，可以运行完全相同的示例。

首先，安装并配置 :doc:`HttpClient组件 </components/http_client>`。然后，使用
:class:`Symfony\\Component\\BrowserKit\\HttpBrowser` 创建将发起外部HTTP请求的客户端::

    use Symfony\Component\BrowserKit\HttpBrowser;
    use Symfony\Component\HttpClient\HttpClient;

    $browser = new HttpBrowser(HttpClient::create());

你现在可以使用本文中展示的任何方法来提取信息、单击链接、提交表单等。
这意味着你不再需要使用专用的web crawler 或 scraper，如 `Goutte`_::

    $browser = new HttpBrowser(HttpClient::create());

    $browser->request('GET', 'https://github.com');
    $browser->clickLink('Sign in');
    $browser->submitForm('Sign in', ['login' => '...', 'password' => '...']);
    $openPullRequests = trim($browser->clickLink('Pull requests')->filter(
        '.table-list-header-toggle a:nth-child(1)'
    )->text());

.. versionadded:: 4.3

    Symfony 4.3中引入了发起外部HTTP请求的功能。

扩展阅读
----------

* :doc:`/testing`
* :doc:`/components/css_selector`
* :doc:`/components/dom_crawler`

.. _`Packagist`: https://packagist.org/packages/symfony/browser-kit
.. _`Goutte`: https://github.com/FriendsOfPHP/Goutte
