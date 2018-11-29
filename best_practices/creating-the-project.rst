创建项目
====================

安装 Symfony
------------------

.. best-practice::

    使用 Composer 和 Symfony Flex 来创建和管理 Symfony 应用。

`Composer`_ 是个依赖管理器，被现代php程序广泛使用。
而 `Symfony Flex`_ 是一个Composer插件，旨在自动执行 Symfony 应用中的一些最常见的任务。
使用 Flex 是可选的，但建议使用，因为它可以显著的提高你的生产效率。

.. best-practice::

    使用 Symfony Skeleton 来创建基于Symfon的新项目。

`Symfony Skeleton`_ 是一个最简化的Symfony项目，你可以基于它来创建你新项目。
与过去的 Symfony 版本不同，这个骨架(skeleton)安装了绝对最少量的依赖项，以构建一个完全正常运行的Symfony项目。
阅读 :doc:`/setup` 文章，了解有关安装Symfony的更多信息。

.. _linux-and-mac-os-x-systems:
.. _windows-systems:

创建博客程序
-----------------------------

在命令控制台中，进入到你有权创建文件的目录并执行以下命令：

.. code-block:: terminal

    $ cd projects/
    $ composer create-project symfony/skeleton blog

上面的命令，创建了一个名为 ``blog`` 的新目录，里面是一个基于Symfony最新稳定版的全新项目。
除此之外，安装器还会检查你的操作系统是否满足了运行Symfony所需之环境。
如果不满足，你会看到一个列表，上面有你需要修复的信息。

.. tip::
    运行 Symfony 的所需的环境要求很简单。
    如果要检查系统是否满足这些要求，请阅读：:doc:`/reference/requirements`。

令程序结构化
---------------------------

创建完程序之后，进入 ``blog/`` 目录，你会发现有一堆文件和文件夹被自动生成了：

.. code-block:: text

    blog/
    ├─ bin/
    │  └─ console
    ├─ config/
    └─ public/
    │  └─ index.php
    ├─ src/
    │  └─ Kernel.php
    ├─ var/
    │  ├─ cache/
    │  └─ log/
    └─ vendor/

这种层级式的文件和目录是符合Symfony推荐的命名约定的，可以令你的程序结构化。每个目录的推荐用法如下：
这些文件和目录的层次结构是Symfony为构建应用程序而提出的约定。
建议保留这种结构，因为它是易于导航，大多数目录名称都是不言自明的，
但你可以：:doc:`重写Symfony的默认目录结构 </configuration/override_dir_structure>`：

程序的bundles
~~~~~~~~~~~~~~~~~~~

Symfony 2.0推出之后，多数开发者很自然地用Symfony 1.x的方式去划分程序的逻辑模块。
这就是为何很多Symfony应用都把它们的代码按逻辑功能进行拆分的原因：UserBundle、ProductBundle、InvoiceBundle等等。
当Symfony 2.0发布时，大多数开发人员自然采用了将应用程序划分为逻辑模块的symfony 1.x方式。

但bundle的真义在于，它是作为软件的一个“可被复用”的独立构成而存在。
如果UserBundle不能 *“原封不动地”* 使用在别的Symfony程序中，它不应该成为bundle。
另外，如果InvoiceBundle依赖于ProductBundle，那便没有任何必要将它们分成两个bundle。

.. best-practice::

    不要创建任何 bundle 来组织应用的逻辑。

Symfony应用仍然可以使用第三方软件包（安装在 ``vendor/`` 中）来添加功能，但是你应该使用PHP命名空间而不是bundle来组织自己的代码。

----

下一章: :doc:`/best_practices/configuration`

.. _`Composer`: https://getcomposer.org/
.. _`Symfony Flex`: https://github.com/symfony/flex
.. _`Symfony Skeleton`: https://github.com/symfony/skeleton
