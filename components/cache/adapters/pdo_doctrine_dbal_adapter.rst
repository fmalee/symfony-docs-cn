.. index::
    single: Cache Pool
    single: PDO Cache, Doctrine DBAL Cache

.. _pdo-doctrine-adapter:

PDO & Doctrine DBAL缓存适配器
=================================

此适配器将缓存项存储在SQL数据库中。它需要一个 `PDO`_、`Doctrine DBAL连接`_ 或 `数据源名称（DSN）`_
作为其第一个参数，并且可选地将命名空间、默认缓存生命周期以及选项数组作为其第二个、第三个和第四个参数::

    use Symfony\Component\Cache\Adapter\PdoAdapter;

    $cache = new PdoAdapter(

        // 一个PDO，Doctrine DBAL连接或通过PDO进行延迟连接的DSN
        $databaseConnectionOrDSN,

        // 一个字符串，用以在此缓存中存储的缓存项的键前面添加前缀
        $namespace = '',

        // 未定义其自身生存期的缓存项的默认生存期（以秒为单位），
        // 值为0会导致项无限期存储（即，直到数据库表被截断(truncated)或对应行被删除为止）。
        $defaultLifetime = 0,

        // 用于配置数据库表和连接的选项数组
        $options = array()
    );

.. versionadded:: 4.2
    在Symfony 4.2中引入了自动创建表的功能。

存储值的表在第一次调用
:method:`Symfony\\Component\\Cache\\Adapter\\PdoAdapter::save`
方法时会自动创建。你还可以通过在你的代码中显式调用
:method:`Symfony\\Component\\Cache\\Adapter\\PdoAdapter::createTable`
方法来创建此数据表。

.. tip::

    传递 `数据源名称（DSN）`_ 字符串（而不是数据库连接类实例）时，连接将在需要时延迟加载。

.. note::

    从Symfony3.4开始，此适配器实现了
    :class:`Symfony\\Component\\Cache\\PruneableInterface`，已允许通过调用其 ``prune()``
    方法来手动 :ref:`清理过期的缓存项 <component-cache-cache-pool-prune>`。

.. _`PDO`: http://php.net/manual/en/class.pdo.php
.. _`Doctrine DBAL连接`: https://github.com/doctrine/dbal/blob/master/lib/Doctrine/DBAL/Connection.php
.. _`数据源名称（DSN）`: https://en.wikipedia.org/wiki/Data_source_name
