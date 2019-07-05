在虚拟机中使用Encore
=================================

Encore兼容虚拟机（如 `VirtualBox`_ 和 `VMWare`_），但你可能需要对配置进行一些更改才能使其正常工作。

文件监视问题
--------------------

使用虚拟机时，会使用 `NFS`_ 与虚拟机共享项目根目录。这会导致文件监视问题，因此你必须启用
`轮询`_ 选项才能使其正常工作：

.. code-block:: javascript

    // webpack.config.js

    // ...

    // 将应用于 `encore dev --watch` 和 `encore dev-server` 命令
    Encore.configureWatchOptions(watchOptions => {
        watchOptions.poll = 250; // 每250毫秒检查一次更改
    });

开发服务器问题
-------------------------

配置公共路径
~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::

    如果你的应用正运行在 ``http://localhost``，而不是像 ``http://app.vm``
    这样的自定义本地域名，则可以跳过此部分。

运行开发服务器时，你可能会在Web控制台中看到以下错误：

.. code-block:: text

    GET http://localhost:8080/build/vendors~app.css net::ERR_CONNECTION_REFUSED
    GET http://localhost:8080/build/runtime.js net::ERR_CONNECTION_REFUSED
    ...

如果你的Symfony应用在自定义域名（例如 ``http://app.vm``）上运行，则必须在你的
``package.json`` 中明确配置公共路径：

.. code-block:: diff

    {
        ...
        "scripts": {
    -        "dev-server": "encore dev-server",
    +        "dev-server": "encore dev-server --public http://app.vm:8080",
            ...
        }
    }

重新启动Encore并重新加载网页后，你可能会在Web控制台中看到不同的问题：

.. code-block:: text

    GET http://app.vm:8080/build/vendors~app.css net::ERR_CONNECTION_REFUSED
    GET http://app.vm:8080/build/runtime.js net::ERR_CONNECTION_REFUSED

你仍需要进行其他配置更改，具体如以下各节中所述。

允许外部访问
~~~~~~~~~~~~~~~~~~~~~

将 ``--host 0.0.0.0`` 参数添加到 ``package.json`` 文件中的 ``dev-server``
配置中，以使开发服务器接受所有传入连接：

.. code-block:: diff

    {
        ...
        "scripts": {
    -        "dev-server": "encore dev-server --public http://app.vm:8080",
    +        "dev-server": "encore dev-server --public http://app.vm:8080 --host 0.0.0.0",
            ...
        }
    }

.. caution::

    要确保仅在你的虚拟机内运行开发服务器；否则其他计算机也可以访问它。

修复"Invalid Host header"问题
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

尝试从开发服务器访问文件时，Webpack将响应 ``Invalid Host header``。要解决此问题，请添加
``--disable-host-check`` 参数：

.. code-block:: diff

    {
        ...
        "scripts": {
    -        "dev-server": "encore dev-server --public http://app.vm:8080 --host 0.0.0.0",
    +        "dev-server": "encore dev-server --public http://app.vm:8080 --host 0.0.0.0 --disable-host-check",
            ...
        }
    }

.. caution::

    请注意，一般 `不建议禁用主机检查`_，但是在虚拟机中使用Encore时需要解决那些问题。

.. _`VirtualBox`: https://www.virtualbox.org/
.. _`VMWare`: https://www.vmware.com
.. _`NFS`: https://en.wikipedia.org/wiki/Network_File_System
.. _`轮询`: https://webpack.js.org/configuration/watch/#watchoptionspoll
.. _`不建议禁用主机检查`: https://webpack.js.org/configuration/dev-server/#devserverdisablehostcheck
