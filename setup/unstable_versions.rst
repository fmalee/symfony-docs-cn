如何安装或升级到最新的未发布的Symfony版本
===================================================================

在本文中，你将学习如何在安装和使用作为稳定版本发布之前新的Symfony版本。

基于不稳定的Symfony版本创建新项目
-----------------------------------------------------------

假设Symfony 4.0版本尚未发布，你想创建一个新项目来测试其功能。
首先，:doc:`安装Composer </setup/composer>` 包管理器。
然后，打开命令控制台，输入项目的目录并执行以下命令：

.. code-block:: terminal

    # 下载最新的测试版
    $ composer create-project symfony/skeleton my_project "4.0.*" -s beta

    # 下载绝对最新提交
    $ composer create-project symfony/skeleton my_project "4.0.*" -s dev

一旦命令完成执行，你将在 ``my_project/`` 目录中创建一个新的Symfony项目。

将项目升级到不稳定的Symfony版本
-----------------------------------------------------

再次假设Symfony 4.0尚未发布，并且你希望升级现有应用以测试你的项目是否可以使用它。

首先，打开位于项目根目录中的 ``composer.json`` 文件。
然后，将所有 ``symfony/*`` 库的值编辑为新版本，并更改 ``minimum-stability`` 为 ``beta``：

.. code-block:: diff

    {
        "require": {
    +         "symfony/framework-bundle": "^4.0",
    +         "symfony/finder": "^4.0",
            "...": "..."
        },
    +     "minimum-stability": "beta"
    }

你也可以设置 ``minimum-stability`` 为 ``dev``。
或者完全省略这一行，并使用像 ``4.0.*@beta`` 这样的约束决定你的每个包的稳定性。

最后，从终端更新项目的依赖项：

.. code-block:: terminal

    $ composer update

升级Symfony版本后，请阅读 :ref:`Symfony升级指南 <upgrade-major-symfony-deprecations>`，
了解如何在新Symfony版本升级你的代码，因为有些功能可能会在新版本中弃用。

.. tip::

    如果你使用Git来管理项目的代码，那么创建一个新分支来测试新的Symfony版本是一个很好的做法。
    此解决方案避免在你的应用中引入任何问题，并让你充满自信地测试新版本：

    .. code-block:: terminal

        $ cd projects/my_project/
        $ git checkout -b testing_new_symfony
        # ... 更新 composer.json 配置
        $ composer update symfony/symfony

        # ... 测试完Symfony的新版本之后
        $ git checkout master
        $ git branch -D testing_new_symfony
