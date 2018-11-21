.. index::
   single: Dotenv
   single: Components; Dotenv

Dotenv组件
====================

    Dotenv组件解析 ``.env`` 文件以使存储在其中的环境变量可通过
    ``getenv()``、``$_ENV`` 或 ``$_SERVER`` 来访问。

安装
------------

.. code-block:: terminal

    $ composer require --dev symfony/dotenv

或者，你可以克隆 `<https://github.com/symfony/dotenv>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

用法
-----

应将敏感信息和依赖于环境的设置定义为环境变量（即 `twelve-factor applications`_ 推荐）。
使用一个 ``.env`` 文件存储这些环境变量可以简化开发和CI管理，方法是将它们保存在一个“标准”位置，
并且不知道你正在使用的技术堆栈（例如Nginx与PHP内置服务器）。

.. note::

    PHP有很多此“模式”的不同实现。这个实现的目标是复制 ``source .env`` 会做的事情。
    它试图与标准shell的行为尽可能相似（因此没有值验证）。

通过 ``Dotenv::load()`` 在PHP应用中加载一个 ``.env`` 文件::

    use Symfony\Component\Dotenv\Dotenv;

    $dotenv = new Dotenv();
    $dotenv->load(__DIR__.'/.env');

    // 你也可以叫做多个文件
    $dotenv->load(__DIR__.'/.env', __DIR__.'/.env.dev');

假设下面为 ``.env`` 文件的内容：

.. code-block:: bash

    # .env
    DB_USER=root
    DB_PASS=pass

使用 ``getenv()`` 在代码中访问该值::

    $dbUser = getenv('DB_USER');
    // 你也可以使用 ``$_ENV`` or ``$_SERVER``

``load()`` 方法永远不会重写现有的环境变量。如果需要重写它们，请使用 ``overload()`` 方法::

    // ...
    $dotenv->overload(__DIR__.'/.env');

.. versionadded:: 4.2
    ``Dotenv::overload()`` 方法是在Symfony 4.2中引入的。

你永远不应该将 ``.env`` 文件存储在代码仓库中，因为它可能包含敏感信息;
可以创建一个具有合理默认值的 ``.env.dist`` 文件。

.. note::

    Symfony Dotenv可用于你的应用的任何环境：开发、测试、升级甚至生产。
    但是，在生产中，建议配置实际的环境变量，以避免为每个请求解析 ``.env`` 文件导致的性能开销。

由于 ``.env`` 文件是一个常规shell脚本，你可以在自己的shell脚本中 ``source`` 它：

.. code-block:: terminal

    source .env

通过为它们添加 ``#`` 前缀来添加注释：

.. code-block:: bash

    # Database credentials
    DB_USER=root
    DB_PASS=pass # This is the secret password

通过在变量前添加 ``$`` 前缀，可以在值中使用环境变量：

.. code-block:: bash

    DB_USER=root
    DB_PASS=${DB_USER}pass # Include the user as a password prefix

通过 ``$()`` 嵌入命令（Windows不支持）：

.. code-block:: bash

    START_TIME=$(date)

.. note::

    请注意，根据你的shell ，使用 ``$()`` 可能不起作用。

.. _Packagist: https://packagist.org/packages/symfony/dotenv
.. _twelve-factor applications: http://www.12factor.net/
