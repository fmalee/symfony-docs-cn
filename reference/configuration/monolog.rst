.. index::
    pair: Monolog; Configuration reference

日志配置参考(MonologBundle)
===============================================

Symfony应用中的MonologBundle集成了Monolog :doc:`日志 </logging>` 库。
所有这些选项都在应用配置中的 ``monolog`` 键下配置。

.. code-block:: terminal

    # 显示Symfony定义的默认配置值
    $ php bin/console config:dump-reference monolog

    # 显示应用使用的实际配置值
    $ php bin/console debug:config monolog

.. note::

    使用XML时，必须使用 ``http://symfony.com/schema/dic/monolog``
    命名空间，并且相关的XSD架构可在以下位置使用：
    ``http://symfony.com/schema/dic/monolog/monolog-1.0.xsd``


.. tip::

    有关处理器类型和相关配置选项的完整列表，请参阅 `Monolog配置`_。

.. note::

    启用分析器后，将添加一个处理器以将日志的消息存储在分析器中。
    分析器使用了“debug”名称，因此它是保留的(reserved)，并且不能在配置中使用。

.. _`Monolog配置`: https://github.com/symfony/monolog-bundle/blob/master/DependencyInjection/Configuration.php
