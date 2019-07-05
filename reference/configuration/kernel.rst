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
* `容器构建时间`_

.. _configuration-kernel-charset:

字符集
~~~~~~~

**类型**: ``string`` **默认值**: ``UTF-8``

此选项定义应用中使用的字符集。该值通过 ``kernel.charset`` 配置参数和
:method:`Symfony\\Component\\HttpKernel\\Kernel::getCharset` 方法公开。

要更改此值，请重写 ``getCharset()`` 方法并返回另一个字符集::

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

.. deprecated:: 4.2

    ``kernel.name`` 参数和 ``Kernel::getName()`` 方法在Symfony4.2中已弃用。
    如果你需要内核的唯一ID，请使用 ``kernel.container_class``
    参数或 ``Kernel::getContainerClass()`` 方法。

内核的名称通常不重要 - 它用于生成缓存文件 - 你可能只会在
:doc:`使用多个内核的应用 </configuration/multiple_kernels>` 时才需要改它。

该值通过 ``kernel.name`` 配置参数和
:method:`Symfony\\Component\\HttpKernel\\Kernel::getName` 方法公开。

要更改此设置，请重写 ``getName()``
方法。或者，将内核移动到其他目录中。例如，如果将内核移动到
``foo/`` 目录（而不是 ``src/``）中，则内核名称将变为 ``foo``。

.. _configuration-kernel-project-directory:

项目目录
~~~~~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``composer.json`` 项目的目录

这将返回Symfony项目的根目录的绝对路径，应用使用该目录来执行相对于项目根目录的文件路径的操作。

默认情况下，其值自动计算为存储主 ``composer.json`` 文件的目录。
该值通过 ``kernel.project_dir`` 配置参数和
:method:`Symfony\\Component\\HttpKernel\\Kernel::getProjectDir` 方法公开。

如果你不使用Composer，或已移动了 ``composer.json``
文件位置或已将其完全删除（例如在生产服务器中），则可以重写
:method:`Symfony\\Component\\HttpKernel\\Kernel::getProjectDir`
方法以返回正确的项目目录::

    // src/Kernel.php
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;
    // ...

    class Kernel extends BaseKernel
    {
        // ...

        public function getProjectDir(): string
        {
            return \dirname(__DIR__);
        }
    }

缓存目录
~~~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``$this->rootDir/cache/$this->environment``

这将返回Symfony项目的缓存目录的绝对路径。它是根据当前
:ref:`环境 <configuration-environments>` 自动计算的。

该值通过 ``kernel.cache_dir`` 配置参数和
:method:`Symfony\\Component\\HttpKernel\\Kernel::getCacheDir` 方法公开。
要更改此设置，请重写 ``getCacheDir()`` 方法以返回正确的缓存目录。

日志目录
~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``$this->rootDir/log``

这将返回Symfony项目的日志目录的绝对路径。它是根据当前
:ref:`环境 <configuration-environments>` 自动计算的。

该值通过 ``kernel.log_dir`` 配置参数和
:method:`Symfony\\Component\\HttpKernel\\Kernel::getLogDir` 方法公开。
要更改此设置，请重写 ``getLogDir()`` 方法以返回正确的日志目录。

容器构建时间
~~~~~~~~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: 执行 ``time()`` 的结果

Symfony遵循 `可重现的构建`_ 原则，确保编译完全相同的源代码的结果不会产生不同的结果。
这有助于检查给定的二进制或可执行代码是从某些可信源代码编译的。

实际上，如果不更改源代码，你的应用的已编译
:doc:`服务容器 </service_container>` 将始终相同。这是通过以下配置参数公开的：

* ``container.build_hash``，所有源文件内容的哈希值;
* ``container.build_time``，构建容器时的时间戳（执行PHP的
  :phpfunction:`time` 函数的结果）;
* ``container.build_id``，合并前两个参数并使用CRC32编码结果的结果。

由于每次编译应用时 ``container.build_time`` 值都会更改，因此构建将不会严格重现。
如果你关心这一点，解决方案是使用另一个名为 ``kernel.container_build_time``
的配置参数，并将其设置为不变的构建时间，以实现严格的可重现构建：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            # ...
            kernel.container_build_time: '1234567890'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <!-- ... -->
                <parameter key="kernel.container_build_time">1234567890</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php

        // ...
        $container->setParameter('kernel.container_build_time', '1234567890');

.. _`可重现的构建`: https://en.wikipedia.org/wiki/Reproducible_builds
