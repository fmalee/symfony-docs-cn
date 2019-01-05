.. index::
    single: Cache Pool
    single: Filesystem Cache

.. _component-cache-filesystem-adapter:

文件系统缓存适配器
========================

此适配器为无法在其环境中安装 :ref:`APCu <apcu-adapter>` 或 :ref:`Redis <redis-adapter>`
等工具的用户提供了提升应用性能的替代方案。
它将缓存项目到期时间和内容作为常规文件存储在本地装载的文件系统上的目录集合中。

.. tip::

    通过使用临时的内存文件系统（例如Linux上的 `tmpfs`_）或许多其他可用的
    `RAM磁盘解决方案`_，可以大大提高此适配器的性能。

文件系统适配器可以选择提供命名空间、默认缓存生存期和缓存的根路径作为构造函数参数::

    use Symfony\Component\Cache\Adapter\FilesystemAdapter;

    $cache = new FilesystemAdapter(

        // 一个字符串，表示储存着缓存项的根缓存目录的子目录
        $namespace = '',

        // 未定义其自身生存期的缓存项的默认生存期（以秒为单位），
        // 值为0会导致项无限期存储（即，直到这些文件被删除为止）。
        $defaultLifetime = 0,

        // 主缓存目录（应用需要对其具有读写权限）
        // 如果未指定，则会在系统临时目录内创建一个目录。
        $directory = null
    );

.. caution::

    文件系统IO的开销经常使这个适配器成为 *较慢* 的选择之一。
    如果吞吐量至关重要，则建议使用内存型适配器（:ref:`Apcu <apcu-adapter>`，
    :ref:`Memcached <memcached-adapter>` 以及
    :ref:`Redis <redis-adapter>`）或数据库型适配器（:ref:`Doctrine <doctrine-adapter>`
    和 :ref:`PDO <pdo-doctrine-adapter>`）。

.. note::

  从Symfony3.4开始，此适配器实现了
  :class:`Symfony\\Component\\Cache\\PruneableInterface`，已允许通过调用其 ``prune()``
  方法来手动 :ref:`清理过期的缓存项 <component-cache-cache-pool-prune>`。

.. _`tmpfs`: https://wiki.archlinux.org/index.php/tmpfs
.. _`RAM磁盘解决方案`: https://en.wikipedia.org/wiki/List_of_RAM_drive_software
