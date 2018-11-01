Encore VS Assetic
======================

Symfony最初附带了对 :doc:`Assetic </frontend/assetic/index>` 的支持：
一个能够处理、组合、压缩CSS与JavaScript文件的纯PHP库。
虽然Encore现在是处理资产的推荐方式，但Assetic仍然运作良好。

那么Assetic和Encore之间有什么区别？

+--------------------------+-------------------------------+-------------------------+
|                          | Assetic                       | Encore                  +
+--------------------------+-------------------------------+-------------------------+
| Language                 | Pure PHP, relies on other     | Node.js                 |
|                          | language tools for some tasks |                         |
+--------------------------+-------------------------------+-------------------------+
| Combine assets?          | Yes                           | Yes                     |
+--------------------------+-------------------------------+-------------------------+
| Minify assets?           | Yes (when configured)         | Yes (out-of-the-box)    |
+--------------------------+-------------------------------+-------------------------+
| Process Sass/Less?       | Yes                           | Yes                     |
+--------------------------+-------------------------------+-------------------------+
| Loads JS Modules? [1]_   | No                            | Yes                     |
+--------------------------+-------------------------------+-------------------------+
| Load CSS Deps in JS? [1] | No                            | Yes                     |
+--------------------------+-------------------------------+-------------------------+
| React, Vue.js support?   | No [2]_                       | Yes                     |
+--------------------------+-------------------------------+-------------------------+
| Support                  | Not actively maintained       | Actively maintained     |
+--------------------------+-------------------------------+-------------------------+

.. [1] JavaScript模块允许你将脚本组织成称为模块的小文件并导入它们：

       .. code-block:: javascript

           // 引入第三方模块
           var $ = require('jquery');

           // 引入你自己的 CoolComponent.js 模块
           var coolComponent = require('./components/CoolComponent');

       Encore（通过Webpack）自动解析这些并创建一个包含所有必需依赖的脚本文件。你甚至可以引入CSS或图像。

.. [2] Assetic仅过时(outdated)支持React.js。Encore附带支持最新的React.js、Vue.js、TypeScript等。

我应该从Assetic升级到Encore？
---------------------------------------

如果你已经在应用中使用Assetic，并且不需要Encore提供超出Assetic的任何功能，那么继续使用Assetic就可以了。
如果你确实需要更多功能，那么你可能需要将业务案例更改为Encore。
