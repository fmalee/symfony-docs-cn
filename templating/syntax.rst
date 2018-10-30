.. index::
    single: Templating; Linting
    single: Twig; Linting
    single: Templating; Syntax Check
    single: Twig; Syntax Check

如何检查Twig模板的语法
==============================================

You can check for syntax errors in Twig templates using the ``lint:twig``
console command:

.. code-block:: terminal

    # You can check by filename:
    $ php bin/console lint:twig templates/article/recent_list.html.twig

    # or by directory:
    $ php bin/console lint:twig templates/
