.. index::
   single: Requirements

.. _requirements-for-running-symfony2:

运行Symfony的要求
================================

以下是运行Symfony 4应用的技术要求：

* **PHP版本**: 7.1.3或更高版本
* **PHP扩展**: (所有这些都在PHP7+中默认安装和启用）

  * `Ctype`_
  * `iconv`_
  * `JSON`_
  * `PCRE`_
  * `Session`_
  * `SimpleXML`_
  * `Tokenizer`_

* **可写目录**: (必须可由Web服务器写入)

  * 项目的缓存目录（默认情况下为 ``var/cache/``，但应用可以
    :ref:`覆盖缓存目录 <override-cache-dir>`）
  * 项目的日志目录（默认情况下为 ``var/log/``，但应用可以
    :ref:`覆盖日志目录 <override-logs-dir>`）

自动的检查要求
-----------------------------------

为简单起见，Symfony提供了一种工具来快速检查你的系统是否满足这些要求。此外，该工具还提供了适用的建议。

运行此命令以安装该工具：

.. code-block:: terminal

    $ cd your-project/
    $ composer require symfony/requirements-checker

请注意，PHP可以为命令控制台和Web服务器使用不同的配置，因此你需要检查两个环境中的要求。

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

.. _`iconv`: https://php.net/book.iconv
.. _`JSON`: https://php.net/book.json
.. _`Session`: https://php.net/book.session
.. _`Ctype`: https://php.net/book.ctype
.. _`Tokenizer`: https://php.net/book.tokenizer
.. _`SimpleXML`: https://php.net/book.simplexml
.. _`PCRE`: https://php.net/book.pcre
