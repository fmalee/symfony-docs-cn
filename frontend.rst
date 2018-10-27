前端
===========================

.. admonition:: 教学视频
    :class: screencast

    更喜欢视频教程? 可以观看 `Webpack Encore screencast series`_ 系列录像.

Symfony附带了一个纯JavaScript库 - 名为 Webpack Encore -- 这使得使用CSS和JavaScript变得更加快乐。
你可以选择使用它，或是其他东西，或者只在 ``public/`` 目录中创建静态CSS和JS文件，并将它们包含在模板中。

.. _frontend-webpack-encore:

Webpack Encore
--------------

`Webpack Encore`_ 是一种将 `Webpack`_ 集成到应用中的简单方法。
它 *封装* 了Webpack，为你提供了一个干净而强大的API，用于捆绑JavaScript模块，预处理CSS和JS以及编译和压缩资产。
Encore为能让你愉快的使用专业的资源系统。

Encore的灵感来自 `Webpacker`_ 和 `Mix`_，但仍然保持Webpack的精髓：
使用其功能，概念和命名约定来获得熟悉的感觉。
它旨在解决最常见的Webpack用例。

.. tip::

    Encore由 `Symfony`_ 发行，能*完美*\集成到Symfony应用中。
    但它可以很容易地用于任何语言的任何应用！

.. _encore-toc:

Encore 文档
--------------------

快速上手
...............

* :doc:`Installation </frontend/encore/installation>`
* :doc:`First Example </frontend/encore/simple-example>`

添加更多功能
....................

* :doc:`CSS Preprocessors: Sass, LESS, etc </frontend/encore/css-preprocessors>`
* :doc:`PostCSS and autoprefixing </frontend/encore/postcss>`
* :doc:`Enabling React.js </frontend/encore/reactjs>`
* :doc:`Enabling Vue.js (vue-loader) </frontend/encore/vuejs>`
* :doc:`Configuring Babel </frontend/encore/babel>`
* :doc:`Source maps </frontend/encore/sourcemaps>`
* :doc:`Enabling TypeScript (ts-loader) </frontend/encore/typescript>`

优化
..........

* :doc:`Versioning (and the manifest.json file) </frontend/encore/versioning>`
* :doc:`Using a CDN </frontend/encore/cdn>`
* :doc:`Creating a "Shared" entry for re-used modules </frontend/encore/shared-entry>`
* :doc:`/frontend/encore/url-loader`

指南
......

* :doc:`Using Bootstrap CSS & JS </frontend/encore/bootstrap>`
* :doc:`Creating Page-Specific CSS/JS </frontend/encore/page-specific-assets>`
* :doc:`jQuery and Legacy Applications </frontend/encore/legacy-apps>`
* :doc:`Passing Information from Twig to JavaScript </frontend/encore/server-data>`
* :doc:`webpack-dev-server and Hot Module Replacement (HMR) </frontend/encore/dev-server>`
* :doc:`Adding custom loaders & plugins </frontend/encore/custom-loaders-plugins>`
* :doc:`Advanced Webpack Configuration </frontend/encore/advanced-config>`

问 & 答
............

* :doc:`FAQ & Common Issues </frontend/encore/faq>`
* :doc:`/frontend/encore/versus-assetic`

完整的API
.............

* `完整的API`_: https://github.com/symfony/webpack-encore/blob/master/index.js

扩展阅读
------------------------

.. toctree::
    :hidden:
    :glob:

    frontend/assetic/index
    frontend/encore/installation
    frontend/encore/simple-example
    frontend/encore/*

.. toctree::
    :maxdepth: 1
    :glob:

    frontend/*

.. _`Webpack Encore`: https://www.npmjs.com/package/@symfony/webpack-encore
.. _`Webpack`: https://webpack.js.org/
.. _`Webpacker`: https://github.com/rails/webpacker
.. _`Mix`: https://laravel.com/docs/mix
.. _`Symfony`: http://symfony.com/
.. _`完整的API`: https://github.com/symfony/webpack-encore/blob/master/index.js
.. _`Webpack Encore screencast series`: https://symfonycasts.com/screencast/webpack-encore
