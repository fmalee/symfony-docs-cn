.. _running-symfony2-tests:

运行Symfony测试
=====================

Symfony项目使用第三方服务，该服务自动为任何提交的 :doc:`补丁 <pull_requests>` 运行测试。
如果新代码中断了任何测试，则拉取请求将显示一条错误消息，其中包含指向完整错误详细信息的链接。

在任何情况下，在提交包含 :doc:`补丁 <pull_requests>` 之前在本地运行测试是一个好习惯 ，以检查你是否未破坏任何内容。

.. _phpunit:
.. _dependencies_optional:

运行测试前
------------------------

要运行Symfony测试套件，请安装测试期间使用的外部依赖项，例如Doctrine，Twig和Monolog。
为此， 请 `安装Composer`_ 并执行以下操作：

.. code-block:: terminal

    $ composer update

.. _running:

运行测试
-----------------

然后，使用以下命令从Symfony根目录运行测试套件：

.. code-block:: terminal

    $ php ./phpunit symfony

输出应该显示 ``OK``。如果没有，请阅读报告的错误以确定发生了什么以及测试是否因新代码而中断。

.. tip::

    整个Symfony套件可能需要几分钟才能完成。如果要测试单个组件，请在 ``phpunit`` 命令后键入其路径，例如：

    .. code-block:: terminal

        $ php ./phpunit src/Symfony/Component/Finder/

.. tip::

    在Windows上，安装 `Cmder`_、`ConEmu`_、`ANSICON`_ 或 `Mintty`_ 等免费应用以查看彩色测试结果。

.. _`安装Composer`: https://getcomposer.org/download/
.. _Cmder: http://cmder.net/
.. _ConEmu: https://conemu.github.io/
.. _ANSICON: https://github.com/adoxa/ansicon/releases
.. _Mintty: https://mintty.github.io/
