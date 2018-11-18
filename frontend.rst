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

    Encore由 `Symfony`_ 发行，能\ *完美*\集成到Symfony应用中。
    但它可以很容易地用于任何语言的任何应用！

.. _encore-toc:

Encore 文档
--------------------

快速上手
...............

* :doc:`安装 </frontend/encore/installation>`
* :doc:`第一个示例 </frontend/encore/simple-example>`

添加更多功能
....................

* :doc:`CSS预处理器：Sass、LESS等 </frontend/encore/css-preprocessors>`
* :doc:`PostCSS和autoprefixing </frontend/encore/postcss>`
* :doc:`启用React.js </frontend/encore/reactjs>`
* :doc:`启用Vue.js (vue-loader) </frontend/encore/vuejs>`
* :doc:`/frontend/encore/copy-files`
* :doc:`配置Babel </frontend/encore/babel>`
* :doc:`源映射 </frontend/encore/sourcemaps>`
* :doc:`启用TypeScript (ts-loader) </frontend/encore/typescript>`

优化
..........

* :doc:`版本控制(以及entrypoints.json/manifest.json文件) </frontend/encore/versioning>`
* :doc:`使用CDN </frontend/encore/cdn>`
* :doc:`/frontend/encore/code-splitting`
* :doc:`/frontend/encore/split-chunks`
* :doc:`为复用模块创建共享条目 </frontend/encore/shared-entry>`
* :doc:`/frontend/encore/url-loader`

指南
......

* :doc:`使用Bootstrap的CSS和JS </frontend/encore/bootstrap>`
* :doc:`创建特定于页面的CSS/JS </frontend/encore/page-specific-assets>`
* :doc:`jQuery和旧应用 </frontend/encore/legacy-apps>`
* :doc:`将信息从Twig传递到脚本 </frontend/encore/server-data>`
* :doc:`webpack-dev-server和热模块更换(HMR) </frontend/encore/dev-server>`
* :doc:`添加自定义加载器&插件 </frontend/encore/custom-loaders-plugins>`
* :doc:`高级Webpack配置 </frontend/encore/advanced-config>`

问 & 答
............

* :doc:`FAQ和常见错误 </frontend/encore/faq>`
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
