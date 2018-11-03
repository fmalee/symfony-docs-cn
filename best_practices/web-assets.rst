网站资源
==========

网站资源（web assets）是指CSS、JavaScript和图像文件之类的东西，它们令网站的前端页面看起来更漂亮。

.. best-practice::

    将资源存储在项目根目录的 ``assets/`` 目录中。

如果应用的所有资源都位于一个中心位置，你的设计人员和前端开发人员的生活将变得更加容易。

.. best-practice::

    使用 `Webpack Encore`_ 编译、组合和最小化网站资源。

`Webpack`_ 是领先的JavaScript模块bundler，可以编译、转换和打包资源，以便在浏览器中使用。
Webpack Encore 是一个JavaScript库，可以摆脱 Webpack 的大部分复杂性，而不会削减其任何功能或扭曲其使用和理念。

Webpack Encore 旨在缩减Symfony应用与基于JavaScript工具的现代Web应用之间的差距。
查看 `Webpack Encore文档`_，了解有关所有可用功能的更多信息。

----

下一章: :doc:`/best_practices/tests`

.. _`Webpack Encore`: https://github.com/symfony/webpack-encore
.. _`Webpack`: https://webpack.js.org/
.. _`Webpack Encore文档`: https://symfony.com/doc/current/frontend.html
