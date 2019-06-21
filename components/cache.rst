.. index::
   single: Cache
   single: Performance
   single: Components; Cache

.. _`cache-component`:

Cache组件
===================

    The Cache component provides features covering simple to advanced caching needs.
    It natively implements `PSR-6`_ and the `Cache Contracts`_ for greatest
    interoperability. It is designed for performance and resiliency, ships with
    ready to use adapters for the most common caching backends. It enables tag-based
    invalidation and cache stampede protection via locking and early expiration.

.. tip::

    The component also contains adapters to convert between PSR-6, PSR-16 and
    Doctrine caches. See :doc:`/components/cache/psr6_psr16_adapters` and
    :doc:`/components/cache/adapters/doctrine_adapter`.

安装
------------

.. code-block:: terminal

    $ composer require symfony/cache

.. include:: /components/require_autoload.rst.inc

缓存契约 & PSR-6
----------------------------

本组件包括 *两种* 不同的缓存方法：

:ref:`PSR-6 Caching <cache-component-psr6-caching>`:
    A generic cache system, which involves cache "pools" and cache "items".

:ref:`Cache Contracts <cache-component-contracts>`:
    A simpler yet more powerful way to cache values based on recomputation callbacks.

.. tip::

    Using the Cache Contracts approach is recommended: it requires less
    code boilerplate and provides cache stampede protection by default.

.. _cache-component-contracts:

缓存契约
---------------

All adapters support the Cache Contracts. They contain only two methods:
``get()`` and ``delete()``. There's no ``set()`` method because the ``get()``
method both gets and sets the cache values.

The first thing you need is to instantiate a cache adapter. The
:class:`Symfony\\Component\\Cache\\Adapter\\FilesystemAdapter` is used in this
example::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;

    $cache = new FilesystemAdapter();

Now you can retrieve and delete cached data using this object. The first
argument of the ``get()`` method is a key, an arbitrary string that you
associate to the cached value so you can retrieve it later. The second argument
is a PHP callable which is executed when the key is not found in the cache to
generate and return the value::

    use Symfony\Contracts\Cache\ItemInterface;

    // The callable will only be executed on a cache miss.
    $value = $cache->get('my_cache_key', function (ItemInterface $item) {
        $item->expiresAfter(3600);

        // ... do some HTTP request or heavy computations
        $computedValue = 'foobar';

        return $computedValue;
    });

    echo $value; // 'foobar'

    // ... and to remove the cache key
    $cache->delete('my_cache_key');

.. note::

    Use cache tags to delete more than one key at the time. Read more at
    :doc:`/components/cache/cache_invalidation`.

The Cache Contracts also comes with built in `Stampede prevention`_. This will
remove CPU spikes at the moments when the cache is cold. If an example application
spends 5 seconds to compute data that is cached for 1 hour and this data is accessed
10 times every second, this means that you mostly have cache hits and everything
is fine. But after 1 hour, we get 10 new requests to a cold cache. So the data
is computed again. The next second the same thing happens. So the data is computed
about 50 times before the cache is warm again. This is where you need stampede
prevention

The first solution is to use locking: only allow one PHP process (on a per-host basis)
to compute a specific key at a time. Locking is built-in by default, so
you don't need to do anything beyond leveraging the Cache Contracts.

The second solution is also built-in when using the Cache Contracts: instead of
waiting for the full delay before expiring a value, recompute it ahead of its
expiration date. The `Probabilistic early expiration`_ algorithm randomly fakes a
cache miss for one user while others are still served the cached value. You can
control its behavior with the third optional parameter of
:method:`Symfony\\Contracts\\Cache\\CacheInterface::get`,
which is a float value called "beta".

By default the beta is ``1.0`` and higher values mean earlier recompute. Set it
to ``0`` to disable early recompute and set it to ``INF`` to force an immediate
recompute::

    use Symfony\Contracts\Cache\ItemInterface;

    $beta = 1.0;
    $value = $cache->get('my_cache_key', function (ItemInterface $item) {
        $item->expiresAfter(3600);
        $item->tag(['tag_0', 'tag_1']);

        return '...';
    }, $beta);

可用的缓存适配器
~~~~~~~~~~~~~~~~~~~~~~~~

有以下缓存适配器可用：

.. toctree::
    :glob:
    :maxdepth: 1

    cache/adapters/*

.. _cache-component-psr6-caching:

通用缓存（PSR-6）
-----------------------------

要使用通用的PSR-6缓存功能，你需要了解其关键概念：

**项(Item)**
    作为键/值对存储的单个信息单元，其中键是信息的唯一标识符，值是其内容;
    有关更多详细信息，请参阅 :doc:`/components/cache/cache_items` 一文。
**池(Pool)**
    缓存项的逻辑仓库。所有缓存操作（保存项、查找项等）都通过池执行。应用可根据需要定义任意数量的池。
**适配器(Adapter)**
    它实现了将信息存储在文件系统、数据库等中的实际缓存机制。
    本组件为常见的缓存后端提供了几个现成的适配器（Redis，APCu，Doctrine，PDO等）。

基本用法（PSR-6）
-------------------

本组件的这一部分是 `PSR-6`_ 的一个实现，这意味着它的基本API与文档中定义的相同。
在开始缓存信息之前，请使用任何内置适配器来创建缓存池。
例如，要创建基于文件系统的缓存，请实例化
:class:`Symfony\\Component\\Cache\\Adapter\\FilesystemAdapter`::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;

    $cache = new FilesystemAdapter();

现在，你可以使用此缓存池来创建、检索、更新和删除缓存项::

    // 通过尝试从缓存中获取新项来创建新项
    $productsCount = $cache->getItem('stats.products_count');

    // 为缓存项指定值并保存它
    $productsCount->set(4711);
    $cache->save($productsCount);

    // 检索缓存项
    $productsCount = $cache->getItem('stats.products_count');
    if (!$productsCount->isHit()) {
        // ... 缓存中不存在对应项
    }
    // 检索缓存项中存储的值
    $total = $productsCount->get();

    // 删除缓存项
    $cache->deleteItem('stats.products_count');

关于所有受支持的适配器的列表，请参阅缓 :doc:`/components/cache/cache_pools`。

高级用法
----------------------

.. toctree::
    :glob:
    :maxdepth: 1

    cache/*

.. _`PSR-6`: http://www.php-fig.org/psr/psr-6/
.. _`Cache Contracts`: https://github.com/symfony/contracts/blob/master/Cache/CacheInterface.php
.. _`PSR-16`: http://www.php-fig.org/psr/psr-16/
.. _Doctrine Cache: https://www.doctrine-project.org/projects/cache.html
.. _Stampede prevention: https://en.wikipedia.org/wiki/Cache_stampede
.. _Probabilistic early expiration: https://en.wikipedia.org/wiki/Cache_stampede#Probabilistic_early_expiration
