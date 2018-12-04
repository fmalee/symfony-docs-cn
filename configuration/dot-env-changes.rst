2018年11月对.env的更改以及如何更新
========================================

在2018年11月，对与 ``.env`` 文件相关的核心Symfony *配方* 进行了一些更改。
这些更改使得处理环境变量变得更容易、更一致 - 尤其是在编写功能测试时。

如果你的应用在2018年11月之前启动，你的应用 **不需要任何更改即可继续使用**。
但是，如果/当你准备好利用这些改进时，你将需要进行一些小的更新。

改变了什么?
---------------------

但首先，究竟改变了什么？在高层，改变并不多。以下是最重要变化的一个摘要：

* A) ``.env.dist`` 文件不再存在。其内容应移至你的 ``.env`` 文件中（参见下一点）。

* B) ``.env`` 文件现在 **会** 提交到为你的仓库。而之前通过 ``.gitignore`` 文件忽略了它（更新的配方不会再忽略此文件）。
  由于此文件已可提交，因此它应包含非敏感的默认值。通俗的讲，``.env.dist`` 文件被移动到 ``.env``。

* C) 现在可以创建一个 ``.env.local`` 文件来 *重写* 你的计算机的环境变量。
  新的 ``.gitignore`` 文件已忽略此文件。

* D) 现在测试时，将读取你的 ``.env`` 文件，以使其与所有其他环境保持一致。
  你还可以创建一个 ``.env.test`` 文件来重写测试环境。

* E) 如果在运行 ``bin/console`` 时传递 ``--env=`` 标志，该值将覆盖你的 ``APP_ENV``
  环境变量（如果有设置的话）。所以，如果你传递了
  ``--env=prod``，DotEnv组件 *将* 尝试加载你的 ``.env*`` 文件。

还有一些其他改进，但这些是最重要的。为了充分利用这些特性，你 *将* 需要在现有的应用修改的几个文件。

更新你的应用
-----------------------

如果你在2018年11月15日之后创建了应用，则无需进行任何更改！
否则，以下是你需要进行的更改列表 - 可以对任何Symfony 3.4或更高版本的应用进行更改：

#. 在项目中创建一个新的 `config/bootstrap.php`_ 文件。
   此文件加载Composer的自动加载器并根据需要加载所有的 ``.env``
   文件 (注意: 在早期的指令中，这个文件被称为 ``src/.bootstrap.php``)。

#. 更新你的 `public/index.php`_ （`index.php diff`_ ）文件以加载新的 ``config/bootstrap.php`` 文件。
   如果你已自定义此文件，请确保保留那些更改（但使用其余更改）。

#. 更新你的 `bin/console`_ （`bin/console diff`_ ）文件以加载新的 ``config/bootstrap.php`` 文件。

#. 更新 ``.gitignore``:

.. code-block:: diff

    # .gitignore
    # ...

    ###> symfony/framework-bundle ###
    - /.env
    + /.env.local
    + /.env.*.local

    # ...

#. 重命名 ``.env`` 为 ``.env.local`` 以及重命名 ``.env.dist`` 为 ``.env``:

.. code-block:: terminal

    # Unix
    $ mv .env .env.local
    $ git mv .env.dist .env

    # Windows
    $ mv .env .env.local
    $ git mv .env.dist .env

    你还可以更新 `.env顶部的注释`_ 以反映新的更改。

#. 如果你正在使用PHPUnit，你还需要 `创建一个新的.env.test`_ 文件并更新你的
   `phpunit.xml.dist文件`_，以便加载 ``config/bootstrap.php`` 文件。

.. _`config/bootstrap.php`: https://github.com/symfony/recipes/blob/master/symfony/framework-bundle/4.2/src/.bootstrap.php
.. _`public/index.php`: https://github.com/symfony/recipes/blob/master/symfony/framework-bundle/4.2/public/index.php
.. _`index.php diff`: https://github.com/symfony/recipes/compare/8a4e5555e30d5dff64275e2788a901f31a214e79...86e2b6795c455f026e5ab0cba2aff2c7a18511f7#diff-473fca613b5bda15d87731036cb31586
.. _`bin/console`: https://github.com/symfony/recipes/blob/master/symfony/console/3.3/bin/console
.. _`bin/console diff`: https://github.com/symfony/recipes/compare/8a4e5555e30d5dff64275e2788a901f31a214e79...86e2b6795c455f026e5ab0cba2aff2c7a18511f7#diff-2af50efd729ff8e61dcbd936cf2b114b
.. _`.env顶部的注释`: https://github.com/symfony/recipes/blob/master/symfony/flex/1.0/.env
.. _`创建一个新的.env.test`: https://github.com/symfony/recipes/blob/master/symfony/phpunit-bridge/3.3/.env.test
.. _`phpunit.xml.dist文件`: https://github.com/symfony/recipes/blob/master/symfony/phpunit-bridge/3.3/phpunit.xml.dist
