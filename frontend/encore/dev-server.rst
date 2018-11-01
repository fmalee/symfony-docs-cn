使用webpack-dev-server和HMR
================================

在开发时，除了使用 ``encore dev --watch``，你也可以使用 `webpack-dev-server`_：

.. code-block:: terminal

    $ ./node_modules/.bin/encore dev-server

该服务会生成新资产于一个新服务器中：``http://localhost:8080``（它实际上不会将任何文件写入磁盘）。
这意味着你的 ``script`` 和 ``link`` 标签需要更改为指向它的URL。

如果你已激活 :ref:`manifest.json版本控制 <load-manifest-files>`，
则意味着你已完工：模板中的路径将自动指向该开发服务器。

你还可以传递选项到 ``dev-server`` 命令：正常 `webpack-dev-server`_ 支持的任何选项。例如：

.. code-block:: terminal

    $ ./node_modules/.bin/encore dev-server --https --port 9000

这将启动服务器于：``https://localhost:9000``。

.. note::

    此Webpack服务器独立于 :doc:`Symfony的开发Web服务器 </setup/built_in_web_server>`，你需要单独运行它们。

VM中使用dev-server
----------------------------

如果你在虚拟机内部使用 ``dev-server``，则需要绑定到所有IP地址并允许任何主机访问该服务器：

.. code-block:: terminal

    $ ./node_modules/.bin/encore dev-server --host 0.0.0.0 --disable-host-check

你现在可以使用虚拟机的IP地址的8000端口访问开发服务器 - 例如 http://192.168.1.1:8080。

热模块更换（HMR）
--------------------------

Encore *确实* 支持 `HMR`_，但仅限于某些领域。请传递 ``--hot`` 选项激活它：

.. code-block:: terminal

    $ ./node_modules/.bin/encore dev-server --hot

HMR目前适用于 :doc:`Vue.js </frontend/encore/vuejs>`，但目前在任何地方都 *不* 适用于样式。

.. _`webpack-dev-server`: https://webpack.js.org/configuration/dev-server/
.. _`HMR`: https://webpack.js.org/concepts/hot-module-replacement/
