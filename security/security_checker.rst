.. index::
    single: Security; Vulnerability Checker

如何检查依赖中的已知安全漏洞
====================================================================

在Symfony项目中使用大量依赖时，其中一些可能包含安全漏洞。
这就是为什么Symfony提供了一个叫 ``security:check`` 的命令，
该命令用于检查你的 ``composer.lock`` 文件以查找已安装的依赖中的任何已知安全漏洞。

首先，在项目中安装安全检查器：

.. code-block:: terminal

    $ composer require sensiolabs/security-checker

然后运行以下命令：

.. code-block:: terminal

    $ php bin/console security:check

一个好的安全实践是定期执行此命令，以便能够尽快更新或替换受威胁的依赖。
在内部，此命令使用FriendsOfPHP组织发布的 `security advisories database`_。

.. tip::

    如果任何依赖受已知安全漏洞的影响，则 ``security:check`` 命令将以非零退出代码的方式终止。
    这允许你将其添加到项目的构建过程和持续集成工作流程中。

.. tip::

    安全检查程器也可作为独立的控制台应用使用，并作为PHAR文件分发，因此你可以在任何PHP应用中使用它。
    查看 `安全检查器仓库`_ 以获取更多详细信息。

.. _`security advisories database`: https://github.com/FriendsOfPHP/security-advisories
.. _`安全检查器仓库`: https://github.com/sensiolabs/security-checker
