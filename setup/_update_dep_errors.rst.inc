依赖错误
~~~~~~~~~~~~~~~~~

如果出现依赖性错误，则可能只是意味着你还需要升级其他Symfony依赖。
在这种情况下，请尝试以下命令：

.. code-block:: terminal

    $ composer update "symfony/*" --with-all-dependencies

此命令更新 ``symfony/symfony`` 和 *所有* 它所依赖的包，其中将包括其他几个包。
通过使用在 ``composer.json`` 中使用严格的版本约束，你可以控制每个库升级到的版本。

如果仍然无效，则你的 ``composer.json`` 文件可能指定了一个与较新的Symfony版本不兼容的库的版本。
在这种情况下，在 ``composer.json`` 中将该库更新为更新版本可能会解决该问题。

或者，你可能有更深层次的问题，其中不同的库依赖于其他库的冲突版本。检查你的错误消息以进行调试。

可能发生的另一个问题是项目依赖项可以安装在本地计算机上，但不能安装在远程服务器上。
当每个机器上的PHP版本不同时，通常会发生这种情况。
解决方案是将 `platform`_ 配置选项添加到 `composer.json` 文件中，以定义依赖允许的最高PHP版本
（将其设置为服务器的PHP版本）。

.. _`platform`: https://getcomposer.org/doc/06-config.md#platform
