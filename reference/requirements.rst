.. index::
   single: Requirements

.. _requirements-for-running-symfony2:

运行Symfony的要求
================================

除了其他次要要求之外，Symfony 4.0还需要运行 **PHP 7.1.3** 或更高版本。
为简单起见，Symfony提供了一种工具，可以快速检查你的系统是否满足所有这些要求。
运行此命令以安装该工具：

.. code-block:: terminal

    $ cd your-project/
    $ composer require symfony/requirements-checker

请注意，PHP可以为命令控制台和Web服务器定义不同的配置，因此你需要检查两个环境中的要求。

检查Web服务器的要求
----------------------------------------

需求检查器工具在 ``public/`` 项目目录中创建一个叫 ``check.php`` 的文件。
使用浏览器打开该文件以检查环境要求。

修复所有报告的问题后，请卸载该需求检查器，以避免将有关应用的内部信息泄露给访问者：

.. code-block:: terminal

    $ cd your-project/
    $ composer remove symfony/requirements-checker

检查命令控制台的要求
---------------------------------------------

需求检查器工具将一个脚本添加到Composer配置中以自动检查需求。
没有必要执行任何命令;如果有任何问题，你将在控制台输出中看到它们。
