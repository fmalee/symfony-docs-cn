.. index::
    single: Cache Item
    single: Cache Expiration
    single: Cache Exceptions

缓存项
===========

缓存项是存储在缓存中作为键/值对的信息单元。在Cache组件中，它们用
:class:`Symfony\\Component\\Cache\\CacheItem`
类来表示。它们用于缓存契约(Contract)和PSR-6接口。

缓存项的键和值
--------------------------

缓存项的 **键** 是一个纯字符串，它充当其标识符，因此它对于每个缓存池必须是唯一的。
你可以自由的选择键，但它们只应包含字母（AZ，az）、数字（0-9）、``_`` 以及 ``.`` 符号。
其他常见的符号（``{``、``}``、``(``、``)``、``/``、``\``、``@`` 以及
``:``）由为供将来使用的PSR-6标准保留。

缓存项的 **值** 是可以通过PHP来序列化的类型来表示的任何数据，例如基本类型（字符串、整数、浮点数、布尔值、空值），数组和对象。

创建缓存项
--------------------

The only way to create cache items is via cache pools. When using the Cache
Contracts, they are passed as arguments to the recomputation callback::
创建缓存项的唯一方法是通过缓存池。
使用缓存契约时，它们作为一个参数传递给重新计算(recomputation)回调::

    // $cache池对象之前就创建了
    $productsCount = $cache->get('stats.products_count', function (ItemInterface $item) {
        // [...]
    });

使用PSR-6时，它们是使用缓存池的 ``getItem($key)`` 方法创建的::

    // $cache池对象之前就创建了
    $productsCount = $cache->getItem('stats.products_count');

然后，使用 ``Psr\\Cache\\CacheItemInterface::set``
方法来设置存储在缓存项中的数据（此步骤在使用缓存契约时自动完成）::

    // 储存一个简单的整数
    $productsCount->set(4711);
    $cache->save($productsCount);

    // 储存一个数组
    $productsCount->set([
        'category1' => 4711,
        'category2' => 2387,
    ]);
    $cache->save($productsCount);

可以使用相应的 *getter* 方法来获取任何给定缓存项的键和值::

    $cacheItem = $cache->getItem('exchange_rate');
    // ...
    $key = $cacheItem->getKey();
    $value = $cacheItem->get();

缓存项到期
~~~~~~~~~~~~~~~~~~~~~

默认情况下，缓存项永久存储。实际上，如 :doc:`/components/cache/cache_pools`
文档中所述的那样，这种“永久存储”可能会根据所使用的缓存类型而有很大差异。

但是，在某些应用中，通常使用生命周期较短的缓存项。
例如，某个应用的最新消息只缓存一分钟。在这些情况下，使用 ``expiresAfter()``
方法来设置缓存项的秒数::

    $latestNews->expiresAfter(60);  // 60秒 = 1分钟

    // 此参数还接收 \DateInterval 实例
    $latestNews->expiresAfter(DateInterval::createFromDateString('1 hour'));

缓存项还定义了另一个相关方法，``expiresAt()`` 用于设置缓存项到期时的确切日期和时间::

    $mostPopularNews->expiresAt(new \DateTime('tomorrow'));

缓存项的命中和未命中
--------------------------

使用缓存机制对于提高应用性能很重要，但不应该影响应用的正常运行。事实上，PSR-6文档明智地指出缓存错误不应导致应用失败。

在PSR-6的实践中，这意味着 ``getItem()`` 方法总是返回一个实现了 ``Psr\Cache\CacheItemInterface``
接口的对象，即使该缓存项不存在也是如此。因此，你不必处理 ``null``
返回值，并且可以安全地存储缓存值，例如 ``false`` 和 ``null``。

为了确定返回的对象是否代表来自存储的值，缓存使用命中和未命中的概念：

* **缓存命中** 当在缓存中找到所请求的缓存项、其值未损坏或无效且未过期时，会发生缓存命中;
* **缓存未命中** 缓存未命中与命中相反，因此当在缓存中找不到缓存项、其值因任何原因损坏或无效或项目已过期时，都会发生缓存未命中。

缓存项对象定义一个布尔 ``isHit()`` 方法，如果缓存命中，该方法返回 ``true``::

    $latestNews = $cache->getItem('latest_news');

    if (!$latestNews->isHit()) {
        // 做一个繁重计算
        $news = ...;
        $cache->save($latestNews->set($news));
    } else {
        $news = $latestNews->get();
    }
