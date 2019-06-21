.. index::
    single: Translation; Lint
    single: Translation; Translation File Errors

如何在翻译文件中查找错误
=======================================

Symfony在执行应用代码之前，将所有应用翻译文件作为编译应用代码的进程的一部分来处理。
因此如果任何翻译文件中存在错误，你将看到一个解释该问题的错误消息。

如果你愿意，还可以使用 ``lint:yaml`` 和 ``lint:xliff``
命令来验证任何YAML和XLIFF翻译文件的内容：

.. code-block:: terminal

    # lint 单个文件
    $ php bin/console lint:yaml translations/messages.en.yaml
    $ php bin/console lint:xliff translations/messages.en.xlf

    # lint 整个目录
    $ php bin/console lint:yaml translations
    $ php bin/console lint:xliff translations

    # lint 多个文件或目录
    $ php bin/console lint:yaml translations path/to/trans
    $ php bin/console lint:xliff translations/messages.en.xlf translations/messages.es.xlf

可以使用 ``--format`` 选项将linter的结果导出为JSON：

.. code-block:: terminal

    $ php bin/console lint:yaml translations/ --format=json
    $ php bin/console lint:xliff translations/ --format=json
