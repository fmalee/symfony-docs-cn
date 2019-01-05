.. index::
    single: Cache Pool
    single: Redis Cache

.. _redis-adapter:

Redis缓存适配器
===================

此适配器使用一个（或多个）Redis服务器实例将值存储在内存中。
与 :ref:`APCu适配器 <apcu-adapter>` 不同，但与 :ref:`Memcached适配器 <memcached-adapter>`
类似，它可以不限于当前服务器的共享内存;你可以独立于PHP环境来存储内容。
还可以使用服务器集群来提供冗余和故障转移。

.. caution::

    **要求**：必须至少安装并运行一台 `Redis服务器`_ 才能使用此适配器。
    此外，该适配器必须一个实现 ``\Redis``、``\RedisArray``、``RedisCluster``
    或 ``\Predis`` 的兼容扩展或库。

此适配器需要将一个 `Redis`_、`RedisArray`_、`RedisCluster`_ 或 `Predis`_
实例作为第一个参数传递。可以选择将命名空间和默认缓存生存期作为第二个和第三个参数传递::

    use Symfony\Component\Cache\Adapter\RedisAdapter;

    $cache = new RedisAdapter(

        // 存储到Redis系统的有效连接的对象
        \Redis $redisConnection,

        // 一个字符串，用以在此缓存中存储的缓存项的键前面添加前缀
        $namespace = '',

        // 未定义其自身生存期的缓存项的默认生存期（以秒为单位），
        // 值为0会导致项无限期存储（即，直到 RedisAdapter::clear() 被调用或该服务器被清空）。
        $defaultLifetime = 0
    );

配置连接
------------------------

:method:`Symfony\\Component\\Cache\\Adapter\\RedisAdapter::createConnection`
辅助方法允许使用 `数据源名称（DSN）`_ 来创建和配置使用Redis的客户端类的实例::

    use Symfony\Component\Cache\Adapter\RedisAdapter;

    // 传递单个DSN字符串以向客户端注册单个服务器
    $client = RedisAdapter::createConnection(
        'redis://localhost'
    );

DSN可以指定IP/主机（和可选端口）或一个套接字路径、用户和密码以及数据库索引。

.. note::

    此适配器的一个 `数据源名称（DSN）`_ 必须使用以下格式。

    .. code-block:: text

        redis://[user:pass@][ip|host|socket[:port]][/db-index]

以下是展示可用值的组合的有效DSN的常见示例::

    use Symfony\Component\Cache\Adapter\RedisAdapter;

    // host "my.server.com" and port "6379"
    RedisAdapter::createConnection('redis://my.server.com:6379');

    // host "my.server.com" and port "6379" and database index "20"
    RedisAdapter::createConnection('redis://my.server.com:6379/20');

    // host "localhost" and SASL use "rmf" and pass "abcdef"
    RedisAdapter::createConnection('redis://rmf:abcdef@localhost');

    // socket "/var/run/redis.sock" and SASL user "user1" and pass "bad-pass"
    RedisAdapter::createConnection('redis://user1:bad-pass@/var/run/redis.sock');

配置选项
---------------------

:method:`Symfony\\Component\\Cache\\Adapter\\RedisAdapter::createConnection`
辅助方法还接受一个选项数组作为第二个参数。期望的格式是一个表示选项名称及其各自值的 ``key => value`` 对关联数组::

    use Symfony\Component\Cache\Adapter\RedisAdapter;

    $client = RedisAdapter::createConnection(

        // 提供一个字符串DSN
        'redis://localhost:6379',

        // 配置选项的关联数组
        array(
            'compression' => true,
            'lazy' => false,
            'persistent' => 0,
            'persistent_id' => null,
            'tcp_keepalive' => 0,
            'timeout' => 30,
            'read_timeout' => 0,
            'retry_interval' => 0,
         )

    );

可用选项
~~~~~~~~~~~~~~~~~

``class`` (类型: ``string``)
    指定要返回的连接库，``\Redis`` 或 ``\Predis\Client``。
    如果未指定，则在redis扩展可用时返回 ``\Redis``，否则返回 ``\Predis\Client``。

``compression`` (类型: ``bool``, 默认: ``true``)
    启用或禁用缓存项压缩。这需要启用LZF支持的phpredis v4或更高版本。

``lazy`` (类型: ``bool``, 默认: ``false``)
    启用或禁用到后端的延迟连接。作为一个独立组件时默认为 ``false``
    ，在Symfony的应用内部使用时，它的默认值是 ``true``。

``persistent`` (类型: ``int``, 默认: ``0``)
    启用或禁用持久连接的使用。值为 ``0`` 将禁用持久连接，值为 ``1`` 将启用持久连接。

``persistent_id`` (类型: ``string|null``, 默认: ``null``)
    指定用于一个持久连接的持久标识字符串。

``read_timeout`` (类型: ``int``, 默认: ``0``)
    指定在操作超时之前对基础网络资源执行读取操作时使用的时间（以秒为单位）。

``retry_interval`` (类型: ``int``, 默认: ``0``)
    指定客户端与服务器失去连接时重新连接尝试之间的延迟（以毫秒为单位）。

``tcp_keepalive`` (类型: ``int``, 默认: ``0``)
    指定连接的 `TCP-keepalive`_ 超时（以秒为单位）。这需要phpredis v4或更高版本以及启用TCP-keepalive的服务器。

``timeout`` (类型: ``int``, 默认: ``30``)
    指定在连接尝试超时之前用于连接Redis服务器的时间（以秒为单位）。

.. note::
    使用 `Predis`_ 库时，可以使用一些其他特定于Predis的选项。有关详细信息，请参阅 `Predis连接参数`_。

.. _`数据源名称（DSN）`: https://en.wikipedia.org/wiki/Data_source_name
.. _`Redis服务器`: https://redis.io/
.. _`Redis`: https://github.com/phpredis/phpredis
.. _`RedisArray`: https://github.com/phpredis/phpredis/blob/master/arrays.markdown#readme
.. _`RedisCluster`: https://github.com/phpredis/phpredis/blob/master/cluster.markdown#readme
.. _`Predis`: https://packagist.org/packages/predis/predis
.. _`Predis连接参数`: https://github.com/nrk/predis/wiki/Connection-Parameters#list-of-connection-parameters
.. _`TCP-keepalive`: https://redis.io/topics/clients#tcp-keepalive
