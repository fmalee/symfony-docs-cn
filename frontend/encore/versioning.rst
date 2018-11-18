资产版本控制
================

.. _encore-long-term-caching:

厌倦了部署和浏览器缓存旧版本的资产？通过调用 ``enableVersioning()``，
每个文件名现在将包括每当该文件的 *内容* 变更时改变的散列（例如 ``app.123abc.js``，而不是 ``app.js``）。
这允许你使用积极的缓存策略（例如，遥远未来的 ``Expires``），因为每当文件更改时，其哈希将改变，从而忽略任何现有缓存：

.. code-block:: diff

    // webpack.config.js
    // ...

    Encore
        .setOutputPath('public/build/')
        // ...
    +     .enableVersioning()

要链接到这些资源，Encore会创建两个文件：``entrypoints.json`` 和 ``manifest.json``。

.. _load-manifest-files:

从entrypoints.json & manifest.json文件加载资产
----------------------------------------------------

每当运行Encore时，都会生成两个配置文件：``entrypoints.json`` 和 ``manifest.json``。
每个文件都相似，并包含最终、版本化的文件名的一个映射。

第一个文件 - ``entrypoints.json`` - 由 ``encore_entry_script_tags()`` 和 ``encore_entry_link_tags()`` Twig助手使用。
如果你正在使用这些，那么你的CSS和JavaScript文件将使用新的版本化的文件名进行渲染。
如果你不使用Symfony，你的应用将需要以类似的方式读取此文件。

``manifest.json`` 文件只需要获取 *其他* 文件的版本化文件名，如字体文件或图像文件（尽管它还包含有关CSS和JavaScript文件的信息）：

.. code-block:: json

    {
        "build/app.js": "/build/app.123abc.js",
        "build/dashboard.css": "/build/dashboard.a4bf2d.css",
        "build/images/logo.png": "/build/images/logo.3eed42.png"
    }

在你的应用中，如果你希望能够链接（例如通过 ``img`` 标签）某些资产，则需要读取此文件。
如果你使用的是Symfony，只需激活 ``json_manifest_file`` 版本控制策略：

.. code-block:: yaml

    # this file is added automatically when installing Encore with Symfony Flex
    # config/packages/assets.yaml
    framework:
        assets:
            json_manifest_path: '%kernel.project_dir%/public/build/manifest.json'

仅此而已！像平常一样确保每个路径封装在Twig的 ``asset()`` 函数中：

.. code-block:: twig

    <img src="{{ asset('build/images/logo.png') }}">

扩展阅读
----------

* :doc:`/components/asset`
