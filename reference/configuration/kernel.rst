.. index::
    single: Configuration reference; Kernel class

内核配置
=========================

某些配置可以在内核类本身中完成（默认情况下位于 ``src/Kernel.php``）。
你可以通过重写父 :class:`Symfony\\Component\\HttpKernel\\Kernel` 类中的特定方法来完成此操作。

配置
-------------

* `字符集`_
* `内核名称`_
* `项目目录`_
* `缓存目录`_
* `日志目录`_

.. _configuration-kernel-charset:

字符集
~~~~~~~

**类型**: ``string`` **默认值**: ``UTF-8``

这将返回应用中使用的字符集。要更改它，请重写
:method:`Symfony\\Component\\HttpKernel\\Kernel::getCharset`
方法并返回另一个字符集，例如::

    // src/Kernel.php
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;
    // ...

    class Kernel extends BaseKernel
    {
        public function getCharset()
        {
            return 'ISO-8859-1';
        }
    }

内核名称
~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``src`` (即保存内核类的目录名)

.. versionadded:: 4.2
    ``kernel.name`` 参数和 ``Kernel::getName()`` 方法在Symfony4.2中已弃用。
    如果你需要内核的唯一ID，请使用 ``kernel.container_class``
    参数或 ``Kernel::getContainerClass()`` 方法。

要更改此设置，请重写 :method:`Symfony\\Component\\HttpKernel\\Kernel::getName`
方法。或者，将内核移动到其他目录中。例如，如果将内核移动到
``foo/`` 目录（而不是 ``src/``）中，则内核名称将变为 ``foo``。

内核的名称通常不重要 - 它用于生成缓存文件 - 你可能只会在
:doc:`使用多个内核的应用 </configuration/multiple_kernels>` 时才需要改它。

项目目录
~~~~~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``composer.json`` 项目的目录

这将返回Symfony项目的根目录。它被计算为存储主 ``composer.json`` 文件的目录。

如果 ``composer.json`` 文件由于某种原因未存储在项目的根目录中，则可以重写
:method:`Symfony\\Component\\HttpKernel\\Kernel::getProjectDir`
方法以返回正确的项目目录::

    // src/Kernel.php
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;
    // ...

    class Kernel extends BaseKernel
    {
        // ...

        public function getProjectDir()
        {
            return realpath(__DIR__.'/../');
        }
    }

缓存目录
~~~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``$this->rootDir/cache/$this->environment``

这将返回缓存目录的路径。要更改它，请重写
:method:`Symfony\\Component\\HttpKernel\\Kernel::getCacheDir`
方法。有关详细信息，请阅读 :ref:`override-cache-dir`。

日志目录
~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``$this->rootDir/log``

这将返回日志目录的路径。要更改它，请重写
:method:`Symfony\\Component\\HttpKernel\\Kernel::getLogDir` 方法。
有关详细信息，请阅读 :ref:`override-logs-dir`。
