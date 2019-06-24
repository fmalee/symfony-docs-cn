.. index::
   single: Dotenv
   single: Components; Dotenv

Dotenv组件
====================

    Dotenv组件解析 ``.env`` 文件以使存储在其中的环境变量可通过
    ``$_ENV`` 或 ``$_SERVER`` 来访问。

安装
------------

.. code-block:: terminal

    $ composer require symfony/dotenv

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

.. code-block:: terminal

    # .env
    DB_USER=root
    DB_PASS=pass

使用 ``$_ENV`` 在代码中访问该值::

    $dbUser = $_ENV['DB_USER'];
    // 你也可以使用 ``$_SERVER``

``load()`` 方法永远不会重写现有的环境变量。如果需要重写它们，请使用 ``overload()`` 方法::

    // ...
    $dotenv->overload(__DIR__.'/.env');

当你使用Dotenv组件时，你会注意到你可能希望根据你正在使用的环境来拥有不同的文件。
通常，这种情况发生在本地开发或持续集成中，你可能希望在 ``test`` 和 ``dev``
环境中使用不同的文件。

你可以使用 ``Dotenv::loadEnv()`` 来简化此过程::

    use Symfony\Component\Dotenv\Dotenv;

    $dotenv = new Dotenv();
    $dotenv->loadEnv(__DIR__.'/.env');

然后，Dotenv组件将按以下顺序查找要加载的正确的 ``.env``
文件，而之后加载的文件将覆盖之前加载的文件中定义的变量：

#. 如果存在 ``.env`` 则首先加载。如果没有 ``.env``文件而有一个
   ``.env.dist``，则会加载此文件。
#. 如果前面提到的其中一个文件包含 ``APP_ENV``
   变量，那么将填充该变量，并在以后用于加载特定于环境的文件。
   如果在前面提到的任何一个文件中都没有定义 ``APP_ENV``，那么 ``APP_ENV``
   将假定为 ``dev``，并在默认情况下填充。
#. 如果有一个代表常规本地环境变量的 ``.env.local`` ，那么它现在就加载了。
#. 如果有一个 ``.env.$env.local`` 文件，这个文件就会被加载。否则，它会回退到 ``.env.$env``。

乍一看，这可能看起来很复杂，但它让你有机会提交多个特定于环境的文件，然后这些文件可以调整到你的本地环境中。
如果你提交了 ``.env``、``.env.test`` 和 ``.env.dev`` 来表示环境的不同配置设置，则可以分别使用
``.env.local``、``.env.test.local`` 和 ``.env.dev.local`` 来调整它们。

.. note::

    ``.env.local`` 在 ``test`` 环境中总是被忽略，因为测试应该为每个人产生相同的结果。

你可以通过将变量作为附加参数传递给
``Dotenv::loadEnv()``，来调整定义环境、默认环境和测试环境的变量（有关详细信息，请参阅
:method:`Symfony\\Component\\Dotenv\\Dotenv::loadEnv` ）。

.. versionadded:: 4.2
    ``Dotenv::loadEnv()`` 方法是在Symfony 4.2中引入的。

你永远不应该将 ``.env`` 文件存储在代码仓库中，因为它可能包含敏感信息;
可以创建一个具有合理默认值的 ``.env.dist`` 文件（或多个特定于环境的文件，如上所示）。

.. note::

    Symfony Dotenv可用于你的应用的任何环境：开发、测试、升级甚至生产。
    但是，在生产中，建议配置实际的环境变量，以避免为每个请求解析 ``.env`` 文件导致的性能开销。

由于 ``.env`` 文件是一个常规shell脚本，你可以在自己的shell脚本中 ``source`` 它：

.. code-block:: terminal

    source .env

通过为它们添加 ``#`` 前缀来添加注释：

.. code-block:: terminal

    # 数据库凭据
    DB_USER=root
    DB_PASS=pass # 这是密码

通过在变量前添加 ``$`` 前缀，可以在值中使用环境变量：

.. code-block:: terminal

    DB_USER=root
    DB_PASS=${DB_USER}pass # 引入用户作为一个密码前缀

通过 ``$()`` 嵌入命令（Windows不支持）：

.. code-block:: terminal

    START_TIME=$(date)

.. note::

    请注意，根据你的shell ，使用 ``$()`` 可能不起作用。

.. _Packagist: https://packagist.org/packages/symfony/dotenv
.. _twelve-factor applications: http://www.12factor.net/
