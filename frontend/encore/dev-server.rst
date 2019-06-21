使用webpack-dev-server和HMR
================================

在开发时，除了使用 ``yarn encore dev --watch``，你也可以使用 `webpack-dev-server`_：

.. code-block:: terminal

    $ yarn encore dev-server

该服务会生成新资产于一个新服务器中：``http://localhost:8080`` （它实际上不会将任何文件写入磁盘）。
这意味着你的 ``script`` 和 ``link`` 标签需要更改为指向它的URL。

如果你正在使用 ``encore_entry_script_tags()`` 和 ``encore_entry_link_tags()``
Twig快捷方式（或者是以某种其他方式 :ref:`通过entrypoints.json处理你的资产 <load-manifest-files>`
），那么你已经完工了：模板中的路径将自动指向该开发服务器。

你还可以传递选项到 ``dev-server`` 命令：正常 `webpack-dev-server`_ 支持的任何选项。例如：

.. code-block:: terminal

    $ yarn encore dev-server --https --port 9000

这将启动服务器于：``https://localhost:9000``。

热模块更换（HMR）
--------------------------

Encore *确实* 支持 `HMR`_，但仅限于某些领域。请传递 ``--hot`` 选项激活它：

.. code-block:: terminal

    $ ./node_modules/.bin/encore dev-server --hot

HMR目前适用于 :doc:`Vue.js </frontend/encore/vuejs>`，但目前在任何地方都 *不* 适用于样式。

.. _`webpack-dev-server`: https://webpack.js.org/configuration/dev-server/
.. _`HMR`: https://webpack.js.org/concepts/hot-module-replacement/
