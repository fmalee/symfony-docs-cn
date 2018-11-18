异步代码拆分
====================

当你引入/导入JavaScript或CSS模块时，Webpack会将该代码编译为最终的JavaScript或CSS文件。
通常，这正是你想要的。但是如果你在某些条件下只需要使用一段代码呢？
例如，如果你想只有用户点击了一个链接后才使用 `video.js`_ 播放视频：

.. code-block:: javascript

    // assets/js/app.js

    import $ from 'jquery';
    // 一个虚构的“large”模块（例如它在内部导入video.js）
    import VideoPlayer from './components/VideoPlayer';

    $('.js-open-video').on('click', function() {
        // use the larger VideoPlayer module
        const player = new VideoPlayer('some-element');
    });

在此示例中，VideoPlayer模块及其导入的所有内容将打包到最终构建的JavaScript文件中，即使它可能不是很常用，但某些人确实需要它。
更好的解决方案是使用 `动态导入`_：在需要时才通过AJAX加载代码：

.. code-block:: javascript

    // assets/js/app.js

    import $ from 'jquery';

    $('.js-open-video').on('click', function() {
        // 你可以在这里启用一个加载动画

        // 使用 import() 作为一个函数 - 它返回一个 Promise
        import('./components/VideoPlayer').then(({ default: VideoPlayer }) => {
            // 你可以在这里停用一个加载动画

            // use the larger VideoPlayer module
            const player = new VideoPlayer('some-element');

        }).catch(error => 'An error occurred while loading the component');
    });

通过使用 ``import()`` 作为函数，该模块将被异步下载，并在完成后执行 ``.then()`` 回调。
回调的 ``VideoPlayer`` 参数将是已加载的模块。换句话说，它就像普通的AJAX调用一样！
在幕后，Webpack会将 ``VideoPlayer`` 模块打包成一个单独的文件（例如 ``0.js``），以便下载。
所有细节都是已你处理好。

``{ default: VideoPlayer }`` 部分可能看起来很奇怪。
使用异步导入时，会向你的 ``.then()`` 回调传递一个对象，其中 *实际* 的模块位于 ``.default`` 键上。
有这样做的理由，但它看起来确实有些古怪。
``{ default: VideoPlayer }`` 代码可确保从该 ``.default`` 属性中读取我们想要的 ``VideoPlayer`` 模块。

有关更多详细信息和配置选项，请参阅Webpack文档的 `动态导入`_。

.. _`video.js`: https://videojs.com/
.. _`动态导入`: https://webpack.js.org/guides/code-splitting/#dynamic-imports
