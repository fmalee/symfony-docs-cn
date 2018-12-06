.. index::
   single: Form; Custom form rendering

如何自定义表单渲染
===============================

Symfony为你提供了多种自定义表单渲染方式的方法。
在本指南中，你将学习如何使用Twig或PHP作为模板引擎，并尽可能少地自定义表单的每个部分。

表单渲染基础
---------------------

回想一下，你可以使用 ``form_row()`` Twig函数或 ``row``
PHP辅助方法来同时渲染表单字段的标签、错误和HTML部件：

.. code-block:: twig

    {{ form_row(form.age) }}

你还可以单独渲染字段的四个部分：

.. code-block:: html+twig

    <div>
        {{ form_label(form.age) }}
        {{ form_errors(form.age) }}
        {{ form_widget(form.age) }}
        {{ form_help(form.age) }}
    </div>

在这两个例子中，表单的标签、错误和HTML部件都是使用Symfony标配的一组标记来渲染的。
例如，以上两个模板都会渲染为：

.. code-block:: html

    <div>
        <label for="form_age">Age</label>
        <ul>
            <li>This field is required</li>
        </ul>
        <input type="number" id="form_age" name="form[age]" />
    </div>

要快速的原型化(prototype)并测试表单，你只需使用一行即可渲染整个表单：

.. code-block:: twig

    {# 渲染所有字段 #}
    {{ form_widget(form) }}

    {# 渲染所有字段 *以及* 表单的开始和结束标签 #}
    {{ form(form) }}

本文的剩余部分将解释如何在几个不同的级别中修改表单标记的每个部分。
有关表单渲染的更多信息，请参阅 :doc:`/form/rendering`。

.. _form-customization-form-themes:

什么是表单主题？
---------------------

Symfony使用表单片段（一个只渲染表单的一部分的模板片段）来渲染一个表单的每个部分
- 字段标签、错误、``input`` 文本字段、``select`` 标签等等。

片段在Twig中定义为区块，在PHP中定义为模板文件。

一个 *主题* 无非就是一组渲染一个表单时要使用的片段。
换句话说，如果要自定义表单渲染方式的一部分，则可以导入一个包含相应表单片段的自定义的 *主题*。

Symfony有一些 **内置表单主题**，用于定义渲染一个表单的每个部分所需的每个片段：

* `form_div_layout.html.twig`_, 将每个表单字段封装在一个 ``<div>`` 元素中。
* `form_table_layout.html.twig`_, 将整个表单封装在一个 ``<table>``
  元素内，并将每个表单字段封装在 ``<tr>`` 元素中。
* `bootstrap_3_layout.html.twig`_, 使用适当的CSS类将每个表单字段封装在一个
  ``<div>`` 元素中，以应用 `Bootstrap 3 CSS框架`_ 的默认样式。
* `bootstrap_3_horizontal_layout.html.twig`_, 它类似于前面的主题，
  但应用的是用于水平显示（即标签和部件处于同一行中）表单的CSS类。
* `bootstrap_4_layout.html.twig`_, 与 ``bootstrap_3_layout.html.twig``
  相同，但对应样式已更新为 `Bootstrap 4 CSS框架`_。
* `bootstrap_4_horizontal_layout.html.twig`_, 与
  ``bootstrap_3_horizontal_layout.html.twig``
  相同，但对应样式已更新为 `Bootstrap 4 CSS框架`_。
* `foundation_5_layout.html.twig`_, 使用适当的CSS类将每个表单字段封装在一个
  ``<div>`` 元素中，以应用 `Foundation CSS框架`_ 默认样式。

.. caution::

    当你使用Bootstrap表单主题并手动渲染字段时，为复选框/单选框字段调用 ``form_label()``
    将不会显示任何内容。因为在Bootstrap内部， ``form_widget()`` 已经将标签显示出来。

.. tip::

    阅读有关 :doc:`Bootstrap4 表单主题 </form/bootstrap4>` 的更多信息。

在下一节中，你将学习如何通过重写部分或全部片段来自定义一个主题。

例如，当渲染 ``integer`` 类型字段的部件时，将生成一个 ``number`` 类型的 ``input`` 字段。

.. code-block:: html+twig

    {{ form_widget(form.age) }}

渲染:

.. code-block:: html

    <input type="number" id="form_age" name="form[age]" required="required" value="33" />

在内部，Symfony使用 ``integer_widget`` 片段来渲染该字段。
这是因为该字段的类型是 ``integer``，并且你正在渲染它的 ``widget``
（而不是 ``label`` 或 ``errors``）。

在Twig中，将使用 `form_div_layout.html.twig`_ 默认模板的 ``integer_widget`` 区块。

而在PHP中，它将使用 ``FrameworkBundle/Resources/views/Form`` 件夹中的
``integer_widget.html.php`` 文件。

``integer_widget`` 片段的默认实现如下所示：

.. code-block:: twig

    {# form_div_layout.html.twig #}
    {% block integer_widget %}
        {% set type = type|default('number') %}
        {{ block('form_widget_simple') }}
    {% endblock integer_widget %}

如你所见，此片段本身渲染另一个片段 - ``form_widget_simple``：

.. code-block:: html+twig

    {# form_div_layout.html.twig #}
    {% block form_widget_simple %}
        {% set type = type|default('text') %}
        <input type="{{ type }}" {{ block('widget_attributes') }} {% if value is not empty %}value="{{ value }}" {% endif %}/>
    {% endblock form_widget_simple %}

关键是，片段决定了一个表单的每个部分的HTML输出。要自定义该表单输出，你需要标识并重写正确的片段。
一组这样的表单片段自定义被称为一个表单“主题”。渲染一个表单时，你可以选择要应用的表单主题。

在Twig中，一个主题是单个模板文件，片段是此文件中定义的区块。

在PHP中，一个主题是一个文件夹，片段是此文件夹中的单个模板文件。

.. _form-customization-sidebar:

.. sidebar:: 如何辨别要自定义的区块

    在此示例中，该自定义片段的名称是 ``integer_widget``，因为你要为所有的
    ``integer`` 字段类型的 ``widget`` 重写HTML。
    如果你需要自定义的是 ``textarea`` 字段，则可以自定义 ``textarea_widget`` 区块。

    片段名称的 ``integer`` 部分来自类名：``IntegerType``，然后基于一个标准最终变成 ``integer``。

    正如你看到的，该片段名称一个组合，它由字段类型和要渲染该字段的哪个部分（例如 ``widget``、
    ``label``、``errors``、``row``）组成。
    例如，要更改针对 ``text`` 类型的输入字段的错误的渲染方式，你可以自定义 ``text_errors`` 片段。

    但是，更常见的是，你需要自定义针对所有字段中的错误的渲染方式。
    你可以通过自定义 ``form_errors`` 片段来完成此操作。这里利用了字段类型继承。
    具体来说，由于 ``text`` 类型从 ``form``
    类型扩展而来，因此Form组件将首先查找特定类型的片段（例如
    ``text_errors``），如果该片段不存在，则回退到其父片段名称（例如 ``form_errors``）。

    有关此话题的更多信息，请参阅 :ref:`form-template-blocks`。

.. _form-theming-methods:

表单主题
------------

要明白表单主题的强大功能，假设你想要使用 ``div`` 标签封装每个 ``number`` 输入字段。
达到目的关键是自定义 ``integer_widget`` 片段。

Twig中的表单主题
--------------------

在Twig中自定义表单字段区块时，根据自定义表单区块存放的 *位置*，你有两个选择：

+------------------------+--------------------+----------------------+
| 方法                   | 优点               | 缺点                 |
+========================+====================+======================+
| 在与表单相同的模板内部 | 快捷方便           | 无法在其他模板中复用 |
+------------------------+--------------------+----------------------+
| 在单独的模板内部       | 可以被许多模板复用 | 需要创建额外的模板   |
+------------------------+--------------------+----------------------+

两种方法都具有相同的效果，但它们在不同的解决方案中又各有优异。

方法 1: 在与表单相同的模板内部
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

自定义 ``integer_widget`` 区块的最简单方法是直接在实际渲染表单的模板中对其进行自定义。

.. code-block:: html+twig

    {% extends 'base.html.twig' %}

    {% form_theme form _self %}

    {% block integer_widget %}
        <div class="integer_widget">
            {% set type = type|default('number') %}
            {{ block('form_widget_simple') }}
        </div>
    {% endblock %}

    {% block content %}
        {# ... 渲染该表单 #}

        {{ form_row(form.age) }}
    {% endblock %}

通过使用特殊的 ``{% form_theme form _self %}``
标签，Twig可以在同一个模板中查找任何被重写的表单区块。
假设 ``form.age`` 是一个 ``integer``
类型的字段，则在渲染该字段的部件时，将使用自定义的 ``integer_widget`` 区块。

此方法的缺点是在其他模板中渲染其他表单时，将无法复用该自定义表单区块。
换句话说，在进行特定于应用的单个表单的表单自定义时，此方法最有用。
如果要在应用中的多个（或所有）表单中复用一个表单自定义，请继续阅读下一节。

方法 2: 在单独的模板内部
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以选择完全的将自定义的 ``integer_widget`` 表单区块放在单独的模板中。
代码和生成结果和上面是一样的，但现在你可以在许多模板中重复使用该表单自定义：

.. code-block:: html+twig

    {# templates/form/fields.html.twig #}
    {% block integer_widget %}
        <div class="integer_widget">
            {% set type = type|default('number') %}
            {{ block('form_widget_simple') }}
        </div>
    {% endblock %}

现在你已经创建了自定义表单区块，你需要通知Symfony使用它。
在你要实际渲染表单的模板内，告诉Symfony通过 ``form_theme`` 标签使用该模板：

.. code-block:: html+twig

    {% form_theme form 'form/fields.html.twig' %}

    {{ form_widget(form.age) }}

当 ``form.age`` 部件被渲染，Symfony的将使用新模板的 ``integer_widget``
区块，并根据该自定义区块的定义，将 ``input`` 标签将封装在 ``div`` 元素内。

多个模板
..................

还可以通过应用多个模板来自定义一个表单。为此，请使用 ``with`` 关键字将所有模板的名称作为数组传递：

.. code-block:: html+twig

    {% form_theme form with ['common.html.twig', 'form/fields.html.twig'] %}

    {# ... #}

模板也可以位于不同的bundle中，使用Twig的命名空间化的路径来引用这些模板，例如
``@AcmeFormExtra/form/fields.html.twig``。

禁用使用全局定义的主题
..........................................

有时你可能希望禁用全局定义的表单主题，以便更好地控制表单的渲染。
例如，在为可以安装在各种Symfony应用上的bundle（此时你无法控制全局定义的主题）创建管理界面时，你可能需要这样做。

你可以通过在表单主题列表后面添加 ``only`` 关键字来执行此操作：

.. code-block:: html+twig

    {% form_theme form with ['common.html.twig', 'form/fields.html.twig'] only %}

    {# ... #}

.. caution::

    使用 ``only`` 关键字时，不会应用Symfony的内置表单主题（``form_div_layout.html.twig`` 等）。
    为了正确渲染你的表单，你需要自己提供一个功能齐全的表单主题，或者使用Twig的 ``use``
    关键字继承其中一个内置表单主题，而不是使用 ``extends`` 来复用原始主题的内容。

    .. code-block:: html+twig

        {# templates/form/common.html.twig #}
        {% use "form_div_layout.html.twig" %}

        {# ... #}

子表单
...........

你还可以将一个表单主题应用于表单的一个特定子表单：

.. code-block:: html+twig

    {% form_theme form.a_child_form 'form/fields.html.twig' %}

当你希望为一个嵌套表单创建一个与主表单不同的自定义主题时，这非常有用。同时指定两个主题：

.. code-block:: html+twig

    {% form_theme form 'form/fields.html.twig' %}

    {% form_theme form.a_child_form 'form/fields_child.html.twig' %}

.. _referencing-base-form-blocks-twig-specific:

引用基础表单区块
----------------------------

到目前为止，要重写特定的表单区块，最好的方法是从 `form_div_layout.html.twig`_
复制默认区块，并将其粘贴到不同的模板中，然后对其进行自定义。
但在许多情况下，你可以通过在自定义时引用基础区块来避免这样做。

这样就减少了很多工作量，但根据你的表单区块自定义是否与表单位于同一模板，使用方法又略有不同。

在与表单相同的模板内部引用区块
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

通过在渲染表单的模板中添加 ``use`` 标签来导入区块：

.. code-block:: twig

    {% use 'form_div_layout.html.twig' with integer_widget as base_integer_widget %}

现在，当从 `form_div_layout.html.twig`_ 导入区块时，``integer_widget``
区块被命名为 ``base_integer_widget``。
这意味着当你重新定义 ``integer_widget`` 区块时，可以通过 ``base_integer_widget`` 来引用默认标记：

.. code-block:: html+twig

    {% block integer_widget %}
        <div class="integer_widget">
            {{ block('base_integer_widget') }}
        </div>
    {% endblock %}

从外部模板引用基础区块
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你的表单自定义位于一个外部模板中，则可以使用 ``parent()`` Twig函数来引用基础区块：

.. code-block:: html+twig

    {# templates/form/fields.html.twig #}
    {% extends 'form_div_layout.html.twig' %}

    {% block integer_widget %}
        <div class="integer_widget">
            {{ parent() }}
        </div>
    {% endblock %}

.. note::

    使用PHP作为模板引擎时，将无法引用基础区块。你必须手动将基础区块中的内容复制到新模板文件中。

.. _twig:

创建应用范围的自定义
--------------------------------------

如果你希望某个表单自定义对你的应用是全局的，那么你可以在外部模板中进行表单自定义，然后再在应用配置中导入它来实现此目的。

通过使用以下配置，将在渲染表单时全局使用 ``form/fields.html.twig`` 模板内的任何自定义表单区块。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            form_themes:
                - 'form/fields.html.twig'
            # ...

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:form-theme>form/fields.html.twig</twig:form-theme>
                <!-- ... -->
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', array(
            'form_themes' => array(
                'form/fields.html.twig',
            ),

            // ...
        ));

默认情况下，Twig在渲染表单时使用一个 *div* 布局。但是，有些人可能更喜欢在 *table* 布局的表单。
可以使用 ``form_table_layout.html.twig`` 资源来使用这样的布局：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            form_themes:
                - 'form_table_layout.html.twig'
            # ...

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:form-theme>form_table_layout.html.twig</twig:form-theme>
                <!-- ... -->
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', array(
            'form_themes' => array(
                'form_table_layout.html.twig',
            ),

            // ...
        ));

如果你只想在单个模板中进行更改，请将以下行添加到你的模板文件中，而不是将该模板添加为一个资源：

.. code-block:: html+twig

    {% form_theme form 'form_table_layout.html.twig' %}

请注意，上面代码中的 ``form`` 变量是你传递给模板的表单视图变量。

如何自定义单个字段
------------------------------------

到目前为止，你已经看到了可以为所有文本字段类型的部件输出进行自定义的不同方法。
你还可以自定义单个字段。例如，假设 ``product`` 表单中有两个 ``text`` 字段 - ``name`` 和
``description`` - 但你只想自定义其中一个字段。
这可以通过自定义一个片段来实现，该片段的名称是该字段的 ``id`` 属性和字段的正在自定义对应部分的一个组合。
例如，要仅自定义 ``name`` 字段：

.. code-block:: html+twig

    {% form_theme form _self %}

    {% block _product_name_widget %}
        <div class="text_widget">
            {{ block('form_widget_simple') }}
        </div>
    {% endblock %}

    {{ form_widget(form.name) }}

在这里，``_product_name_widget`` 片段定义了用于 *id* 为
``product_name`` （和名称为 ``product[name]``）的字段的模板。

.. tip::

    该字段的 ``product`` 部分是表单名称，可以手动设置或根据表单类型名称自动生成（例如
    ``ProductType`` 等同于 ``product``）。
    如果你不确定表单名称是什么，请查看已渲染表单的HTML代码。

    如果要更改区块名称 ``_product_name_widget`` 的 ``product`` 或
    ``name`` 部分，可以在表单类型中设置 ``block_name`` 选项::

        use Symfony\Component\Form\FormBuilderInterface;
        use Symfony\Component\Form\Extension\Core\Type\TextType;

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            // ...

            $builder->add('name', TextType::class, array(
                'block_name' => 'custom_name',
            ));
        }

    然后块名称将是 ``_product_custom_name_widget``。

你还可以使用相同的方法重写整个字段行的标记：

.. code-block:: html+twig

    {% form_theme form _self %}

    {% block _product_name_row %}
        <div class="name_row">
            {{ form_label(form) }}
            {{ form_errors(form) }}
            {{ form_widget(form) }}
            {{ form_help(form) }}
        </div>
    {% endblock %}

    {{ form_row(form.name) }}

.. _form-custom-prototype:

如何自定义集合的原型
---------------------------------------

使用一个 :doc:`表单集合 </form/form_collections>`
时，可以通过重写一个区块来将原本的原型重写为一个完全自定义的原型。
例如，如果你的表单字段命名为 ``tasks``，则你可以按下面的方式来更改每个任务的部件：

.. code-block:: html+twig

    {% form_theme form _self %}

    {% block _tasks_entry_widget %}
        <tr>
            <td>{{ form_widget(form.task) }}</td>
            <td>{{ form_widget(form.dueDate) }}</td>
        </tr>
    {% endblock %}

你不仅可以重写已渲染的部件，还可以更改完整的表单行或标签。
对于上面给出的 ``tasks`` 字段，区块名称将如下：

================  =======================
表单部分           区块名称
================  =======================
``label``         ``_tasks_entry_label``
``widget``        ``_tasks_entry_widget``
``row``           ``_tasks_entry_row``
================  =======================

其他常用的自定义
---------------------------

到目前为止，本文已经向你展示了用来自定义表单的渲染方式的几种不同的方法。
这里关键的点是，自定义一个与要控制的表单部分相对应的特定片段（请参阅
:ref:`命名表单区块 <form-customization-sidebar>`）。

在接下来的章节中，你将了解如何创建多种常见的表单自定义。
要应用这些自定义，请使用 :ref:`form-theming-methods` 章节中描述的其中一个方法。

自定义错误输出
~~~~~~~~~~~~~~~~~~~~~~~~

.. note::

    Form组件仅处理验证错误 *如何* 渲染，而不处理实际的验证错误消息。
    错误消息自身由你应用于对象的验证约束确定。
    有关更多信息，请参阅 :doc:`验证 </validation>` 文档。

在提交有错误的表单时，有许多不同的方法可以自定义错误的渲染方式。
使用 ``form_errors()`` 辅助方法，将渲染一个字段的错误消息：

.. code-block:: twig

    {{ form_errors(form.age) }}

默认情况下，该错误在一个无序列表中渲染：

.. code-block:: html

    <ul>
        <li>This field is required</li>
    </ul>

要为 *所有* 字段重写错误消息的渲染方式，请复制、粘贴，然后自定义 ``form_errors`` 片段。

.. code-block:: html+twig

    {% form_theme form _self %}

    {# form_errors.html.twig #}
    {% block form_errors %}
        {% spaceless %}
            {% if errors|length > 0 %}
            <ul>
                {% for error in errors %}
                    <li>{{ error.message }}</li>
                {% endfor %}
            </ul>
            {% endif %}
        {% endspaceless %}
    {% endblock form_errors %}

.. tip::

    请参阅 :ref:`form-theming-methods` 以了解如何应用此自定义。

你还可以仅为一种特定字段类型自定义错误输出。
要自定义 *仅* 用于这些错误的标记，请按照上述说明进行相同操作，但将内容放在一个相对的
``_errors`` 区块中（如果是PHP模板，则是放入文件）。
例如：``text_errors`` （或 ``text_errors.html.php``）。

.. tip::

    请参阅表 :ref:`form-template-blocks` 以找出你需要自定义的特定区块或文件。

某些更全局的针对表单（即不仅仅针对一个字段）的错误会单独渲染，通常位于表单的顶部：

.. code-block:: twig

    {{ form_errors(form) }}

要自定义 *仅* 用于这些错误的标记，请按照上述说明进行相同的操作，但现在需要检查
``compound`` 变量是否设置为 ``true``。
如果为 ``true``，则意味着当前渲染的是一个字段集合（例如整个表单），而不仅仅是单个字段。

.. code-block:: html+twig

    {% form_theme form _self %}

    {# form_errors.html.twig #}
    {% block form_errors %}
        {% spaceless %}
            {% if errors|length > 0 %}
                {% if compound %}
                    <ul>
                        {% for error in errors %}
                            <li>{{ error.message }}</li>
                        {% endfor %}
                    </ul>
                {% else %}
                    {# ... 为单个字段显示错误 #}
                {% endif %}
            {% endif %}
        {% endspaceless %}
    {% endblock form_errors %}

自定义"表单行"
~~~~~~~~~~~~~~~~~~~~~~~~~~

当你可以控制它时，渲染一个表单字段的最简单方法是使用 ``form_row()`` 函数，该函数渲染一个字段的标签、错误和HTML部件。
要自定义用于渲染 *所有* 表单字段行的标记，请重写 ``form_row`` 片段。
例如，假设你要为每行周围的 ``div`` 元素添加一个样式类：

.. code-block:: html+twig

    {# form_row.html.twig #}
    {% block form_row %}
        <div class="form_row">
            {{ form_label(form) }}
            {{ form_errors(form) }}
            {{ form_widget(form) }}
            {{ form_help(form) }}
        </div>
    {% endblock form_row %}

.. tip::

    请参阅 :ref:`form-theming-methods` 以了解如何应用此自定义。

向字段标签添加一个“Required”星号
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果要使用一个星号（``*``）来表示所有的必需字段，可以通过自定义 ``form_label`` 片段来完成此操作。

如果你在与表单相同的模板中进行此表单自定义，请修改 ``use`` 标签并添加以下内容：

.. code-block:: html+twig

    {% use 'form_div_layout.html.twig' with form_label as base_form_label %}

    {% block form_label %}
        {{ block('base_form_label') }}

        {% if label is not same as(false) and required %}
            <span class="required" title="This field is required">*</span>
        {% endif %}
    {% endblock %}

如果要在一个单独的模板中进行此表单自定义，请使用以下内容：

.. code-block:: html+twig

    {% extends 'form_div_layout.html.twig' %}

    {% block form_label %}
        {{ parent() }}

        {% if label is not same as(false) and required %}
            <span class="required" title="This field is required">*</span>
        {% endif %}
    {% endblock %}

.. tip::

    请参阅 :ref:`form-theming-methods` 以了解如何应用此自定义。

.. sidebar:: 仅使用CSS

    默认情况下，必需字段的 ``label`` 标签会渲染一个 ``required`` CSS类。
    因此，你也可以仅使用CSS来完成星号的添加：

    .. code-block:: css

        label.required:before {
            content: "* ";
        }

添加"help"消息
~~~~~~~~~~~~~~~~~~~~~~

你还可以自定义表单部件以获得可选的“help”消息。

如果你在与表单相同的模板中进行此表单自定义，请修改 ``use`` 标签并添加以下内容：

.. code-block:: html+twig

    {% use 'form_div_layout.html.twig' with form_widget_simple as base_form_widget_simple %}

    {% block form_widget_simple %}
        {{ block('base_form_widget_simple') }}

        {% if help is defined %}
            <span class="help-block">{{ help }}</span>
        {% endif %}
    {% endblock %}

如果要在一个单独的模板中进行表单自定义，请使用以下内容：

.. code-block:: html+twig

    {% extends 'form_div_layout.html.twig' %}

    {% block form_widget_simple %}
        {{ parent() }}

        {% if help is defined %}
            <span class="help-block">{{ help }}</span>
        {% endif %}
    {% endblock %}

要在一个字段下面渲染帮助消息，请传入一个 ``help`` 变量：

.. code-block:: twig

    {{ form_widget(form.title, {'help': 'foobar'}) }}

.. tip::

    请参阅 :ref:`form-theming-methods` 以了解如何应用此自定义。

使用表单变量
--------------------

大多数可用于渲染表单不同部分（例如表单部件、表单标签、表单错误等）的函数也允许你直接进行某些自定义。
请看以下示例：

.. code-block:: twig

    {# 渲染一个表单部件, 同时添加为它一个 "foo" 样式类 #}
    {{ form_widget(form.name, { 'attr': {'class': 'foo'} }) }}

作为第二个参数传递的数组包含着表单“变量”。
有关Twig中此概念的更多详细信息，请参阅 :ref:`twig-reference-form-variables`。

.. _`form_div_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/form_div_layout.html.twig
.. _`form_table_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/form_table_layout.html.twig
.. _`bootstrap_3_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/bootstrap_3_layout.html.twig
.. _`bootstrap_3_horizontal_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/bootstrap_3_horizontal_layout.html.twig
.. _`bootstrap_4_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/bootstrap_4_layout.html.twig
.. _`bootstrap_4_horizontal_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/bootstrap_4_horizontal_layout.html.twig
.. _`Bootstrap 3 CSS框架`: https://getbootstrap.com/docs/3.3/
.. _`Bootstrap 4 CSS框架`: https://getbootstrap.com/docs/4.1/
.. _`foundation_5_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/foundation_5_layout.html.twig
.. _`Foundation CSS框架`: http://foundation.zurb.com/
