安装Encore
=====================================

.. tip::

    如果你的项目 **不** 使用Symfony Flex中，请参阅 :doc:`/frontend/encore/installation-no-flex`。

首先，确保 `安装Node.js`_ 以及 `Yarn包管理器`_。然后运行：

.. code-block:: terminal

    $ composer require encore
    $ yarn install

这将创建一个 ``webpack.config.js`` 文件，添加 ``assets/`` 目录，然后添加 ``node_modules/`` 到 ``.gitignore``。

干得好！通过阅读 :doc:`/frontend/encore/simple-example` 来编写你的第一个JavaScript和CSS ！

.. _`安装Node.js`: https://nodejs.org/en/download/
.. _`Yarn包管理器`: https://yarnpkg.com/lang/en/docs/install/
