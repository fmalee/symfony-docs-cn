设置或修复文件权限
=====================================

在Symfony 3.x中，你需要做一些额外的工作以确保缓存目录是可写的。
但那都是过去式了！在Symfony 4中，一切都自动运行：

* In the ``dev`` environment, ``umask()`` is used in ``bin/console`` and ``public/index.php``
  so that any created files are writable by everyone.
  在 ``dev`` 环境，在 ``bin/console`` 和 ``public/index.php`` 中使用 ``umask()``，因此任何创建的文件都可以被所有人写入。

* 在 ``prod`` 环境中（即``APP_ENV`` 是 ``prod`` 以及 ``APP_DEBUG`` 为``0``），
  只要运行 ``php bin/console cache:warmup``，就不需要在运行时将缓存文件写入磁盘。

.. note::

    如果你决定将日志文件存储在磁盘上，则需要确保你的日志目录（例如 ``var/log/``）可由Web服务器用户和终端用户写入。
    可以这样做的一种方法是使用 ``chmod -R 777 var/log/``。
    请注意，此时生产系统上的任何用户都可以读取你的日志。
