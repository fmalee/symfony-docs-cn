分析器
========

分析器是一个强大的 **开发工具**，它能将任何请求的执行结果详细的展示出来。

**不要** 在生产环境启用分析器，它会让你的项目出现一些安全隐患。

安装
------------

在应用中使用 :doc:`Symfony Flex </setup/flex>`，在使用前运行该命令安装分析器：

.. code-block:: terminal

    $ composer require --dev symfony/profiler-pack

.. toctree::
    :maxdepth: 1

    profiler/data_collector
    profiler/profiling_data
    profiler/matchers
    profiler/wdt_follow_ajax
