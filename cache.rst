.. index::
    single: Cache

缓存
=====

使用缓存是一种使应用运行更快的好方法。Symfony缓存组件开箱附带许多适配器以适配不同的存储系统。
每个适配器都是为高性能而开发的。

缓存的基本用法如下所示::

    use Symfony\Contracts\Cache\ItemInterface;

    // 该可调用将只在缓存未命中时执行。
    $value = $pool->get('my_cache_key', function (ItemInterface $item) {
        $item->expiresAfter(3600);

        // ... 做一些HTTP请求或繁重的计算
        $computedValue = 'foobar';

        return $computedValue;
    });

    echo $value; // 'foobar'

    // ... 还有删除缓存键
    $pool->delete('my_cache_key');

Symfony支持缓存契约、PSR-6/16和Doctrine缓存接口。你可以在
:doc:`组件文档 </components/cache>` 中阅读有关这些的更多信息。

.. versionadded:: 4.2

    缓存契约是在Symfony 4.2中引入的。

.. _cache-configuration-with-frameworkbundle:

使用FrameworkBundle配置缓存
--------------------------------------

配置缓存组件时，你应该了解一些概念：

**池**
    这是你将与之交互的一个服务。每个池将始终具有自己的命名空间和缓存项。池之间永远不会发生冲突。
**适配器**
    适配器是用于创建池的一个 *模板*。
**提供器**
    提供器是某些适配器用于连接到存储系统的一个服务。Redis和Memcached就是这种适配器的例子。
    如果将一个DSN作为提供器，则会自动创建相关服务。

默认情况下，会始终启用两个池。他们是 ``cache.app`` 和
``cache.system``。系统缓存用于注释、序列化和验证等操作。``cache.app``
可以在你的代码中使用。你可以使用 ``app`` 和 ``system``
键来配置他们各自使用的适配器（模板），如：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/cache.yaml
        framework:
            cache:
                app: cache.adapter.filesystem
                system: cache.adapter.system

    .. code-block:: xml

        <!-- config/packages/cache.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:cache app="cache.adapter.filesystem"
                    system="cache.adapter.system"
                />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/cache.php
        $container->loadFromExtension('framework', [
            'cache' => [
                'app' => 'cache.adapter.filesystem',
                'system' => 'cache.adapter.system',
            ],
        ]);

缓存组件附带一系列预配置的适配器：

* :doc:`cache.adapter.apcu </components/cache/adapters/apcu_adapter>`
* :doc:`cache.adapter.array </components/cache/adapters/array_cache_adapter>`
* :doc:`cache.adapter.doctrine </components/cache/adapters/doctrine_adapter>`
* :doc:`cache.adapter.filesystem </components/cache/adapters/filesystem_adapter>`
* :doc:`cache.adapter.memcached </components/cache/adapters/memcached_adapter>`
* :doc:`cache.adapter.pdo </components/cache/adapters/pdo_doctrine_dbal_adapter>`
* :doc:`cache.adapter.psr6 </components/cache/adapters/proxy_adapter>`
* :doc:`cache.adapter.redis </components/cache/adapters/redis_adapter>`

其中一些适配器可以通过快捷方式来配置。使用这些快捷方式将会创建服务ID为 ``cache.[type]`` 的池。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/cache.yaml
        framework:
            cache:
                directory: '%kernel.cache_dir%/pools' # 仅用于cache.adapter.filesystem

                # 服务: cache.doctrine
                default_doctrine_provider: 'app.doctrine_cache'
                # 服务: cache.psr6
                default_psr6_provider: 'app.my_psr6_service'
                # 服务: cache.redis
                default_redis_provider: 'redis://localhost'
                # 服务: cache.memcached
                default_memcached_provider: 'memcached://localhost'
                # 服务: cache.pdo
                default_pdo_provider: 'doctrine.dbal.default_connection'

    .. code-block:: xml

        <!-- config/packages/cache.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <!--
                default_doctrine_provider: Service: cache.doctrine
                default_psr6_provider: Service: cache.psr6
                default_redis_provider: Service: cache.redis
                default_memcached_provider: Service: cache.memcached
                default_pdo_provider: Service: cache.pdo
                -->
                <framework:cache directory="%kernel.cache_dir%/pools"
                    default_doctrine_provider="app.doctrine_cache"
                    default_psr6_provider="app.my_psr6_service"
                    default_redis_provider="redis://localhost"
                    default_memcached_provider="memcached://localhost"
                    default_pdo_provider="doctrine.dbal.default_connection"
                />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/cache.php
        $container->loadFromExtension('framework', [
            'cache' => [
                // Only used with cache.adapter.filesystem
                'directory' => '%kernel.cache_dir%/pools',

                // Service: cache.doctrine
                'default_doctrine_provider' => 'app.doctrine_cache',
                // Service: cache.psr6
                'default_psr6_provider' => 'app.my_psr6_service',
                // Service: cache.redis
                'default_redis_provider' => 'redis://localhost',
                // Service: cache.memcached
                'default_memcached_provider' => 'memcached://localhost',
                // Service: cache.pdo
                'default_pdo_provider' => 'doctrine.dbal.default_connection',
            ],
        ]);

创建自定义（命名空间）池
----------------------------------

你还可以创建更多的自定义池：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/cache.yaml
        framework:
            cache:
                default_memcached_provider: 'memcached://localhost'

                pools:
                    # 创建一个 "custom_thing.cache" 服务
                    # 通过 "CacheInterface $customThingCache" 进行自动装配
                    # 使用 "app" 缓存的配置
                    custom_thing.cache:
                        adapter: cache.app

                    # 创建一个 "my_cache_pool" 服务
                    # 通过 "CacheInterface $myCachePool" 进行自动装配
                    my_cache_pool:
                        adapter: cache.adapter.array

                    # 使用上面的 default_memcached_provider
                    acme.cache:
                        adapter: cache.adapter.memcached

                    # 控制适配器的配置
                    foobar.cache:
                        adapter: cache.adapter.memcached
                        provider: 'memcached://user:password@example.com'

                    # 使用 “foobar.cache” 池作为其后端，但控制生存期，
                    # 并且（与所有池一样）具有单独的缓存命名空间
                    short_cache:
                        adapter: foobar.cache
                        default_lifetime: 60

    .. code-block:: xml

        <!-- config/packages/cache.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:cache default_memcached_provider="memcached://localhost">
                    <framework:pool name="custom_thing.cache" adapter="cache.app"/>
                    <framework:pool name="my_cache_pool" adapter="cache.adapter.array"/>
                    <framework:pool name="acme.cache" adapter="cache.adapter.memcached"/>
                    <framework:pool name="foobar.cache" adapter="cache.adapter.memcached" provider="memcached://user:password@example.com"/>
                    <framework:pool name="short_cache" adapter="foobar.cache" default_lifetime="60"/>
                </framework:cache>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/cache.php
        $container->loadFromExtension('framework', [
            'cache' => [
                'default_memcached_provider' => 'memcached://localhost',
                'pools' => [
                    'custom_thing.cache' => [
                        'adapter' => 'cache.app',
                    ],
                    'my_cache_pool' => [
                        'adapter' => 'cache.adapter.array',
                    ],
                    'acme.cache' => [
                        'adapter' => 'cache.adapter.memcached',
                    ],
                    'foobar.cache' => [
                        'adapter' => 'cache.adapter.memcached',
                        'provider' => 'memcached://user:password@example.com',
                    ],
                    'short_cache' => [
                        'adapter' => 'foobar.cache',
                        'default_lifetime' => 60,
                    ],
                ],
            ],
        ]);

每个池都管理一组独立的缓存键：不同池的键 *永不会* 发生冲突，即使它们共享相同的后端。
这是通过为键添加一个命名空间来实现的，该命名空间是通过散列池的名称、已编译容器类的名称和默认为项目目录的
:ref:`可配置种子<reference-cache-prefix-seed>` 生成的。

每个自定义池都成为一个服务ID为池名称的服务（例如 ``custom_thing.cache``）。
也使用其名称的驼峰拼写版本来为每个池创建一个自动装配别名 - 例如，``custom_thing.cache``
可以通过名为 ``$customThingCache`` 的参数来完成自动注入，并选择
:class:`Symfony\\Contracts\\Cache\\CacheInterface` 或
``Psr\\Cache\\CacheItemPoolInterface`` 其一作为类型约束::

    use Symfony\Contracts\Cache\CacheInterface;

    // 从一个控制器的方法
    public function listProducts(CacheInterface $customThingCache)
    {
        // ...
    }

    // 在一个服务中
    public function __construct(CacheInterface $customThingCache)
    {
        // ...
    }

自定义提供器的选项
-----------------------

某些提供器具有特定的选项可供配置。
:doc:`RedisAdapter </components/cache/adapters/redis_adapter>` 允许你使用
``timeout``、``retry_interval`` 等选项来创建供应器。
要将这些选项与非默认值一起使用，你需要创建自己的 ``\Redis`` 提供器并在配置池时使用它。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/cache.yaml
        framework:
            cache:
                pools:
                    cache.my_redis:
                        adapter: cache.adapter.redis
                        provider: app.my_custom_redis_provider

        services:
            app.my_custom_redis_provider:
                class: \Redis
                factory: ['Symfony\Component\Cache\Adapter\RedisAdapter', 'createConnection']
                arguments:
                    - 'redis://localhost'
                    - { retry_interval: 2, timeout: 10 }

    .. code-block:: xml

        <!-- config/packages/cache.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:cache>
                    <framework:pool name="cache.my_redis" adapter="cache.adapter.redis" provider="app.my_custom_redis_provider"/>
                </framework:cache>
            </framework:config>

            <services>
                <service id="app.my_custom_redis_provider" class="\Redis">
                    <argument>redis://localhost</argument>
                    <argument type="collection">
                        <argument key="retry_interval">2</argument>
                        <argument key="timeout">10</argument>
                    </argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/packages/cache.php
        $container->loadFromExtension('framework', [
            'cache' => [
                'pools' => [
                    'cache.my_redis' => [
                        'adapter' => 'cache.adapter.redis',
                        'provider' => 'app.my_custom_redis_provider',
                    ],
                ],
            ],
        ]);

        $container->getDefinition('app.my_custom_redis_provider', \Redis::class)
            ->addArgument('redis://localhost')
            ->addArgument([
                'retry_interval' => 2,
                'timeout' => 10
            ]);

创建缓存链
----------------------

不同的缓存适配器具有不同的优点和缺点。有些可能非常快但迷你，有些可能包含大量数据但速度很慢。
为了两全其美，你可以使用一个适配器链。这个想法就是首先查找快速适配器，然后继续使用速度较慢的适配器。
在最坏的情况下，该值需要重新计算。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/cache.yaml
        framework:
            cache:
                pools:
                    my_cache_pool:
                        adapter: cache.adapter.psr6
                        provider: app.my_cache_chain_adapter
                    cache.my_redis:
                        adapter: cache.adapter.redis
                        provider: 'redis://user:password@example.com'
                    cache.apcu:
                        adapter: cache.adapter.apcu
                    cache.array:
                        adapter: cache.adapter.array


        services:
            app.my_cache_chain_adapter:
                class: Symfony\Component\Cache\Adapter\ChainAdapter
                arguments:
                    - ['@cache.array', '@cache.apcu', '@cache.my_redis']
                    - 31536000 # One year

    .. code-block:: xml

        <!-- config/packages/cache.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:cache>
                    <framework:pool name="my_cache_pool" adapter="cache.adapter.psr6" provider="app.my_cache_chain_adapter"/>
                    <framework:pool name="cache.my_redis" adapter="cache.adapter.redis" provider="redis://user:password@example.com"/>
                    <framework:pool name="cache.apcu" adapter="cache.adapter.apcu"/>
                    <framework:pool name="cache.array" adapter="cache.adapter.array"/>
                </framework:cache>
            </framework:config>

            <services>
                <service id="app.my_cache_chain_adapter" class="Symfony\Component\Cache\Adapter\ChainAdapter">
                    <argument type="collection">
                        <argument type="service" value="cache.array"/>
                        <argument type="service" value="cache.apcu"/>
                        <argument type="service" value="cache.my_redis"/>
                    </argument>
                    <argument>31536000</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/packages/cache.php
        $container->loadFromExtension('framework', [
            'cache' => [
                'pools' => [
                    'my_cache_pool' => [
                        'adapter' => 'cache.adapter.psr6',
                        'provider' => 'app.my_cache_chain_adapter',
                    ],
                    'cache.my_redis' => [
                        'adapter' => 'cache.adapter.redis',
                        'provider' => 'redis://user:password@example.com',
                    ],
                    'cache.apcu' => [
                        'adapter' => 'cache.adapter.apcu',
                    ],
                    'cache.array' => [
                        'adapter' => 'cache.adapter.array',
                    ],
                ],
            ],
        ]);

        $container->getDefinition('app.my_cache_chain_adapter', \Symfony\Component\Cache\Adapter\ChainAdapter::class)
            ->addArgument([
                new Reference('cache.array'),
                new Reference('cache.apcu'),
                new Reference('cache.my_redis'),
            ])
            ->addArgument(31536000);

.. note::

    在此配置中，``my_cache_pool`` 池使用 ``cache.adapter.psr6`` 适配器和
    ``app.my_cache_chain_adapter``服务作为提供器。这是因为 ``ChainAdapter``
    不支持 ``cache.pool`` 标签。因此，它用 ``ProxyAdapter``装饰。

使用缓存标签
----------------

在具有许多缓存键的应用中，组织存储的数据以便能够更有效地使缓存失效可能是有帮助的。
实现这一目标的一种方法是使用缓存标签。可以将一个或多个标签添加到缓存项。
具有相同键的所有项可以通过一个函数调用来使其失效::

    use Symfony\Contracts\Cache\ItemInterface;

    $value0 = $pool->get('item_0', function (ItemInterface $item) {
        $item->tag(['foo', 'bar'])

        return 'debug';
    });

    $value1 = $pool->get('item_1', function (ItemInterface $item) {
        $item->tag('foo')

        return 'debug';
    });

    // 删除所有标签为“bar”的缓存键
    $pool->invalidateTags(['bar']);

缓存适配器需要实现 :class:`Symfony\\Contracts\\Cache\\TagAwareCacheInterface``
才能启用此功能。可以使用以下配置来添加此功能。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/cache.yaml
        framework:
            cache:
                pools:
                    my_cache_pool:
                        adapter: cache.adapter.redis
                        tags: true

    .. code-block:: xml

        <!-- config/packages/cache.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:cache>
                  <framework:pool name="my_cache_pool" adapter="cache.adapter.redis" tags="true"/>
                </framework:cache>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/cache.php
        $container->loadFromExtension('framework', [
            'cache' => [
                'pools' => [
                    'my_cache_pool' => [
                        'adapter' => 'cache.adapter.redis',
                        'tags' => true,
                    ],
                ],
            ],
        ]);

默认情况下，标签存储在同一个池中。这在大多数情况下都很好。
但有时将标签存储在不同的池中可能会更好。这可以通过指定适配器来实现。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/cache.yaml
        framework:
            cache:
                pools:
                    my_cache_pool:
                        adapter: cache.adapter.redis
                        tags: tag_pool
                    tag_pool:
                        adapter: cache.adapter.apcu

    .. code-block:: xml

        <!-- config/packages/cache.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:cache>
                  <framework:pool name="my_cache_pool" adapter="cache.adapter.redis" tags="tag_pool"/>
                  <framework:pool name="tag_pool" adapter="cache.adapter.apcu"/>
                </framework:cache>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/cache.php
        $container->loadFromExtension('framework', [
            'cache' => [
                'pools' => [
                    'my_cache_pool' => [
                        'adapter' => 'cache.adapter.redis',
                        'tags' => 'tag_pool',
                    ],
                    'tag_pool' => [
                        'adapter' => 'cache.adapter.apcu',
                    ],
                ],
            ],
        ]);

.. note::

    :class:`Symfony\\Contracts\\Cache\\TagAwareCacheInterface``
    接口已自动装配到 ``cache.app`` 服务。

清除缓存
------------------

要清除缓存，你可以使用 ``bin/console cache:pool:clear [pool]`` 命令。
这将删除你的存储中的所有条目，你将不得不重新计算所有的值。
你还可以将你的池分组到“缓存清除器”。默认情况下有3个缓存清除器：

* ``cache.global_clearer``
* ``cache.system_clearer``
* ``cache.app_clearer``

全局清除器清除每个池中的所有缓存。系统缓存清除器在 ``bin/console cache:clear``
命令中使用。应用清除器则是默认的清除器。

要查看所有可用的缓存池：

.. code-block:: terminal

    $ php bin/console cache:pool:list

.. versionadded:: 4.3

    Symfony 4.3引入了 ``cache:pool:list`` 命令。

清除一个池：

.. code-block:: terminal

    $ php bin/console cache:pool:clear my_cache_pool

清除所有自定义池：

.. code-block:: terminal

    $ php bin/console cache:pool:clear cache.app_clearer

清除所有缓存：

.. code-block:: terminal

    $ php bin/console cache:pool:clear cache.global_clearer
