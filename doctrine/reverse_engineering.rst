.. index::
   single: Doctrine; Generating entities from existing database

如何从现有数据库生成实体
==================================================

在开始使用数据库的全新项目时，自然会出现两种不同的情况。
在大多数情况下，数据库模型是从头开始设计和构建的。
但是，有时你会从现有的、可能不可更改的数据库模型开始。
幸运的是，Doctrine附带了许多工具来帮助从现有数据库生成模型类。

.. note::

    正如 `Doctrine工具文档`_ 所说，逆向工程是一个开始项目的一次性过程。
    Doctrine能够根据字段、索引和外键约束转换大约70-80％的必要映射信息。
    Doctrine不能发现反向关联、继承类型、具有外键作为主键的实体或对诸如级联或生命周期事件之类的关联的语义操作。
    之后，有必要对生成的实体进行一些额外的工作，以便根据你的域模型特性进行设计。

本教程假设你使用的是一个简单的博客应用，其中包含以下两个表：``blog_post`` 和 ``blog_comment``。
由于外键约束，一个评论记录链接到一个帖子记录。

.. code-block:: sql

    CREATE TABLE `blog_post` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT,
      `title` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
      `content` longtext COLLATE utf8_unicode_ci NOT NULL,
      `created_at` datetime NOT NULL,
      PRIMARY KEY (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

    CREATE TABLE `blog_comment` (
      `id` bigint(20) NOT NULL AUTO_INCREMENT,
      `post_id` bigint(20) NOT NULL,
      `author` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
      `content` longtext COLLATE utf8_unicode_ci NOT NULL,
      `created_at` datetime NOT NULL,
      PRIMARY KEY (`id`),
      KEY `blog_comment_post_id_idx` (`post_id`),
      CONSTRAINT `blog_post_id` FOREIGN KEY (`post_id`) REFERENCES `blog_post` (`id`) ON DELETE CASCADE
    ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

在深入了解指令之前，请确保在 ``.env`` 文件 (或 ``.env.local`` 重写文件)中正确设置了数据库连接参数。

从现有数据库构建实体类的第一步是要求Doctrine自检(introspect)数据库并生成相应的元数据文件。
该元数据文件描述要生成的基于表字段的实体类。

.. code-block:: terminal

    $ php bin/console doctrine:mapping:import 'App\Entity' annotation --path=src/Entity

此命令行工具要求Doctrine自检(introspect)数据库并生成带有注释元数据的新PHP类到 ``src/Entity``。
这会生成两个文件：``BlogPost.php`` 和 ``BlogComment.php``。

.. tip::

    也可以将元数据文件生成为XML或YAML：

    .. code-block:: terminal

        $ php bin/console doctrine:mapping:import 'App\Entity' xml --path=config/doctrine

生成Getters＆Setters或PHP类
-----------------------------------------------

生成的PHP类现在具有属性和注释元数据，但它们 *没有* 任何getter或setter方法。
如果你生成了XML或YAML元数据，那么你甚至没有PHP类！

要生成缺少的getter/setter方法（或者在必要时 *创建* 类），请运行：

.. code-block:: terminal

    // 生成 getter/setter 方法
    $ php bin/console make:entity --regenerate App

.. note::

    如果你想拥有OneToMany关系，则需要手动将其添加到实体
    （例如，添加 ``comments`` 属性到 ``BlogPost``）或生成的XML或YAML文件中。
    在特定实体上为一对多添加一个切片(section)，用于定义 ``inversedBy`` 和 ``mappedBy``。

现在可以使用生成的实体了。玩得开心！

.. _`Doctrine工具文档`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/tools.html#reverse-engineering
.. _`doctrine/doctrine#729`: https://github.com/doctrine/DoctrineBundle/issues/729
