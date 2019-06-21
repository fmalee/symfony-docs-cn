.. index::
   single: Debug
   single: Components; Debug

Debug组件
===================

    Debug组件提供了简化调试PHP代码的工具。

安装
------------

.. code-block:: terminal

    $ composer require symfony/debug

.. include:: /components/require_autoload.rst.inc

用法
-----

Debug组件提供了几个工具来帮助你调试PHP代码。
通过调用此方法启用所有它们：

    use Symfony\Component\Debug\Debug;

    Debug::enable();

:method:`Symfony\\Component\\Debug\\Debug::enable`
方法注册了错误处理器，异常处理器和 :ref:`一个殊的类加载器 <component-debug-class-loader>`。

有关不同可用工具的更多信息，请阅读以下章节。

.. caution::

    你不应该在生产环境中启用除错误处理器之外的调试工具，因为它们可能会向用户暴露敏感信息。

启用错误处理器
--------------------------

:class:`Symfony\\Component\\Debug\\ErrorHandler` 类捕获PHP错误，并将它们转换成异常（
:phpclass:`ErrorException` 类或
:class:`Symfony\\Component\\Debug\\Exception\\FatalErrorException` 致命错误类）::

    use Symfony\Component\Debug\ErrorHandler;

    ErrorHandler::register();

当应用使用FrameworkBundle时，默认情况下会在生产环境中启用此错误处理器，因为它会生成更友好的错误日志。

启用异常处理器
------------------------------

:class:`Symfony\\Component\\Debug\\ExceptionHandler`
类捕获未捕获的PHP异常并将它们转换成一个漂亮的PHP响应。
在调试模式下非常有用，它会用一些更漂亮和更有用的东西替换默认的PHP/XDebug输出::

    use Symfony\Component\Debug\ExceptionHandler;

    ExceptionHandler::register();

.. note::

    如果 :doc:`HttpFoundation组件 </components/http_foundation>`
    可用，则处理器使用一个Symfony Response对象；如果没有，它会回退到常规的PHP响应。

.. _component-debug-class-loader:

调试一个类加载器
------------------------

当已注册的自动加载器(autoloader)找不到一个类时，:class:`Symfony\\Component\\Debug\\DebugClassLoader`
会尝试抛出更多有用的异常。
实现 ``findFile()`` 方法的所有自动加载器都被替换为一个 ``DebugClassLoader`` 封装器。

要使用 ``DebugClassLoader``，可以通过调用其
:method:`Symfony\\Component\\Debug\\DebugClassLoader::enable` 方法::

    use Symfony\Component\Debug\DebugClassLoader;

    DebugClassLoader::enable();

.. _Packagist: https://packagist.org/packages/symfony/debug
