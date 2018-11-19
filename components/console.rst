.. index::
    single: Console; CLI
    single: Components; Console

Console组件
=====================

    Console组件简化了创建漂亮且可测试的命令行界面的过程。

Console组件允许你创建命令行命令。你的控制台命令可用于任何重复任务，例如cron作业，导入或其他批处理作业。

安装
------------

.. code-block:: terminal

    $ composer require symfony/console

或者，你可以克隆 `<https://github.com/symfony/console>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

创建控制台应用
------------------------------

.. seealso::

    本文介绍如何在任何PHP应用中将控制台功能用作独立组件。
    阅读 :doc:`/console` 文档，以了解如何在Symfony应用中使用它。

首先，你需要创建一个PHP脚本来定义该控制台应用::

    #!/usr/bin/env php
    <?php
    // application.php

    require __DIR__.'/vendor/autoload.php';

    use Symfony\Component\Console\Application;

    $application = new Application();

    // ... 注册命令

    $application->run();

然后，你可以使用 :method:`Symfony\\Component\\Console\\Application::add` 注册该命令::

    // ...
    $application->add(new GenerateAdminCommand());

有关如何创建命令的信息，请参阅 :doc:`/console` 文档。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /console
    /components/console/*
    /components/console/helpers/index
    /console/*

.. _Packagist: https://packagist.org/packages/symfony/console
