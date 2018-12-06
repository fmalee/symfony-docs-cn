.. index::
   single: Forms; Theming
   single: Forms; Customizing fields

如何使用表单主题
============================

可以自定义表单渲染方式的每个部分。
你可以自由更改每个表单的“row”的渲染方式，更改用于渲染错误的标记，甚至可以自定义 ``textarea`` 标签的渲染方式。
没有什么是禁止的，并且可以在不同的地方使用不同的自定义。

Symfony使用模板来渲染一个表单的每个部分，例如 ``label`` 标签、``input`` 标签、错误消息以及其他所有内容。

在Twig中，每个表单“片段”由一个Twig区块来表示。要自定义一个表单渲染方式的任何部分，你需要重写相应的区块。

在PHP中，每个表单“片段”都通过一个单独的模板文件进行渲染。要自定义一个表单渲染方式的任何部分，请通过创建新模板来重写现有模板。

要了解其工作原理，请自定义 ``form_row`` 片段并添加一个类属性到围绕每个row的 ``div`` 元素中。
为此，请创建一个用于存储新标记的新模板文件：

.. code-block:: html+twig

    {# templates/form/fields.html.twig #}
    {% block form_row %}
    {% spaceless %}
        <div class="form_row">
            {{ form_label(form) }}
            {{ form_errors(form) }}
            {{ form_widget(form) }}
            {{ form_help(form) }}
        </div>
    {% endspaceless %}
    {% endblock form_row %}

当通过 ``form_row`` 函数渲染大多数字段时，就是使用该 ``form_row()`` 表单片段。
要告诉Form组件使用上面定义的新 ``form_row()`` 片段，请将以下内容添加到渲染表单的模板的顶部：

.. code-block:: html+twig

    {# templates/default/new.html.twig #}
    {% form_theme form 'form/fields.html.twig' %}

    {# 或者你想使用多个主题 #}
    {% form_theme form 'form/fields.html.twig' 'form/fields2.html.twig' %}

    {# ... 渲染该表单 #}

给定模板中的片段在通过 ``form_theme`` 标签(在Twig中)"导入"后，将会在渲染表单时被应用。
换句话说，当稍后在此模板中调用 ``form_row()`` 函数时 ，它将使用你的自定义主题中的 ``form_row`` 区块（而不是Symfony附带的默认 ``form_row`` 区块）。

你的自定义主题不必重写所有的区块。渲染一个未在你的自定义主题中重写的区块时，主题引擎将回退到全局主题（在bundle级别定义）。

如果提供了多个自定义主题，则会在回退到全局主题之前并按列出的顺序进行搜索对应主题。

要自定义一个表单的任何部分，请重写相应的片段。想确切地知道要重写哪个区块或文件是下一节的主题。

有关更广泛的讨论，请参阅 :doc:`/form/form_customization`。

.. index::
   single: Forms; Template fragment naming

.. _form-template-blocks:

表单片段命名
--------------------

在Symfony中，被渲染的一个表单的每个部分（HTML表单元素、错误、标签等）都定义在一个基础主题中，
基础主题是Twig中的区块集合和PHP中的模板文件集合。

在Twig中，所需的每个区块都定义在单个模板文件中（例如 `form_div_layout.html.twig`_），它位于 `Twig Bridge`_ 中。
在此文件中，你可以看到渲染一个表单和每个默认字段类型所需的每个区块。

在PHP中，片段是单独的模板文件。默认情况下，它们位于FrameworkBundle的 ``Resources/views/Form`` 目录中（`在GitHub上查看`_）。

每个片段名称都遵循相同的基本模式，并分为两部分，由单个下划线字符（``_``）分隔。一些例子是：

* ``form_row`` - 用于通过 ``form_row()`` 来渲染大多数字段;
* ``textarea_widget`` - 用于通过 ``form_widget()`` 来渲染一个 ``textarea`` 字段类型;
* ``form_errors`` - 用于通过 ``form_errors()`` 来渲染字段的一个错误信息;

每个片段遵循相同的基本模式：``type_part``。``type`` 部分对应于要渲染的字段
*类型* （例如 ``textarea``、``checkbox``、``date`` 等等），而 ``part``
部分对应于正在渲染 *什么* （例如 ``label``、``widget``、``errors`` 等等）。
一个要被渲染的表单有4个可能的部分：

+-------------+----------------------------+---------------------------------------------------------+
| ``label``   | (例如 ``form_label()``)    | 渲染给定字段的标签                                      |
+-------------+----------------------------+---------------------------------------------------------+
| ``widget``  | (例如 ``form_widget()``)   | 渲染给定字段的HTML内容                                  |
+-------------+----------------------------+---------------------------------------------------------+
| ``errors``  | (例如 ``form_errors()``)   | 渲染给定字段的错误信息                                  |
+-------------+----------------------------+---------------------------------------------------------+
| ``help``    | (例如 ``form_help()``)     | 渲染给定字段的帮助                                      |
+-------------+----------------------------+---------------------------------------------------------+
| ``row``     | (例如 ``form_row()``)      | 渲染给定字段的整行 (label, widget & errors)             |
+-------------+----------------------------+---------------------------------------------------------+

.. note::

    实际上有两个其他 *部分* - ``rows`` 和 ``rest`` - 你应该很少需要操心的重写它们。

通过了解字段类型（例如 ``textarea``）以及你要自定义的部分（例如
``widget``），你就可以命名需要重写的片段名称（例如 ``textarea_widget``）。

.. index::
   single: Forms; Template fragment inheritance

模板片段继承
-----------------------------

在某些情况下，你要自定义的片段似乎缺失。
例如，Symfony提供的默认主题中没有 ``textarea_errors`` 片段。那么一个文本框字段的错误是如何渲染的呢？

答案是：通过 ``form_errors`` 片段。
当Symfony为一个文本框类型渲染错误时，它首先查找一个 ``textarea_errors`` 片段，然后回退到
``form_errors`` 片段。
每个字段类型都有一个 *父* 类型（``text`` 是 ``textarea`` 的父类型，而其父类型是
``form``），如果基础片段不存在，Symfony将使用该类型作为父类型。

因此，如果 *仅* 需要重写 ``textarea`` 字段的错误，请复制 ``form_errors`` 片段，将其重命名为
``textarea_errors`` 并自定义。
但如果要重写 *所有* 字段的默认错误渲染，请直接复制和自定义 ``form_errors`` 片段。

.. tip::

    每种字段类型的“父”类型在每种字段类型的 :doc:`表单类型参考 </reference/forms/types>` 中都可用。

.. index::
   single: Forms; Global Theming

.. _forms-theming-global:

全局表单主题
-------------------

在上面的例子中，你使用 ``form_theme`` 助手（在Twig中）“导入”的自定义表单片段 *只是* 针对单个表单。
你还可以告诉Symfony在整个项目中导入表单的自定义。

.. _forms-theming-twig:

Twig
....

要自动在所有模板中引入先前创建的 ``fields.html.twig`` 模板中的自定义区块，请修改应用的配置文件：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            form_themes:
                - '...'
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
                http://symfony.com/schema/dic/twig http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:theme>...</twig:theme>
                <twig:theme>form/fields.html.twig</twig:theme>
                <!-- ... -->
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', array(
            'form_themes' => array(
                '...',
                'form/fields.html.twig',
            ),
            // ...
        ));

.. note::

    请添加自定义主题到 ``form_themes`` 列表的末尾，因为每个主题都会重写前面的所有主题。

现在，``fields.html.twig`` 模板中的任何区块都可以用于全局的定义表单输出。

.. sidebar::  使用Twig在单个文件中自定义表单输出

    在Twig中，你还可以在需要自定义的模板内部自定义一个表单区块：

    .. code-block:: html+twig

        {% extends 'base.html.twig' %}

        {# 导入 "_self" 作为表单主题 #}
        {% form_theme form _self %}

        {# 创建表单片段的自定义 #}
        {% block form_row %}
            {# 自定义字段行的输出 #}
        {% endblock form_row %}

        {% block content %}
            {# ... #}

            {{ form_row(form.task) }}
        {% endblock %}

    ``{% form_theme form _self %}`` 标签允许直接在需要这些自定义的模板中自定义表单区块。
    使用此方法可以快速创建一个只在单个模板中生效的表单输出自定义。

    .. caution::

        *仅当* 你的模板继承另一个模板时，``{% form_theme form _self %}`` 功能才有效。
        如果你的模板没有继承，则必须将 ``form_theme`` 指向一个单独的模板。

PHP
...

要自动在 *所有* 模板中引入先前创建的 ``templates/form`` 目录中的自定义模板，请修改应用的配置文件：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            templating:
                form:
                    resources:
                        - 'form'
        # ...

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:templating>
                    <framework:form>
                        <framework:resource>form</framework:resource>
                    </framework:form>
                </framework:templating>
                <!-- ... -->
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            'templating' => array(
                'form' => array(
                    'resources' => array(
                        'form',
                    ),
                ),
            ),
            // ...
        ));

``templates/form`` 目录中的任何片段现在都用于全局的定义表单输出。

.. _`form_div_layout.html.twig`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bridge/Twig/Resources/views/Form/form_div_layout.html.twig
.. _`Twig Bridge`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bridge/Twig
.. _`在GitHub上查看`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bundle/FrameworkBundle/Resources/views/Form
