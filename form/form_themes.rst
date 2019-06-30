.. index::
   single: Forms; Theming
   single: Forms; Customizing fields

如何使用表单主题
============================

本文介绍如何在你的应用中使用Symfony提供的任何表单主题以及如何创建自己的自定义表单主题。

.. _symfony-builtin-forms:

Symfony内置表单主题
----------------------------

Symfony附带几个 **内置的表单主题**，让你的表单在使用一些最流行的CSS框架时很好的搭配起来。
每个主题都在一个Twig模板中定义：

* `form_div_layout.html.twig`_，将每个表单字段封装在一个 ``<div>``
  元素中，它是Symfony应用中默认使用的主题，除非你按照本文后面的说明对其进行配置。
* `form_table_layout.html.twig`_，将整个表单封装在一个 ``<table>``
  元素内，并将每个表单字段封装在 ``<tr>`` 元素中。
* `bootstrap_3_layout.html.twig`_，使用适当的CSS类将每个表单字段封装在一个
  ``<div>`` 元素中，以应用 `Bootstrap 3 CSS框架`_ 使用的样式。
* `bootstrap_3_horizontal_layout.html.twig`_，它类似于前面的主题，
  但应用的是用于水平显示（即标签和部件处于同一行中）表单的CSS类。
* `bootstrap_4_layout.html.twig`_，与 ``bootstrap_3_layout.html.twig``
  相同，但对应样式已更新为 `Bootstrap 4 CSS框架`_。
* `bootstrap_4_horizontal_layout.html.twig`_，与 ``bootstrap_3_horizontal_layout.html.twig``
  相同，但对应样式已更新为 `Bootstrap 4 CSS框架`_。
* `foundation_5_layout.html.twig`_，使用适当的CSS类将每个表单字段封装在一个
  ``<div>`` 元素中，以应用 `Foundation CSS框架`_ 使用的样式。

.. tip::

    阅读有关 :doc:`Symfony的 Bootstrap 4 表单主题 </form/bootstrap4>`
    的文章，以了解有关它的更多信息。

.. _forms-theming-global:
.. _forms-theming-twig:

将主题应用于所有表单
----------------------------

Symfony默认使用 ``form_div_layout.html.twig``
主题。如果要对应用的所有表单使用其他主题，请在 ``twig.form_themes`` 选项中进行配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            form_themes: ['bootstrap_4_horizontal_layout.html.twig']
            # ...

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:form-theme>bootstrap_4_horizontal_layout.html.twig</twig:form-theme>
                <!-- ... -->
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'form_themes' => [
                'bootstrap_4_horizontal_layout.html.twig',
            ],
            // ...
        ]);

你可以将多个主题传递给此选项，因为有时单个表单主题仅重新定义了一些元素。
这样的话，如果某个主题没有覆盖某个元素，Symfony会继续查找其他主题。

``twig.form_themes`` 选项中主题的顺序很重要。
每个主题都会覆盖所有以前的主题，因此你必须将最重要的主题放在列表的末尾。

将主题应用于单个表单
-------------------------------

虽然大多数时候你都会在全局的应用表单主题，但你可能只需要将一个主题仅应用于某些特定表单。
你可以使用 :ref:`form_theme Twig标签 <reference-twig-tag-form-theme>` 来做到这 一点：

.. code-block:: twig

    {# 此表单主题将仅应用于此模板的表单 #}
    {% form_theme form 'foundation_5_layout.html.twig' %}

    {{ form_start(form) }}
        {# ... #}
    {{ form_end(form) }}

``form_theme`` 标签的第一个参数（在此示例中为 ``form``）是存储表单视图对象的变量的名称。
第二个参数是定义表单主题的Twig模板的路径。

将多个主题应用于单个表单
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

还可以通过应用多个主题来自定义表单。为此，使用 ``with``
关键字将所有Twig模板的路径作为数组传递（它们的顺序很重要，因为每个主题都会覆盖所有之前的主题）：

.. code-block:: twig

    {# 应用多个表单主题，但仅适用于此模板的表单 #}
    {% form_theme form with [
        'foundation_5_layout.html.twig',
        'forms/my_custom_theme.html.twig'
    ] %}

    {# ... #}

将不同主题应用于子表单
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

你还可以将一个表单主题应用于表单的特定子表单：

.. code-block:: twig

    {% form_theme form.a_child_form 'form/my_custom_theme.html.twig' %}

当你希望为嵌套式表单创建一个与主表单不同的自定义主题时，这非常有用。指定两个主题：

.. code-block:: twig

    {% form_theme form 'form/my_custom_theme.html.twig' %}
    {% form_theme form.a_child_form 'form/my_other_theme.html.twig' %}

.. _disabling-global-themes-for-single-forms:

禁用单个表单的全局主题
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

应用中定义的全局表单主题始终应用于所有表单，甚至是那些使用 ``form_theme`` 标签来应用自己主题的表单。
你可能希望禁用此功能，例如在为可以安装在不同Symfony应用（因此你无法控制全局启用哪些主题）上的软件包创建管理界面时。
为此，请在表单主题列表后添加 ``only`` 关键字：

.. code-block:: twig

    {% form_theme form with ['foundation_5_layout.html.twig'] only %}

    {# ... #}

.. caution::

    使用 ``only`` 关键字时，将不会应用Symfony的内置表单主题（``form_div_layout.html.twig``
    等）。为了正确渲染表单，你需要自己提供功能齐全的表单主题，或者使用Twig的 ``use``
    关键字扩展其中一个内置表单主题，而不是使用 ``extends`` 来复用原始主题的内容。

    .. code-block:: twig

        {# templates/form/common.html.twig #}
        {% use "form_div_layout.html.twig" %}

        {# ... #}

创建自己的表单主题
----------------------------

Symfony使用Twig区块来渲染一个表单的每个部分 - 字段标签、错误，``<input>``
文本字段、``<select>`` 标签等。一个 *主题*
就是一个Twig模板，其中包含你在渲染表单时要使用的一个或多个区块。

例如，假设表示一个整数属性的名为 ``age`` 的表单字段。如果将其添加到模板：

.. code-block:: twig

    {{ form_widget(form.age) }}

生成的HTML内容将是这样的（它将根据你的应用中启用的表单主题而有所不同）：

.. code-block:: html

    <input type="number" id="form_age" name="form[age]" required="required" value="33"/>

Symfony使用一个名为 ``integer_widget`` 的Twig区块来渲染该字段。这是因为该字段的类型是
``integer``，并且你正在渲染它的 ``widget``（而不是它的 ``label``、``errors`` 或
``help``）。创建表单主题的第一步是知道要重写哪个Twig区块，具体可以参照以下部分所述。

.. _form-customization-sidebar:
.. _form-fragment-naming:

表单片段命名
~~~~~~~~~~~~~~~~~~~~

表单片段的命名取决于你的需求：

* 如果要自定义 **相同类型的所有字段**（例如所有的 ``<textarea>``），请使用
  ``field-type_field-part`` 模式（例如 ``textarea_widget``）。
* 如果你想 **只自定义一个特定字段**（例如，用于编辑产品的表单字段 ``description``
  的 ``<textarea>``），请使用 ``_field-id_field-part``
  模式（例如 ``_product_description_widget``）。

在这两种情况下，``field-part`` 可以是以下任何有效的表单字段的部分：

.. raw:: html

    <object data="../_images/form/form-field-parts.svg" type="image/svg+xml"></object>

相同类型的所有字段的片段命名
...............................................

这些片段名称遵循 ``type_part`` 模式，其中 ``type`` 对应于被渲染的字段 *类型*（例如
``textarea``、``checkbox``、``date`` 等等），而 ``part``
则对应于要渲染 *什么*（例如 ``textarea``、``checkbox`` 等等）

一些片段名称的示例有：

* ``form_row`` - 被 :ref:`form_row() <reference-forms-twig-row>` 用来渲染大多数字段;
* ``textarea_widget`` - 被 :ref:`form_widget() <reference-forms-twig-widget>`
  用来渲染 ``textarea`` 字段类型;
* ``form_errors`` - 被 :ref:`form_errors() <reference-forms-twig-errors>`
  用来渲染一个字段的错误信息;

单个字段的片段命名
.....................................

这些片段名称遵循 ``_id_part`` 模式，其中 ``id`` 对应于字段的 ``id`` 属性（例如
``product_description``、``user_age`` 等等），而 ``part``
则对应于要渲染 *什么*（例如 ``label``、``widget`` 等等）

``id`` 属性同时包含表单名称和字段名称（例如
``product_price``）。表单名称可以手动设置，也可以根据表单类型的名称来自动生成（例如
``ProductType`` 等同于 ``product``）。
如果你不确定表单的名称是什么，请查看渲染该表单的HTML代码。你还可以使用
``block_name`` 选项来显式的定义此值::

    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormBuilderInterface;

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('name', TextType::class, [
            'block_name' => 'custom_name',
        ]);
    }

在此示例中，片段名称将是 ``_product_custom_name_widget``，取代了默认的
``_product_name_widget``。

.. _form-fragment-custom-naming:

单个字段的自定义片段命名
............................................

``block_prefix`` 选项允许表单字段定义自己的自定义片段名。
这对于自定义同一字段的某些实例非常有用，而无需
:doc:`创建自定义表单类型 </form/create_custom_field_type>`::

    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormBuilderInterface;

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder->add('name', TextType::class, [
            'block_prefix' => 'wrapped_text',
        ]);
    }

.. versionadded:: 4.3

    Symfony 4.3中引入了 ``block_prefix`` 选项。

现在，你可以使用 ``wrapped_text_row``、``wrapped_text_widget`` 等作为区块名称。

.. _form-custom-prototype:

集合的片段命名
...............................

使用一个 :doc:`表单集合 </form/form_collections>`
时，每个集合项的片段都遵循一个预定义的模式。例如，思考以下复杂示例，其中
``TaskManagerType`` 拥有一个 ``TaskListType`` 集合，而该集合又拥有一个
``TaskType`` 集合::

    class TaskManagerType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options = [])
        {
            // ...
            $builder->add('taskLists', CollectionType::class, [
                'entry_type' => TaskListType::class,
                'block_name' => 'task_lists',
            ]);
        }
    }

    class TaskListType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options = [])
        {
            // ...
            $builder->add('tasks', CollectionType::class, [
                'entry_type' => TaskType::class,
            ]);
        }
    }

    class TaskType
    {
        public function buildForm(FormBuilderInterface $builder, array $options = [])
        {
            $builder->add('name');
            // ...
        }
    }

然后你会得到以下所有的自定义区块（其中 ``*``
可以被替换为 ``row``、``widget``、``label`` 或 ``help``）：

.. code-block:: twig

    {% block _task_manager_task_lists_* %}
        {# TaskManager的集合字段 #}
    {% endblock %}

    {% block _task_manager_task_lists_entry_* %}
        {# the inner TaskListType #}
    {% endblock %}

    {% block _task_manager_task_lists_entry_tasks_* %}
        {# TaskListType的集合字段 #}
    {% endblock %}

    {% block _task_manager_task_lists_entry_tasks_entry_* %}
        {# the inner TaskType #}
    {% endblock %}

    {% block _task_manager_task_lists_entry_tasks_entry_name_* %}
        {# TaskType的字段 #}
    {% endblock %}

模板片段继承
.............................

每个字段类型都有一个 *父* 类型（例如，``textarea`` 的父类型为 ``text``；``text``
的父类型为 ``form``），如果该基础片段不存在，Symfony将使用其父类型的片段。

例如当Symfony渲染一个 ``textarea`` 类型的错误时，它首先会查找一个
``textarea_errors`` 片段，然后才会后退到 ``text_errors`` 和``form_errors`` 片段。

.. tip::

    每种字段类型的“父”类型在每种字段类型的
    :doc:`表单类型引用 </reference/forms/types>` 中都可用。

在与表单相同的模板中创建表单主题
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在对应用中的单个表单进行特定的自定义时，建议使用此方法，例如更改表单的所有
``<textarea>`` 元素或自定义将由JavaScript处理的非常特殊的表单字段。

你只需要将特殊的 ``{% form_theme form _self %}``
标签添加到要渲染表单的同一模板中。这使得Twig可以在模板内部查找任何被覆盖的表单区块：

.. code-block:: html+twig

    {% extends 'base.html.twig' %}

    {% form_theme form _self %}

    {# 这将覆盖任何整数类型字段的小部件，但又仅限于在此模板内渲染的表单。 #}
    {% block integer_widget %}
        <div class="...">
            {# ... 渲染HTML元素以显示此字段 ... #}
        </div>
    {% endblock %}

    {# 这将覆盖 "id" = "product_stock"(并且 "name" = "product[stock]") 的字段的整个row， #}
    {# 但又仅限于在此模板内渲染的表单。 #}
    {% block _product_stock_row %}
        <div class="..." id="...">
            {# ... 渲染整个字段内容，包括其错误信息 ... #}
        </div>
    {% endblock %}

    {# ... 渲染表单 ... #}

这种方法的主要缺点是它只有在你的模板扩展了另一个模板时才有效（前面的例子中的
``'base.html.twig'`` ）。如果你的模板不存在，则必须将 ``form_theme``
指向单独的模板，具体如下一节中所述。

另一个缺点，就是在其他模板中渲染其他表单时，无法重用该自定义的表单区块。
如果这就是你所需要的，请在一个单独的模板中创建表单主题，具体如下一节中所述。

在单独的模板中创建表单主题
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在创建要在整个应用中使用或甚至在不同的Symfony中复用的表单主题时，建议使用此方法。
你只需要在某处创建一个Twig模板，然后按照 :ref:`表单片段命名 <form-fragment-naming>`
规则来获取要定义的Twig区块。

例如，如果表单主题很简单，并且你只想重写 ``<input type="integer">`` 元素，请创建此模板：

.. code-block:: twig

    {# templates/form/my_theme.html.twig #}
    {% block integer_widget %}

        {# ... 添加渲染此字段所需的所有HTML、CSS和JavaScript #}

    {% endblock %}

现在你需要告诉Symfony使用此表单主题而不是默认主题（或除此之外的主题）。
如前文所述，如果要将主题全局的应用于所有表单，要定义 ``twig.form_themes`` 选项：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            form_themes: ['form/my_theme.html.twig']
            # ...

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:form-theme>form/my_theme.html.twig</twig:form-theme>
                <!-- ... -->
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'form_themes' => [
                'form/my_theme.html.twig',
            ],
            // ...
        ]);

如果你只想将其应用于某些特定表单，请使用 ``form_theme`` 标签：

.. code-block:: twig

    {% form_theme form 'form/my_theme.html.twig' %}

    {{ form_start(form) }}
        {# ... #}
    {{ form_end(form) }}

.. _referencing-base-form-blocks-twig-specific:

复用内置表单主题的某部分
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

创建完整的表单主题需要大量的工作，因为有太多不同的表单字段类型。
你可以只定义你感兴趣的区块，然后在你的应用或模板中配置多个表单主题，而不是定义所有这些Twig区块。
这样做是因为在渲染未在自定义主题中重写的区块时，Symfony会回退到其他主题。

另一种解决方案是使用 `Twig的 "use" 标签`_ 让表单主题模板从其中一个内置主题扩展，而不是
使用 ``extends`` 标签，这样你就可以继承该主题的所有区块（如果你不确定，则从默认的
``form_div_layout.html.twig`` 主题扩展）：

.. code-block:: twig

    {# templates/form/my_theme.html.twig #}
    {% use 'form_div_layout.html.twig' %}

    {# ... 仅重写你感兴趣的区块 #}

最后，你还可以使用 `Twig的 parent() 函数`_ 来复用内置主题的原始内容。
当你只想进行微小更改时，这非常有用，例如使用某些元素来封装生成的HTML：

.. code-block:: html+twig

    {# templates/form/my_theme.html.twig #}
    {% use 'form_div_layout.html.twig' %}

    {% block integer_widget %}
        <div class="some-custom-class">
            {{ parent() }}
        </div>
    {% endblock %}

当在渲染表单的同一模板中定义表单主题时，此技术也有效。但是，从内置主题中导入区块会有点复杂：

.. code-block:: html+twig

    {% form_theme form _self %}

    {# 从内置主题导入一个区块并重命名，使其不与此模板中已定义的同一个区块冲突。 #}
    {% use 'form_div_layout.html.twig' with integer_widget as base_integer_widget %}

    {% block integer_widget %}
        <div class="some-custom-class">
            {{ block('base_integer_widget') }}
        </div>
    {% endblock %}

    {# ... 渲染该表单 ... #}

自定义表单验证错误
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你的对象定义了
:doc:`验证规则 </validation>`，则在提交的数据无效时，你会看到一些验证错误消息。这些消息通过
:ref:`form_errors() <reference-forms-twig-errors>`
函数来展示，并且可以在任何表单主题中使用 ``form_errors`` Twig区块进行自定义，具体如前文所述。

需要考虑的一个重要事项是某些错误会与整个表单而不是特定的字段相关联。
为了区分全局错误和局部错误，请使用一个属于
:ref:`表单可用变量 <reference-form-twig-variables>` 之一的名为 ``compound``
的变量。如果其为 ``true``，则意味着当前要渲染的是一个字段集合（例如整个表单），而不是单个字段：

.. code-block:: html+twig

    {# templates/form/my_theme.html.twig #}
    {% block form_errors %}
        {% if errors|length > 0 %}
            {% if compound %}
                {# ... display the global form errors #}
                <ul>
                    {% for error in errors %}
                        <li>{{ error.message }}</li>
                    {% endfor %}
                </ul>
            {% else %}
                {# ... display the errors for a single field #}
            {% endif %}
        {% endif %}
    {% endblock form_errors %}

.. _`form_div_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/form_div_layout.html.twig
.. _`Twig Bridge`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bridge/Twig
.. _`view on GitHub`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bundle/FrameworkBundle/Resources/views/Form
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
.. _`Twig的 "use" 标签`: https://twig.symfony.com/doc/2.x/tags/use.html
.. _`Twig的 parent() 函数`: https://twig.symfony.com/doc/2.x/functions/parent.html
