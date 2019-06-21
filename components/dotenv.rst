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

As you're working with the Dotenv component you'll notice that you might want
to have different files depending on the environment you're working in. Typically
this happens for local development or Continuous Integration where you might
want to have different files for your ``test`` and ``dev`` environments.
当你使用Dotenv组件时，你会注意到你可能希望根据你正在使用的环境拥有不同的文件。
通常，这种情况发生在本地开发或持续集成中，你可能希望为你的文件提供不同的文件test和dev环境。

You can use ``Dotenv::loadEnv()`` to ease this process::
你可以使用它Dotenv::loadEnv()来简化此过程：

    use Symfony\Component\Dotenv\Dotenv;

    $dotenv = new Dotenv();
    $dotenv->loadEnv(__DIR__.'/.env');

The Dotenv component will then look for the correct ``.env`` file to load
in the following order whereas the files loaded later override the variables
defined in previously loaded files:
然后，Dotenv组件将按.env以下顺序查找要加载的正确文件，而稍后加载的文件将覆盖先前加载的文件中定义的变量：

#. If ``.env`` exists, it is loaded first. In case there's no ``.env`` file but a
   ``.env.dist``, this one will be loaded instead.
   如果.env存在，则首先加载。如果没有.env文件而是a .env.dist，则会加载此文件 。
#. If one of the previously mentioned files contains the ``APP_ENV`` variable, the
   variable is populated and used to load environment-specific files hereafter. If
   ``APP_ENV`` is not defined in either of the previously mentioned files, ``dev`` is
   assumed for ``APP_ENV`` and populated by default.
   如果前面提到的文件之一包含APP_ENV变量，则填充该变量并用于以后加载特定于环境的文件。如果 APP_ENV未在前面提到的任何一个文件中定义，dev则APP_ENV默认为假定并填充。
#. If there's a ``.env.local`` representing general local environment variables it's loaded now.
   如果有一个.env.local代表一般的本地环境变量，它现在已加载。
#. If there's a ``.env.$env.local`` file, this one is loaded. Otherwise, it falls
   back to ``.env.$env``.
   如果有.env.$env.local文件，则加载此文件。否则，它会回落.env.$env。

This might look complicated at first glance but it gives you the opportunity to
commit multiple environment-specific files that can then be adjusted to your
local environment. Given you commit ``.env``, ``.env.test`` and ``.env.dev`` to
represent different configuration settings for your environments, each of them
can be adjusted by using ``.env.local``, ``.env.test.local`` and
``.env.dev.local`` respectively.
乍一看这可能看起来很复杂，但它让你有机会提交多个特定于环境的文件，然后可以将这些文件调整到你的本地环境。给你承诺.env，.env.test并.env.dev为你的环境中代表不同的配置设置，他们每个人都可以通过调整.env.local，.env.test.local并  .env.dev.local分别。

.. note::

    ``.env.local`` is always ignored in ``test`` environment because tests should produce the
    same results for everyone.
    .env.local在test环境中总是被忽略，因为测试应该为每个人产生相同的结果。

You can adjust the variable defining the environment, default environment and test
environments by passing them as additional arguments to ``Dotenv::loadEnv()``
(see :method:`Symfony\\Component\\Dotenv\\Dotenv::loadEnv` for details).
你可以通过将它们作为附加参数传递来调整定义环境，默认环境和测试环境的变量Dotenv::loadEnv() （loadEnv()有关详细信息，请参阅参考资料）。

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

    # Database credentials
    DB_USER=root
    DB_PASS=pass # This is the secret password

通过在变量前添加 ``$`` 前缀，可以在值中使用环境变量：

.. code-block:: terminal

    DB_USER=root
    DB_PASS=${DB_USER}pass # Include the user as a password prefix

通过 ``$()`` 嵌入命令（Windows不支持）：

.. code-block:: terminal

    START_TIME=$(date)

.. note::

    请注意，根据你的shell ，使用 ``$()`` 可能不起作用。

.. _Packagist: https://packagist.org/packages/symfony/dotenv
.. _twelve-factor applications: http://www.12factor.net/
