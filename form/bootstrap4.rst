Bootstrap4表单主题
======================

Symfony提供了几种将Bootstrap集成到应用中的方法。
最直接的方法是 在模板中添加必需元素 ``<link>`` 和 ``<script>`` 元素
（通常只将它们包含在由其他模板扩展的主布局模板中）：

.. code-block:: html+twig

    {# templates/base.html.twig #}

    {# 请注意，模板中的区块名称可能不同 #}
    {% block head_css %}
        <!-- 从 https://getbootstrap.com/docs/4.1/getting-started/introduction/#css 复制CSS -->
    {% endblock %}
    {% block head_js %}
        <!-- 从 https://getbootstrap.com/docs/4.1/getting-started/introduction/#js 复制JavaScript -->
    {% endblock %}

如果你的应用使用现代前端实践，最好使用 :doc:`Webpack Encore </frontend>`
并按照 :doc:`本教程 </frontend/encore/bootstrap>` 将Bootstrap的源代码导入到你的SCSS和JavaScript文件。

下一步是在渲染表单时将Symfony应用配置为使用Bootstrap 4样式。如果要将它们应用于所有表单，请定义此配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            form_themes: ['bootstrap_4_layout.html.twig']

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:form-theme>bootstrap_4_layout.html.twig</twig:form-theme>
                <!-- ... -->
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'form_themes' => [
                'bootstrap_4_layout.html.twig',
            ],

            // ...
        ]);

如果你希望将基础表单上的Bootstrap样式应用于表单，请在使用这些表单的模板中引入 ``form_theme`` 标记：

.. code-block:: html+twig

    {# ... #}
    {# 此标签仅适用于此模板中定义的表单 #}
    {% form_theme form 'bootstrap_4_layout.html.twig' %}

    {% block body %}
        <h1>User Sign Up:</h1>
        {{ form(form) }}
    {% endblock %}

辅助功能
-------------

Bootstrap 4框架已经做得很好，使其可以辅助机能差异，如视力受损和认知障碍。
Symfony更进一步确保表单主题符合 `WCAG 2.0 标准`_。

这并不意味着你的整个网站自动符合该完整标准，但它确实意味着你的作品已经为 **所有** 用户做好了准备。

自定义表单
------------

Bootstrap 4有一个称为“`自定义表单`_”的功能。
该功能通过添加一个分别叫 ``radio-custom`` 和 ``checkbox-custom``
的类来在Symfony表单 ``RadioType`` 和 ``CheckboxType`` 中启用。

.. code-block:: twig

    {{ form_row(form.myRadio, {label_attr: {class: 'radio-custom'} }) }}
    {{ form_row(form.myCheckbox, {label_attr: {class: 'checkbox-custom'} }) }}

标签和错误
-----------------

当你使用Bootstrap表单主题并手动渲染字段时，为复选框/单选框字段调用 ``form_label()`` 将不会渲染任何内容。
因为在Bootstrap内部，该标签已经通过 ``form_widget()`` 渲染。

表单错误将在 ``<label>`` 元素 **内部** 渲染，以确保该错误与 ``<input>`` 之间存在紧密联系，
这是 `WCAG 2.0 标准`_ 所要求的。

.. _`它们的文档`: https://getbootstrap.com/docs/4.1/
.. _`WCAG 2.0 标准`: https://www.w3.org/TR/WCAG20/
.. _`自定义表单`: https://getbootstrap.com/docs/4.1/components/forms/#custom-forms
