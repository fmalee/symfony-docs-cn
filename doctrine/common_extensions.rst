.. index::
   single: Doctrine; Common extensions

如何使用Doctrine扩展：Timestampable，Sluggable，Translatable等。
============================================================================

Doctrine2非常灵活，社区已经创建了一系列有用的Doctrine扩展来帮助你完成与实体相关的常见任务。

特别是一个库 - `DoctrineExtensions`_ 库 -
为 `Sluggable`_、`Translatable`_、`Timestampable`_、`Loggable`_、`Tree`_ 和 `Sortable`_
行为提供集成功能。

在该仓库中解释了每个扩展的用法。

但是，要安装/激活每个扩展，你必须注册并激活 :doc:`事件监听器 </doctrine/event_listeners_subscribers>`。
为此，你有两种选择：

#. 使用 `StofDoctrineExtensionsBundle`_，它集成了上面的库。

#. 通过遵循与Symfony集成的文档直接实现此服务：`在Symfony2中安装Gedmo Doctrine2扩展`_

.. _`DoctrineExtensions`: https://github.com/Atlantic18/DoctrineExtensions
.. _`StofDoctrineExtensionsBundle`: https://symfony.com/doc/master/bundles/StofDoctrineExtensionsBundle/index.html
.. _`Sluggable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/sluggable.md
.. _`Translatable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/translatable.md
.. _`Timestampable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/timestampable.md
.. _`Loggable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/loggable.md
.. _`Tree`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/tree.md
.. _`Sortable`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/sortable.md
.. _`在Symfony2中安装Gedmo Doctrine2扩展`: https://github.com/Atlantic18/DoctrineExtensions/blob/master/doc/symfony2.md
