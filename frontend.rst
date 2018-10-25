管理 CSS 和 JavaScript
===========================

.. admonition:: Screencast
    :class: screencast

    Do you prefer video tutorials? Check out the `Webpack Encore screencast series`_.

Symfony ships with a pure-JavaScript library - called Webpack Encore - that makes
working with CSS and JavaScript a joy. You can use it, use something else, or just
create static CSS and JS files in your ``public/`` directory and include them in your
templates.

.. _frontend-webpack-encore:

Webpack Encore
--------------

`Webpack Encore`_ is a simpler way to integrate `Webpack`_ into your application.
It *wraps* Webpack, giving you a clean & powerful API for bundling JavaScript modules,
pre-processing CSS & JS and compiling and minifying assets. Encore gives you professional
asset system that's a *delight* to use.

Encore is inspired by `Webpacker`_ and `Mix`_, but stays in the spirit of Webpack:
using its features, concepts and naming conventions for a familiar feel. It aims
to solve the most common Webpack use cases.

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
..................

* :doc:`FAQ & Common Issues </frontend/encore/faq>`
* :doc:`/frontend/encore/versus-assetic`

Full API
........

* `Full API`_: https://github.com/symfony/webpack-encore/blob/master/index.js

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
.. _`Full API`: https://github.com/symfony/webpack-encore/blob/master/index.js
.. _`Webpack Encore screencast series`: https://symfonycasts.com/screencast/webpack-encore
