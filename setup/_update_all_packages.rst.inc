更新其他包
~~~~~~~~~~~~~~~~~~~~~~~~

你可能还想升级其余的库。
如果你已经在 ``composer.json`` 的 `版本约束`_ 中做好了正确的配置，你可以非常安全地运行：

.. code-block:: terminal

    $ composer update

.. caution::

    请注意，如果你的 ``composer.json`` 中存在某些未指定的 `版本约束`_ （例如 ``dev-master``），
    则可能会将某些非Symfony库升级到破坏向后兼容性的新版本。

.. _`版本约束`: https://getcomposer.org/doc/01-basic-usage.md#package-versions
