.. index:: Flex

使用Flex来管理Symfony应用
=================================================

`Symfony Flex`_ 是安装和管理Symfony应用的新方法。
Flex不是一个新的Symfony版本，而是一个替换和改进 `Symfony安装器`_ 和 `Symfony标准版`_ 的工具。

Symfony Flex **可自动执行Symfony应用的最常见任务**，例如安装和删除Bundel以及其他Composer依赖项。
Symfony Flex适用于Symfony 3.3及更高版本。从Symfony 4.0开始，默认情况下应该使用Flex，但它仍然是可选的。

Flex如何工作
------------------

Symfony的Flex是一个Composer插件，修改 ``require``、``update`` 以及 ``remove`` 命令的行为。
在启用Flex的应用中安装或删除依赖时，Symfony可以在执行Composer任务之前和之后处理任务。

请思考以下示例：

.. code-block:: terminal

    $ cd my-project/
    $ composer require mailer

如果你在不使用Flex的Symfony应用中执行该命令，你将会看到一个Composer错误，说明  ``mailer`` 不是有效的软件包名称。
但是，如果应用安装了Symfony Flex，那么该命令最终会安装并启用SwiftmailerBundle，
这是集成Swiftmailer（Symfony应用的官方邮件程序）的最佳方式。

当应用中已安装Symfony Flex并执行时 ``composer require`` 时，
应用会在尝试使用Composer安装软件包之前向Symfony Flex服务器发出请求。

* 如果没有关于该软件包的信息，则Flex服务器不返回任何内容，而软件包安装遵循基于Composer的常规过程;

* 如果有关于该软件包的指定信息，Flex会将其返回到名为“recipe”的文件中，
  并且应用使用它来确定要安装的软件包以及安装后要运行的自动化任务。

在上面的示例中，Symfony Flex查询 ``mailer`` 包，Symfony Flex服务器检测到 ``mailer``
实际上是SwiftmailerBundle的别名，并返回关于它的“指令”。

Flex会将其安装的指令记录保存在 ``symfony.lock`` 文件中，该文件必须提交到你的代码仓库。

Flex指令
~~~~~~~~~~~~~~~~~~~~

指令在 ``manifest.json`` 文件中定义，可以包含任意数量的其他文件和目录。
例如，这是SwiftmailerBundle的 ``manifest.json`` ：

.. code-block:: javascript

    {
        "bundles": {
            "Symfony\\Bundle\\SwiftmailerBundle\\SwiftmailerBundle": ["all"]
        },
        "copy-from-recipe": {
            "config/": "%CONFIG_DIR%/"
        },
        "env": {
            "MAILER_URL": "smtp://localhost:25?encryption=&auth_mode="
        },
        "aliases": ["mailer", "mail"]
    }

``aliases`` 选项允许Flex安装软件包时使用简短易记的名称
（``composer require mailer`` vs ``composer require symfony/swiftmailer-bundle``）。
``bundles`` 选项告诉Flex应该在哪些环境中自动启用此Bundle（在本例中是 ``all``）。
``env`` 选项使Flex可以向应用添加新的环境变量。
最后，``copy-from-recipe`` 选项允许指令将文件和目录复制到你的应用中。

Symfony Flex在卸载依赖（例如 ``composer remove mailer``）时也会依据 ``manifest.json`` 中定义的指令来撤消所有更改。
这意味着Flex可以从应用中移除SwiftmailerBundle，删除 ``MAILER_URL`` 环境变量以及该指令创建的任何其他文件和目录。

Symfony Flex指令由社区提供，它们存储在两个公共仓库中：

* `主指令仓库`_，是高质量和正维护的软件包的指令的精选列表。默认情况下，Symfony Flex仅在此仓库中查找。

* `Contrib指令仓库`_，包含社区创建的所有指令。所有这些都保证可以工作，但是它们相关的包可能没有再维护。
  在安装任何这些指令之前，Symfony Flex会征得你的同意。

阅读 `Symfony指令文档`_，了解有关如何为自己的软件包创建配方的所有信息。

在新应用中使用Flex
--------------------------------------

Symfony发布了一个新的“骨架”项目，这是一个推荐用于创建新应用的最小Symfony项目。
这个“骨架”已经包含Symfony Flex作为依赖项。
这意味着你可以通过执行以下命令来创建新的启用Flex的Symfony应用：

.. code-block:: terminal

    $ composer create-project symfony/skeleton:4.1.* my-project

.. note::

    从Symfony 3.3开始，不再推荐使用Symfony安装器来创建新的应用。请改用Composer的 ``create-project`` 命令。

.. _upgrade-to-flex:

将现有应用升级到Flex
---------------------------------------

使用Symfony Flex是可选的，即使在Symfony4中默认使用Flex。
但是，Flex非常方便，可以提高你的工作效率，因此强烈建议你升级现有应用。

唯一需要注意的是，Symfony Flex要求应用使用以下目录结构，该结构在Symfony 4中默认使用：

.. code-block:: text

    your-project/
    ├── assets/
    ├── bin/
    │   └── console
    ├── config/
    │   ├── bundles.php
    │   ├── packages/
    │   ├── routes.yaml
    │   └── services.yaml
    ├── public/
    │   └── index.php
    ├── src/
    │   ├── ...
    │   └── Kernel.php
    ├── templates/
    ├── tests/
    ├── translations/
    ├── var/
    └── vendor/

这意味着在应用中安装 ``symfony/flex`` 依赖是不够的。
你还必须将目录结构升级到上面显示的目录结构。没有自动工具来完成此升级，因此你必须遵循以下手动步骤：

#. 安装Flex作为项目的一个依赖：

   .. code-block:: terminal

       $ composer require symfony/flex

#. 如果项目的 ``composer.json`` 文件包含 ``symfony/symfony`` 依赖
   （仍然依赖于Symfony标准版，但Symfony 4中不再使用）。
   首先，删除该依赖项：

   .. code-block:: terminal

       $ composer remove symfony/symfony

   现在将 ``symfony/symfony`` 软件包添加到 ``composer.json`` 文件的 ``conflict`` 部分中
   （如 `“骨架”项目的示例所示`_），以便它不会再次安装：

   .. code-block:: diff

       {
           "require": {
               "symfony/flex": "^1.0",
       +     },
       +     "conflict": {
       +         "symfony/symfony": "*"
           }
       }

   现在，你必须在 ``composer.json`` 中添加项目所需的所有Symfony依赖项。
   一种快速的方法是添加先前 ``symfony/symfony`` 依赖中包含的所有组件，稍后你可以删除任何你不需要的组件：

   .. code-block:: terminal

       $ composer require annotations asset orm-pack twig \
         logger mailer form security translation validator
       $ composer require --dev dotenv maker-bundle orm-fixtures profiler

#. 如果项目的 ``composer.json`` 文件不包含 ``symfony/symfony`` 依赖，则它已根据Flex的要求显式定义其依赖。
   重新安装所有依赖以强制让Flex在 ``config/`` 中生成配置文件，这是升级过程中最乏味的部分：

   .. code-block:: terminal

       $ rm -rf vendor/*
       $ composer install

#. 无论你遵循以下哪个步骤。此时，你将在 ``config/`` 中拥有大量新的配置文件。
   它们包含Symfony定义的默认配置，因此你必须检查在  ``app/config/`` 中的原始文件并在新文件中进行必要的更改。
   Flex配置不会在配置文件中使用后缀，所以旧的 ``app/config/config_dev.yml`` 会变成 ``config/packages/dev/*.yaml`` 等等。

#. 最重要的配置文件是 ``app/config/services.yml``，现在位于 ``config/services.yaml``。
   复制 `默认services.yaml文件`_ 的内容， 然后添加自己的服务配置。
   稍后你可以重新访问此文件，因为借助Symfony的 :doc:`自动装配 </service_container/3.3-di-changes>` 功能，你可以删除大多数服务配置。

   .. note::

       确保先前的配置文件没有 ``imports`` 声明指向通过``Kernel::configureContainer()``
       或 ``Kernel::configureRoutes()`` 加载的资源。

#. 如下所示移动其余的 ``app/`` 内容（然后删除 ``app/`` 目录）：

   * ``app/Resources/views/`` -> ``templates/``
   * ``app/Resources/translations/`` -> ``translations/``
   * ``app/Resources/<BundleName>/views/`` -> ``templates/bundles/<BundleName>/``
   * ``app/Resources/`` 其余文件 -> ``src/Resources/``

#. 将原始PHP源代码从 ``src/AppBundle/*`` 移动到 ``src/``，
   但bundle的特定文件（如 ``AppBundle.php`` 和 ``DependencyInjection/``） 除外。
   删除 ``src/AppBundle/``。

   除了移动文件之外，还要更新如 `本示例所示`_ 的 ``composer.json`` 的 ``autoload`` 和 ``autoload-dev`` 的值，
   以便使用 ``App\`` 和 ``App\Tests\`` 作为应用的命名名空间（高级IDE可以自动执行此操作）。

   如果你使用多个Bundle来组织代码，则必须将代码重新组织到 ``src/``。
   例如，如果你有 ``src/UserBundle/Controller/DefaultController.php``
   和 ``src/ProductBundle/Controller/DefaultController.php``，你可以将它们移动为 ``src/Controller/UserController.php`` 和 ``src/Controller/ProductController.php``。

#. 将公共资源（如图像或已编译的CSS/JS文件）从 ``src/AppBundle/Resources/public/`` 移动到 ``public/``
   （例如 ``public/images/``）。

#. 将那些资源的源（例如SCSS文件）移动到 ``assets/`` 并使用 :doc:`Webpack Encore </frontend>` 来管理和编译它们。

#. 创建新的 ``public/index.php`` 前端控制器（`复制Symfony的index.php源代码`_）。
   如果在 ``web/app.php`` 和 ``web/app_dev.php`` 文件中进行了任何自定义，则将这些更改复制到新文件中。
   你现在可以删除 ``web/`` 旧目录了。

#. 更新 ``bin/console`` 脚本（`复制Symfony的bin/console源代码`_）并根据原始控制台脚本来更改对应内容。

#. 删除 ``bin/symfony_requirements`` 脚本，如果需要替代功能，请使用新的 `Symfony Requirements Checker`_。

.. _`Symfony Flex`: https://github.com/symfony/flex
.. _`Symfony安装器`: https://github.com/symfony/symfony-installer
.. _`Symfony标准版`: https://github.com/symfony/symfony-standard
.. _`主指令仓库`: https://github.com/symfony/recipes
.. _`Contrib指令仓库`: https://github.com/symfony/recipes-contrib
.. _`Symfony指令文档`: https://github.com/symfony/recipes/blob/master/README.rst
.. _`默认services.yaml文件`: https://github.com/symfony/recipes/blob/master/symfony/framework-bundle/3.3/config/services.yaml
.. _`本示例所示`: https://github.com/symfony/skeleton/blob/8e33fe617629f283a12bbe0a6578bd6e6af417af/composer.json#L24-L33
.. _`“骨架”项目的示例所示`: https://github.com/symfony/skeleton/blob/8e33fe617629f283a12bbe0a6578bd6e6af417af/composer.json#L44-L46
.. _`复制Symfony的index.php源代码`: https://github.com/symfony/recipes/blob/master/symfony/framework-bundle/3.3/public/index.php
.. _`复制Symfony的bin/console源代码`: https://github.com/symfony/recipes/blob/master/symfony/console/3.3/bin/console
.. _`Symfony Requirements Checker`: https://github.com/symfony/requirements-checker
