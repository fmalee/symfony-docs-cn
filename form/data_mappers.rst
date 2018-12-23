.. index::
    single: Form; Data mappers

何时以及如何使用数据映射器
================================

当一个表单是复合型(compound)时，需要将初始数据传递给子表单，以便每个表单都可以显示自己的输入值。在提交时，需要将子值写回表单中。

数据映射器负责从父表单读取和写入数据。

主内置数据映射器使用 :doc:`PropertyAccess组件 </components/property_access>`
组件，并且适合大多数案例。但是，你可以创建自己的实现，例如，可以通过其构造函数将提交的数据传递给不可变对象。

数据转换器和映射器之间的区别
----------------------------------------------------

了解 :doc:`数据转换器 </form/data_transformers>` 和映射器之间的区别非常重要。

* **数据转换器** 改变了一个值的表示（例如从 ``"2016-08-12"`` 转换到一个 ``DateTime`` 实例）;
* **数据映射器** 则将数据（例如对象或数组）映射到表单字段，反之亦然。

将一个 ``YYYY-mm-dd`` 字符串值更改为一个 ``DateTime`` 实例是由数据转换器完成的。
而使用一个 ``DateTime`` 实例填充一个复合日期类型的内部字段（例如年、小时等）则由数据映射器完成。

创建数据映射器
----------------------

假设你要将一组颜色保存到数据库中。为此，你使用了一个不可变颜色对象::

    // src/App/Painting/Color.php
    namespace App\Painting;

    final class Color
    {
        private $red;
        private $green;
        private $blue;

        public function __construct(int $red, int $green, int $blue)
        {
            $this->red = $red;
            $this->green = $green;
            $this->blue = $blue;
        }

        public function getRed(): int
        {
            return $this->red;
        }

        public function getGreen(): int
        {
            return $this->green;
        }

        public function getBlue(): int
        {
            return $this->blue;
        }
    }

表单类型应被允许编辑一个颜色。但是因为你决定使 ``Color``
对象不可变，所以每次更改其中一个值时都必须创建一个新的颜色对象。

.. tip::

    如果你正在使用一个带有构造函数参数的可变对象，而不是使用数据映射器，则应该如
    :ref:`如何为表单类配置空数据 <forms-empty-data-closure>`
    中所述的那样，使用一个闭包来配置 ``empty_data`` 选项。

必须将红色、绿色和蓝色表单字段映射到构造函数参数，同时 ``Color`` 实例必须映射到红色、绿色和蓝色表单字段。
认识一个熟悉的模式？现在是数据映射器的时间！
form fields. Recognize a familiar pattern? It's time for a data mapper!

.. code-block:: php

    // src/App/Form/DataMapper/ColorMapper.php
    namespace App\Form\DataMapper;

    use App\Painting\Color;
    use Symfony\Component\Form\DataMapperInterface;
    use Symfony\Component\Form\Exception\UnexpectedTypeException;
    use Symfony\Component\Form\FormInterface;

    final class ColorMapper implements DataMapperInterface
    {
        /**
         * @param Color|null $data
         */
        public function mapDataToForms($data, $forms)
        {
            // 还没有数据，所以没有任何数据可以预先填充
            if (null === $data) {
                return;
            }

            // 无效的数据类型
            if (!$data instanceof Color) {
                throw new UnexpectedTypeException($data, Color::class);
            }

            /** @var FormInterface[] $forms */
            $forms = iterator_to_array($forms);

            // 初始化表单字段值
            $forms['red']->setData($data->getRed());
            $forms['green']->setData($data->getGreen());
            $forms['blue']->setData($data->getBlue());
        }

        public function mapFormsToData($forms, &$data)
        {
            /** @var FormInterface[] $forms */
            $forms = iterator_to_array($forms);

            // 因为数据是通过引用传递的，所以重写它也会在表单对象中同时更改。
            // 请注意不一致的类型，请参阅下面的注意事项。
            $data = new Color(
                $forms['red']->getData(),
                $forms['green']->getData(),
                $forms['blue']->getData()
            );
        }
    }

.. caution::

    传递给映射器的数据 *尚未验证*。这意味着你的对象应允许以一个无效状态来创建，以便在表单中生成对用户友好的错误。

使用映射器
----------------

你已准备好为 ``ColorType`` 表单使用数据映射器。可以使用
:method:`Symfony\\Component\\Form\\FormConfigBuilderInterface::setDataMapper`
方法来配置数据映射器::

    // src/App/Form/Type/ColorType.php
    namespace App\Form\Type;

    use App\Form\DataMapper\ColorMapper;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\IntegerType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    final class ColorType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('red', IntegerType::class, array(
                    // 强制类型的严格性以确保 Color 类的构造函数不会中断
                    'empty_data' => '0',
                ))
                ->add('green', IntegerType::class, array(
                    'empty_data' => '0',
                ))
                ->add('blue', IntegerType::class, array(
                    'empty_data' => '0',
                ))
                ->setDataMapper(new ColorMapper())
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            // 在创建一个新颜色时，初始数据应为 null
            $resolver->setDefault('empty_data', null);
        }
    }

酷！现在使用 ``ColorType`` 表单时，自定义的 ``ColorMapper`` 将立即创建一个新的 ``Color`` 对象。

.. caution::

    当一个表单拥有一个设置为 ``true`` 的 ``inherit_data`` 选项时，它将不使用数据映射器并让其父表单来映射内部值。

.. tip::

    你还可以在 ``ColorType`` 中实现 ``DataMapperInterface``，并直接在表单类型中添加
    ``mapDataToForms()`` 和 ``mapFormsToData()``，以避免创建一个新类。
    然后你就必须调用 ``$builder->setDataMapper($this)``。
