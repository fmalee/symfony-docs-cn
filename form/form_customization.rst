.. index::
   single: Form; Custom form rendering

如何自定义表单渲染
===============================

Symfony为你提供了几种自定义表单渲染方式的方法。
在本文中，你将学习如何对表单的一个或多个字段进行独家定制。
如果你需要以相同的方式自定义所有表单，请创建一个 :doc:`表单主题 </form/form_themes>`
或使用任何内置主题，例如 :doc:`Symfony表单的Bootstrap主题 </form/bootstrap4>`。

.. _form-rendering-basics:

表单渲染函数
------------------------

就足以渲染整个表单，包括其所有字段和错误消息：

.. code-block:: twig

    {# $form->createView()form 是从控制器传递的变量，通过调用 $form->createView() 方法创建 #}
    {{ form(form) }}

下一步是使用 :ref:`form_start() <reference-forms-twig-start>`、
:ref:`form_end() <reference-forms-twig-end>`、
:ref:`form_errors() <reference-forms-twig-errors>` 以及
:ref:`form_row() <reference-forms-twig-row>`
等Twig函数来渲染不同的表单部分，以便你可以添加自定义的HTML元素和属性：

.. code-block:: html+twig

    {{ form_start(form) }}
        <div class="my-custom-class-for-errors">
            {{ form_errors(form) }}
        </div>

        <div class="row">
            <div class="col">
                {{ form_row(form.task) }}
            </div>
            <div class="col" id="some-custom-id">
                {{ form_row(form.dueDate) }}
            </div>
        </div>
    {{ form_end(form) }}

``form_row()`` 函数输出整个字段内容，包括标签、帮助消息、HTML元素和错误消息。
所有这些都可以使用其他Twig函数进一步自定义，如下图所示：

.. raw:: html

    <object data="../_images/form/form-field-parts.svg" type="image/svg+xml"></object>

:ref:`form_label() <reference-forms-twig-label>`、
:ref:`form_widget() <reference-forms-twig-widget>`、
:ref:`form_help() <reference-forms-twig-help>` 以及
:ref:`form_errors() <reference-forms-twig-errors>`
等Twig函数可以让你完全控制每个表单字段的渲染，这样你就可以完全自定义它们：

.. code-block:: html+twig

    <div class="form-control">
        <i class="fa fa-calendar"></i> {{ form_label(form.dueDate) }}
        {{ form_widget(form.dueDate) }}

        <small>{{ form_help(form.dueDate) }}</small>

        <div class="form-error">
            {{ form_errors(form.dueDate) }}
        </div>
    </div>

.. note::

    在本文的后面部分，你可以找到这些Twig函数的完整参考以及更多用法示例。

表单渲染参数
------------------------

上一节中提到的一些Twig函数允许传递变量来配置它们的行为。例如，``form_label()``
函数允许你定义自定义标签以重写表单中定义的标签：

.. code-block:: twig

    {{ form_label(form.task, 'My Custom Task Label') }}

某些 :doc:`表单字段类型 </reference/forms/types>` 具有可以传递给部件的额外渲染选项。
每种类型都记录了这些选项，但有一个常见选项  ``attr``，它允许你修改表单元素上的HTML属性。
下面将 ``task_field`` CSS类添加到渲染的输入文本字段中：

.. code-block:: twig

    {{ form_widget(form.task, {'attr': {'class': 'task_field'}}) }}

.. note::

    如果你一次渲染整个表单（或整个嵌入式表单），则 ``variables``
    参数将仅应用于表单本身而不是其子元素。换句话说，以下内容
    **不会** 将 “foo” 类属性传递给表单中的所有子字段：

    .. code-block:: twig

        {# **不会** 生效 - 变量不是递归的 #}
        {{ form_widget(form, { 'attr': {'class': 'foo'} }) }}

如果需要“手动”渲染表单字段，则可以使用其 ``vars`` 属性访问字段（例如 ``id``、``name``
和 ``label``）的各个值。例如，获取 ``id``：

.. code-block:: twig

    {{ form.task.vars.id }}

.. note::

    在本文后面，你可以找到这些Twig变量及其描述的完整参考。

表单主题
-----------

前面部分中展示的Twig函数和变量可以帮助你自定义表单的一个或多个字段。
但是，此自定义无法应用于应用的其余表单。

如果要以相同方式来自定义所有表单（例如，将生成的HTML代码调整为应用中使用的CSS框架），则必须创建一个
:doc:`表单主题 </form/form_themes>`。

.. _reference-form-twig-functions-variables:

表单函数和变量参考
--------------------------------------

.. _reference-form-twig-functions:

函数
~~~~~~~~~

.. _reference-forms-twig-form:

form(form_view, variables)
..........................

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

form_start(form_view, variables)
................................

渲染表单的开始标签。此辅助函数负责打印已配置的方法和表单的动作。
如果表单包含上传字段，它还将包含正确的 ``enctype`` 属性。

.. code-block:: twig

    {# 渲染开始标签并更改提交方法 #}
    {{ form_start(form, {'method': 'GET'}) }}

.. _reference-forms-twig-end:

form_end(form_view, variables)
..............................

渲染表单的结束标签。

.. code-block:: twig

    {{ form_end(form) }}

除非你设置 ``render_rest`` 为 ``false``，否则此助手也将输出 ``form_rest()``
（这将在本文后面解释）：

.. code-block:: twig

    {# 不渲染未手动渲染的字段 #}
    {{ form_end(form, {'render_rest': false}) }}

.. _reference-forms-twig-label:

form_label(form_view, label, variables)
.......................................

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

.. _reference-forms-twig-help:

form_help(form_view)
....................

渲染给定字段的帮助文本。

.. code-block:: twig

    {{ form_help(form.name) }}

.. _reference-forms-twig-errors:

form_errors(form_view)
......................

渲染给定字段的任何错误。

.. code-block:: twig

    {# 仅渲染与此字段相关的错误消息 #}
    {{ form_errors(form.name) }}

    {# 渲染与任何表单字段无关的任何“全局”错误 #}
    {{ form_errors(form) }}

.. _reference-forms-twig-widget:

form_widget(form_view, variables)
.................................

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

form_row(form_view, variables)
..............................

渲染给定字段的“行”，即该字段的标签、错误、帮助和部件的组合。

.. code-block:: twig

    {# 渲染一个字段行，同时显示带有“foo”文本的标签 #}
    {{ form_row(form.name, {'label': 'foo'}) }}

``form_row()`` 的第二个参数是一个变量数组。Symfony提供的模板仅允许重写标签，如上例所示。

请参阅 ":ref:`twig-reference-form-variables`" 以了解 ``variables`` 参数。

.. _reference-forms-twig-rest:

form_rest(form_view, variables)
...............................

这将渲染尚未为给定表单渲染的所有剩余字段。
总是将它放置在你的表单中的某个地方是一个好主意，因为它会为你渲染隐藏的字段，
从而更容易发现那些你忘记渲染的任何字段（因为它会为你渲染该字段）。

.. code-block:: twig

    {{ form_rest(form) }}

测试
~~~~~

可以使用Twig中的 ``is`` 运算符以创建条件来执行测试。
阅读 `Twig文档`_ 以获取更多信息。

.. _form-twig-selectedchoice:

selectedchoice(selected_value)
..............................

此测试将检查当前选择是否等于 ``selected_value`` ，或当
``selected_value`` 是一个数组时，当前选择是否在该数组中。

.. code-block:: html+twig

    <option {% if choice is selectedchoice(value) %}selected="selected"{% endif %} ...>

.. _form-twig-rootform:

rootform
........

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

.. _twig-reference-form-variables:
.. _reference-form-twig-variables:

表单变量参考
~~~~~~~~~~~~~~~~~~~~~~~~

以下变量对于每种字段类型都是通用的。某些字段类型可能会定义更多变量，而某些变量仅适用于某些类型。
要了解每种类型可用的确切变量，请查看你的 :doc:`表单主题 </form/form_themes>` 所使用的模板代码。

假设模板中有一个 ``form`` 变量并且你想在 ``name`` 字段上引用该变量，则可以通过在
:class:`Symfony\\Component\\Form\\FormView` 对象上使用一个 ``vars`` 公有属性来访问该变量:

.. code-block:: html+twig

    <label for="{{ form.name.vars.id }}"
        class="{{ form.name.vars.required ? 'required' }}">
        {{ form.name.vars.label }}
    </label>

======================  ======================================================================================
变量                    用法
======================  ======================================================================================
``action``              当前表单的动作。
``attr``                一个键值对数组，将在字段上渲染为HTML属性。
``block_prefixes``      父类型的所有名称的数组。
``cache_key``           用于缓存的一个唯一键。
``compound``            该字段是否实际上是一组子字段的持有者。
                        （例如，一个 ``choice`` 字段实际上是一组复选框）。
``data``                类型的规范化数据。
``disabled``            如果为 ``true``，将在该字段添加 ``disabled="disabled``。
``errors``              附加到 *此* 特定字段的一个任何错误的数组（例如 ``form.title.errors``）。
                        请注意，不能使用 ``form.errors`` 来确定一个表单是否有效，
                        因为此变量只会返回“全局”的错误：某些单独的字段可能有错误。
                        所以，请使用 ``valid`` 选项。
``form``                当前的 ``FormView`` 实例。
``full_name``           要渲染的 ``name`` HTML属性。
``help``                The help message that will be rendered.
``id``                  要渲染的 ``id`` HTML属性。
``label``               要渲染的字符串标签。
``label_attr``          一个键值对数组，将在标签上渲染为HTML属性。
``method``              当前表单的方法（POST，GET等）。
``multipart``           如果是 ``true``，``form_enctype`` 将渲染 ``enctype="multipart/form-data"``。
``name``                字段的名称（例如 ``title``） - 但不是 ``name`` HTML属性，``full_name`` 才是。
``required``            如果是 ``true``，则在该字段中添加一个 ``required`` 属性以激活HTML5验证。
                        另外，在标签中添加了一个 ``required`` 样式类。
``submitted``           返回 ``true`` 或 ``false``，这取决于整个表单是否提交。
``translation_domain``  此表单的翻译域。
``valid``               返回 ``true`` 或 ``false``，这取决于整个表单是否有效。
``value``               渲染时要使用的值（通常是 ``value`` HTML属性）。
                        此变量仅适用于根表单元素。
======================  ======================================================================================

.. tip::

    在幕后，当表单组件在表单树的每个“节点”上调用 ``buildView()`` 和 ``finishView()``
    时，这些变量对表单的 ``FormView`` 对象可用。
    要查看特定字段具有哪些“view”变量，请查找表单字段（及其父字段）的源代码，并查看上面的两个函数。

.. _`Twig文档`: https://twig.symfony.com/doc/2.x/templates.html#test-operator
