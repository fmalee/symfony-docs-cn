Symfony本地Web服务器
========================

你可以使用任何Web服务器（Apache，nginx，PHP内置Web服务器等）运行Symfony应用。
但是，Symfony提供了自己的Web服务器，可以在开发应用时提高工作效率。

虽然此服务器不适合生产使用，但它支持
HTTP/2、TLS/SSL、自动生成安全证书、本地域名以及开发Web项目时迟早需要的许多其他功能。
此外，该服务器不依赖于Symfony，你也可以将它与任何PHP应用一起使用，甚至可以与HTML/SPA（单页面应用）一起使用。

安装
------------

Symfony服务器作为免费的可安装二进制文件分发，没有任何依赖，并且支持Linux、macOS和Windows。
转到 `symfony.com/download`_ 并按照对应的操作系统说明来进行操作。

.. note::

    如果你想报告错误或建议新功能，请在 `symfony/cli`_ 上创建一个问题。

入门
---------------

Symfony服务器会为每个项目都启动一次，因此最终可能会有多个实例（每个实例都在监听不同的端口）。
这是为一个Symfony项目提供服务的通用工作流程：

.. code-block:: terminal

    $ cd my-project/
    $ symfony server:start

      [OK] Web server listening on http://127.0.0.1:....
      ...

    # 现在，浏览给定的URL，或运行以下命令：
    $ symfony open:local

以这种方式运行服务器会使其在控制台中显示日志消息，因此你将无法同时运行其他命令。
如果你愿意，可以在后台运行Symfony服务器：

.. code-block:: terminal

    $ cd my-project/

    # 在后台启动服务器
    $ symfony server:start -d

    # 继续工作并运行其他命令...

    # 显示最新的日志消息
    $ symfony server:log

启用TLS
------------

在本地浏览应用的安全版本，对于尽早检测混合内容的问题，以及运行仅在HTTPS中运行的库非常重要。
传统上，这是痛苦和复杂的设置，但symfony服务器自动化完成一切。首先，运行以下命令：

.. code-block:: terminal

    $ symfony server:ca:install

此命令创建一个本地证书颁发机构，在系统信任存储区中注册，在Firefox中注册（这仅对该浏览器是必需的），并为
``localhost`` 和 ``127.0.0.1`` 创建默认证书。换句话说，它为你做好了一切。

在使用HTTPS而不是HTTP浏览本地应用之前，请停止并重新启动其服务器。

每个项目的不同PHP设置
----------------------------------

选择不同的PHP版本
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你的计算机上安装了多个PHP版本，你可以在项目根目录下创建名为
``.php-version`` 的文件来告诉symfony要使用哪个版本：

.. code-block:: terminal

    $ cd my-project/

    # 使用特定的PHP版本
    $ echo "7.2" > .php-version

    # 使用任何可用的php 7.x版本
    $ echo "7" > .php-version

.. tip::

    Symfony服务器会遍历目录结构直到根目录，因此你可以在某个父目录中创建一个 ``.php-version``
    文件，以便为该目录下的一组项目设置相同的PHP版本。

如果你不记得计算机上安装的所有PHP版本，请运行以下命令：

.. code-block:: terminal

    $ symfony local:php:list

      # 你将看到每个版本支持的所有SAPI（CGI、FastCGI等）。
      # 尽可能的使用FastCGI（php-fpm）；然后是CGI（它也充当一个FastCGI服务器），
      # 最后，服务器回退到普通的CGI。

按项目重写PHP配置选项
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你可以通过在项目根目录中创建名为 ``php.ini``
的文件来更改每个项目的PHP运行时的任何配置选项的值。仅添加要重写的选项：

.. code-block:: terminal

    $ cd my-project/

    # 此项目仅重写默认的PHP时区
    $ cat php.ini
    [Date]
    date.timezone = Asia/Tokyo

使用不同的PHP版本来运行命令
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

运行不同的PHP版本时，使用 ``symfony`` 主命令作为 ``php``
命令的封装器很有用。这允许你始终根据运行命令的项目来选择最合适的PHP版本。
它还会自动加载环境变量，这在运行非Symfony命令时很重要：

.. code-block:: terminal

    # 使用默认的PHP版本运行该命令
    $ php -r "..."

    # 使用项目指定的PHP版本来运行命令
    # (如果项目没有指定一个PHP版本，则为默认的版本)
    $ symfony php -r "..."

如果你经常使用此封装器，请考虑将该命令改名为 ``php``：

.. code-block:: terminal

    $ cd ~/.symfony/bin
    $ cp symfony php
    # 现在你可以运行 “php ...” ，而不是执行 “symfony” 命令

    # 使用这个技巧也可以封装其他PHP命令
    $ cp symfony php-config
    $ cp symfony pear
    $ cp symfony pecl

本地域名
------------------

默认情况下，可以在本地IP ``127.0.0.1`` 的某个随机端口中访问项目。但是，有时最好将域名与它们关联起来：

* 这样当你在同一个项目上连续工作时会更方便，因为端口号可以改变，但域不会改变;
* 某些应用的行为取决于其域名/子域;
* 拥有稳定的端点，例如OAuth2的本地重定向URL。

设置本地代理
~~~~~~~~~~~~~~~~~~~~~~~~~~

由于Symfony服务器提供的本地代理，本地域名成才为可能。首先，启动代理：

.. code-block:: terminal

    $ symfony proxy:start

如果这是你第一次运行代理，则必须执行以下额外步骤：

* 打开操作系统的 **网络配置**；
* 找到 **代理设置** 并选择 **“自动代理配置”**；
* 将其值设置为 ``http://127.0.0.1:7080/proxy.pac``。

定义本地域名
~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，Symfony使用
``.wip``（即 *Work in Progress*）作为本地域名。你也可以为项目自定义本地域名，如下所示：

.. code-block:: terminal

    $ cd my-project/
    $ symfony proxy:domain:attach my-domain

如果你已按照上一节中的说明安装了本地代理，则现在可以浏览 ``https://my-domain.wip``
以使用新的自定义域名来访问本地项目。

.. tip::

    浏览 http://127.0.0.1:7080 网址以获取本地项目的目录、自定义域名和端口号的完整列表。

运行控制台命令时，添加 ``HTTPS_PROXY`` 环境变量以使器在自定义域名下工作：

.. code-block:: terminal

    $ HTTPS_PROXY=http://127.0.0.1:7080 curl https://my-domain.wip

.. tip::

    如果你更喜欢使用其他TLD，请编辑 ``~/.symfony/proxy.json``
    文件（``~`` 代表用户目录的路径），并将 ``tld`` 选项中的 ``wip``
    值更改为任何其他TLD。

长时间运行的命令
---------------------

长时间运行的命令（例如编译前端Web资产的命令）会阻塞终端，并且你无法同时运行其他命令。
Symfony服务器提供了一个 ``run`` 命令来封装它们，如下所示：

.. code-block:: terminal

    # 使用symfony Encore编译Webpack资产...
    # 但是在后台这样做不会阻塞终端
    $ symfony run -d yarn encore dev --watch

    # 继续工作并运行其他命令...

    # 如果需要，随时检查命令日志
    $ symfony server:log

    # 你还可以检查命令是否仍在运行
    $ symfony server:status
    Web server listening on ...
    Command "yarn ..." running with PID ...

    # 完成后停止Web服务器（以及所有关联的命令）
    $ symfony server:stop

Docker集成
------------------

本地Symfony服务器为使用它的项目提供完整的 `Docker`_ 集成。首先，确保暴露了容器端口：

.. code-block:: yaml

    # docker-compose.override.yaml
    services:
        database:
            ports:
                - "3306"

        redis:
            ports:
                - "6379"

        # ...

然后，检查你的服务名称并在需要时更新它们（Symfony根据服务名称创建环境变量，以便它们可以自动配置）：

.. code-block:: yaml

    # docker-compose.yaml
    services:
        # DATABASE_URL
        database: ...
        # MONGODB_DATABASE, MONGODB_SERVER
        mongodb: ...
        # REDIS_URL
        redis: ...
        # ELASTISEARCH_HOST, ELASTICSEARCH_PORT
        elasticsearch: ...
        # RABBITMQ_DSN
        rabbitmq: ...

如果你的 ``docker-compose.yaml`` 文件不使用Symfony期望的环境变量名称（例如，你使用
``MYSQL_URL`` 而不是 ``DATABASE_URL``
时），则需要在Symfony应用中重命名所有这些环境变量。更简单的替代方法是使用``.env.local``
文件重新分配环境变量：

.. code-block:: bash

    # .env.local
    DATABASE_URL=${MYSQL_URL}
    # ...

现在你可以启动容器并将其所有服务公开。
浏览应用的任何页面，然后检查Web调试工具栏中的“Symfony Server”部分，你会看到“Doc​​ker Compose”是“Up”状态。

SymfonyCloud集成
------------------------

Symfony本地服务器提供与 `SymfonyCloud`_
完整但可选的集成，SymfonyCloud是一种优化的服务，用于在云上运行Symfony应用。
它提供的功能包括创建环境、备份/快照、甚至可以从本地计算机访问生产数据的副本，以帮助调试任何问题。

`阅读SymfonyCloud的技术文档`_.

额外功能
--------------

除了作为本地Web服务器之外，Symfony服务器还提供其他有用的功能：

寻找安全漏洞
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你可以运行以下命令，而不是将 :doc:`Symfony安全检查器 </security/security_checker>`
安装为项目的依赖：

.. code-block:: terminal

    $ symfony security:check

此命令使用与Symfony安全检查器相同的漏洞数据库，但它不会对官方的API端点进行HTTP调用。
所有内容（克隆公共数据库除外）都是在本地完成的，这对CI（持续集成）方案来说是最好的。

创建Symfony项目
~~~~~~~~~~~~~~~~~~~~~~~~~

除了 `安装Symfony的不同方式`_，你还可以在Symfony服务器中使用以下命令：

.. code-block:: terminal

    # 基于 symfony/skeleton 创建新项目
    $ symfony new my_project_name

    # 基于 symfony/website-skeleton 创建新项目
    $ symfony new --full my_project_name

    # 基于 symfony/demo 创建新项目
    $ symfony new --demo my_project_name

你也可以基于 **开发版本** 来创建项目（请注意，Composer还会将所有根依赖的稳定性设置为 ``dev``）：

.. code-block:: terminal

    # 基于Symfony的主分支创建新项目
    $ symfony new --version=dev-master my_project_name

    # 基于Symfony的 4.3 dev 分支创建新项目
    $ symfony new --version=4.3.x-dev my_project_name

.. _`symfony.com/download`: https://symfony.com/download
.. _`symfony/cli`: https://github.com/symfony/cli
.. _`安装Symfony的不同方式`: https://symfony.com/download
.. _`Docker`: https://en.wikipedia.org/wiki/Docker_(software)
.. _`SymfonyCloud`: https://symfony.com/cloud/
.. _`阅读SymfonyCloud的技术文档`: https://symfony.com/doc/master/cloud/intro.html
