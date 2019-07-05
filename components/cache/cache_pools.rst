.. index::
    single: Cache Pool
    single: APCu Cache
    single: Array Cache
    single: Chain Cache
    single: Doctrine Cache
    single: Filesystem Cache
    single: Memcached Cache
    single: PDO Cache, Doctrine DBAL Cache
    single: Redis Cache

.. _component-cache-cache-pools:

缓存池及其支持的适配器
==================================

缓存池是缓存项的逻辑仓库。它们对缓存执行所有常见操作，例如保存或查找缓存项。
缓存池独立于实际的缓存实现。因此，即使底层缓存机制从基于文件系统的缓存更改为基于Redis或数据库的缓存，应用也可以继续使用相同的缓存池。

.. _component-cache-creating-cache-pools:

创建缓存池
--------------------

缓存池是通过 **缓存适配器** 创建的，缓存适配器是实现了
:class:`Symfony\\Contracts\\Cache\\CacheInterface` 和
``Psr\\Cache\\CacheItemPoolInterface``
的类。本组件提供了几个可在你的应用中使用的适配器。

.. toctree::
    :glob:
    :maxdepth: 1

    adapters/*

使用缓存契约
-------------------------

:class:`Symfony\\Contracts\\Cache\\CacheInterface`
允许仅使用两个方法和一个回调来获取、存储和删除缓存项::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;
    use Symfony\Contracts\Cache\ItemInterface;

    $cache = new FilesystemAdapter();

    // 可调用只会在高速缓存未命中时执行。
    $value = $cache->get('my_cache_key', function (ItemInterface $item) {
        $item->expiresAfter(3600);

        // ... 做一些HTTP请求或繁重的计算
        $computedValue = 'foobar';

        return $computedValue;
    });

    echo $value; // 'foobar'

    // ... 并删除缓存键
    $cache->delete('my_cache_key');

开箱即用，使用此接口的锁定和提前到期来提供踩踏保护(stampede protection)。
可以通过 :method:`Symfony\\Contracts\\Cache\\CacheInterface::get()`
方法的第三个参数 “beta” 来控制提前到期。有关更多信息，请参阅
:doc:`/components/cache` 一文。

可以通过调用 :method:`Symfony\\Contracts\\Cache\\ItemInterface::isHit()`
方法来在回调内检测提前到期：如果返回 ``true``，则表示我们当前正在重新计算其到期日期之前的值。

对于高级用例，回调可以接受通过引用传递的第二个参数 ``bool &$save``。
通过在回调中设置 ``$save`` 为 ``false``，你可以指示在后端的缓存池 *不要* 存储返回的值。

使用PSR-6
-----------

查找缓存项
~~~~~~~~~~~~~~~~~~~~~~~

缓存池定义了三种查找缓存项的方法。最常见的方法是返回给定键标识的缓存项的 ``getItem($key)``::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;

    $cache = new FilesystemAdapter('app.cache');
    $latestNews = $cache->getItem('latest_news');

如果没有为给定键定义任何项，该方法不会返回一个 ``null`` 值，而是返回一个实现
:class:`Symfony\\Component\\Cache\\CacheItem` 类的空对象。

如果需要同时获取多个缓存项，请使用 ``getItems([$key1, $key2, ...])`` 方法::

    // ...
    $stocks = $cache->getItems(['AAPL', 'FB', 'GOOGL', 'MSFT']);

同样，如果该键不表示任何有效的缓存项，不会获得一个 ``null`` 值，而是返回一个空的 ``CacheItem`` 对象。

与获取缓存项相关的最后一个方法是 ``hasItem($key)``，如果存在由给定键标识的缓存项，则返回 ``true``::

    // ...
    $hasBadges = $cache->hasItem('user_'.$userId.'_badges');

保存缓存项
~~~~~~~~~~~~~~~~~~

保存缓存项的最常用方法是将项立即存储在缓存中的
``Psr\Cache\CacheItemPoolInterface::save``（如果项已保存，返回
``true``，如果发生了一些错误，则返回 ``false``）::

    // ...
    $userFriends = $cache->getItem('user_'.$userId.'_friends');
    $userFriends->set($user->getFriends());
    $isSaved = $cache->save($userFriends);

有时你可能不希望立即保存对象以提高应用性能。在这些情况下，使用
``Psr\Cache\CacheItemPoolInterface::saveDeferred``
方法将缓存项标记为“准备好持久化”，然后在准备好将其全部持久化时调用
``Psr\Cache\CacheItemPoolInterface::commit`` 方法::

    // ...
    $isQueued = $cache->saveDeferred($userFriends);
    // ...
    $isQueued = $cache->saveDeferred($userPreferences);
    // ...
    $isQueued = $cache->saveDeferred($userRecentProducts);
    // ...
    $isSaved = $cache->commit();

当缓存项已成功添加到“持久队列”时，``saveDeferred()`` 方法返回 ``true``，否则返回
``false``。``commit()`` 方法在成功保存所有待处理项时返回 ``true``，否则返回 ``false``。

删除缓存项
~~~~~~~~~~~~~~~~~~~~

缓存池包含删除其中一些或全部缓存项的方法。最常见的是
``Psr\Cache\CacheItemPoolInterface::deleteItems``，
该方法删除由给定键标识的缓存项（当项成功删除或不存在给定缓存项时返回 ``true``，否则返回 ``false``）::

    // ...
    $isDeleted = $cache->deleteItem('user_'.$userId);

可以使用 ``Psr\Cache\CacheItemPoolInterface::deleteItem``
方法来同时删除多个缓存项（仅当所有项都已删除时才返回 ``true``，即使其中任何一个或部分项不存在）::

    // ...
    $areDeleted = $cache->deleteItems(['category1', 'category2']);

最后，要删除存储在池中的所有缓存项，请使用 ``Psr\Cache\CacheItemPoolInterface::clear``
方法（在成功删除所有项时返回 ``true``）::

    // ...
    $cacheIsEmpty = $cache->clear();

.. tip::

    如果在Symfony应用中使用缓存组件，则可以使用以下命令（驻留在
    :ref:`framework bundle <framework-bundle-configuration>`）从缓存池中删除缓存项：

    要从 *给定的池* 删除 *一个特定的项* ：

    .. code-block:: terminal

        $ php bin/console cache:pool:delete <cache-pool-name> <cache-key-name>

        # 从“cache.app”池中删除"cache_key"项
        $ php bin/console cache:pool:delete cache.app cache_key

    你还可以从 *给定池* 中删除 *所有缓存项*：

    .. code-block:: terminal

        $ php bin/console cache:pool:clear <cache-pool-name>

        # 清除 "cache.app" 池
        $ php bin/console cache:pool:clear cache.app

        # 清除 "cache.validation" 以及 "cache.app" 池
        $ php bin/console cache:pool:clear cache.validation cache.app

.. _component-cache-cache-pool-prune:

清理缓存项
-------------------

某些缓存池不包含用于清理(Pruning)过期缓存项的自动机制。
例如 :ref:`FilesystemAdapter <component-cache-filesystem-adapter>`
缓存不会删除过期的缓存项，*直到该项被明确请求并确定过期为止*，例如，调用了
``Psr\Cache\CacheItemPoolInterface::getItem``。
在某些工作负载下，这可能导致过时的缓存条目在过期后持续存在，从而导致过多的过期缓存项大量消耗磁盘或内存空间。

这个缺点已经通过引入 :class:`Symfony\\Component\\Cache\\PruneableInterface`
来解决，它定义了 :method:`Symfony\\Component\\Cache\\PruneableInterface::prune`
抽象方法。:ref:`ChainAdapter <component-cache-chain-adapter>`、
:ref:`FilesystemAdapter <component-cache-filesystem-adapter>`、
:ref:`PdoAdapter <pdo-doctrine-adapter>` 以及
:ref:`PhpFilesAdapter <component-cache-files-adapter>`
都实现了这个新的接口，以允许手动清除陈旧的缓存项::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;

    $cache = new FilesystemAdapter('app.cache');
    // ... 做一些 set 和 get 操作
    $cache->prune();

:ref:`ChainAdapter <component-cache-chain-adapter>` 实现自身不直接包含任何删除逻辑。
相反，在调用该适配器的
:method:`Symfony\\Component\\Cache\\Adapter\\ChainAdapter::prune`
方法时，会将该调用委托给其所有兼容的缓存适配器（而那些未实现
``PruneableInterface`` 的适配器将被静默忽略）::

    use Symfony\Component\Cache\Adapter\ApcuAdapter;
    use Symfony\Component\Cache\Adapter\ChainAdapter;
    use Symfony\Component\Cache\Adapter\FilesystemAdapter;
    use Symfony\Component\Cache\Adapter\PdoAdapter;
    use Symfony\Component\Cache\Adapter\PhpFilesAdapter;

    $cache = new ChainAdapter([
        new ApcuAdapter(),       // 未实现 PruneableInterface
        new FilesystemAdapter(), // 已经实现 PruneableInterface
        new PdoAdapter(),        // 已经实现 PruneableInterface
        new PhpFilesAdapter(),   // 已经实现 PruneableInterface
        // ...
    ]);

    // prune()将代理对PdoAdapter、FilesystemAdapter以及PhpFilesAdapter的调用，
    // 同时静默跳过ApcuAdapter
    $cache->prune();

.. tip::

    如果是在symfony应用内使用缓存组件，可以使用下面的命令（其驻留在
    :ref:`framework bundle <framework-bundle-configuration>`）来从 *所有的池* 中清理 *所有的项*：

    .. code-block:: terminal

        $ php bin/console cache:pool:prune
