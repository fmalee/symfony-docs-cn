.. index::
   single: Cache
   single: Performance
   single: Components; Cache

.. _`cache-component`:

Cache组件
===================

    Cache组件提供了涵盖简单到高级的缓存需求的功能。
    它原生地实现了 `PSR-6`_ 和 `缓存契约`_，以实现最大的互操作性。
    它专为提高性能和弹性而设计，随附适用于最常见的缓存后端的适配器。
    它通过锁定和提前到期来启用基于标签的失效和缓存踩踏（stampede）保护。

.. tip::

    该组件还包含在PSR-6、PSR-16和Doctrine缓存之间进行转换的适配器。请参阅
    :doc:`/components/cache/psr6_psr16_adapters` 和
    :doc:`/components/cache/adapters/doctrine_adapter`。

安装
------------

.. code-block:: terminal

    $ composer require symfony/cache

.. include:: /components/require_autoload.rst.inc

缓存契约 & PSR-6
----------------------------

本组件包括 *两种* 不同的缓存方法：

:ref:`PSR-6缓存 <cache-component-psr6-caching>`:
    一个通用缓存系统，涉及缓存“池”和缓存“项”。

:ref:`缓存契约 <cache-component-contracts>`:
    一种更简单但更强大的方法，可以根据重算回调来缓存值。

.. tip::

    建议使用缓存契约方案：默认情况下，它需要较少的样板代码并提供缓存踩踏保护。

.. _cache-component-contracts:

缓存契约
---------------

所有适配器都支持缓存契约。它们只包含两种方法：``get()`` 和 ``delete()``。没有
``set()`` 方法是因为 ``get()`` 方法就可以获取并设置缓存值。

你要做的第一件事就是实例化一个缓存适配器。在此示例中使用的是
:class:`Symfony\\Component\\Cache\\Adapter\\FilesystemAdapter`::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;

    $cache = new FilesystemAdapter();

现在你可以使用此对象来检索和删除缓存的数据。``get()``
方法的第一个参数是一个键，一个与缓存值关联的任意字符串，以便你以后可以检索它。
第二个参数是一个PHP可调用，当在缓存中找不到对应的键时，会执行该调用以生成并返回值::

    use Symfony\Contracts\Cache\ItemInterface;

    // 该可调用将只在缓存未命中时执行
    $value = $cache->get('my_cache_key', function (ItemInterface $item) {
        $item->expiresAfter(3600);

        // ... 做一些HTTP请求或繁重的计算
        $computedValue = 'foobar';

        return $computedValue;
    });

    echo $value; // 'foobar'

    // ... 并删除缓存键
    $cache->delete('my_cache_key');

.. note::

    使用缓存标签可以删除多个键。阅读
    :doc:`/components/cache/cache_invalidation` 以了解更多内容。

缓存契约还附带内置的 `踩踏预防`_。它用于在缓存冷却时消除CPU峰值。
如果一个示例应用花费5秒钟来计算要缓存1小时的数据，并且每秒访问此数据10次，这意味着你大多数都有缓存命中，一切都很好。
但是1小时后，冷缓存中还收到10个新请求。所以再次计算数据。下一秒同样的事情发生了。
因此，在缓存再次生效之前，数据重复计算大约50次。这是你需要预防踩踏的地方。

第一种解决方案是使用锁定：只允许一个PHP进程（基于每个主机）一次计算一个特定的键。
默认情况下，锁定是内置的，因此除了利用缓存契约之外，你无需执行任何操作。

第二种解决方案在使用缓存契约时也是内置的：在一个值到期日期之前重新计算它，而不是其完全到期之后。
概率提前到期 算法随机假货高速缓存未命中的一个用户，而另一些仍担任缓存值。
`概率性提前到期`_ 算法随机地为一个用户伪造一个缓存未命中，而其他用户仍然得到缓存值。
你可以使用 :method:`Symfony\\Contracts\\Cache\\CacheInterface::get`
的第三个可选参数来控制其行为，它是一个名为“beta”的浮点值。

默认情况下，beta值为 ``1.0``，更高的值意味着更早的重新计算。
将其设置 ``0`` 以禁用提前重新计算，而将其设置 ``INF`` 则为强制立即重新计算::

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
.. _`缓存契约`: https://github.com/symfony/contracts/blob/master/Cache/CacheInterface.php
.. _`PSR-16`: http://www.php-fig.org/psr/psr-16/
.. _Doctrine缓存: https://www.doctrine-project.org/projects/cache.html
.. _踩踏预防: https://en.wikipedia.org/wiki/Cache_stampede
.. _概率性提前到期: https://en.wikipedia.org/wiki/Cache_stampede#Probabilistic_early_expiration
