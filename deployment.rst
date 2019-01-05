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
当你准备升级时，就从版本控制系统中抓取最新更新。
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
    一个Symfony bundle，可为你的应用添加部署工具。

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
    `Symfony plugin`_ 是一个简化Symfony相关任务的插件，灵感来自 `Capifony`_ （仅适用于Capistrano 2）。

`sf2debpkg`_
    帮助你为Symfony项目构建一个原生(native)Debian软件包。

Basic scripting
    你可以使用shell、`Ant`_ 或任何其他构建工具来编写项目的部署脚本。

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

大多数Symfony应用从环境变量中读取其配置。
在本地开发时，你通常会将它们存储在 ``.env`` 和 ``.env.local`` （用于本地覆盖）。
在部署时，你有两种选择：

1. 创建“真实”的环境变量。如何设置环境变量取决于你的设置：
   可以在命令行，Nginx配置中或通过托管服务提供的其他方法设置它们。

2. 或者，像本地开发时一样创建一个 ``.env.local`` 文件（参见下面的注释）

这两种方案中的任何一种都没有明显的优势：使用托管环境中最自然的东西。

.. note::

    如果在生产中使用 ``.env.*`` 文件，
    则可能需要在 ``composer.json`` 中将你的 ``symfony/dotenv`` 依赖项从 ``require-dev`` 移至 ``require``：

    .. code-block:: terminal

        $ composer remove symfony/dotenv
        $ composer require symfony/dotenv

C) 安装/更新依赖
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你可以在传输源代码之前进行更新（如更新 ``vendor/`` 目录，然后和源代码一个传输），或之后在服务器上进行。
无论哪种方式，只需像往常一样更新你的依赖(vendor)：

.. code-block:: terminal

    $ composer install --no-dev --optimize-autoloader

.. tip::

    ``--optimize-autoloader`` 参数通过构建一个 "类映射" ，大幅改进了Composer的自动加载性能。
    ``--no-dev`` 参数可确保开发环境的软件包不被安装到生产环境。

.. caution::

    如果在这一步你得到 "class not found" 错误，你可能需要在执行前述命令之前先运行 ``export APP_ENV=prod``
    (或 ``export SYMFONY_ENV=prod`` 如果你没有使用 :doc:`Symfony Flex </setup/flex>` 的话)
    以便在 ``prod`` 环境下运行 ``post-install-cmd`` 脚本。

D) 清除Symfony缓存
~~~~~~~~~~~~~~~~~~~~~~~~~~~

确保清除并预热(warm-up) Symfony缓存：

.. code-block:: terminal

    $ APP_ENV=prod APP_DEBUG=0 php bin/console cache:clear

E) 其他任务！
~~~~~~~~~~~~~~~~

根据你的设置，你可能还需要做很多其他事情：

* 运行任何数据库迁移
* 清除APC缓存
* 添加/编辑 CRON 任务
* 使用Webpack Encore :ref:`建立和压缩你的资源 <how-do-i-deploy-my-encore-assets>`
* 发布资源到一个 CDN
* ...

程序生命周期：持续整合，质量保证等
-------------------------------------------------------

虽然本文覆盖了部署过程的技术细节，但是代码从开发到生产时的完整生命周期可能需要更多步骤：
部署到模拟环境(staging)，QA(质量保证)，运行测试，等等。

强烈建议使用模拟，测试，QA，持续集成(continuous integration)，数据库迁移以及在发生故障时回滚的能力。
有简单和更复杂的工具，可以使部署与你的环境一样简单（或复杂）。

别忘了在部署过程中也牵扯到更新依赖（一般通过Composer），迁移数据库，清除缓存以及其他潜在事项，
诸如将资源发布到CDN（参考 `常见部署后任务`_）。

故障排除
---------------

不使用 ``composer.json`` 文件部署
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony应用提供了 ``kernel.project_dir`` 参数和相关的
:method:`Symfony\\Component\\HttpKernel\\Kernel::getProjectDir` 方法。
你可以使用此方法对文件路径执行相对于项目根目录的操作。
查找项目根目录的逻辑基于主 ``composer.json`` 文件的位置。

如果部署的方式不使用Composer，你可能已经了删除 ``composer.json`` 文件，那么该应用将无法在生产服务器上运行。
解决方案是覆盖应用内核中的 ``getProjectDir()``  方法并返回项目的根目录::

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
