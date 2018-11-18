复制&引用图像
============================

需要引用一个静态文件 - 比如 ``img`` 标签的图像路径？如果将资产存储在公共文档根目录之外，那可能会很棘手。
幸运的是，根据你的情况，有一个解决方案！

引用Webpacked脚本文件内部的图像
----------------------------------------------------------

要从脚本文件中引用一个图像标签，需要 *引入(require)* 以下文件：

.. code-block:: javascript

    // assets/js/app.js

    // 返回此文件的最终 *公共* 路径
    // 使用相对于此文件的路径 - 例如 assets/images/logo.png
    const logoPath = require('../images/logo.png');

    var html = `<img src="${logoPath}">`;

当你 ``require`` （或 ``import``）一个图像文件时，Webpack会将其复制到你的输出目录中，并返回该文件的最终 *公共* 路径。

从模板引用图像文件
---------------------------------------

要从Webpack处理的脚本文件外部引用图像文件（如模板），你可以使用 ``copyFiles()`` 方法将这些文件复制到最终的输出目录中。

.. code-block:: diff

    // webpack.config.js

    Encore
        // ...
        .setOutputPath('public/build/')

    +     .copyFiles({
    +         from: './assets/images',
    +
    +         // 可选的目标路径, 相对于输出目录
    +         //to: 'images/[path][name].[ext]',
    +
    +         // 只复制匹配此模式的文件
    +         //pattern: /\.(png|jpg|jpeg)$/
    +     })

这会将 ``assets/images`` 中的所有文件复制到 ``public/build`` （输出路径）中。
如果启用了 :doc:`版本控制 <versioning>`，则复制的文件将包含基于其内容的哈希值。

要在Twig内部渲染，请使用 ``asset()`` 函数：

.. code-block:: html+twig

    {# assets/images/logo.png 被复制到 public/build/logo.png #}
    <img src="{{ asset('build/logo.png') }}"

    {# assets/images/subdir/logo.png 被复制到 public/build/subdir/logo.png #}
    <img src="{{ asset('build/subdir/logo.png') }}"

请确保已启用 :ref:`json_manifest_path <load-manifest-files>`
选项，该选项告诉 ``asset()`` 函数从 ``manifest.json`` 文件中读取最终路径。
如果你不清楚要传递给 ``asset()`` 函数的路径参数，请在 ``manifest.json`` 中找到该文件并使用它的 *键* 作为参数。
