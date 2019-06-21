.. index::
    single: Configuration reference; WebProfiler

分析器配置参考 (WebProfilerBundle)
====================================================

WebProfilerBundle是一个 **开发工具**，提供有关每个请求执行的详细技术信息，并在Web调试工具栏和
:doc:`分析器 </profiler>` 中显示。所有这些选项都在应用配置中的 ``web_profiler`` 键下配置。

.. code-block:: terminal

    # 显示Symfony定义的默认配置值
    $ php bin/console config:dump-reference web_profiler

    # 显示应用使用的实际配置值
    $ php bin/console debug:config web_profiler

.. note::

    使用XML时，必须使用 ``http://symfony.com/schema/dic/webprofiler``
    命名空间，并且相关的XSD架构可在以下位置使用：
    ``https://symfony.com/schema/dic/webprofiler/webprofiler-1.0.xsd``

.. caution::

    Web调试工具栏不适用于 ``StreamedResponse`` 类型的响应。

配置
-------------

.. class:: list-config-options

* `excluded_ajax_paths`_
* `intercept_redirects`_
* `toolbar`_

excluded_ajax_paths
~~~~~~~~~~~~~~~~~~~

**类型**: ``string`` **默认值**: ``'^/((index|app(_[\w]+)?)\.php/)?_wdt'``

当工具栏记录Ajax请求时，它会根据此正则表达式匹配其URL。如果URL匹配，则该请求不会显示在工具栏中。
当应用发出大量Ajax请求或者它们很繁重(heavy)并且你想要排除其中一些URL时，这很有用。

.. _intercept_redirects:

intercept_redirects
~~~~~~~~~~~~~~~~~~~

**类型**: ``boolean`` **默认值**: ``false``

如果在HTTP响应期间发生重定向，浏览器会自动跟踪它，而你将看不到原始URL的工具栏或分析器，只看到重定向的URL。

当此选项设置为 ``true`` 时，浏览器会在进行任何重定向之前 *停止* 重定向，并显示要重定向到的URL、其工具栏以及及其分析器。
检查完工具栏/分析器的数据后，可以单击给定链接以执行该重定向。

toolbar
~~~~~~~

**类型**: ``boolean`` **默认值**: ``false``

它控制工具栏的启用和禁用。
通常在 ``dev`` 与 ``test`` 环境将其设置为 ``true``，在 ``prod`` 环境中设置为 ``false``。
