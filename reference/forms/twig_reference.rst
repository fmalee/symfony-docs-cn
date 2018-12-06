.. index::
   single: Forms; Twig form function reference

Twig模板表单函数和变量参考
==================================================

在一个模板中处理表单时，你可以使用两个强大的功能：

* 用于渲染表单的每个部分的 :ref:`函数 <reference-form-twig-functions>`;
* 用于获取任何字段的 *任何* 信息的 :ref:`变量 <twig-reference-form-variables>`。

你将经常使用函数来渲染字段。
另一方面，变量不太常用，但是无限强大，因为你可以访问字段的标签、id属性、错误以及有关该字段的任何其他内容。

.. _reference-form-twig-functions:

表单渲染函数
------------------------

本参考手册涵盖了可用于渲染表单的所有可用的Twig函数。
有几种不同的函数可用，每种函数都负责渲染表单的不同部分（例如标签、错误、部件等）

.. _reference-forms-twig-form:

form(view, variables)
---------------------

渲染一个完整表单的HTML。

.. code-block:: twig

    {# 渲染表单并更改提交方法 #}
    {{ form(form, {'method': 'GET'}) }}

你将主要使用此辅助函数进行原型设计(prototyping)或使用自定义表单主题。
如果在渲染表单时需要更大的灵活性，则应使用其他辅助函数来渲染表单的各个部分：

.. code-block:: twig

    {{ form_start(form) }}
        {{ form_errors(form) }}

        {{ form_row(form.name) }}
        {{ form_row(form.dueDate) }}

        {{ form_row(form.submit, { 'label': 'Submit me' }) }}
    {{ form_end(form) }}

.. _reference-forms-twig-start:

form_start(view, variables)
---------------------------

渲染表单的开始标签。此辅助函数负责打印已配置的方法和表单的动作。
如果表单包含上传字段，它还将包含正确的 ``enctype`` 属性。

.. code-block:: twig

    {# 渲染开始标签并更改提交方法 #}
    {{ form_start(form, {'method': 'GET'}) }}

.. _reference-forms-twig-end:

form_end(view, variables)
-------------------------

渲染表单的结束标签。

.. code-block:: twig

    {{ form_end(form) }}

除非你设置 ``render_rest`` 为 ``false``，否则此助手也将输出 ``form_rest()``：

.. code-block:: twig

    {# 不渲染未手动渲染的字段 #}
    {{ form_end(form, {'render_rest': false}) }}

.. _reference-forms-twig-label:

form_label(view, label, variables)
----------------------------------

渲染给定字段的标签。你可以在第二个参数中传入需要显示的特定标签。

.. code-block:: twig

    {{ form_label(form.name) }}

    {# 以下两种语法是等效的 #}
    {{ form_label(form.name, 'Your Name', {'label_attr': {'class': 'foo'}}) }}

    {{ form_label(form.name, null, {
        'label': 'Your name',
        'label_attr': {'class': 'foo'}
    }) }}

请参阅 ":ref:`twig-reference-form-variables`" 以了解 ``variables`` 参数。

.. _reference-forms-twig-errors:

form_errors(view)
-----------------

渲染给定字段的任何错误。

.. code-block:: twig

    {{ form_errors(form.name) }}

    {# 渲染任何"全局"错误 #}
    {{ form_errors(form) }}

.. _reference-forms-twig-widget:

form_widget(view, variables)
----------------------------

渲染给定字段的HTML部件。如果将此应用于整个表单或字段集合，则将渲染每个底层表单行。

.. code-block:: twig

    {# 渲染一个部件，同时添加一个“foo”样式类 #}
    {{ form_widget(form.name, {'attr': {'class': 'foo'}}) }}

``form_widget()`` 的第二个参数是一个变量数组。
最常见的变量是 ``attr``，它是应用于HTML部件的一个HTML属性数组。
在某些情况下，某些类型还具有可以传递与模板相关的其他选项。
这些是逐个类型讨论的。如果你一次（例如 ``form_widget(form)``）渲染多个字段，则
``attributes`` 不会递归地应用于子字段。

请参阅 ":ref:`twig-reference-form-variables`" 以了解 ``variables`` 参数。

.. _reference-forms-twig-row:

form_row(view, variables)
-------------------------

渲染给定字段的“行”，即该字段的标签、错误和部件的组合。

.. code-block:: twig

    {# 渲染一个字段行，同时显示带有“foo”文本的标签 #}
    {{ form_row(form.name, {'label': 'foo'}) }}

``form_row()`` 的第二个参数是一个变量数组。Symfony提供的模板仅允许重写标签，如上例所示。

请参阅 ":ref:`twig-reference-form-variables`" 以了解 ``variables`` 参数。

.. _reference-forms-twig-rest:

form_rest(view, variables)
--------------------------

这将渲染尚未为给定表单渲染的所有剩余字段。
总是将它放置在你的表单中的某个地方是一个好主意，因为它会为你渲染隐藏的字段，并让那些你忘记渲染的任何字段更加明显
（因为它会为你渲染该字段）。

.. code-block:: twig

    {{ form_rest(form) }}

表单测试参考
--------------------

可以使用Twig中的 ``is`` 运算符以创建条件来执行测试。
阅读 `Twig文档`_ 以获取更多信息。

.. _form-twig-selectedchoice:

selectedchoice(selected_value)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

此测试将检查当前选择是否等于 ``selected_value`` ，或当
``selected_value`` 是一个数组时，当前选择是否在该数组中。

.. code-block:: twig

    <option {% if choice is selectedchoice(value) %} selected="selected"{% endif %} ...>

.. _form-twig-rootform:

rootform
~~~~~~~~

此测试将检查当前的 ``form`` 是否有一个父表单视图。

.. code-block:: twig

    {# DON'T DO THIS: 这个简单的检查不能区分具有父表单视图的表单与
       一个定义了名为“parent”的嵌套表单字段的表单之间的却别 #}

    {% if form.parent is null %}
        {{ form_errors(form) }}
    {% endif %}

   {# DO THIS：此检查始终可靠，即使该表单定义了一个名为“parent”的字段。 #}

    {% if form is rootform %}
        {{ form_errors(form) }}
    {% endif %}

.. _`twig-reference-form-variables`:

表单变量
-------------------------

.. tip::

    有关变量的完整列表，请参阅 :ref:`reference-form-twig-variables`。

在上面的几乎每个Twig函数中，在渲染表单的一部分时，最后一个参数都是一个“变量”数组。
例如，以下内容将渲染字段的“部件”并修改其属性以包含一个特殊样式类：

.. code-block:: twig

    {# 渲染一个部件，同时添加一个“foo”样式类 #}
    {{ form_widget(form.name, { 'attr': {'class': 'foo'} }) }}

这些变量的目的 - 它们的作用以及它们来自何处 - 可能不会立即明确，但它们非常强大。
每当渲染表单的任何部分时，渲染它的区块都会使用许多变量。
默认情况下，这些区块位于 `form_div_layout.html.twig`_ 中。

看一下这个 ``form_label`` 例子：

.. code-block:: twig

    {% block form_label %}
        {% if not compound %}
            {% set label_attr = label_attr|merge({'for': id}) %}
        {% endif %}

        {% if required %}
            {% set label_attr = label_attr|merge({
                'class': (label_attr.class|default('') ~ ' required')|trim
            }) %}
        {% endif %}

        {% if label is empty %}
            {% set label = name|humanize %}
        {% endif %}

        <label
            {% for attrname, attrvalue in label_attr -%}
                {{ attrname }}="{{ attrvalue }}"
            {%- endfor %}
        >
            {{ label|trans({}, translation_domain) }}
        </label>
    {% endblock form_label %}

该区块利用几个变量：``compound``、``label_attr``、``required``、``label``、``name``
以及 ``translation_domain``。这些变量由表单渲染系统提供。
但更重要的是，这些是你在调用 ``form_label()`` （因为在此示例中，你正在渲染标签）时可以重写的变量。

可以重写的确切变量取决于你要渲染的表单的对应部分（例如标签与部件）以及你正在渲染的字段（例如，
``choice`` 部件有额外的 ``expanded`` 选项）。
如果你能自在的浏览 `form_div_layout.html.twig`_，你将始终能够看到可用的选项。

.. tip::

    在幕后，当Form组件在你的表单树的每个“节点”上调用 ``buildView()`` 和 ``finishView()``
    时，这些变量都在该表单的 ``FormView`` 对象上可用。
    要查看一个特定字段具有的“视图”变量，请查找表单字段（及其父字段）的源代码，并浏览上述两个函数。

.. note::

    如果你一次渲染整个表单（或整个嵌入表单），则 ``variables``
    参数将仅应用于表单本身而不是其子字段。
    换句话说，以下内容 **不会** 将“foo”样式类属性传递给表单中的所有子字段：

    .. code-block:: twig

        {# **不会** 生效 - 变量不是递归的 #}
        {{ form_widget(form, { 'attr': {'class': 'foo'} }) }}

.. _reference-form-twig-variables:

表单变量参考
~~~~~~~~~~~~~~~~~~~~~~~~

以下变量对于每种字段类型都是通用的。某些字段类型可能包含更多变量，而某些变量则仅适用于某些类型。

假设模板中有一个 ``form`` 变量并且你想在 ``name`` 字段上引用该变量，则可以通过在
:class:`Symfony\\Component\\Form\\FormView` 对象上使用一个 ``vars`` 公有属性来访问该变量:

.. code-block:: html+twig

    <label for="{{ form.name.vars.id }}"
        class="{{ form.name.vars.required ? 'required' }}">
        {{ form.name.vars.label }}
    </label>

+------------------------+-------------------------------------------------------------------------------------+
| 变量                   | 用法                                                                                |
+========================+=====================================================================================+
| ``form``               | 当前的 ``FormView`` 实例。                                                          |
+------------------------+-------------------------------------------------------------------------------------+
| ``id``                 | 要渲染的 ``id`` HTML属性。                                                          |
+------------------------+-------------------------------------------------------------------------------------+
| ``name``               | 字段的名称（例如 ``title``） - 但不是 ``name`` HTML属性，``full_name`` 才是。       |
+------------------------+-------------------------------------------------------------------------------------+
| ``full_name``          | 要渲染的 ``name`` HTML属性。                                                        |
+------------------------+-------------------------------------------------------------------------------------+
| ``errors``             | 附加到 *此* 特定字段的一个任何错误的数组（例如 ``form.title.errors``）。            |
|                        | 请注意，不能使用 ``form.errors`` 来确定一个表单是否有效，                           |
|                        | 因为此变量只会返回“全局”的错误：某些单独的字段可能有错误。                          |
|                        | 所以，请使用 ``valid`` 选项。                                                       |
+------------------------+-------------------------------------------------------------------------------------+
| ``submitted``          | 返回 ``true`` 或 ``false``，这取决于整个表单是否提交。                              |
+------------------------+-------------------------------------------------------------------------------------+
| ``valid``              | 返回 ``true`` 或 ``false``，这取决于整个表单是否有效。                              |
+------------------------+-------------------------------------------------------------------------------------+
| ``value``              | 渲染时要使用的值（通常是 ``value`` HTML属性）。                                     |
+------------------------+-------------------------------------------------------------------------------------+
| ``disabled``           | 如果为 ``true``，将在该字段添加 ``disabled="disabled``。                            |
+------------------------+-------------------------------------------------------------------------------------+
| ``required``           | 如果是 ``true``，则在该字段中添加一个 ``required`` 属性以激活HTML5验证。            |
|                        | 另外，在标签中添加了一个 ``required`` 样式类。                                      |
+------------------------+-------------------------------------------------------------------------------------+
| ``label``              | 要渲染的字符串标签。                                                                |
+------------------------+-------------------------------------------------------------------------------------+
| ``multipart``          | 如果是 ``true``，``form_enctype`` 将渲染 ``enctype="multipart/form-data"``。        |
|                        | 此变量仅适用于根表单元素。                                                          |
+------------------------+-------------------------------------------------------------------------------------+
| ``attr``               | 一个键值对数组，将在字段上渲染为HTML属性。                                          |
+------------------------+-------------------------------------------------------------------------------------+
| ``label_attr``         | 一个键值对数组，将在标签上渲染为HTML属性。                                          |
+------------------------+-------------------------------------------------------------------------------------+
| ``compound``           | 该字段是否实际上是一组子字段的持有者。                                              |
|                        | 另外，在标签中添加了一个 ``required`` 样式类。                                      |
+------------------------+-------------------------------------------------------------------------------------+
| ``block_prefixes``     | 父类型的所有名称的数组。                                                            |
+------------------------+-------------------------------------------------------------------------------------+
| ``translation_domain`` | 此表单的翻译域。                                                                    |
+------------------------+-------------------------------------------------------------------------------------+
| ``cache_key``          | 用于缓存的一个唯一键。                                                              |
+------------------------+-------------------------------------------------------------------------------------+
| ``data``               | 类型的规范化数据。                                                                  |
+------------------------+-------------------------------------------------------------------------------------+
| ``method``             | 当前表单的方法（POST，GET等）。                                                     |
+------------------------+-------------------------------------------------------------------------------------+
| ``action``             | 当前表单的动作。                                                                    |
+------------------------+-------------------------------------------------------------------------------------+

.. _`form_div_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/form_div_layout.html.twig
.. _`Twig文档`: https://twig.symfony.com/doc/2.x/templates.html#test-operator
