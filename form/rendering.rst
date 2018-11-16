.. index::
    single: Forms; Rendering in a template

如何控制表单的渲染
======================================

到目前为止，你已经看到了如何使用一行代码渲染整个表单。当然，渲染时通常需要更多的灵活性：

.. code-block:: html+twig

    {# templates/default/new.html.twig #}
    {{ form_start(form) }}
        {{ form_errors(form) }}

        {{ form_row(form.task) }}
        {{ form_row(form.dueDate) }}
    {{ form_end(form) }}

你已经了解 ``form_start()`` 和 ``form_end()`` 函数，但其他函数有何功能？

``form_errors(form)``
    将任何错误全局的渲染给整个表单（每个字段旁边都会显示特定于该字段的错误）。

``form_row(form.dueDate)``
    默认情况下，使用 ``div`` 元素为给定字段（例如 ``dueDate``）内部渲染标签、任何错误和HTML表单部件。

大部分工作由 ``form_row()`` 帮助函数完成，默认情况下，它会在一个 ``div``
标记内渲染每个字段的标签、错误和HTML表单部件。
在 :doc:`/form/form_themes` 文档中，你将了解如何在多个不同级别上自定义 ``form_row()`` 输出。

.. tip::

    你可以通过 ``form.vars.value`` 访问表单的当前数据：

.. code-block:: twig

    {{ form.vars.value.task }}

.. index::
   single: Forms; Rendering each field by hand

手动渲染每个字段
----------------------------

``form_row()`` 辅助函数很好用，因为你可以非常快速地渲染表单的每个字段（并且用于该“row”的标记很容易自定义）。
但由于生活并非总是那么简单，你也可以完全用手渲染每个字段。
以下的最终成品与你使用 ``form_row()`` 辅助函数时的相同：

.. code-block:: html+twig

    {{ form_start(form) }}
        {{ form_errors(form) }}

        <div>
            {{ form_label(form.task) }}
            {{ form_errors(form.task) }}
            {{ form_widget(form.task) }}
            {{ form_help(form.task) }}
        </div>

        <div>
            {{ form_label(form.dueDate) }}
            {{ form_errors(form.dueDate) }}
            {{ form_widget(form.dueDate) }}
            {{ form_help(form.dueDate) }}
        </div>

        <div>
            {{ form_widget(form.save) }}
        </div>

    {{ form_end(form) }}

如果一个字段自动生成的标签不完全正确，则可以显性的指定它：

.. code-block:: html+twig

    {{ form_label(form.task, 'Task Description') }}

某些字段类型具有可以传递给小部件的其他渲染选项。
每种类型都记录了这些选项，但有一个 ``attr`` 常用选项，它允许你修改表单元素上的属性。
以下将添加 ``task_field`` 样式类到渲染的文本字段中：

.. code-block:: html+twig

    {{ form_widget(form.task, {'attr': {'class': 'task_field'}}) }}

如果你需要“手动”渲染表单字段，那么你可以独立的获取该字段的各个值，例如 ``id``、``name`` 和 ``label``。
例如获取字段的 ``id``：

.. code-block:: html+twig

    {{ form.task.vars.id }}

要获取用于表单字段的名称属性的值，你需要使用 ``full_name`` 值：

.. code-block:: html+twig

    {{ form.task.vars.full_name }}

Twig模板函数参考
--------------------------------

如果你正在使用Twig，则 :doc:`参考手册 </reference/forms/twig_reference>` 中提供了表单渲染函数的完整参考。
阅读该文档以了解可用辅助函数的所有内容以及可用于每个辅助函数的选项。
