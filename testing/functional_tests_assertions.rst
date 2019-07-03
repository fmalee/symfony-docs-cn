.. index::
   single: Tests; Assertions

功能测试的特定断言
===================================

.. versionadded:: 4.3

    Symfony 4.3中引入了使用 ``WebTestCase`` 进行断言的快捷方法。

在进行功能测试时，有时你需要进行复杂的断言，以检查 ``Request``、``Response`` 或
``Crawler`` 是否包含使测试成功的预期信息。

这是一个简单的PHPUnit示例::

    $this->assertGreaterThan(
        0,
        $crawler->filter('html:contains("Hello World")')->count()
    );

现在这里是Symfony特有的断言示例::

    $this->assertSelectorTextContains('html', 'Hello World');

.. note::

    这些断言仅在扩展 ``WebTestCase`` 类的测试用例中使用 ``Client`` 发起请求时才起作用。

断言参考
---------------------

响应
~~~~~~~~

- ``assertResponseIsSuccessful()``
- ``assertResponseStatusCodeSame()``
- ``assertResponseRedirects()``
- ``assertResponseHasHeader()``
- ``assertResponseNotHasHeader()``
- ``assertResponseHeaderSame()``
- ``assertResponseHeaderNotSame()``
- ``assertResponseHasCookie()``
- ``assertResponseNotHasCookie()``
- ``assertResponseCookieValueSame()``

请求
~~~~~~~

- ``assertRequestAttributeValueSame()``
- ``assertRouteSame()``

浏览器
~~~~~~~

- ``assertBrowserHasCookie()``
- ``assertBrowserNotHasCookie()``
- ``assertBrowserCookieValueSame()``

抓取器
~~~~~~~

- ``assertSelectorExists()``
- ``assertSelectorNotExists()``
- ``assertSelectorTextContains()``
- ``assertSelectorTextSame()``
- ``assertSelectorTextNotContains()``
- ``assertPageTitleSame()``
- ``assertPageTitleContains()``
- ``assertInputValueSame()``
- ``assertInputValueNotSame()``

故障排除
---------------

这些断言不适用于 `symfony/panther`_，因为它们使用了 ``HttpFoundation`` 组件的
``Request`` 和 ``Response`` 对象，以及 ``FrameworkBundle`` 的
``KernelBrowser``。而Panther仅使用了 ``BrowserKit`` 组件。

.. _`symfony/panther`: https://github.com/symfony/panther
