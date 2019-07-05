.. index::
   single: Form; Custom field type

如何创建自定义表单字段类型
======================================

Symfony附带 :doc:`几十种表单类型 </reference/forms/types>` （在其他项目中称为“表单字段”），可以在你的应用中使用。
但是，创建自定义表单类型以用于项目中的特定用途是很常见的。

基于Symfony内置类型来创建表单类型
---------------------------------------------------

创建一个表单类型的最简单方法是在 :doc:`现有表单类型 </reference/forms/types>`
的基础上创建。想象一下，你的项目将“运送选项”列表显示为 ``<select>`
HTML元素。这可以通过 :doc:`ChoiceType </reference/forms/types/choice>`
实现，其中 ``choices`` 选项设置为可用的运送选项列表。

但是，如果你在多个表单中使用相同的表单类型，则每次使用它时重复 ``choices``
列表会很快就会变得无聊。在此示例中，更好的解决方案是基于 ``ChoiceType``
创建自定义表单类型。该自定义类型的外观和行为类似于
``ChoiceType``，但选项列表已填充了运送选项，因此你无需定义它们。

表单类型是实现了 :class:`Symfony\\Component\\Form\\FormTypeInterface`
的PHP类，但你应该从实现了该接口并提供了一些实用工具的
:class:`Symfony\\Component\\Form\\AbstractType` 扩展。按照惯例，它们存储在
``src/Form/Type/`` 目录中::

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

``configureOptions()`` 方法将在本文后面介绍，它定义了可以被表单类型配置的选项，并设置这些选项的默认值。

``getParent()`` 方法定义哪个是用作此类型基础的表单类型。在这个例子中，该类型从
``ChoiceType`` 扩展，以重用该字段类型的所有逻辑和渲染。

.. note::

    PHP类扩展机制和Symfony表单字段扩展机制不一样。在 ``getParent()``
    中返回的父类型是Symfony用于构建和管理字段类型的。
    使PHP类扩展自 ``AbstractType`` 只是实现所需的 ``FormTypeInterface`` 的一种便捷方式。

现在，你可以在 :doc:`创建Symfony表单 </forms>` 时添加此表单类型::

    // src/Form/Type/OrderType.php
    namespace App\Form\Type;

    use App\Form\Type\ShippingType;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class OrderType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('shipping', ShippingType::class)
            ;
        }

        // ...
    }

仅此而已。``shipping`` 表单字段将在任何模板中正确渲染，因为它重用了它的父类 ``ChoiceType``
中定义的模板逻辑。如果你愿意，还可以如本文后面所述为该自定义类型定义一个模板。

创建全新的表单类型
----------------------------------------

某些表单类型非常特定于你的项目，因此它们不能基于任何
:doc:`现有的表单类型 </reference/forms/types>`，因为它们太不同了。
考虑一个想要在不同的表单中重用以下字段集作为“邮政地址”的应用：

.. raw:: html

    <object data="../_images/form/form-custom-type-postal-address.svg" type="image/svg+xml"></object>

如上所述，表单类型是实现了 :class:`Symfony\\Component\\Form\\FormTypeInterface`
的PHP类，尽管从 :class:`Symfony\\Component\\Form\\AbstractType` 扩展表单类型更方便::

    // src/Form/Type/PostalAddressType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\FormType;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class PostalAddressType extends AbstractType
    {
        // ...
    }

当表单类型不从另一个特定类型扩展时，不需要实现 ``getParent()``
方法（Symfony将使该类型从通用的
:class:`Symfony\\Component\\Form\\Extension\\Core\\Type\\FormType`
扩展，这是所有其他类型的父类）。

以下是表单类型类可以定义的最重要的方法：

.. _form-type-methods-explanation:

``buildForm()``
    它用于把其他类型添加并配置为此类型。它与
    :ref:`创建Symfony表单类 <form-creating-form-classes>` 时使用的方法相同。

``buildView()``
    它用于配置在模板中渲染该字段时需要的任何额外变量。

``configureOptions()``
    它用于定义使用该表单类型时的可配置选项，这些选项也同时可用在 ``buildForm()`` 和
    ``buildView()`` 方法中。

``finishView()``
    创建一个包含许多字段的表单类型时，此方法允许修改任何这些字段的“视图”。
    对于任何其他用例，建议使用 ``buildView()`` 方法。

定义表单类型
~~~~~~~~~~~~~~~~~~~~~~

首先添加 ``buildForm()`` 方法以配置邮政地址中包含的所有类型。目前，所有字段都
``TextType`` 类型::

    // src/Form/Type/PostalAddressType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormBuilderInterface;

    class PostalAddressType extends AbstractType
    {
        // ...

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('addressLine1', TextType::class, [
                    'help' => 'Street address, P.O. box, company name',
                ])
                ->add('addressLine2', TextType::class, [
                    'help' => 'Apartment, suite, unit, building, floor',
                ])
                ->add('city', TextType::class)
                ->add('state', TextType::class, [
                    'label' => 'State',
                ])
                ->add('zipCode', TextType::class, [
                    'label' => 'ZIP Code',
                ])
            ;
        }
    }

.. tip::

    运行以下命令以验证表单类型是否已在应用中成功注册：

    .. code-block:: terminal

        $ php bin/console debug:form

此表单类型已经可以在其他表单中使用了，并且在任何模板中都可以正确渲染其所有字段::

    // src/Form/Type/OrderType.php
    namespace App\Form\Type;

    use App\Form\Type\PostalAddressType;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;

    class OrderType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('address', PostalAddressType::class)
            ;
        }

        // ...
    }

但是，自定义表单类型的真正功能是通过自定义表单选项（使其更灵活）和自定义模板（使它们看起来更好）实现的。

.. _form-type-config-options:

添加表单类型的配置选项
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

想象一下，你的项目需要以两种方式进行 ``PostalAddressType`` 配置：

* 除了 "address line 1" 和 "address line 2" 之外，应允许一些地址显示
  "address line 3" 以存储额外的地址信息；
* 某些地址应该能够将可能的状态限制为一个给定列表，而不是显示自由的文本输入。

这可以通过允许配置表单类型的行为的“表单类型选项”来解决。这些选项在 ``configureOptions()``
方法中定义，你可以使用所有的 :doc:`OptionsResolver组件功能 </components/options_resolver>`
来定义、验证和处理它们的值::

    // src/Form/Type/PostalAddressType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\OptionsResolver\Options;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class PostalAddressType extends AbstractType
    {
        // ...

        public function configureOptions(OptionsResolver $resolver)
        {
            // 定义可用选项及其默认值，它们会在应用了该表单类型但未显式的配置可用选项时使用。
            $resolver->setDefaults([
                'allowed_states' => null,
                'is_extended_address' => false,
            ]);

            // 也可以选择限制这些选项的类型（为最终用户获取自动类型验证和有用的错误消息）
            $resolver->setAllowedTypes('allowed_states', ['null', 'string', 'array']);
            $resolver->setAllowedTypes('is_extended_address', 'bool');

            // 或者，你可以转换选项的给定值，以简化这些选项的进一步处理。
            $resolver->setNormalizer('allowed_states', static function (Options $options, $states) {
                if (null === $states) {
                    return $states;
                }

                if (is_string($states)) {
                    $states = (array) $states;
                }

                return array_combine(array_values($states), array_values($states));
            });
        }
    }

现在，你可以在使用表单类型时配置这些选项了::

    // src/Form/Type/OrderType.php
    // ...

    class OrderType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('address', PostalAddressType::class, [
                    'is_extended_address' => true,
                    'allowed_states' => ['CA', 'FL', 'TX'],
                    // 在本例中，此配置也将有效：
                    // 'allowed_states' => 'CA',
                ])
            ;
        }

        // ...
    }

最后一步是在构建表单时使用这些选项::

    // src/Form/Type/PostalAddressType.php
    // ...

    class PostalAddressType extends AbstractType
    {
        // ...

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            // ...

            if (true === $options['is_extended_address']) {
                $builder->add('addressLine3', TextType::class, [
                    'help' => 'Extended address info',
                ]);
            }

            if (null !== $options['allowed_states']) {
                $builder->add('state', ChoiceType::class, [
                    'choices' => $options['allowed_states'],
                ]);
            } else {
                $builder->add('state', TextType::class, [
                    'label' => 'State/Province/Region',
                ]);
            }
        }
    }

创建表单类型模板
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，将使用应用中配置的 :doc:`表单主题 </form/form_themes>` 来渲染自定义表单类型。
但是，对于某些类型，你可能更喜欢创建一个自定义模板，以便自定义它们的外观或HTML结构。

首先，在应用的任何位置创建一个新的Twig模板，以存储用于渲染该类型的片段：

.. code-block:: twig

    {# templates/form/custom_types.html.twig #}

    {# ... 在这里你将添加Twig代码 ... #}

然后，更新 :ref:`form_themes选项 <reference-twig-tag-form-theme>`
以在列表的开头添加此新模板（第一个文件会覆盖其余文件）：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            form_themes:
                - 'form/custom_types.html.twig'
                - '...'

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
                <twig:form-theme>form/custom_types.html.twig</twig:form-theme>
                <twig:form-theme>...</twig:form-theme>
            </twig:config>
        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'form_themes' => [
                'form/custom_types.html.twig',
                '...',
            ],
        ]);

最后一步是创建将渲染该类型的实际Twig模板。
模板内容取决于应用中使用的HTML、CSS和JavaScript框架和库：

.. code-block:: twig

    {# templates/form/custom_types.html.twig #}
    {% block postal_address_row %}
        {% for child in form.children if not child.rendered %}
            <div class="form-group">
                {{ form_label(child) }}
                {{ form_widget(child) }}
                {{ form_help(child) }}
                {{ form_errors(child) }}
            </div>
        {% endfor %}
    {% endblock %}

.. note::

    ``FormRenderer::searchAndRenderBlock``。这就是为什么前面的例子包含
    ``... if not child.rendered`` 声明。

Twig区块名称的第一部分（例如 ``postal_address``）来自类名（``PostalAddressType``
-> ``postal_address``）。这可以通过在 ``PostalAddressType`` 中重写 ``getBlockPrefix()``
方法来控制。Twig区块名称的第二部分（例如
``_row``）定义要渲染表单类型的哪部分（row、widget、help、errors等）

有关表单主题的文章详细解释了 :ref:`表单片段命名规则 <form-fragment-naming>`。
下图展示了此示例中定义的一些Twig区块名称：

.. raw:: html

    <object data="../_images/form/form-custom-type-postal-address-fragment-names.svg" type="image/svg+xml"></object>

.. caution::

    当你的表单类的名称与任何内置字段类型相匹配时，该表单可能无法正确渲染。名为
    `App\Form\PasswordType`` 的表单类型将具有与内置 ``PasswordType``
    相同的区块名称，并且无法正确渲染。为避免冲突，可以重写 ``getBlockPrefix()``
    方法以返回一个唯一的区块前缀（例如 ``app_password``）。

将变量传递给表单类型模板
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony会将一系列变量传递给模板以渲染表单类型。
你也可以传递自己的变量，这些变量可以基于表单定义的选项，也可以完全独立::

    // src/Form/Type/PostalAddressType.php
    use Doctrine\ORM\EntityManagerInterface;
    // ...

    class PostalAddressType extends AbstractType
    {
        private $entityManager;

        public function __construct(EntityManagerInterface $entityManager)
        {
            $this->entityManager = $entityManager;
        }

        // ...

        public function buildView(FormView $view, FormInterface $form, array $options)
        {
            // 将表单类型的选项直接传递到模板
            $view->vars['isExtendedAddress'] = $options['is_extended_address'];

            // 进行数据库查询以查找与邮政地址相关的可能通知（例如显示如
            // 'Delivery to XX and YY states will be added next week!'之类的动态消息）
            $view->vars['notification'] = $this->entityManager->find('...');
        }
    }

如果你使用的是
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，则此示例已经可以使用！
否则，为此表单类 :ref:`创建一个服务 <service-container-creating-service>`
并使用 ``form.type`` 标签进行 :doc:`标记 </service_container/tags>`。

在 ``buildView()`` 中添加的变量与任何其他常规Twig变量一样，都在表单类型模板中可用：

.. code-block:: twig

    {# templates/form/custom_types.html.twig #}
    {% block postal_address_row %}
        {# ... #}

        {% if isExtendedAddress %}
            {# ... #}
        {% endif %}

        {% if notification is not empty %}
            <div class="alert alert-primary" role="alert">
                {{ notification }}
            </div>
        {% endif %}
    {% endblock %}
