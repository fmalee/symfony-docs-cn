.. index::
   single: Form; Custom field type

如何创建自定义表单字段类型
======================================

Symfony附带有一堆可用于构建表单的核心字段类型。
但是，在某些情况下，你可能希望为特定目的创建一个自定义表单字段类型。
本文假设你需要一个字段定义，其中包含基于现有 ``choice`` 字段的物流（shipping）选项。
本节将介绍字段如何定义以及如何自定义其布局。

定义字段类型
-----------------------

要创建自定义字段类型，首先必须创建表示该字段的类。
在这个例子中，保存该字段类型的类被命名为 ``ShippingType``，并将该文件存储在表单字段的默认位置，即
``App\Form\Type``。

所有的字段类型都必须实现 :class:`Symfony\\Component\\Form\\FormTypeInterface`，
但你应该继承已经实现了该接口并提供了一些实用工具的
:class:`Symfony\\Component\\Form\\AbstractType`::

    // src/Form/Type/ShippingType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\ChoiceType;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class ShippingType extends AbstractType
    {
        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'choices' => [
                    'Standard Shipping' => 'standard',
                    'Expedited Shipping' => 'expedited',
                    'Priority Shipping' => 'priority',
                ],
            ]);
        }

        public function getParent()
        {
            return ChoiceType::class;
        }
    }

.. tip::

    此文件的位置并不重要 - ``Form\Type`` 目录只是一个约定。

``getParent()`` 函数的返回值表示你正在扩展 ``ChoiceType`` 字段。
这意味着，默认情况下，你继承了该字段类型的所有逻辑和渲染。
要查看其中一些逻辑，请查看 `ChoiceType`_ 类。

.. note::

    PHP类扩展机制和Symfony表单字段扩展机制不一样。在 ``getParent()``
    中返回的父类型是Symfony用于构建和管理字段类型的。
    使PHP类扩展自 ``AbstractType`` 只是实现所需的 ``FormTypeInterface`` 的一种便捷方式。

有三种方法特别重要：

.. _form-type-methods-explanation:

``buildForm()``
    每个字段类型都有一个 ``buildForm()`` 方法，你可以在其中配置和构建任何字段。
    请注意，这与用于设置 *你的* 表单的是同一个方法，在此处的工作方式也相同。

``buildView()``
    此方法用于设置在模板中渲染该字段时需要的任何额外变量。
    例如，在 `ChoiceType`_ 中设置了一个 ``multiple``
    变量，而该变量在模板中被使用来设置（或不设置） ``select`` 字段上的 ``multiple`` 属性。
    有关更多详细信息，请参阅 `为字段创建一个模板`_。

``configureOptions()``
    这里定义了可以在 ``buildForm()`` 和 ``buildView()`` 中使用的表单类型的选项。
    所有字段都有许多共同的选项（请参阅 :doc:`/reference/forms/types/form`），但你可以在此处创建所需的任何其他选项。

.. tip::

    如果你要创建一个包含许多字段的字段，请务必将“父”类型设置为 ``form`` 或某些继承 ``form`` 的类。
    此外，如果你需要从你的父类型修改任何子类型的“视图”，请使用 ``finishView()`` 方法。

此字段的目标是扩展 ``choice`` 类型以启用物流类型的选择选项。
这是通过将 ``choices`` 修复为可用的运输选项列表来实现的。

.. tip::

    运行以下命令以验证表单类型是否已在应用中成功注册：

    .. code-block:: terminal

        $ php bin/console debug:form

为字段创建一个模板
---------------------------------

每个字段类型都由模板片段渲染，模板片段的名称部分取决于你的类型的类名称。有关更多详细信息，请阅读
:ref:`表单片段命名 <form-fragment-naming>` 规则。

.. note::

    前缀的第一部分（例如 ``shipping``）来自类名称（``ShippingType`` -> ``shipping``）。
    这可以通过重写 ``ShippingType`` 中的 ``getBlockPrefix()`` 来控制。

.. caution::

    当你的表单类的名称与任何内置字段类型相同时，表单可能无法正确渲染。名为
    ``App\Form\PasswordType`` 的表单类型将具有与内置 ``PasswordType``
    相同的区块名称，并且将无法正确渲染。
    为避免冲突，请重写 ``getBlockPrefix()`` 方法以返回唯一的区块前缀（例如 ``app_password``）。

在这个例子中，由于父字段是 ``ChoiceType``，你 *不*
再需要做任何工作，因为该自定义字段类型将自动渲染为一个类似 ``ChoiceType`` 的外观。
但是为了展示这个例子，现在假设你的字段是“expanded”（即单选按钮或复选框，而不是一个选择字段），
你想要总是在一个 ``ul`` 元素中渲染它。
在你的表单主题模板中（有关详细信息，请参见上面的链接），创建一个 ``shipping_widget`` 区块来：

.. code-block:: html+twig

    {# templates/form/fields.html.twig #}
    {% block shipping_widget %}
        {% spaceless %}
            {% if expanded %}
                <ul {{ block('widget_container_attributes') }}>
                    {% for child in form if not child.rendered %}
                        <li>
                            {{ form_widget(child) }}
                            {{ form_label(child) }}
                        </li>
                    {% endfor %}
                </ul>
            {% else %}
                {# 让 choice 部件渲染选择标签 #}
                {{ block('choice_widget') }}
            {% endif %}
        {% endspaceless %}
    {% endblock %}

.. note::

    Symfony 4.2已弃用(deprecated)为已经渲染的字段调用 ``FormRenderer::searchAndRenderBlock``。
    这就是为什么前面的例子会引入 ``... if not child.rendered`` 语句。

.. tip::

    你可以进一步自定义用于渲染 choice 类型的每个子项的模板。
    在这种情况下要重写的区块被命名为：
    “区块名称”+ ``_entry_`` +“元素名称”（``label``、``errors`` 或
    ``widget``）（例如，自定义Shipping部件的子项的标签，你需要定义
    ``{% block shipping_entry_label %} ... {% endblock %}``）。

.. note::

    确保使用正确的部件前缀。在这个例子中，名称应该是 ``shipping_widget``
    （请参阅 :ref:`form fragment naming <form-fragment-naming>` 规则）。
    此外，主配置文件应指向自定义表单模板，以便在渲染所有表单时使用它。

    使用Twig时，应该是：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/twig.yaml
            twig:
                form_themes:
                    - 'form/fields.html.twig'

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
                    <twig:form-theme>form/fields.html.twig</twig:form-theme>
                </twig:config>
            </container>

        .. code-block:: php

            // config/packages/twig.php
            $container->loadFromExtension('twig', [
                'form_themes' => [
                    'form/fields.html.twig',
                ],
            ]);

    对于PHP模板引擎，你的配置应如下所示：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            framework:
                templating:
                    form:
                        resources:
                            - ':form:fields.html.php'

        .. code-block:: xml

            <!-- config/packages/framework.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <framework:config>
                    <framework:templating>
                        <framework:form>
                            <framework:resource>:form:fields.html.php</twig:resource>
                        </framework:form>
                    </framework:templating>
                </framework:config>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->loadFromExtension('framework', [
                'templating' => [
                    'form' => [
                        'resources' => [
                            ':form:fields.html.php',
                        ],
                    ],
                ],
            ]);

使用字段类型
--------------------

你现在可以通过在你的其中一个表单中创建该类型的新实例来使用该自定义字段类型::

    // src/Form/Type/OrderType.php
    namespace App\Form\Type;

    use App\Form\Type\ShippingType;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class OrderType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('shippingCode', ShippingType::class, [
                'placeholder' => 'Choose a delivery option',
            ]);
        }
    }

但这只能在 ``ShippingType()`` 非常简单的情况下起作用。
如果运输代码存储在配置或数据库中，该怎么办？下一节将介绍如何使用更复杂的字段类型来解决此问题。

.. _form-field-service:
.. _creating-your-field-type-as-a-service:

访问服务和配置
-----------------------------

如果你需要从表单类访问 :doc:`服务 </service_container>`，请像往常一样添加一个
``__construct()`` 方法::

    // src/Form/Type/ShippingType.php
    namespace App\Form\Type;

    // ...
    use Doctrine\ORM\EntityManagerInterface;

    class ShippingType extends AbstractType
    {
        private $entityManager;

        public function __construct(EntityManagerInterface $entityManager)
        {
            $this->entityManager = $entityManager;
        }

        // 在任何你想要的地方使用 $this->entityManager ...
    }

如果你正在使用默认的 ``services.yaml`` 配置（即已从 ``Form/``
加载服务并启用了 ``autoconfigure``），那就已经自动生效！
有关更多详细信息，请参阅 :ref:`service-container-creating-service`。

.. tip::

    如果你没有使用 :ref:`自动配置 <services-autoconfigure>`，请务必使用
    ``form.type`` 标签来 :doc:`标记 </service_container/tags>` 你的服务。

玩得开心！

.. _`ChoiceType`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Extension/Core/Type/ChoiceType.php
.. _`FieldType`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Form/Extension/Core/Type/FieldType.php
