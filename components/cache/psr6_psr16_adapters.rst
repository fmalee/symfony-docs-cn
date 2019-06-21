.. index::
   single: Cache
   single: Performance
   single: Components; Cache

适用于在PSR-6和PSR-16缓存之间交互的适配器
============================================================

有时，你可能有一个实现了 `PSR-16`_ 标准的缓存对象，但需要将其传递给一个需要
:ref:`PSR-6 <cache-component-psr6-caching>` 缓存适配器的对象。
或者，你可能会遇到相反的情况。缓存组件包含用于PSR-6和PSR-16缓存之间的双向交互的两个类。

将PSR-16缓存对象作为一个PSR-6缓存
--------------------------------------------

假设你要使用一个需要PSR-6缓存池对象的类。例如::

    use Psr\Cache\CacheItemPoolInterface;

    // 只是举个例子
    class GitHubApiClient
    {
        // ...

        // 这里需要一个PSR-6缓存对象
        public function __construct(CacheItemPoolInterface $cachePool)
        {
            // ...
        }
    }

但是，你已经拥有一个PSR-16缓存对象，而你希望将其传递给 ``GitHubApiClient``
类。没问题！缓存组件为此用例提供了
:class:`Symfony\\Component\\Cache\\Adapter\\SimpleCacheAdapter` 类::

    use Symfony\Component\Cache\Adapter\SimpleCacheAdapter;
    use Symfony\Component\Cache\Simple\FilesystemCache;

    // 要使用的PSR-16缓存对象
    $psr16Cache = new FilesystemCache();

    // 在内部使用你的缓存的一个PSR-6缓存！
    $psr6Cache = new SimpleCacheAdapter($psr16Cache);

    // 现在你在哪使用都行
    $githubApiClient = new GitHubApiClient($psr6Cache);

将PSR-6缓存对象作为一个PSR-16缓存
--------------------------------------------

假设你要使用一个需要PSR-16缓存对象的类。例如::

    use Psr\SimpleCache\CacheInterface;

    // 只是举个例子
    class GitHubApiClient
    {
        // ...

        // 这里需要一个PSR-16缓存对象
        public function __construct(CacheInterface $cache)
        {
            // ...
        }
    }

但是，你已经拥有一个PSR-6缓存对象，而你希望将其传递给 ``GitHubApiClient``
类。没问题！缓存组件为此用例提供了
:class:`Symfony\\Component\\Cache\\Simple\\Psr6Cache` 类::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;
    use Symfony\Component\Cache\Simple\Psr6Cache;

    // 要使用的PSR-6缓存对象
    $psr6Cache = new FilesystemAdapter();

    // 在内部使用你的缓存的一个PSR-16缓存！
    $psr16Cache = new Psr6Cache($psr6Cache);

    // 现在你在哪使用都行
    $githubApiClient = new GitHubApiClient($psr16Cache);

.. _`PSR-16`: http://www.php-fig.org/psr/psr-16/
