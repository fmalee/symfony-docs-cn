.. index::
    single: Upgrading; Major Version

升级主要版本（例如3.4.0至4.1.0）
===============================================

每两年，Symfony都会发布一个新的主要版本（第一个数字发生变化）。
升级这些版本是最棘手的，因为它们可能破坏向后兼容性。但是，Symfony会让这个升级过程尽可能的顺利。

这意味着你可以在主要版本实际发布之前更新大部分代码。这称为使你的代码与 *未来兼容*。

升级一个主要版本有几个步骤：

#. :ref:`清理弃用的代码 <upgrade-major-symfony-deprecations>`;
#. :ref:`通过Composer更新到新的主要版本 <upgrade-major-symfony-composer>`;
#. :ref:`更新你的代码以使用新版本 <upgrade-major-symfony-after>`.

.. _upgrade-major-symfony-deprecations:

1) 清理弃用的代码
----------------------------------

在一个主要版本的生命周期中，添加了新功能，并更改了方法签名和公共API用法。
但是，:doc:`次要版本 </setup/upgrade_minor>` 不应包含任何不向后兼容的更改。
为了实现这一点，“旧”代码（例如函数、类等）会仍然有效，但被标记为 *已弃用*，表示未来将会删除/更改它并且你应该停止使用它。

发布主要版本（例如 ``4.1.0``）时，将删除所有已弃用的功能。
因此，只要你更新了代码以停止在主要版本之前的最后一个版本中使用这些已弃用的功能
（例如 ``3.4.*``），你应该能够毫无问题地进行升级。

为了帮助你解决此问题，每当你使用已弃用的功能时，都会触发弃用通知。
使用浏览器在 :doc:`开发环境 </configuration/environments>` 中访问应用时，这些通知会显示在Web开发工具栏中：

.. image:: /_images/install/deprecations-in-profiler.png
   :align: center
   :class: with-browser

归根结底，你应该停止使用已弃用的功能。有时这很容易：该警告可能会告诉你确切要改变的内容。

但其他时候，该警告可能不够明确：在某处设置可能会导致一个类触发更深层次的警告。
在这种情况下，Symfony会尽力给出明确的信息，但你可能需要进一步研究该警告。

有时，警告可能来自你正在使用的第三方库或bundle。
如果是这样，那么这些弃用很可能已经更新。在这种情况下，请升级库以修复它们。

一旦所有弃用警告消失，你就可以更自信地进行升级。

PHPUnit中的弃用
~~~~~~~~~~~~~~~~~~~~~~~

使用PHPUnit运行测试时，不会显示弃用通知。
为了帮助你解决这个问题，Symfony提供了一个PHPUnit桥接。
此桥接将会在测试报告的末尾为你显示所有弃用通知的摘要。

你需要做的就是安装PHPUnit桥接：

.. code-block:: terminal

    $ composer require --dev symfony/phpunit-bridge

现在，你可以开始修改该通知了：

.. code-block:: terminal

    # 在运行 "composer require --dev symfony/phpunit-bridge" 后该命令才可用
    $ ./bin/phpunit
    ...

    OK (10 tests, 20 assertions)

    Remaining deprecation notices (6)

    The "request" service is deprecated and will be removed in 3.0. Add a type-hint for
    Symfony\Component\HttpFoundation\Request to your controller parameters to retrieve the
    request instead: 6x
        3x in PageAdminTest::testPageShow from Symfony\Cmf\SimpleCmsBundle\Tests\WebTest\Admin
        2x in PageAdminTest::testPageList from Symfony\Cmf\SimpleCmsBundle\Tests\WebTest\Admin
        1x in PageAdminTest::testPageEdit from Symfony\Cmf\SimpleCmsBundle\Tests\WebTest\Admin

一旦你修复了所有警告，命令以 ``0`` （success）结束，你就完工可！

.. sidebar:: 使用弱弃用模式

    有时，你无法修复所有弃用（例如，某些功能已在3.4中弃用，但你仍然需要支持3.3）。
    在这些情况下，你仍然可以使用该桥接器来可能多的修复尽弃用，然后切换到弱测试模式以使测试再次通过。
    你可以通过使用 ``SYMFONY_DEPRECATIONS_HELPER`` 环境变量来完成此操作：

    .. code-block:: xml

        <!-- phpunit.xml.dist -->
        <phpunit>
            <!-- ... -->

            <php>
                <env name="SYMFONY_DEPRECATIONS_HELPER" value="weak"/>
            </php>
        </phpunit>

    （你也可以直接执行 ``SYMFONY_DEPRECATIONS_HELPER=weak phpunit`` 命令）。

.. _upgrade-major-symfony-composer:

2) 通过Composer更新到新的主要版本
-----------------------------------------------

一旦你的代码修复完成，你可以通过修改 ``composer.json`` 文件来使用Composer更新Symfony库：

.. code-block:: json

    {
        "...": "...",

        "require": {
            "symfony/symfony": "^4.1",
        },
        "...": "..."
    }

接下来，使用Composer下载新版本的库：

.. code-block:: terminal

    $ composer update symfony/symfony

.. include:: /setup/_update_dep_errors.rst.inc

.. include:: /setup/_update_all_packages.rst.inc

.. _upgrade-major-symfony-after:

3) 更新你的代码以使用新版本
------------------------------------------------

下一个主要版本也 *可能* 包含新的BC中断，因为BC层并不总是可行的。
确保你阅读了包含在Symfony仓库中的 ``UPGRADE-X.0.md`` （其中X是新的主要版本），以了解你需要注意的任何BC中断。

4) 更新到Symfony 4 Flex目录结构
-----------------------------------------------------

升级到Symfony 4时，你可能还希望升级到新的Symfony 4目录结构，以便可以利用Symfony Flex。
这需要一些工作，但是是可选的。有关详细信息，请参阅 :ref:`upgrade-to-flex`。

.. _`Symfony-Upgrade-Fixer`: https://github.com/umpirsky/Symfony-Upgrade-Fixer
