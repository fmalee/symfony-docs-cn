.. index::
   single: Installing and Setting up Symfony

安装
=============================================

.. admonition:: 教学视频
    :class: screencast

    更喜欢视频教程? 可以观看 `Stellar Development with Symfony`_ 系列录像.

要创建一个 Symfony 应用，受限要确保你使用的是 PHP 7.1(或是更高的版本)，并且安装了 `Composer`_ 。
如果没有，那就在你的系统上 :doc:`全局安装 Composer </setup/composer>`。

运行一下的命令来创建你的项目:

.. code-block:: terminal

    $ composer create-project symfony/website-skeleton my-project

该命令会创建一个叫 ``my-project`` 的新目录，里面已经下载好一些依赖，并生成了一些基本的目录和文件让你使用。
一句话，你的新应用已经安装完成！

.. tip::

    ``website-skeleton`` 是为传统的 Web 项目而优化的。如果你需要构建微服务、控制台应用，或是 API，
    可以使用更加简洁的 ``skeleton`` 项目:

    .. code-block:: terminal

        $ composer create-project symfony/skeleton my-project

运行 Symfony 应用
--------------------------------

在生成环境下，你应该使用 Nginx 或 Apache 等Web服务器
（请参阅 :doc:`配置一个Web服务器来运行Symfony </setup/web_server_configuration>`）。
但是对于开发来说，使用 :doc:`Symfony的本地服务器 <setup/symfony_server>` 会更方便。

.. note::

    如果你想要使用虚拟机(VM)，可以参考 :doc:`Homestead </setup/homestead>`。

进入你的新项目并且运行内置服务器：

.. code-block:: terminal

    $ cd my-project
    $ symfony server:start

打开你的浏览器并访问 ``http://localhost:8000/``。如果一切正常，你应该会看到一个欢迎页面。
最后，你在完成工作后想要关闭服务器，可以在终端中按 ``Ctrl+C`` 关闭服务器。

.. tip::

    如果你在运行 Symfony 时碰到问题，可能是你的系统无法满足基本的环境需求。
    使用 :doc:`Symfony 运行环境检测 </reference/requirements>` 工具来确保
    你的系统已经安装好所需的环境。

.. tip::

    如果你是使用 VM，那么需要让服务器绑定所有的IP地址:

    .. code-block:: terminal

        $ php bin/console server:start 0.0.0.0:8000

    你永远不应该在一台电脑上直接监听所有的接口(IP)，因为这样的话会让网络直接访问到你的电脑。

使用 Git 保存项目
---------------------------

在GitHub，GitLab和Bitbucket等服务中存储你的项目与任何其他代码项目一样！
用 ``Git`` 初始化一个新的仓库，你就可以推送代码到你的远程服务器了：

.. code-block:: terminal

    $ git init
    $ git add .
    $ git commit -m "Initial commit"

你的项目已经有一个合适的 ``.gitignore`` 文件。
当你安装更多软件包时，名为 :ref:`Flex <flex-quick-intro>` 的系统将在需要时向该文件添加更多内容。

.. _install-existing-app:

设置现有的Symfony项目
--------------------------------------

如果你正在使用现有的Symfony应用，则只需获取项目代码并使用Composer安装依赖项。
假设你的团队使用Git，请使用以下命令设置项目：

.. code-block:: terminal

    # 克隆项目以下载其内容
    $ cd projects/
    $ git clone ...

    # 让 Composer 将项目的依赖项安装到 vendor/
    $ cd my-project/
    $ composer install

你可能还需要自定义你的 :ref:`.env <config-dot-env>` 并执行一些其他项目特定任务（例如，创建数据库模式(schema)）。
在首次使用现有Symfony应用时，运行此命令可能会有用，该命令显示有关应用的信息：

.. code-block:: terminal

    $ php bin/console about

检查安全漏洞
-------------------------------------

Symfony 提供了一个名为“Security Checker”的工具来检查你的项目的依赖项是否包含任何已知的安全漏洞。
请查看 `安全检查器`_ 的集成说明以进行设置。

Symfony 演示程序
-------------------------------------

`Symfony演示程序`_ 是一个功能齐全的应用，它展示了开发Symfony应用的推荐方法。
它是Symfony新手的一个很好的学习工具，它的代码包含大量的注释和有用的备注。

要查看其代码并在本地安装，请参阅 `symfony/symfony-demo`_。

开始开发!
-----------------

完成这些配置后，是时候 :doc:`在Symfony中创建第一个页面 </page_creation>`。

扩展阅读
------------

.. toctree::
    :hidden:

    page_creation

.. toctree::
    :maxdepth: 1
    :glob:

    setup/homestead
    setup/built_in_web_server
    setup/web_server_configuration
    setup/composer
    setup/*

.. _`Stellar Development with Symfony`: http://symfonycasts.com/screencast/symfony
.. _`Composer`: https://getcomposer.org/
.. _`安全检查器`: https://github.com/sensiolabs/security-checker#integration
.. _`Symfony演示程序`: https://github.com/symfony/demo
.. _`symfony/symfony-demo`: https://github.com/symfony/demo
