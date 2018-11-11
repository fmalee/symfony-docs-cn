.. index::
    single: Templating; Linting
    single: Twig; Linting
    single: Templating; Syntax Check
    single: Twig; Syntax Check

如何检查Twig模板的语法
==============================================

你可以使用 ``lint:twig`` 控制台命令检查Twig模板的语法错误：

.. code-block:: terminal

    # 可以使用文件名进行检查:
    $ php bin/console lint:twig templates/article/recent_list.html.twig

    # 或是通过目录:
    $ php bin/console lint:twig templates/
