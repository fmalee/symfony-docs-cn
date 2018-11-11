.. index::
    single: Upgrading; Minor Version

升级次要版本（例如4.0.0到4.1.0）
===============================================

如果你在升级一个次要版本（中间的数字发生变化），那么你应该不会遇到显著的向后兼容性改变。
有关详细信息，请参阅 :doc:`Symfony向后兼容性承诺 </contributing/code/bc>`。

但是，一些向后兼容性破坏 *是* 可能发生的，你将在短时间内学会如何为它们做好准备。

升级次要版本有两个步骤：

#. :ref:`通过Composer更新Symfony库 <upgrade-minor-symfony-composer>`;
#. :ref:`更新你的代码以使用新版本 <upgrade-minor-symfony-code>`.

.. _`upgrade-minor-symfony-composer`:

1) 通过Composer更新Symfony库
------------------------------------------

你的 ``composer.json`` 文件应该已配置为允许将Symfony软件包升级到次要版本。
但是，如果一个软件包未升级，请检查Symfony依赖项的版本约束是否如下所示：

.. code-block:: json

    {
        "...": "...",

        "require": {
            "symfony/cache": "^4.0",
            "symfony/config": "^4.0",
            "symfony/console": "^4.0",
            "symfony/debug": "^4.0",
            "symfony/dependency-injection": "^4.0",
            "symfony/dotenv": "^4.0",
            "...": "..."
        },
        "...": "...",
    }

接下来，使用Composer下载新版本的库：

.. code-block:: terminal

    $ composer update "symfony/*" --with-all-dependencies

.. include:: /setup/_update_dep_errors.rst.inc

.. include:: /setup/_update_all_packages.rst.inc

.. _`upgrade-minor-symfony-code`:

2) 更新你的代码以使用新版本
--------------------------------------------------

从理论上讲，你应该已经完工了！但是，你 *可能* 需要对代码进行一些更改才能使一切正常运行。
此外，你正在使用的某些功能可能仍然有效，但现在可能已被弃用。
虽然依然能正常运行，但如果你知道这些弃用的代码，就可以开始修复它们了。

每个版本的Symfony都附带一个包含在Symfony目录中的UPGRADE文件（例如 `UPGRADE-4.1.md`_），用于描述这些更新。
如果你按照文档中的说明进行操作并相应地更新代码，那么对于将来的更新应该更安全。

这些文档也可以在 `Symfony仓库`_ 中找到。

.. _`Symfony仓库`: https://github.com/symfony/symfony
.. _`UPGRADE-4.1.md`: https://github.com/symfony/symfony/blob/4.1/UPGRADE-4.1.md
