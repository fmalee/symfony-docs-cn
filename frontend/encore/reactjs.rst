启用React.js
=================

使用React？确保安装了React，以及 `babel-preset-react`_：

.. code-block:: terminal

    $ yarn add --dev babel-preset-react
    $ yarn add react react-dom prop-types

在你的 ``webpack.config.js`` 中启用react：

.. code-block:: javascript

    // webpack.config.js
    // ...

    Encore
        // ...
        .enableReactPreset()
    ;

仅此而已！你的 ``.js`` 和 ``.jsx`` 文件现在将通过 ``babel-preset-react`` 转换(transform)。

.. _`babel-preset-react`: https://babeljs.io/docs/plugins/preset-react/
