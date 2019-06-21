贡献文档
=================================

在你的第一次贡献之前
------------------------------

**在做出贡献之前**, 你需要：

* 注册一个免费的 `GitHub`_ 帐户，这是托管Symfony文档的服务商。
* 熟悉 `reStructuredText`_ 标记语言，该语言用于编写Symfony文档。
  阅读 :doc:`本文档 </contributing/documentation/format>` 以获得快速概述。

.. _minor-changes-e-g-typos:

快速在线贡献
-------------------------

如果你做了一个相对较小的改变 - 比如修正拼写错误或重新措辞 - 最简单的贡献方式就是直接在GitHub上操作！
你可以在阅读Symfony文档时执行此操作。

**步骤1.** 单击右上角的 **edit this page** 按钮，你将被重定向到GitHub：

.. image:: /_images/contributing/docs-github-edit-page.png
   :align: center
   :class: with-browser

**步骤2.** 编辑内容，描述你的更改，然后单击 **Propose file change** 按钮。

**步骤3.** GitHub现在将为你的更改创建一个分支和提交
（如果这是你的第一个贡献，则首先forking该仓库），它还将显示你的更改的预览：

.. image:: /_images/contributing/docs-github-create-pr.png
   :align: center
   :class: with-browser

如果一切正确，请单击 **Create pull request** 按钮。

**步骤4.** GitHub将显示一个新页面，你可以在创建拉取请求之前对其进行最后一分钟的更改。
对于简单的贡献，你可以安全地忽略这些选项，只需再次单击 **Create pull request** 按钮。

**恭喜！** 你刚刚创建了一个对官方Symfony文档的拉取请求！
社区现在将审核你的拉取请求并（可能）审议调整。

如果你的贡献很大或者你更喜欢在自己的计算机上工作，请继续阅读本指南以了解向Symfony文档发起拉取请求的替代方法。

你的第一份文件贡献
-------------------------------------

在本节中，你将学习如何首次参与Symfony文档。
下一部分将解释你在第一次之后的每个贡献将要遵循的较短流程。

让我们假设你想要改进安装指南。要进行更改，请按以下步骤操作：

**步骤1.** 转到位于 `github.com/symfony/symfony-docs`_ 的官方Symfony文档仓库，
然后单击 **Fork** 按钮将仓库分叉到你的个人帐户。只有在你第一次参与Symfony贡献时才需要这样做。

**步骤2.** 克隆分叉库到本地机器（本例中使用 ``projects/symfony-docs/`` 目录来存储文件;相应地更改此值）：

.. code-block:: terminal

    $ cd projects/
    $ git clone git://github.com/YOUR-GITHUB-USERNAME/symfony-docs.git

**步骤3.** 将原始Symfony文档仓库库添加为执行此命令的“Git remote”：

.. code-block:: terminal

    $ cd symfony-docs/
    $ git remote add upstream https://github.com/symfony/symfony-docs.git

如果情况正常，在列出项目的“remotes”时会看到以下内容：

.. code-block:: terminal

    $ git remote -v
    origin  git@github.com:YOUR-GITHUB-USERNAME/symfony-docs.git (fetch)
    origin  git@github.com:YOUR-GITHUB-USERNAME/symfony-docs.git (push)
    upstream  https://github.com/symfony/symfony-docs.git (fetch)
    upstream  https://github.com/symfony/symfony-docs.git (push)

通过执行以下命令获取上游分支的所有提交：

.. code-block:: terminal

    $ git fetch upstream

此步骤的目的是允许你在官方Symfony仓库和你自己的fork上同时工作。你马上就会看到这一点。

**步骤4.** 为你的更改创建专用的 **新分支**。为新分支使用简短且令人难忘的名称（如果要修复一个已报告的issue，使用 ``fix_XXX`` 作为其分支名称， ``XXX`` 代表该issue的编号）：

.. code-block:: terminal

    $ git checkout -b improve_install_article upstream/3.4

在此示例中，分支的名称是 ``improve_install_article``，并且该 ``upstream/3.4``
值告诉Git基于 ``upstream`` 远程的 ``3.4`` 分支创建此分支，该分支是原始的Symfony文档仓库。

修复应始终基于包含错误的最旧维护分支。现在是 ``3.4`` 分支。
如果你正在书写新功能，请切换到包含它的第一个Symfony版本，例如 ``upstream/3.1``。
不确定是哪个分支？没关系！只需使用 ``upstream/master`` 分支。

**步骤5.** 现在在文档中进行更改。添加、调整、重新创建甚至删除任何内容，并尽力遵守
:doc:`/contributing/documentation/standards`。然后提交你的更改！

.. code-block:: terminal

    # if the modified content existed before
    $ git add setup.rst
    $ git commit setup.rst

**步骤6.** 将更改推送到分叉(forked)仓库：

.. code-block:: terminal

    $ git push origin improve_install_article

``origin`` 值是与分叉仓库对应的Git远程名称，而 ``improve_install_article`` 是你之前创建的分支的名称。

**步骤7.** 现在一切都准备好发起 **拉取请求**。
转到你位于 ``https://github.com/YOUR-GITHUB-USERNAME/symfony-docs`` 的分叉仓库，
然后单击侧栏中的 **Pull Requests** 链接。

然后，单击 **New pull request** 大按钮。
由于GitHub无法猜测你想要提出的确切更改，请选择应该应用更改的相应分支：

.. image:: /_images/contributing/docs-pull-request-change-base.png
   :align: center

在此示例中，**base fork** 应该是 ``symfony/symfony-docs``，并且 **base** 分支应该是 ``3.4``，
就是选择你更改的那个分支。
**head fork** 应该是你的分叉副本 ``symfony-docs``，**compare** 分支应该是 ``improve_install_article``，
这是你所创建的分支的名称以及你做修改的地方。

.. _pull-request-format:

**步骤8.** 最后一步是准备拉取请求的 **描述**。一个描述变更的短语或段落足以确保你的贡献可以得到审核。

**步骤9.** 既然你已成功提交了Symfony文档的第一份文稿，那就去庆祝吧！
文档管理员将在短时间内仔细审核你的作品，他们会告知你任何所需的更改。

如果要求你添加或修改某些内容，请不要创建新的拉取请求。
相反，请确保你在正确的分支上进行更改并推送新的更改：

.. code-block:: terminal

    $ cd projects/symfony-docs/
    $ git checkout improve_install_article

    # ... do your changes

    $ git push

**步骤10.** 在最终接受并在Symfony文档中合并你的请求后，你将被包含在 `Symfony文档贡献者`_ 列表中。
此外，如果你碰巧拥有一个 `SymfonyConnect`_ 账号，你将获得一个很酷的 `Symfony文档徽章`_。

你的下一个文档贡献
-------------------------------------

看看你！你已经为Symfony文档做出了第一个贡献！有人要举办派对了！
你的第一个贡献需要一些额外的时间，因为你需要学习一些标准并设置你的计算机。
但是从现在开始，你的贡献将更容易完成。

这是一个 **步骤清单**，将指导你完成对Symfony文档的下一个贡献：

.. code-block:: terminal

    # 基于最早维护的版本创建一个新分支
    $ cd projects/symfony-docs/
    $ git fetch upstream
    $ git checkout -b my_changes upstream/3.4

    # ... 做修改工作

    # (可选) 如果这是一个新内容，请添加它
    $ git add xxx.rst

    # 提交你的更改并将它们推送到你的fork
    $ git commit xxx.rst
    $ git push origin my_changes

    # ... 转到GitHub并创建拉取请求

    # (可选) 按照审阅者的要求进行更改并提交
    $ git commit xxx.rst
    $ git push

完成下一个贡献后，还可以在 `Symfony文档贡献者`_ 列表中查看你的排名。
你猜对了：经过这么辛苦的工作，**是时候再次庆祝了！**

检查你的更改
-------------------

`SymfonyCloud`_ 会在单独环境中自动构建和部署每个GitHub拉取请求，你可以在浏览器上访问该环境以查看更改。

.. image:: /_images/contributing/docs-pull-request-symfonycloud.png
   :align: center
   :alt:   SymfonyCloud Pull Request Deployment

要访问 `SymfonyCloud`_ 环境的URL，请转到GitHub上的Pull Request页面，单击 **Show all checks** 链接，
最后单击 ``Details`` 来显示SymfonyCloud服务的链接。

.. note::

    SymfonyCloud仅自动构建对维护分支的拉取请求。查阅维护分支的 `路线图`_ 。

在本地构建文档
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果您的计算机上安装了Docker，请运行以下命令来构建文档：

.. code-block:: terminal

    # 构建镜像...
    $ docker build . -t symfony-docs

    # ...然后启动本地Web服务器
    # （如果'8080'端口已经在使用中，请更改为其他任何端口）
    $ docker run --rm -p 8080:80 symfony-docs

你现在可以在 ``http://127.0.0.1:8080``
上阅读文档（如果你使用虚拟机，则浏览其IP而不是localhost；例如 ``http://192.168.99.100:8080``）。

如果你不使用Docker，请按照以下步骤在本地构建文档：

#. 按照 `pip安装`_ 文档中的说明安装 `pip`_;

#. 安装 `Sphinx`_ 和 `PHP和Symfony的Sphinx扩展`_ （根据你的系统，你可能需要以root用户身份执行此命令）：

   .. code-block:: terminal

        $ pip install sphinx~=1.3.0 git+https://github.com/fabpot/sphinx-php.git

#. 运行以下命令以HTML格式构建文档：

   .. code-block:: terminal

       $ cd _build/
       $ make html

生成的文档可在 ``_build/html`` 目录中找到。

常见问题
--------------------------

为什么我的更改需要很长时间才能被审核和/或合并？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

请耐心等待。你的拉取请求可能需要几天才能完全审核。合并更改后，可能需要几个小时才能在symfony.com网站上显示更改。

为什么我应该使用最旧的维护分支而不是主分支？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

与Symfony的源代码一致，文档仓库被拆分为多个分支，对应于Symfony本身的不同版本。
而 ``master`` 分支对应的是代码开发分支的文档。

除非你书写在Symfony3.4之后引入的功能，否则你的更改应始终基于 ``3.4`` 分支。
文档管理员将使用必要的Git-magic将你的更改应用于文档的所有活动分支。

如果我想在没有完全完成的情况下提交作品怎么办？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你可以这样做。但请使用这两个前缀中的一个让审阅者了解你的工作状态：

* ``[WIP]`` （正在进行中）当你尚未完成拉取请求，但你希望对其进行审核是，会使用它。
  在你说准备就绪之前，拉取请求不会合并。

* ``[WCM]`` （等待代码合并）当你书写一个新功能或尚未被接收到核心代码的更改时，将使用它。
  拉取请求在合并进核心代码之前不会合并（如果更改被拒绝，则会关闭拉取请求）。

你会接受一个有很多变化的巨大拉动请求吗？
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

首先，确保这些更改都有关联。否则，请创建单独的拉取请求。
无论如何，在提交巨大的更改之前，在Symfony文档仓库中打开一个issue来询问管理员是否同意你提议的更改可能是一个好主意，。
否则，在你付出所有努力进行更改之后，他们可能拒绝你的提案。我们绝对不希望你浪费自己的时间！

.. _`github.com/symfony/symfony-docs`: https://github.com/symfony/symfony-docs
.. _`reStructuredText`: http://docutils.sourceforge.net/rst.html
.. _`GitHub`: https://github.com/
.. _`fork the repository`: https://help.github.com/articles/fork-a-repo
.. _`Symfony文档贡献者`: https://symfony.com/contributors/doc
.. _`SymfonyConnect`: https://connect.symfony.com/
.. _`Symfony文档徽章`: https://connect.symfony.com/badge/36/symfony-documentation-contributor
.. _`sync your fork`: https://help.github.com/articles/syncing-a-fork
.. _`SymfonyCloud`: https://symfony.com/cloud
.. _`路线图`: https://symfony.com/roadmap
.. _`pip`: https://pip.pypa.io/en/stable/
.. _`pip安装`: https://pip.pypa.io/en/stable/installing/
.. _`Sphinx`: http://sphinx-doc.org/
.. _`PHP和Symfony的Sphinx扩展`: https://github.com/fabpot/sphinx-php
