.. index::
   single: Components; Installation
   single: Components; Usage

.. _how-to-install-and-use-the-symfony2-components:

如何安装和使用 Symfony 组件
=============================================

如果你要启动一个“使用了一或多个组件”的全新项目（或者你已经拥有一个项目），最简单的办法是用 `Composer`_ 来整合所有东西。
Composer在下载组件时足够智能，以至你只需坐等自动下载，然后直接使用类库。

本文通过 doc:`/components/finder` 带你入门，但却适用于其他任何组件。

使用 Finder 组件
--------------------------

**1.** 如果你是新建项目，为项目新建一个目录。

**2.** 打开命令行，使用 Composer 抓取类库。

.. code-block:: terminal

    $ composer require symfony/finder

 ``symfony/finder`` 是被写入文件体系最上层的名字（译注：参考组件自带的composer.json第一个name选项即知），为的是应对你想要的任何组件。

.. tip::

    请先 `Install Composer`_，如果你的系统中没有的话。
    根据不同系统下的安装，你的目录下可能会有一个 ``composer.phar`` 文件，如果是这种情况，不要急！只需运行 ``php composer.phar require symfony/finder`` 命令。

**3.** 写代码！

一旦 Composer 把组件下载完毕，你要做的是，包容Composer生成的 ``vendor/autoload.php`` 文件。这个文件负责所有的类库的自动加载，以便你能立即使用：

    // 文件范例: src/script.php

    // 修改下面这行代码，使相对于 script.php 这个文件的 “vendor/” 目录，能够被正确包容进来
    require_once __DIR__.'/../vendor/autoload.php';

    use Symfony\Component\Finder\Finder;

    $finder = new Finder();
    $finder->in('../data/');

    // ...

又该如何?
---------

现在，组件已经被安装和自动加载，参考特定组件的文档，以便掌握如何使用它。

And have fun!

.. _Composer: https://getcomposer.org
.. _Install Composer: https://getcomposer.org/download/
