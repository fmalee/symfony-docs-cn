.. index::
   single: Performance; Byte code cache; OPcache; APC

性能
===========

Symfony很快，开箱即用。但是，如果你按照以下性能检查表中的说明优化服务器和应用，则可以加快速度。

应用的清单
-----------------------------

#. :ref:`如果你的服务器使用APC，请安装APCu Polyfill <performance-install-apcu-polyfill>`

生产服务器的清单
---------------------------

#. :ref:`使用OPcache缓存 <performance-use-opcache>`
#. :ref:`配置OPcache以获得最佳性能 <performance-configure-opcache>`
#. :ref:`不要检查PHP文件的时间戳 <performance-dont-check-timestamps>`
#. :ref:`配置PHP实际路径缓存 <performance-configure-realpath-cache>`
#. :ref:`优化Composer Autoloader <performance-optimize-composer-autoloader>`

.. _performance-install-apcu-polyfill:

如果你的服务器使用APC，请安装APCu Polyfill
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你的生产服务器仍使用旧版APC PHP扩展而不是OPcache，
请在应用中安装 `APCu Polyfill component`_ 以启用与 `APCu PHP functions`_ 的兼容性，
并解锁对Symfony高级功能的支持，例如APCu缓存适配器。

.. _performance-use-opcache:

使用 OPcache 字节码缓存
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OPcache存储已编译的PHP文件，以避免为每个请求重新编译它们。
有一些 `字节码缓存`_ 可用，但从PHP 5.5开始，PHP内置了 `OPcache`_。
对于旧版本，最广泛使用的字节码缓存是 `APC`_。

.. _performance-configure-opcache:

配置OPcache以获得最佳性能
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

默认的OPcache配置不适用于Symfony应用，因此建议更改这些设置，如下所示：】

.. code-block:: ini

    ; php.ini
    ; maximum memory that OPcache can use to store compiled PHP files
    opcache.memory_consumption=256

    ; maximum number of files that can be stored in the cache
    opcache.max_accelerated_files=20000

.. _performance-dont-check-timestamps:

不要检查PHP文件的时间戳
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在生产服务器中，除非部署新的应用版本，否则PHP文件永远不会变化。
但是，默认情况下，OPcache会检查已缓存的文件是否已发生变化。
此检查修改了一些可以避免的开销，如下所示：

.. code-block:: ini

    ; php.ini
    opcache.validate_timestamps=0

每次部署后，你必须清空并重新生成OPcache的缓存。否则，你将看不到应用中所做的更新。
由于在PHP中CLI和Web进程不共享相同的OPcache，你无法通过在终端中执行某些命令来清除Web服务器OPcache。
以下是一些可能的解决方案：

1. 重启Web服务器;
2. 通过Web服务器调用 ``apc_clear_cache()`` 或 ``opcache_reset()`` 函数
   （即，在你通过Web执行的脚本中使用这些函数）;
3. 使用 `cachetool`_ 工具从CLI控制APC和OPcache。

.. _performance-configure-realpath-cache:

配置PHP实际路径缓存
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

当相对路径转换为其实际的绝对路径时，PHP会缓存结果以提高性能。
会打开许多PHP文件的应用（例如Symfony项目），应至少使用以下值：

.. code-block:: ini

    ; php.ini
    ; maximum memory allocated to store the results
    realpath_cache_size=4096K

    ; save the results for 10 minutes (600 seconds)
    realpath_cache_ttl=600

.. _performance-optimize-composer-autoloader:

优化 Composer 的自动加载
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在开发应用时使用的类加载器是为查找新的和更改的类做优化的。
在生产服务器中，除非部署新的应用版本，否则PHP文件不会变化。
这就是为什么你可以优化Composer的自动加载器来一次性扫描整个应用并构建一个“类映射”，它是所有类的位置的一个大数组，它存储在 ``vendor/composer/autoload_classmap.php`` 中。

执行此命令以生成类映射（并使其成为部署过程的一部分）：

.. code-block:: bash

    $ composer dump-autoload --optimize --no-dev --classmap-authoritative

* ``--optimize`` 转储(dump)你的应用中使用的每个PSR-0和PSR-4兼容类;
* ``--no-dev`` 排除仅在开发环境中需要的类（例如测试）;
* ``--classmap-authoritative`` 阻止Composer在文件系统中寻找那些没有出现在类映射中的类。

扩展阅读
----------

* :doc:`/http_cache/varnish`
* :doc:`/http_cache/form_csrf_caching`

.. _`字节码缓存`: https://en.wikipedia.org/wiki/List_of_PHP_accelerators
.. _`OPcache`: https://php.net/manual/en/book.opcache.php
.. _`bootstrap文件`: https://github.com/sensiolabs/SensioDistributionBundle/blob/master/Composer/ScriptHandler.php
.. _`Composer's autoloader optimization`: https://getcomposer.org/doc/articles/autoloader-optimization.md
.. _`APC`: https://php.net/manual/en/book.apc.php
.. _`APCu Polyfill component`: https://github.com/symfony/polyfill-apcu
.. _`APCu PHP functions`: https://php.net/manual/en/ref.apcu.php
.. _`cachetool`: https://github.com/gordalina/cachetool
