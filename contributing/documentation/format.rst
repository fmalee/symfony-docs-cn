文档格式
====================

Symfony文档使用 `reStructuredText`_ 作为其标记语言，使用 `Sphinx`_ 生成给最终用户阅读的格式文档，例如HTML和PDF。

reStructuredText
----------------

reStructuredText是一种类似于Markdown的纯文本标记语法，但其语法更为严格。
如果你不熟悉reStructuredText，请阅读现有的 `Symfony文档`_ 源代码，花些时间熟悉此格式。

如果你想了解有关此格式的更多信息，请参阅 `reStructuredText启蒙`_ 教程和 `reStructuredText参考`_。

.. caution::

    如果你熟悉Markdown，请小心，因为事情有时非常相似但不同：

    * 列表从一行的开头开始（不允许缩进）;
    * 内联代码块使用双刻度（````like this````）。

Sphinx
------

Sphinx_ 是一个构建系统，它提供了从reStructuredText文档创建文档的工具。
因此，它将新指令和解释文本角色添加到标准的reST标记中。
阅读更多关于 `Sphinx Markup Constructs`_ 的信息。

Syntax语法高亮
~~~~~~~~~~~~~~~~~~~

PHP是应用于所有代码块的默认语法高亮器。您可以使用 ``code-block`` 指令更改它：

.. code-block:: rst

    .. code-block:: yaml

        { foo: bar, bar: { foo: bar, bar: baz } }

.. note::

    除了所有主要的编程语言之外，语法高亮器还支持各种标记和配置语言。
    在语法高亮显示器网站上查看 `支持的语言`_ 列表。

.. _docs-configuration-blocks:

配置块
~~~~~~~~~~~~~~~~~~~~

当你要引用一个配置方案，使用 ``configuration-block`` 指令来展示所有支持的配置格式的配置
（``PHP``、``YAML`` 和 ``XML``）。例如：

.. code-block:: rst

    .. configuration-block::

        .. code-block:: yaml

            # Configuration in YAML

        .. code-block:: xml

            <!-- Configuration in XML -->

        .. code-block:: php

            // Configuration in PHP

之前的reST代码段渲染如下：

.. configuration-block::

    .. code-block:: yaml

        # Configuration in YAML

    .. code-block:: xml

        <!-- Configuration in XML -->

    .. code-block:: php

        // Configuration in PHP

当前支持的格式列表如下：

===================  ======================================
标记格式              用它来显示
===================  ======================================
``html``             HTML
``xml``              XML
``php``              PHP
``yaml``             YAML
``twig``             纯Twig标记
``html+twig``        Twig代码与HTML混合
``html+php``         PHP代码与HTML混合
``ini``              INI
``php-annotations``  PHP注释
===================  ======================================

添加链接
~~~~~~~~~~~~

最常见的链接类型是指向其他文档页面的\ **内部链接**，它们使用以下语法：

.. code-block:: rst

    :doc:`/absolute/path/to/page`

页面名称不应包含文件扩展名（``.rst``）。例如：

.. code-block:: rst

    :doc:`/controller`

    :doc:`/components/event_dispatcher`

    :doc:`/configuration/environments`

链接页面的标题将自动用作链接的文本。如果要修改该标题，请使用以下语法替代：

.. code-block:: rst

    :doc:`Spooling Email </email/spool>`

.. note::

    虽然它们在技术上是正确的，但是请避免使用如下所示的相关的内部链接，因为它们会破坏生成的PDF文档中的引用：

    .. code-block:: rst

        :doc:`controller`

        :doc:`event_dispatcher`

        :doc:`environments`

**API的链接** 遵循不同的语法，你必须在其中指定链接资源的类型（``namespace``、``class`` 或 ``method``）：

.. code-block:: rst

    :namespace:`Symfony\\Component\\BrowserKit`

    :class:`Symfony\\Component\\Routing\\Matcher\\ApacheUrlMatcher`

    :method:`Symfony\\Component\\HttpKernel\\Bundle\\Bundle::build`

**PHP文档的链接** 遵循非常相似的语法：

.. code-block:: rst

    :phpclass:`SimpleXMLElement`

    :phpmethod:`DateTime::createFromFormat`

    :phpfunction:`iterator_to_array`

新功能或行为更改
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你要书写一个全新的功能或在Symfony中进行的更改，你应该在更改的描述之前加上 ``.. versionadded:: 2.X`` 指令和简短描述：

.. code-block:: rst

    .. versionadded:: 2.7
        The ``askHiddenResponse()`` method was introduced in Symfony 2.7.

    You can also ask a question and hide the response. This is particularly [...]

如果你正在书写一个行为更改，则简要描述行为是如何更改的可能会有所帮助：

.. code-block:: rst

    .. versionadded:: 2.7
        The ``include()`` function is a new Twig feature that's available in
        Symfony 2.7. Prior, the ``{% include %}`` tag was used.

每当发布一个新的Symfony次要版本（例如2.4，2.5等）时，就会从 ``master`` 分支创建一个新的文档分支。
此时，将会删除已达到维护截止期的Symfony版本的所有 ``versionadded`` 标签。
例如，如果Symfony 2.5今天发布，并且2.2最近已达到其维护终止期，则2.2 ``versionadded`` 标签将从新的 ``2.5`` 分支中删除。

.. _reStructuredText: http://docutils.sourceforge.net/rst.html
.. _Sphinx: http://sphinx-doc.org/
.. _`Symfony文档`: https://github.com/symfony/symfony-docs
.. _`reStructuredText启蒙`: http://sphinx-doc.org/rest.html
.. _`reStructuredText参考`: http://docutils.sourceforge.net/docs/user/rst/quickref.html
.. _`Sphinx Markup Constructs`: http://sphinx-doc.org/markup/
.. _`支持的语言`: http://pygments.org/languages/
