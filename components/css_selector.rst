.. index::
   single: CssSelector
   single: Components; CssSelector

CssSelector组件
=========================

    CssSelector组件将CSS选择器转换为XPath表达式。

安装
------------

.. code-block:: terminal

    $ composer require symfony/css-selector

或者，你可以克隆 `<https://github.com/symfony/css-selector>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

用法
-----

.. seealso::

    本文介绍如何在任何PHP应用中将CssSelector功能用作独立组件。
    阅读 :ref:`Symfony功能测试 <functional-tests>` 文档，以了解如何在创建Symfony测试时使用它。

为什么要使用CSS选择器？
~~~~~~~~~~~~~~~~~~~~~~~~~

迄今为止最强大的解析HTML或XML文档的方法是XPath。

XPath表达式非常灵活，因此几乎总有一个XPath表达式可以找到你需要的元素。
不幸的是，它们也变得非常复杂，学习曲线也很陡峭。
即使是常见的操作（例如查找具有特定类的元素）也可能需要冗长且难以处理的表达式。

许多开发人员 - 特别是Web开发人员 - 更习惯使用CSS选择器来查找元素。
除了在样式表中工作之外，CSS选择器还在JavaScript中与
``querySelectorAll()`` 函数一起使用，并在流行的JavaScript库中使用，例如jQuery，Prototype和MooTools。

CSS选择器的功能不如XPath，但更容易编写、阅读和理解。
由于它们功能较弱，几乎所有的CSS选择器都可以转换为等价的XPath。
然后，此XPath表达式可以与使用XPath来查找文档中元素的其他函数和类一起使用。

CssSelector组件
~~~~~~~~~~~~~~~~~~~~~~~~~

该组件的唯一目标是将CSS选择器转换为其等效的XPath，使用
:method:`Symfony\\Component\\CssSelector\\CssSelectorConverter::toXPath`::

    use Symfony\Component\CssSelector\CssSelectorConverter;

    $converter = new CssSelectorConverter();
    var_dump($converter->toXPath('div.item > h4 > a'));

这给出了以下输出：

.. code-block:: text

    descendant-or-self::div[@class and contains(concat(' ',normalize-space(@class), ' '), ' item ')]/h4/a

你可以使用这个表达式配合，例如，:phpclass:`DOMXPath` 或 :phpclass:`SimpleXMLElement` 来查找一个文档中的元素。

.. tip::

    :method:`Crawler::filter() <Symfony\\Component\\DomCrawler\\Crawler::filter>`
    方法通过CssSelector组件来使用一个CSS选择器字符串查找元素。
    有关更多详细信息，请参阅 :doc:`/components/dom_crawler`。

CssSelector组件的局限性
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

并非所有的CSS选择器都可以转换为等价的XPath。

有几个CSS选择器只在Web浏览器的上下文中有意义。

* 链路状态选择器: ``:link``, ``:visited``, ``:target``
* 基于用户动作的选择器: ``:hover``, ``:focus``, ``:active``
* UI状态选择器: ``:invalid``, ``:indeterminate`` (但是, ``:enabled``、
  ``:disabled``、``:checked`` 和 ``:unchecked`` 是可用的)

伪元素 (``:before``、``:after``、``:first-line``、
``:first-letter``) 不受支持，因为他们选择的是文本部分而不是元素。

尚不支持几个伪类(pseudo-classes)：

* ``*:first-of-type``、``*:last-of-type``、``*:nth-of-type``、
  ``*:nth-last-of-type``、``*:only-of-type``。
  它们使用元素名称（例如 ``li:first-of-type``）但没有使用 ``*``。

.. _Packagist: https://packagist.org/packages/symfony/css-selector

扩展阅读
----------

* :doc:`/testing`
* :doc:`/components/dom_crawler`
