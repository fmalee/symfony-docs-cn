文档标准
=======================

贡献必须遵循这些标准，以匹配Symfony其他文档的风格和基调。

Sphinx
------

* 为不同的标题级别选择以下字符：级别1是 ``=`` （等号），级别2  ``-`` （破折号），
  级别3  ``~`` （代字号），级别4 ``.`` （点）和级别5 ``"`` （双引号）;
* 每一行应该大约中断在第一个跨过第72个字符的单词之后（因此大多数行最终为72-78个字符）;
* The ``::`` shorthand is *preferred* over ``.. code-block:: php`` to begin a PHP
  code block unless it results in the marker being on its own line
  （你需要简写时，请参阅 `Sphinx文档`_）;
* **不** 使用内联超链接。分割链接及其目标定义，并将链接添加再页面底部;
* 内联标记应该与开放字符串在同一行上关闭;

示例
~~~~~~~

.. code-block:: text

    示例
    =======

    当你在处理文档时，应该遵循
    `Symfony文档`_ 标准。

    第二层
    -------

    一个PHP示例应该是::

        echo 'Hello World';

    第三层
    ~~~~~~~

    .. code-block:: php

        echo 'You cannot use the :: shortcut here';

    .. _`Symfony文档`: https://symfony.com/doc

代码示例
-------------

* 该代码遵循 :doc:`Symfony代码标准 </contributing/code/standards>` 以及 `Twig代码标准`_;
* 代码示例应该是真实的Web应用上下文。避免抽象的或简单的例子（``foo``、``bar``、``demo`` 等）;
* 代码应遵循 :doc:`Symfony最佳实践 </best_practices/introduction>`。
* 代码需要一个vendor名称是，使用 ``Acme``;
* 使用 ``example.com`` 作为示例网址的域名，需要额外的域名时，使用 ``example.org`` 与 ``example.net`` 。
  所有这些域都 `由IANA保留`_。
* 为了避免代码块的水平滚动，我们倾向于在它穿过第85个字符时正确地新起一行;
* 折叠一行或多行代码时，请 ``...`` 在折叠处放置注释。
  这些注释是：``// ...`` (php), ``# ...`` (yaml/bash), ``{# ... #}``
  (twig), ``<!-- ... -->`` (xml/html), ``; ...`` (ini), ``...`` (text);
* 折叠一行的一部分时（例如变量值）， 放置``...``（不带注释）在折叠处;
* 折叠代码说明:(可选）

  * 如果你折叠多行：折叠的描述可以放在 ``...``;
  * 如果只折叠一行的一部分：描述可以放在行之前;

* 如果对读者有用，一个PHP代码示例应该从声明命名空间开始;
* 引用类时，请务必在代码块的顶部显示 ``use`` 语句。
  你不需要在每个示例中显示 *所有*  ``use`` 语句，只需显示代码块中实际使用的内容;
* 如果有用，一个 ``codeblock`` 应该以"包含代码块的文件的文件名的注释”开头。
  除非下一行也是注释，否则请勿在该注释后留空行;
* 你应该放置一个 ``$`` 到每个bash线前面。

格式
~~~~~~~

配置示例应使用 :ref:`配置块 <docs-configuration-blocks>` 来显示所有支持的格式。
支持的格式（以及他们的排序）是：

* **配置** （包括服务）: YAML, XML, PHP
* **路由**: 注释, YAML, XML, PHP
* **验证**: 注释, YAML, XML, PHP
* **Doctrine映射**: 注释, YAML, XML, PHP
* **翻译**: XML, YAML, PHP

示例
~~~~~~~

.. code-block:: php

    // src/Foo/Bar.php
    namespace Foo;

    use Acme\Demo\Cat;
    // ...

    class Bar
    {
        // ...

        public function foo($bar)
        {
            // set foo with a value of bar
            $foo = ...;

            $cat = new Cat($foo);

            // ... check if $bar has the correct value

            return $cat->baz($bar, ...);
        }
    }

.. caution::

    在YAML中，你应该在 ``{`` 之后和 ``}`` 之前各放置一个空格（例如 ``{ _controller: ... }``），
    但这不应该在Twig中这样做（例如 ``{'hello' : 'value'}``）。

文件和目录
---------------------

* 引用目录时，始终添加尾部斜杠以避免与常规文件混淆
  （例如 “执行 ``bin/`` 目录中的 ``console`` 脚本”）。
* 明确引用文件扩展名时，应为每个扩展名包含一个前导点（例如 “XML文件使用 ``.xml`` 扩展名”）。
* 列出Symfony文件/目录层次结构时，请将 ``your-project/`` 用作顶级目录。例如

  .. code-block:: text

      your-project/
      ├─ app/
      ├─ src/
      ├─ vendor/
      └─ ...

英语语言标准
--------------------------

Symfony文档使用美国英语方言，通常称为 `美国英语`_。使用 `美国英语牛津字典`_ 作为词汇参考。

此外，文档遵循以下规则:

* **章节标题**: 使用一个标题案例的变体，其中第一个单词总是大写，
  所有其他词(other words)都大写，除了封闭类词（阅读维基百科关于 `headings and titles`_ 的文章）。

  E.g.: The Vitamins are in my Fresh California Raisins

* **标点符号**: 避免使用 `连续 (牛津) 逗号`_;
* **代词**: 避免使用 `nosism`_ 并总是使用 *你* 而不是 *我们*。（即避免第一人称观点：改用第二人称）;
* **性别中立的语言**: 在引用假设的人时，例如 *“带有会话cookie的用户”*，
  使用性别中性的代词（they/their/them）。例如：

  * he or she, use they
  * him or her, use them
  * his or her, use their
  * his or hers, use theirs
  * himself or herself, use themselves

.. _`Sphinx文档`: http://sphinx-doc.org/rest.html#source-code
.. _`Twig代码标准`: https://twig.symfony.com/doc/2.x/coding_standards.html
.. _`由IANA保留`: http://tools.ietf.org/html/rfc2606#section-3
.. _`美国英语`: https://en.wikipedia.org/wiki/American_English
.. _`美国英语牛津字典`: http://en.oxforddictionaries.com/definition/american_english/
.. _`headings and titles`: https://en.wikipedia.org/wiki/Letter_case#Headings_and_publication_titles
.. _`连续 (牛津) 逗号`: https://en.wikipedia.org/wiki/Serial_comma
.. _`nosism`: https://en.wikipedia.org/wiki/Nosism
