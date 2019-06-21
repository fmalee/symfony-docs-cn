配置
=============

配置信息通常被细分到程序的不同部分（比如基本信息和安全凭证）和不同的环境（开发环境、生产环境）。
这就是为何Symfony推荐你将程序配置分为三部分的原因。

基础架构相关配置
------------------------------------

这些是从一台计算机更改为另一台计算机的选项（例如，从你的开发计算机更改为生产服务器），但不会更改应用行为。

.. best-practice::

    将基础结构相关的配置选项定义为 :ref:`环境变量 </configuration/environment_variables>`。
    在开发过程中，使用项目根目录下的 ``.env`` 和 ``.env.local`` 文件来设置它们。

默认情况下，在应用中安装新的依赖项时，Symfony会将这些类型的选项添加到 ``.env`` 文件中：

.. code-block:: bash

    # .env
    ###> doctrine/doctrine-bundle ###
    DATABASE_URL=sqlite:///%kernel.project_dir%/var/data/blog.sqlite
    ###< doctrine/doctrine-bundle ###

    ###> symfony/swiftmailer-bundle ###
    MAILER_URL=smtp://localhost?encryption=ssl&auth_mode=login&username=&password=
    ###< symfony/swiftmailer-bundle ###

    # ...

这些选项未在 ``config/services.yaml`` 文件中定义，因为它们与应用的行为无关。
换句话说，只要数据库配置正确，你的应用就不关心数据库的位置或访问它的凭据。

要使用机器特定值(machine-specific)或敏感值来重写这些变量，请创建一个 ``.env.local`` 文件。
此文件不应该添加到版本控制中。

.. caution::

    请注意，打印(dumping) ``$ _SERVER`` 和 ``$ _ENV`` 变量的内容或输出 ``phpinfo()`` 内容将显示环境变量的值，
    从而暴露敏感信息，如数据库凭据。

规范参数
~~~~~~~~~~~~~~~~~~~~

.. best-practice::

    在 ``.env`` 文件中定义应用的所有环境变量。

Symfony在项目根目录中包含一个名为 ``.env`` 的配置文件，它存储这应用的环境变量的规范(Canonical)列表。
此文件应存储在版本控制中，因此只应包含非敏感的默认值。

.. caution::

    在2018年11月之前创建的应用有一个稍微不同的系统，涉及一个 ``.env.dist`` 文件。
    有关升级的信息，请参阅 :doc:`/configuration/dot-env-changes`。

应用相关的配置
---------------------------------

.. best-practice::

    在 ``config/services.yaml`` 文件中定义与应用行为相关的配置选项。

``services.yaml`` 文件包含应用用于修改其行为的选项，例如电子邮件通知的发件人或启用 `功能切换`_。
将这些配置值定义在 ``.env`` 是不必要的，因为这将增加一个额外的配置层，而你不需要或不希望在每个服务器上更改这些配置值。

``services.yaml`` 中定义的配置选项可能因 :ref:`环境 </configuration/environments>` 而异。
这也能解释为什么Symfony还自带了 ``config/services_dev.yaml`` 和 ``config/services_prod.yaml`` 文件，它们能让你覆写针对不同环境的特定的选项值。

常量 v.s. 配置选项
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

一个常见的错误是，在定义程序级别的配置时去新建一个“永远不变”的全新选项，比如分页中所显示的条目数。

.. best-practice::

    使用常量来定义很少更改的配置选项。

定义配置选项的传统方法已导致许多Symfony应用包含如下选项，用于控制要在博客主页上显示的帖子数量：

.. code-block:: yaml

    # config/services.yaml
    parameters:
        homepage.number_of_items: 10

如果你已经这样做了，实际上你可能 *很少* 去改变这些值。
创建一个配置选项然后从不去改变它，那就是不必要。
我们推荐你将这些值定义为常量，比如你可以在 ``Post`` 实体中定义一个 ``NUMBER_OF_ITEMS`` 常量::

    // src/Entity/Post.php
    namespace App\Entity;

    class Post
    {
        const NUMBER_OF_ITEMS = 10;

        // ...
    }

这样做的好处是你可以在程序中的任何地方使用这个值。而使用参数时，你只能通过使用容器来访问它们。

常量可以在Twig模板中使用，多亏了 `constant()函数`_：

.. code-block:: html+twig

    <p>
        Displaying the {{ constant('NUMBER_OF_ITEMS', post) }} most recent results.
    </p>

并且，Doctrine实体和仓库也可以访问这些值，而它们无法访问容器参数::

    namespace App\Repository;

    use App\Entity\Post;
    use Doctrine\ORM\EntityRepository;

    class PostRepository extends EntityRepository
    {
        public function findLatest($limit = Post::NUMBER_OF_ITEMS)
        {
            // ...
        }
    }

使用常量作为配置值的唯一显着缺点是在测试中重新定义它们的值会很复杂。

参数命名
----------------

.. best-practice::

    配置参数的名称应尽可能短，并且应包含整个应用的公共前缀。

使用 ``app.`` 作为参数前缀是避免Symfony和第三方bundles库的参数冲突的常见做法。
然后，只用一两个词来描述参数的用途：

.. code-block:: yaml

    # config/services.yaml
    parameters:
        # 不要这样做：'dir' 太通用了，它没有任何意义
        app.dir: '...'
        # 这样做：简短而易懂的名字
        app.contents_dir: '...'
        # 可以使用点号、下划线、短划线或任何内容，但应该始终保持一致并对所有参数使用相同的格式
        app.dir.contents: '...'
        app.contents-dir: '...'

----

下一章: :doc:`/best_practices/business-logic`

.. _`功能切换`: https://en.wikipedia.org/wiki/Feature_toggle
.. _`constant()函数`: https://twig.symfony.com/doc/2.x/functions/constant.html
