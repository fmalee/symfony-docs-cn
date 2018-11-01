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

要链接到这些资源，Encore会创建一个包含新文件名映射的 ``manifest.json`` 文件。

.. _load-manifest-files:

从manifest.json文件加载资产
------------------------------------------

无论何时运行Encore，会在你的 ``outputPath`` 目录中自动创建一个 ``manifest.json`` 文件：

.. code-block:: json

    {
        "build/app.js": "/build/app.123abc.js",
        "build/dashboard.css": "/build/dashboard.a4bf2d.css"
    }

在你的应用中，你需要读取此文件以在你的 ``script`` 和 ``link`` 标签中动态渲染正确的路径。
如果你使用的是Symfony，请激活 ``json_manifest_file`` 版本控制策略：

.. code-block:: yaml

    # this file is added automatically when installing Encore with Symfony Flex
    # config/packages/assets.yaml
    framework:
        assets:
            json_manifest_path: '%kernel.project_dir%/public/build/manifest.json'

仅此而已！像平常一样确保每个路径封装在Twig的 ``asset()`` 函数中：

.. code-block:: twig

    <script src="{{ asset('build/app.js') }}"></script>

    <link href="{{ asset('build/dashboard.css') }}" rel="stylesheet" />

扩展阅读
----------

* :doc:`/components/asset`
