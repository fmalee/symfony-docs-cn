.. index::
   single: Components; Installation
   single: Components; Usage

.. _how-to-install-and-use-the-symfony2-components:

如何安装和使用Symfony组件
=============================================

如果你正在开始一个将使用一个或多个组件的新项目（或已经有一个项目），那么集成所有内容的最简单方法就是使用 `Composer`_。
Composer非常智能，可以下载你需要的组件并负责自动加载，以便你可以立即开始使用这些库。


本文将指导你使用 :doc:`/components/finder`，但这适用于使用任何组件。

使用Finder组件
--------------------------

**1.** 如果要创建新项目，请为其创建一个新的空目录。

**2.** 打开终端并使用Composer抓取库。

.. code-block:: terminal

    $ composer require symfony/finder

写在文档的顶部的 ``symfony/finder`` 名称，可以是你想要的任何组件。

.. tip::

    如果你的系统上没有Composer，请安装 `安装Composer`_。
    根据你的安装方式，你最终可能会在目录中找到一个 ``composer.phar`` 文件。
    在那种情况下，不用担心！这是你的命令行将是 ``php composer.phar require symfony/finder``。

**3.** 编写代码！

Composer下载好组件后，你需要做的就是引入它生成的  ``vendor/autoload.php`` 文件。
此文件负责自动加载所有库，以便你可以立即使用它们::

    // 示例文件: src/script.php

    // 将这里修改为相对于此文件的 “vendor/” 目录的路径
    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\Finder\Finder;

    $finder = new Finder();
    $finder->in('../data/');

    // ...

然后呢？
---------

现在组件已安装并自动加载。请阅读特定组件的文档以了解有关如何使用它的更多信息。

玩得开心！

.. _Composer: https://getcomposer.org
.. _安装Composer: https://getcomposer.org/download/
