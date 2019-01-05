.. index::
   single: Cache
   single: Performance
   single: Components; Cache

.. _`cache-component`:

Cache组件
===================

    Cache组件提供扩展的 `PSR-6`_ 实现以及一个 `PSR-16`_ “简单缓存”实现，用于向应用添加缓存。
    它专为提高性能和弹性而设计，附带最常见的缓存后端适配器，包括用于 `Doctrine Cache`_ 的代理。

安装
------------

.. code-block:: terminal

    $ composer require symfony/cache

或者，你可以克隆 `<https://github.com/symfony/cache>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

缓存（PSR-6）& 简单缓存（PSR-16）
------------------------------------------

本组件包括 *两种* 不同的缓存方法：

:ref:`PSR-6缓存 <cache-component-psr6-caching>`:
     一个功能齐全的缓存系统，包括缓存“池”、更高级的缓存“项”以及
     :ref:`用于实现失效的缓存标签 <cache-component-tags>`。

:ref:`PSR-16简单缓存 <cache-component-psr16-caching>`:
    一种从缓存中存储、获取和删除缓存项的简单方法。

两种方法都支持 *相同* 的缓存适配器，并且可以提供非常相近的性能。

.. tip::

    本组件还包含用于在PSR-6和PSR-16缓存之间进行转换的适配器。请参阅
    :doc:`/components/cache/psr6_psr16_adapters`。

.. _cache-component-psr16-caching:

简单缓存（PSR-16）
-----------------------

本组件的这一部分是 `PSR-16`_ 的一个实现，这意味着它的基本API与标准中定义的相同。
首先，从其中一个内置缓存类中创建缓存对象。例如，要创建基于文件系统的缓存，请实例化
:class:`Symfony\\Component\\Cache\\Simple\\FilesystemCache`::

    use Symfony\Component\Cache\Simple\FilesystemCache;

    $cache = new FilesystemCache();

现在，你可以使用此对象创建、检索、更新和删除缓存项::

    // 在缓存中保存新项
    $cache->set('stats.products_count', 4711);

    // 或者用自定义TTL设置
    // $cache->set('stats.products_count', 4711, 3600);

    // 检索缓存项
    if (!$cache->has('stats.products_count')) {
        // ... 缓存中不存在对应项
    }

    // 检索该缓存项存储的值
    $productsCount = $cache->get('stats.products_count');

    // 或者指定默认值（如果该键不存在）
    // $productsCount = $cache->get('stats.products_count', 100);

    // 删除缓存键
    $cache->delete('stats.products_count');

    // 清除 *所有* 缓存键
    $cache->clear();

你还可以同时处理多个缓存项::

    $cache->setMultiple(array(
        'stats.products_count' => 4711,
        'stats.users_count' => 1356,
    ));

    $stats = $cache->getMultiple(array(
        'stats.products_count',
        'stats.users_count',
    ));

    $cache->deleteMultiple(array(
        'stats.products_count',
        'stats.users_count',
    ));

可用的简单缓存（PSR-16）类
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

有以下可用的缓存适配器：

.. tip::

    要了解有关每个类的更多信息，可以阅读
    :doc:`PSR-6缓存池 </components/cache/cache_pools>` 文档。
    这些“简单”的（PSR-16）缓存类与该文档中的PSR-6适配器不同，但它们都共享构造函数参数和用例。

* :class:`Symfony\\Component\\Cache\\Simple\\ApcuCache`
* :class:`Symfony\\Component\\Cache\\Simple\\ArrayCache`
* :class:`Symfony\\Component\\Cache\\Simple\\ChainCache`
* :class:`Symfony\\Component\\Cache\\Simple\\DoctrineCache`
* :class:`Symfony\\Component\\Cache\\Simple\\FilesystemCache`
* :class:`Symfony\\Component\\Cache\\Simple\\MemcachedCache`
* :class:`Symfony\\Component\\Cache\\Simple\\NullCache`
* :class:`Symfony\\Component\\Cache\\Simple\\PdoCache`
* :class:`Symfony\\Component\\Cache\\Simple\\PhpArrayCache`
* :class:`Symfony\\Component\\Cache\\Simple\\PhpFilesCache`
* :class:`Symfony\\Component\\Cache\\Simple\\RedisCache`
* :class:`Symfony\\Component\\Cache\\Simple\\TraceableCache`

.. _cache-component-psr6-caching:

更高级的缓存（PSR-6）
-----------------------------

要使用更高级的PSR-6缓存功能，你需要了解其关键概念：

**项(Item)**
    作为键/值对存储的单个信息单元，其中键是信息的唯一标识符，值是其内容;
**池(Pool)**
    缓存项的逻辑仓库。所有缓存操作（保存项、查找项等）都通过池执行。应用可根据需要定义任意数量的池。
**适配器(Adapter)**
    它实现了将信息存储在文件系统、数据库等中的实际缓存机制。
    本组件为常见的缓存后端提供了几个现成的适配器（Redis，APCu，Doctrine，PDO等）。

基本用法（PSR-6）
-------------------

本组件的这一部分是 `PSR-6`_ 的一个实现，这意味着它的基本API与标准中定义的相同。
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

高级用法（PSR-6）
----------------------

.. toctree::
    :glob:
    :maxdepth: 1

    cache/*

.. _`PSR-6`: http://www.php-fig.org/psr/psr-6/
.. _`PSR-16`: http://www.php-fig.org/psr/psr-16/
.. _Doctrine Cache: https://www.doctrine-project.org/projects/cache.html
