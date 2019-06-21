.. index::
    single: Security; Vulnerability Checker

如何检查依赖中的已知安全漏洞
====================================================================

在Symfony项目中使用大量依赖项时，其中一些可能包含安全漏洞。这就是为什么
:doc:`Symfony本地服务器 </setup/symfony_server>` 包含一个 ``security:check``
命令，该命令检查你的 ``composer.lock`` 文件以查找已安装的依赖中的已知安全漏洞：

.. code-block:: terminal

    $ symfony security:check

一个好的安全措施是定期执行此命令，以便能够尽快更新或替换受损的依赖。
通过克隆FriendsOfPHP组织发布的 `安全建议数据库`_
可以在本地完成安全检查，因此你的 ``composer.lock`` 文件不会发送到网络上。

.. tip::

    如果任何依赖受已知安全漏洞影响，则 ``security:check`` 命令将以非零退出代码。
    这样，你可以将其添加到项目构建过程和持续集成工作流程中，以便在存在漏洞时使其失效。

.. _`安全建议数据库`: https://github.com/FriendsOfPHP/security-advisories
