.. index::
   single: Bundles

.. _page-creation-bundles:

Bundle
=================

.. caution::

    在4.0之前的Symfony版本中，建议使用bundle来组织你自己的应用代码。
    现在不再推荐此选项，bundle只应该用来在多个应用之间共享代码和功能。

bundle类似于其他软件中的插件，但却更好。
Symfony框架的核心功能是使用bundle（FrameworkBundle，SecurityBundle，DebugBundle等）实现的。
它们还用于通过 `第三方bundle`_ 在应用程序中添加新功能。

在应用中使用bundel，必须在 ``config/bundles.php`` 文件中的每个 :doc:`环境 </configuration/environments>`
中启用它::

    // config/bundles.php
    return [
        // 'all' 意味着该bundle会在任何环境中使用
        Symfony\Bundle\FrameworkBundle\FrameworkBundle::class => ['all' => true],
        Symfony\Bundle\SecurityBundle\SecurityBundle::class => ['all' => true],
        Symfony\Bundle\TwigBundle\TwigBundle::class => ['all' => true],
        Symfony\Bundle\MonologBundle\MonologBundle::class => ['all' => true],
        Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle::class => ['all' => true],
        Doctrine\Bundle\DoctrineBundle\DoctrineBundle::class => ['all' => true],
        Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle::class => ['all' => true],
        // 该bundle在 'dev' 和 'test' 环境下启用, 所以你无法再 'prod' 环境下使用它
        Symfony\Bundle\WebProfilerBundle\WebProfilerBundle::class => ['dev' => true, 'test' => true],
    ];

.. tip::

    在使用 :doc:`Symfony Flex </setup/flex>` 的默认的Symfony应用中，
    在安装/删除bundle时会自动启用/禁用它们，因此你无需查看或编辑 ``bundles.php`` 文件。

创建Bundle
-----------------

此部分创建并启用新的bundle以显示执行此操作的简单程度。
新的bundle称为 AcmeTestBundle，其中 ``Acme`` 部分只是一个虚拟名称，
应该用代表你或你的组织的某个“vendor”名称来替换
（例如某个名为 ``ABC`` 的公司的ABCTestBundle）。

首先，创建一个 ``src/Acme/TestBundle/`` 目录并添加一个名为 ``AcmeTestBundle.php`` 的新文件::

    // src/Acme/TestBundle/AcmeTestBundle.php
    namespace App\Acme\TestBundle;

    use Symfony\Component\HttpKernel\Bundle\Bundle;

    class AcmeTestBundle extends Bundle
    {
    }

.. tip::

    AcmeTestBundle命名遵循标准的 :ref:`Bundle命名约定 <bundles-naming-conventions>`。
    你还可以通过命名此类为TestBundle（并命名文件 ``TestBundle.php``）来选择将bundle的名称简化为 TestBundle。

这个空类是创建新bundle所需的唯一部分。
虽然通常为空，但此类功能强大，可用于自定义bundle的行为。
现在你已经创建了bundle，启用它::

    // config/bundles.php
    return [
        // ...
        App\Acme\TestBundle\AcmeTestBundle::class => ['all' => true],
    ];

虽然它还没有做任何事情，但现在可以使用AcmeTestBundle了。

Bundle目录结构
--------------------------

bundle目录是简单而有弹性的。
默认条件下，bundle系统遵循着一组命名约定，以保持所有Symfony bundle的代码一致性。
看一眼AcmeDemoBundle，它包括了一个bundle最常见的某些元素：

``Controller/``
    里面有该bundle的控制器（如 ``RandomController.php``）。

``DependencyInjection/``
    里面有特定的依赖注入扩展类，用来导入服务配置信息，注册compiler passes，以及更多内容（这个目录并非必需）。

``Resources/config/``
    存放配置信息，包括路由配置（如 ``routing.yaml``）。

``Resources/views/``
    存放模板。依控制器名字来组织子文件夹（如 ``Random/index.html.twig``）。

``Resources/public/``
    存放web资源（图片，样式表等），通过console命令 ``assets:install``
    以复制或符号链接的方式导入到项目的 ``public/`` 目录。

``Tests/``
    存放bundle的所有测试类。

一个bundle依其实现的功能而或小或大。它只包含你需要的文件，再无其他。

在你通读指南的过程中，你将学到如何持久化对象到数据库中，创建和验证表单，为程序增加翻译功能，编写测试，以及更多内容。
这些中的每一个在bundel中都有自己的位置和角色。

扩展阅读
----------

* :doc:`/bundles/override`
* :doc:`/bundles/best_practices`
* :doc:`/bundles/configuration`
* :doc:`/bundles/extension`
* :doc:`/bundles/prepend_extension`

.. _`第三方bundle`: https://github.com/search?q=topic%3Asymfony-bundle&type=Repositories
