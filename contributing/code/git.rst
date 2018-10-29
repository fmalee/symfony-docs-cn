Git
===

本文档解释了我们使用Git管理Symfony代码的方式的一些约定和特性。

拉取请求
-------------

每当合并一个拉取请求时，拉取请求中包含的所有信息（包括注释）都将保存仓库中。

你可以识别拉取请求合并，因为提交消息始终遵循此模式：

.. code-block:: text

    merged branch USER_NAME/BRANCH_NAME (PR #1111)

PR参考允许你查看GitHub上的原始拉取请求：https：//github.com/symfony/symfony/pull/1111。
但是你可以从仓库本身获得GitHub上可以获得的所有信息。

合并提交消息包含来自更改作者的原始消息。通常，这有助于了解变更的内容以及变更背后的原因。

此外，当时可能发生的完整讨论也存储为Git备注（在2013年3月22日之前，讨论是主合并提交消息的一部分）。
要访问这些笔记，请将此行添加到你的 ``.git/config`` 文件中：

.. code-block:: ini

    fetch = +refs/notes/*:refs/notes/*

抓取(fetch)之后，获取提交的GitHub讨论就是在 ``git show`` 命令上添加 ``--show-notes=github-comments`` 参数的问题：

.. code-block:: terminal

    $ git show HEAD --show-notes=github-comments
