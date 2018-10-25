.. index::
   single: Deployment; Deployment tools

.. _how-to-deploy-a-symfony2-application:

部署
=============

部署Symfony可能是一个复杂和多样的任务，取决于你的程序的设置和需求。
本文并非手把手的指南，而是罗列了部署时的常见需求和建议。

.. _symfony2-deployment-basics:

部署基础
-------------------------

发生在部署Symfony时的典型步骤包括：

#. 把你的代码上传到生产服务器;
#. 安装第三方依赖 (通常透过Composer完成，并且可以在上传程序之前完成);
#. 运行数据库迁移或类似任务，以更新“已改变的”数据结构;
#. 清除（并可选择预热）缓存。

部署过程还包括其他任务，诸如：

* 将特定版本的代码标记为源代码控制仓库中的发行版;
* 创建临时暂存区域，方便以“离线”的方式构建你已更新的设置;
* 运行任何可用的测试，以确保代码和/或服务器的稳定性;
* 从 ``public/`` 目录删除任何不必要的文件以保持生产环境干净;
* 清理外部缓存系统 (像是 `Memcached`_ 或 `Redis`_)。

如何部署Symfony应用
-----------------------------------

部署Symfony应用时有几种方式。从一些基本的部署策略开始，然后从那里构建。

基本文件传输
~~~~~~~~~~~~~~~~~~~

部署一套应用最基本的方式是通过FTP/SCP（或类似方法）手动拷贝文件。
其欠点是，比如在升级过程中，你缺少对系统的控制。
这种方法也需要你在文传输之后执行一些手动步骤（参考 `常见部署后任务`_）。

使用版本控制
~~~~~~~~~~~~~~~~~~~~

如果你使用了版本控制（比如Git或SVN），则可以通过将实时安装(live installation)也作为仓库的副本来简化。
当你准备升级时，简单到如同从版本控制系统中获取最新更新一样。
使用Git时，常见的方法是为每个版本创建一个标签，然后在部署时检出相应的标签（请参阅 `Git标签`_）。

这令你的文件更新变得\* 更容易*\，但你仍然需要考虑手动执行其他步骤（参考 `常见部署后任务`_）。

使用平台服务
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

使用平台服务（PaaS）可以快速轻松地部署Symfony应用。
有很多PaaS  - 下面有一些与Symfony配合得更好：

* `Heroku`_
* `Platform.sh`_
* `Azure`_
* `fortrabbit`_

使用构建脚本和其他工具
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

有几种工具可以帮助减轻部署时的痛苦。其中的一些几乎是为Symfony的需求而量身定制的：

`EasyDeployBundle`_
    一个Symfony bundle，可为你的应用添加简单的部署工具。

`Deployer`_
    这是Capistrano的另一个原生PHP重写，为Symfony提供了一些现成的指令。

`Ansistrano`_
    一个 Ansible 角色，允许你通过YAML文件配置强大的部署。

`Magallanes`_
    这种类似Capistrano的部署工具是用PHP构建的，PHP开发人员可以更轻松地扩展以满足他们的需求。

`Fabric`_
    这个基于Python的库提供了一组基本操作，用于执行本地或远程shell命令以及上载/下载文件。

`Capistrano`_ 与 `Symfony plugin`_
    `Capistrano`_ 是一个用Ruby编写的远程服务器自动化和部署工具。
    `Symfony plugin`_ 是一个简化Symfony相关任务的插件，灵感来自 `Capifony`_（仅适用于Capistrano 2）。

`sf2debpkg`_
    帮助你为Symfony项目构建一个原生(native)Debian软件包。

Basic scripting
    你当然可以使用shell、`Ant`_ 或任何其他构建工具来编写项目的部署脚本。

常见部署后任务
----------------------------

在部署了你的真正源代码之后，有一些常见事项需要你来做：

A) 需求检查
~~~~~~~~~~~~~~~~~~~~~

使用 :doc:`Symfony 运行环境检测 </reference/requirements>`
检查你的服务器是否满足运行Symfony应用的技术要求。

.. _b-configure-your-app-config-parameters-yml-file:

B) 配置你的环境变量
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Most Symfony applications read their configuration from environment variables.
While developing locally, you'll usually store these in a ``.env`` file. On production,
you have two options:

1. Create "real" environment variables. How you set environment variables, depends
   on your setup: they can be set at the command line, in your Nginx configuration,
   or via other methods provided by your hosting service.

2. Or, create a ``.env`` file just like your local development (see note below)

There is no significant advantage to either of the two options: use whatever is
most natural in your hosting environment.

.. note::

    If you use the ``.env`` file on production, you may need to move your
    ``symfony/dotenv`` dependency from ``require-dev`` to ``require`` in ``composer.json``:

    .. code-block:: terminal

        $ composer remove symfony/dotenv
        $ composer require symfony/dotenv

C) 安装/更新依赖
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Your vendors can be updated before transferring your source code (i.e.
update the ``vendor/`` directory, then transfer that with your source
code) or afterwards on the server. Either way, just update your vendors
as you normally do:

.. code-block:: terminal

    $ composer install --no-dev --optimize-autoloader

.. tip::

    The ``--optimize-autoloader`` flag improves Composer's autoloader performance
    significantly by building a "class map". The ``--no-dev`` flag ensures that
    development packages are not installed in the production environment.
    通过构建一个 "class map" 类映射，--optimize-autoloader 旗标大幅改进了Composer的自动加载性能。--no-dev 旗标可确保开发环境的包不被安装到生产环境。

.. caution::

    If you get a "class not found" error during this step, you may need to
    run ``export APP_ENV=prod`` (or ``export SYMFONY_ENV=prod`` if you're not
    using :doc:`Symfony Flex </setup/flex>`) before running this command so
    that the ``post-install-cmd`` scripts run in the ``prod`` environment.
    如果在这一步你得到 "class not found" 错误，你可能需要在执行前述命令之前先运行 export SYMFONY_ENV=prod 以便 post-install-cmd 脚本运行在 prod 环境下。

D) 清除Symfony缓存
~~~~~~~~~~~~~~~~~~~~~~~~~~~

确保清除（以及warm-up）了你的Symfony缓存。
Make sure you clear and warm-up your Symfony cache:

.. code-block:: terminal

    $ APP_ENV=prod APP_DEBUG=0 php bin/console cache:clear

E) 其他任务！
~~~~~~~~~~~~~~~~

There may be lots of other things that you need to do, depending on your
setup:

* Running any database migrations
* Clearing your APC cache
* Add/edit CRON jobs
* :ref:`Building and minifying your assets <how-do-i-deploy-my-encore-assets>` with Webpack Encore
* Pushing assets to a CDN
* ...

程序生命周期：持续整合，质量保证等
-------------------------------------------------------

虽然本文覆盖了部署过程的技术细节，但是代码从开发到生产时的完整生命周期可能需要更多步骤（考虑staging部署，QA[Quality Assurance/质量保证]，运行测试，等等）
While this article covers the technical details of deploying, the full lifecycle
of taking code from development up to production may have more steps:
deploying to staging, QA (Quality Assurance), running tests, etc.

staging、测试、QA、持续整合（continuous integration），数据库迁移以及失败时的向下兼容，统统被强烈建议。有各种简单或复杂的工具，其中的某一款会令你的部署在满足环境需求的过程变得容易（或老练）。
The use of staging, testing, QA, continuous integration, database migrations
and the capability to roll back in case of failure are all strongly advised. There
are simple and more complex tools and one can make the deployment as easy
(or sophisticated) as your environment requires.

别忘了在部署过程中也牵扯到更新依赖（一般通过Composer），迁移数据库，清除缓存以及其他潜在事项，诸如将资源发布到CDN（参考 常见的后部署任务）。
Don't forget that deploying your application also involves updating any dependency
(typically via Composer), migrating your database, clearing your cache and
other potential things like pushing assets to a CDN (see `Common Post-Deployment Tasks`_).

Troubleshooting
---------------

Deployments not Using the ``composer.json`` File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony applications provide a ``kernel.project_dir`` parameter and a related
:method:`Symfony\\Component\\HttpKernel\\Kernel::getProjectDir` method.
You can use this method to perform operations with file paths relative to your
project's root directory. The logic to find that project root directory is based
on the location of the main ``composer.json`` file.

If your deployment method doesn't use Composer, you may have removed the
``composer.json`` file and the application won't work on the production server.
The solution is to override the ``getProjectDir()`` method in the application
kernel and return your project's root directory::

    // src/Kernel.php
    // ...
    class Kernel extends BaseKernel
    {
        // ...

        public function getProjectDir()
        {
            return __DIR__.'/..';
        }
    }

扩展阅读
----------

.. toctree::
    :maxdepth: 1

    deployment/proxies

.. _`Capifony`: https://github.com/everzet/capifony
.. _`Capistrano`: http://capistranorb.com/
.. _`sf2debpkg`: https://github.com/liip/sf2debpkg
.. _`Fabric`: http://www.fabfile.org/
.. _`Ansistrano`: https://ansistrano.com/
.. _`Magallanes`: https://github.com/andres-montanez/Magallanes
.. _`Ant`: http://blog.sznapka.pl/deploying-symfony2-applications-with-ant
.. _`Memcached`: http://memcached.org/
.. _`Redis`: http://redis.io/
.. _`Symfony plugin`: https://github.com/capistrano/symfony/
.. _`Deployer`: http://deployer.org/
.. _`Git标签`: https://git-scm.com/book/en/v2/Git-Basics-Tagging
.. _`Heroku`: https://devcenter.heroku.com/articles/getting-started-with-symfony
.. _`platform.sh`: https://docs.platform.sh/frameworks/symfony.html
.. _`Azure`: https://azure.microsoft.com/en-us/develop/php/
.. _`fortrabbit`: https://help.fortrabbit.com/install-symfony
.. _`EasyDeployBundle`: https://github.com/EasyCorp/easy-deploy-bundle
