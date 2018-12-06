.. index::
   single: Form; Form type extension

如何创建表单类型扩展
===================================

表单类型扩展 *令人难以置信* 的强大：它们允许你 *修改* 整个系统中的任何现有表单字段类型。

他们有两个主要用例：

#. 你想要将一个 **特定功能添加到单个表单类型** （例如向 ``FileType`` 字段类型添加“下载”功能）;
#. 你希望 **为几种类型添加通用功能** （例如向每个“输入文本”类型添加一个“帮助”文本）。

想象一下，你有一个 ``Media`` 实体，并且每个媒体都与一个文件相关联。
你的 ``Media`` 表单使用一个文件类型，但在编辑实体时，你希望在文件输入旁边的自动渲染其图像。

定义表单类型扩展
--------------------------------

首先，创建继承自 :class:`Symfony\\Component\\Form\\AbstractTypeExtension`
的表单类型扩展类（如果你愿意，也可以用实现
:class:`Symfony\\Component\\Form\\FormTypeExtensionInterface` 来代替）::

    // src/Form/Extension/ImageTypeExtension.php
    namespace App\Form\Extension;

    use Symfony\Component\Form\AbstractTypeExtension;
    use Symfony\Component\Form\Extension\Core\Type\FileType;

    class ImageTypeExtension extends AbstractTypeExtension
    {
        /**
         * 返回被扩展的类型的类。
         */
        public static function getExtendedTypes(): iterable
        {
            // 返回 FormType::class 来修改系统中的（几乎）每个字段
            return array(FileType::class);
        }
    }

你 **必须** 实现的唯一的方法是 ``getExtendedTypes()``，该方法用于配置要修改 *哪个* 字段类型。

.. versionadded:: 4.2
    ``getExtendedTypes()`` 方法是在Symfony 4.2中引入的。

根据你的使用情况，你可能需要重写以下某些方法：

* ``buildForm()``
* ``buildView()``
* ``configureOptions()``
* ``finishView()``

有关这些方法的更多信息，请参阅 :ref:`自定义表单字段类型 <form-type-methods-explanation>`。

注册表单类型扩展为服务
-------------------------------------------------

表单类型扩展必须注册为服务并使用 ``form.type_extension`` 标签进行
:doc:`标记 </service_container/tags>`。如果你使用
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，得益于
:ref:`自动配置 <services-autoconfigure>`，你已经完工了。

.. tip::

    有一个名为 ``priority`` 的默认为 ``0`` 的可选标签属性，它控制加载表单类型扩展的顺序（优先级越高，越早加载该扩展）。
    当你需要保证在另一个扩展之前或之后加载一个扩展时，这就非常有用。
    要使用此属性，你必须显式的添加服务配置。

注册扩展后，只要构建给定类型(``FileType``)的 *任何* 字段，就会调用任何你已重写的方法（例如 ``buildForm()``）。

添加扩展的业务逻辑
-----------------------------------

你的扩展的目标是在文件输入框旁边显示一个漂亮的图像（当底层模型包含图像时）。
为此，假设你使用类似于 :doc:`如何使用Doctrine处理文件上传 </controller/upload_file>`
中描述的方法：你的Media模型有一个表示路径的属性，它对应于数据库中的图像路径::

    // src/Entity/Media.php
    namespace App\Entity;

    use Symfony\Component\Validator\Constraints as Assert;

    class Media
    {
        // ...

        /**
         * @var string 媒体路径 - 通常存储在数据库中
         */
        private $path;

        // ...

        public function getWebPath()
        {
            // ... $webPath 是要在模板中使用的完整图像URL

            return $webPath;
        }
    }

为了继承 ``FileType::class`` 表单类型，你的表单类型扩展类还需要做两件事：

#. 重写 ``configureOptions()`` 方法，以便任何 ``FileType`` 字段都可以有一个
   ``image_property`` 选项;
#. 重写 ``buildView()`` 方法以将图像URL传递给视图。

例如::

    // src/Form/Extension/ImageTypeExtension.php
    namespace App\Form\Extension;

    use Symfony\Component\Form\AbstractTypeExtension;
    use Symfony\Component\Form\FormView;
    use Symfony\Component\Form\FormInterface;
    use Symfony\Component\PropertyAccess\PropertyAccess;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Form\Extension\Core\Type\FileType;

    class ImageTypeExtension extends AbstractTypeExtension
    {
        public static function getExtendedTypes(): iterable
        {
            // 返回 FormType::class 来修改系统中的（几乎）每个字段
            return array(FileType::class);
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            // 使 FileType 字段具有一个合法的 image_property 选项
            $resolver->setDefined(array('image_property'));
        }

        public function buildView(FormView $view, FormInterface $form, array $options)
        {
            if (isset($options['image_property'])) {
                // 这是绑定到你的表单的任何类/实体（例如媒体）
                $parentData = $form->getParent()->getData();

                $imageUrl = null;
                if (null !== $parentData) {
                    $accessor = PropertyAccess::createPropertyAccessor();
                    $imageUrl = $accessor->getValue($parentData, $options['image_property']);
                }

                // 设置一个渲染此字段时可用的 “image_url” 变量
                $view->vars['image_url'] = $imageUrl;
            }
        }

    }

重写File Widget的模板片段
------------------------------------------

每个字段类型都由一个模板片段来渲染。你可以重写这些模板片段以自定义表单渲染。
有关更多信息，请参阅 :ref:`form-customization-form-themes` 文档。

在你的扩展类中，你添加了一个新变量（``image_url``），但仍需要在模板中利用此新变量。
具体来说，你需要重写 ``file_widget`` 区块：

.. code-block:: html+twig

    {# templates/form/fields.html.twig #}
    {% extends 'form_div_layout.html.twig' %}

    {% block file_widget %}
        {% spaceless %}

        {{ block('form_widget') }}
        {% if image_url is not null %}
            <img src="{{ asset(image_url) }}"/>
        {% endif %}

        {% endspaceless %}
    {% endblock %}

请务必 :ref:`配置此表单主题模板 <forms-theming-global>`，以便表单系统能看到它。

使用表单类型扩展
-----------------------------

从现在开始，在向表单添加一个 ``FileType::class`` 类型的字段时，你可以指定一个
``image_property`` 选项，该选项将用于在文件字段旁边显示一个图像。例如::

    // src/Form/Type/MediaType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\Extension\Core\Type\FileType;

    class MediaType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('name', TextType::class)
                ->add('file', FileType::class, array('image_property' => 'webPath'));
        }
    }

显示该表单时，如果底层模型已与一个图像关联，你将会看到它显示在文件输入框旁边。

通用表单类型扩展
----------------------------

你可以通过指定它们的公共父级（:doc:`/reference/forms/types`）来一次修改多个表单类型。
例如，有一些表单类型均继承自 ``TextType`` 表单类型（如 ``EmailType``、``SearchType``、``UrlType`` 等等）。
应用于 ``TextType`` （即，其 ``getExtendedType()`` 方法返回 ``TextType::class``）的一个表单类型扩展将适用于所有这些表单类型。

同样的，由于Symfony中本机可用的 **大多数** 表单类型都从 ``FormType``
表单类型继承，如果一个表单类型扩展应用于 ``FormType``，那么它也就同时在所有这些字段类型上生效（值得注意的例外是
``ButtonType`` 表单类型）。

另外请记住，如果你创建（或正在使用）一个 *自定义* 的表单类型，它可能 *没有* 继承
``FormType``，因此你的表单类型扩展可能不会应用于它。

另一种选择是在 ``getExtendedTypes()`` 方法中返回多个表单类型以扩展这些表单类型::

    // ...
    use Symfony\Component\Form\Extension\Core\Type\DateTimeType;
    use Symfony\Component\Form\Extension\Core\Type\DateType;
    use Symfony\Component\Form\Extension\Core\Type\TimeType;

    class DateTimeExtension extends AbstractTypeExtension
    {
        // ...

        public static function getExtendedTypes(): iterable
        {
            return array(DateTimeType::class, DateType::class, TimeType::class);
        }
    }

.. versionadded:: 4.2
    Symfony 4.2中引入了使用单个扩展类扩展多个表单类型的功能。
