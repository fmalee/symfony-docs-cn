约定
===========

:doc:`standards` 文档描述了Symfony的项目和内部及第三方bundle的编码标准。
本文档描述了核心框架中使用的编码标准和约定，以使其更加一致和可预测。
我们鼓励你在自己的代码中关注它们，但你不需要这样做。

方法名称
------------

当一个对象与相关的“事物”（对象，参数......）有很多关系(relation)时，方法名称被规范化：

* ``get()``
* ``set()``
* ``has()``
* ``all()``
* ``replace()``
* ``remove()``
* ``clear()``
* ``isEmpty()``
* ``add()``
* ``register()``
* ``count()``
* ``keys()``

只有在明确存在主要关系时才允许使用这些方法：

* 一个 ``CookieJar`` 有很多 ``Cookie`` 对象;

* 一个服务 ``Container`` 有许多服务和许多参数（因为服务是主要关系，命名约定用于此关系）;

* 一个控制台 ``Input`` 有许多参数和许多选项。没有“主要”关系，因此不适用命名约定。

对于不适用约定的许多关系，必须使用以下方法代替（ ``XXX`` 是相关事物的名称）：

+----------------+-------------------+
| Main Relation  |  Other Relation   |
+================+===================+
| ``get()``      | ``getXXX()``      |
+----------------+-------------------+
| ``set()``      | ``setXXX()``      |
+----------------+-------------------+
| n/a            | ``replaceXXX()``  |
+----------------+-------------------+
| ``has()``      | ``hasXXX()``      |
+----------------+-------------------+
| ``all()``      | ``getXXXs()``     |
+----------------+-------------------+
| ``replace()``  | ``setXXXs()``     |
+----------------+-------------------+
| ``remove()``   | ``removeXXX()``   |
+----------------+-------------------+
| ``clear()``    | ``clearXXX()``    |
+----------------+-------------------+
| ``isEmpty()``  | ``isEmptyXXX()``  |
+----------------+-------------------+
| ``add()``      | ``addXXX()``      |
+----------------+-------------------+
| ``register()`` | ``registerXXX()`` |
+----------------+-------------------+
| ``count()``    | ``countXXX()``    |
+----------------+-------------------+
| ``keys()``     | n/a               |
+----------------+-------------------+

.. note::

    虽然“setXXX”和“replaceXXX”非常相似，但有一个值得注意的区别：“setXXX”可以替换或添加新元素到关系。
    另一方面，“replaceXXX”无法添加新元素。如果将无法识别的键传递给“replaceXXX”，则必会抛出异常。

.. _contributing-code-conventions-deprecations:

弃用
------------

有时会在框架中弃用某些类或方法；
当由于向后兼容性问题而无法更改功能实现时会发生这种情况，但我们仍然希望提出一个“更好”的替代方案。
在这种情况下，可以 **弃用** 旧的实现。

通过向相关类，方法，属性......添加一个 ``@deprecated`` phpdoc，可以将一个功能标记为已弃用::

    /**
     * @deprecated since vendor-name/package-name 2.8, to be removed in 3.0. Use XXX instead.
     */

弃用消息应指出不推荐使用该类/方法时的版本，将被删除时的版本以及可能的替换功能。

还必须触发一个PHP ``E_USER_DEPRECATED`` 错误，
以帮助迁移人员在“删除了该功能的版本”之前启动一个或两个次要版本（取决于该删除的重要性）::

    @trigger_error('XXX() is deprecated since vendor-name/package-name 2.8 and will be removed in 3.0. Use XXX instead.', E_USER_DEPRECATED);

如果没有 `@-silencing运算符`_，用户将需要手动选择关闭(opt-out)弃用通知。
静默交换此行为（Silencing swaps this behavior）并允许用户在准备好应对它们时选择加入(opt-in)
（通过在Web调试工具栏或PHPUnit桥接器中的一个中添加自定义错误处理器）。

.. _`@-silencing运算符`: https://php.net/manual/en/language.operators.errorcontrol.php

在弃用整个类时，应该在命名空间和use声明之间调用 ``trigger_error()``，就像在 `ArrayParserCache`_ 中的示例一样::

    namespace Symfony\Component\ExpressionLanguage\ParserCache;

    @trigger_error('The '.__NAMESPACE__.'\ArrayParserCache class is deprecated since version 3.2 and will be removed in 4.0. Use the Symfony\Component\Cache\Adapter\ArrayAdapter class instead.', E_USER_DEPRECATED);

    use Symfony\Component\ExpressionLanguage\ParsedExpression;

    /**
     * @author Adrien Brault <adrien.brault@gmail.com>
     *
     * @deprecated since Symfony 3.2, to be removed in 4.0. Use the Symfony\Component\Cache\Adapter\ArrayAdapter class instead.
     */
    class ArrayParserCache implements ParserCacheInterface

.. _`ArrayParserCache`: https://github.com/symfony/symfony/blob/3.2/src/Symfony/Component/ExpressionLanguage/ParserCache/ArrayParserCache.php
