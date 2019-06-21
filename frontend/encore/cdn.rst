使用CDN
===========

你在部署CDN吗？那太棒了:)。确保将构建的文件上载到CDN后，在Encore中对其进行配置：

.. code-block:: diff

    // webpack.config.js
    // ...

    Encore
        .setOutputPath('public/build/')
        // 在开发模式下不使用CDN
        .setPublicPath('/build');
        // ...
    ;

    + if (Encore.isProduction()) {
    +     Encore.setPublicPath('https://my-cool-app.com.global.prod.fastly.net');
    +
    +     // 保证 manifest.json 中的键 *仍然* 以 “build/” 为前缀
    +     // (例如 "build/dashboard.js": "https://my-cool-app.com.global.prod.fastly.net/dashboard.js")
    +     Encore.setManifestKeyPrefix('build/');
    + }

仅此而已！在内部，Webpack现在知道从你的CDN加载资产 - 例如 ``https://my-cool-app.com.global.prod.fastly.net/dashboard.js``。

.. note::

    你仍然有必要将你的资产放在CDN上 - 例如通过上传或使用“origin pull”，你的CDN直接从你的Web服务器拉取资产。

你 *还* 需要确保的是，在你的网页内的 ``script`` 和 ``link`` 标签也使用CDN。
幸运的是，:ref:`entrypoints.json <encore-entrypointsjson-simple-description>`
路径已被更新为包含CDN的完整URL。

如果你正使用 ``Encore.enableIntegrityHashes()`` 并且你的CDN和你的域不是
`same-origin`_，你可能需要在你的 `webpack_encore.yaml` 配置中设置
``crossorigin`` 选项为 ``anonymous`` 或 ``use-credentials`` 来克服CORS错误。

.. _`same-origin`: https://en.wikipedia.org/wiki/Same-origin_policy
