.. index::
    single: Cache; Invalidation
    single: Cache; Tags

缓存失效
==================

缓存失效是删除与你的模型状态更改相关的所有缓存项的过程。
最基本的失效类型是直接删除缓存项。但是当主要资源的状态分散在多个缓存项中时，要保持它们同步可能很困难。

Symfony的缓存组件提供了两种机制来帮助解决此问题：

* :ref:`基于标签的失效 <cache-component-tags>` 用来管理数据依赖;
* :ref:`基于过期的失效 <cache-component-expiration>` 用来管理相关依赖.

.. _cache-component-tags:

使用缓存标签
----------------

要从基于标签的失效中受益，你需要将适当的标签附加到每个缓存项中。
每个标签都是一个纯字符串标识符，你可以随时使用它来触发与此标签关联的所有缓存项的删除。

要将标签附加到缓存项，你需要使用由使用缓存项实现的
:method:`Symfony\\Contracts\\Cache\\ItemInterface::tag` 方法::

    $item = $cache->get('cache_key', function (ItemInterface $item) {
        // [...]
        // 添加一个或更多标签
        $item->tag('tag_1');
        $item->tag(['tag_2', 'tag_3']);

        return $cachedValue;
    });

如果 ``$cache`` 实现了
:class:`Symfony\\Contracts\\Cache\\TagAwareCacheInterface`，则可以通过调用
:method:`Symfony\\Contracts\\Cache\\TagAwareCacheInterface::invalidateTags`
方法来使缓存项无效::

    // 使所有与“tag_1”或“tag_3”相关的缓存项无效
    $cache->invalidateTags(['tag_1', 'tag_3']);

    // 如果知道缓存键，也可以直接删除该项
    $cache->deleteItem('cache_key');

在跟踪缓存键变得困难时，使用标签失效会非常有用。

标签感知适配器
~~~~~~~~~~~~~~~~~~

要存储标签，你需要使用
:class:`Symfony\\Component\\Cache\\Adapter\\TagAwareAdapter` 类或实现了
:class:`Symfony\\Contracts\\Cache\\TagAwareCacheInterface`
及其唯一
:method:`Symfony\\Component\\Cache\\Adapter\\TagAwareAdapterInterface::invalidateTags`
方法封装的缓存适配器。

:class:`Symfony\\Component\\Cache\\Adapter\\TagAwareAdapter`
类实现瞬间失效（时间复杂度是 ``O(N)``，其中 ``N`` 为无效标签的数量）。
它需要一个或两个缓存适配器：第一个必需的缓存适配器用于存储缓存项;
第二个可选适配器用于存储标签及其无效版本号（概念上类似于其最新的无效日期）。
当只使用一个适配器时，缓存的项和标签都存储在同一个地方。通过使用两个适配器，你可以将一些大的缓存项存储在文件系统或数据库中，并将标签保存在一个Redis数据库中，以同步你的所有前端并进行非常快速的失效检查::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;
    use Symfony\Component\Cache\Adapter\RedisAdapter;
    use Symfony\Component\Cache\Adapter\TagAwareAdapter;

    $cache = new TagAwareAdapter(
        // 用于储存缓存项的适配器
        new FilesystemAdapter(),
        // 用于储存标签的适配器
        new RedisAdapter('redis://localhost')
    );

.. note::

    从Symfony3.4开始，:class:`Symfony\\Component\\Cache\\Adapter\\TagAwareAdapter` 实现了
    :class:`Symfony\\Component\\Cache\\PruneableInterface`，已允许通过调用其 ``prune()``
    方法来手动 :ref:`清理过期的缓存项 <component-cache-cache-pool-prune>`。

.. _cache-component-expiration:

使用缓存过期
----------------------

如果你的数据仅在有限的时间段内有效，则可以如
:doc:`/components/cache/cache_items` 文档中所述，使用PSR-6接口来指定其生命周期或到期日期。
