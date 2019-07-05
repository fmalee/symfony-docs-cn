.. index::
    single: Web Server; Built-in Web Server

如何使用PHP内置Web服务器
====================================

PHP CLI SAPI附带一个 `内置的Web服务器`_`。
它可以本地化的运行PHP应用，用于开发、测试或应用演示。这样，你就不必费心配置
:doc:`Apache 或 Nginx </setup/web_server_configuration>` 等功能齐全的Web服务器。

.. tip::

    开发Symfony应用的首选方法是使用 :doc:`Symfony本地Web服务器 </setup/symfony_server>`。

.. caution::

    内置Web服务器旨在在受控环境中运行。它不适用于公共网络。

Symfony提供了一个构建在此PHP服务器之上的Web服务器，以简化你的本地配置。
此服务器作为一个bundle分发，因此你必须先安装并启用该服务器bundle。


安装Web服务器bundle
--------------------------------

进入项目目录并运行以下命令：

.. code-block:: terminal

    $ cd your-project/
    $ composer require --dev symfony/web-server-bundle

启动Web服务器
-----------------------

要使用PHP的内置Web服务器来运行Symfony应用，请执行 ``server:start`` 命令：

.. code-block:: terminal

    $ php bin/console server:start

这将在后台启动Web服务器，在 ``localhost:8000`` 上为你的Symfony应用提供服务。

默认情况下，Web服务器监听环回设备上的8000端口。
你可以通过将IP地址和端口传递为一个命令行参数来修改该套接字：

.. code-block:: terminal

    # 传递一个指定的IP和端口
    $ php bin/console server:start 192.168.0.1:8080

    # 传递'\*'作为IP，意味着使用 0.0.0.0 (即任何本地IP地址)
    $ php bin/console server:start *:8080

.. note::

    你可以使用 ``server:status`` 命令来检查Web服务器是否正在监听：

    .. code-block:: terminal

        $ php bin/console server:status

.. tip::

    某些系统不支持 ``server:start`` 命令，在这些情况下你可以执行 ``server:run`` 命令。
    此命令的行为略有不同。
    它不会在后台启动服务器，而是会冻结(block)当前终端直到你终止它（这通常是通过按Ctrl和C来完成的）。

.. sidebar:: 从虚拟机内部使用内置Web服务器

    如果要从虚拟机内部使用内置Web服务器，然后从你的主机上的浏览器加载该站点，则需要监听
    ``0.0.0.0:8000`` 地址（即，分配给该虚拟机的所有IP地址）：

    .. code-block:: terminal

        $ php bin/console server:start 0.0.0.0:8000

    .. caution::

        你应该 **永不** 监听可以从互联网直接访问的计算机上的所有接口。
        因为内置Web服务器不适用于公共网络。

命令选项
~~~~~~~~~~~~~~~

内置的Web服务器需要一个“路由”脚本（阅读 `php.net`_ 上有关“路由”脚本的内容）作为参数。
当在 ``prod`` 或 ``dev`` 环境中执行该命令时，Symfony已经传递了一个这样的路由脚本。
使用 ``--router`` 选项可以使用你自己的路由脚本：

.. code-block:: terminal

    $ php bin/console server:start --router=config/my_router.php

如果应用的文档根目录与标准目录布局不同，则必须使用 ``--docroot`` 选项来传递正确的位置：

.. code-block:: terminal

    $ php bin/console server:start --docroot=public_html

停止服务器
-------------------

完成工作后，可以使用以下命令来停止Web服务器：

.. code-block:: terminal

    $ php bin/console server:stop

.. _`内置的Web服务器`: https://php.net/manual/en/features.commandline.webserver.php
.. _`php.net`: https://php.net/manual/en/features.commandline.webserver.php#example-411
