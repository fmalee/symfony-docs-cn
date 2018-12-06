.. index::
    single: Forms; Custom Type Guesser

创建自定义类型猜测器
==============================

Form组件可以使用类型猜测器猜测表单字段的类型和一些选项。
该组件已经内置一个使用Validation组件的断言的类型猜测器，但你也可以添加自己的自定义类型猜测器。

.. sidebar:: 桥接中的表单类型猜测器

    Symfony还在桥接中提供了一些表单类型猜测器：

    * :class:`Symfony\\Bridge\\Propel1\\Form\\PropelTypeGuesser` 由Propel1桥接提供;
    * :class:`Symfony\\Bridge\\Doctrine\\Form\\DoctrineOrmTypeGuesser` 由Doctrine桥接提供.

创建一个PHPDoc类型猜测器
----------------------------

在本节中，你将构建一个猜测器，它从属性的PHPDoc中读取有关字段的信息。
首先，你需要创建一个实现 :class:`Symfony\\Component\\Form\\FormTypeGuesserInterface`
的类。此接口需要四种方法：

:method:`Symfony\\Component\\Form\\FormTypeGuesserInterface::guessType`
    试图猜测一个字段的类型;
:method:`Symfony\\Component\\Form\\FormTypeGuesserInterface::guessRequired`
    试图猜测 :ref:`required <reference-form-option-required>` 选项的值;
:method:`Symfony\\Component\\Form\\FormTypeGuesserInterface::guessMaxLength`
    试图猜测 ``maxlength`` 输入属性的值;
:method:`Symfony\\Component\\Form\\FormTypeGuesserInterface::guessPattern`
    试图猜测 ``pattern`` 输入属性的值。

首先创建该类和这些方法。接下来，你将学习如何填写每个方法::

    // src/Form/TypeGuesser/PHPDocTypeGuesser.php
    namespace App\Form\TypeGuesser;

    use Symfony\Component\Form\FormTypeGuesserInterface;

    class PHPDocTypeGuesser implements FormTypeGuesserInterface
    {
        public function guessType($class, $property)
        {
        }

        public function guessRequired($class, $property)
        {
        }

        public function guessMaxLength($class, $property)
        {
        }

        public function guessPattern($class, $property)
        {
        }
    }

猜测类型
~~~~~~~~~~~~~~~~~

在猜测类型时，该方法返回一个 :class:`Symfony\\Component\\Form\\Guess\\TypeGuess`
实例或什么都没有，以确定该类型猜测器无法猜测对应的类型。

``TypeGuess`` 构造函数需要三个选项：

* 类型名称（:doc:`表单类型 </reference/forms/types>` 之一）;
* 其他选项（例如，当类型是 ``entity`` 时，你还要设置 ``class`` 选项）。
  如果没有猜到对应类型，则应将其设置为一个空数组;
* 猜测出来的类型的正确程度。它可以是
  :class:`Symfony\\Component\\Form\\Guess\\Guess` 类的常量之一：``LOW_CONFIDENCE``、
  ``MEDIUM_CONFIDENCE``、``HIGH_CONFIDENCE``、``VERY_HIGH_CONFIDENCE``。
  在执行完所有类型猜测器之后，使用具有最高可信度的类型。

有了这些知识，你就可以实现 ``PHPDocTypeGuesser`` 的 ``guessType()`` 方法::

    namespace App\Form\TypeGuesser;

    use Symfony\Component\Form\Guess\Guess;
    use Symfony\Component\Form\Guess\TypeGuess;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\Extension\Core\Type\IntegerType;
    use Symfony\Component\Form\Extension\Core\Type\NumberType;
    use Symfony\Component\Form\Extension\Core\Type\CheckboxType;

    class PHPDocTypeGuesser implements FormTypeGuesserInterface
    {
        public function guessType($class, $property)
        {
            $annotations = $this->readPhpDocAnnotations($class, $property);

            if (!isset($annotations['var'])) {
                return; // 如果 @var 注释不可用，则不进行猜测
            }

            // 否则，将基于@var注释获取类型
            switch ($annotations['var']) {
                case 'string':
                    // 类型是文本时，有很高的可信度
                    // 应用 @var string
                    return new TypeGuess(TextType::class, array(), Guess::HIGH_CONFIDENCE);

                case 'int':
                case 'integer':
                    // 整数也可以是一个实体的id或一个复选框（0或1）
                    return new TypeGuess(IntegerType::class, array(), Guess::MEDIUM_CONFIDENCE);

                case 'float':
                case 'double':
                case 'real':
                    return new TypeGuess(NumberType::class, array(), Guess::MEDIUM_CONFIDENCE);

                case 'boolean':
                case 'bool':
                    return new TypeGuess(CheckboxType::class, array(), Guess::HIGH_CONFIDENCE);

                default:
                    // 如果此处是正确类型，则赋予非常低的可信度
                    return new TypeGuess(TextType::class, array(), Guess::LOW_CONFIDENCE);
            }
        }

        protected function readPhpDocAnnotations($class, $property)
        {
            $reflectionProperty = new \ReflectionProperty($class, $property);
            $phpdoc = $reflectionProperty->getDocComment();

            // 将 $phpdoc 解析为一个数组:
            // array('var' => 'string', 'since' => '1.0')
            $phpdocTags = ...;

            return $phpdocTags;
        }

        // ...
    }

这个类型猜测器现在可以猜测一个属性的字段类型了，如果该属性有PHPdoc的话！

猜测字段选项
~~~~~~~~~~~~~~~~~~~~~~

其他三个方法（``guessMaxLength()``、``guessRequired()`` 和
``guessPattern()``）返回一个带有选项值的
:class:`Symfony\\Component\\Form\\Guess\\ValueGuess`
实例。它的构造函数有2个参数：

* 选项的值;
* 猜测出来的值的正确程度（使用 ``Guess`` 类的常量）。

当你认为不应该设置选项的值时，就会被猜测为 ``null``。

.. caution::

    你应该非常小心地使用 ``guessPattern()`` 方法。
    当类型是一个浮点数时，你不能使用它来确定浮点数的最小值或最大值（例如，你希望一个浮点数大于
    ``5``，``4.512313`` 会无效，但是 ``length(4.512314) > length(5)`` 有效，因此模式将成功）。
    在这种情况下，应使用一个 ``MEDIUM_CONFIDENCE`` 并将值设置为 ``null``。

注册类型猜测器
--------------------------

如果你正在使用 :ref:`自动装配 <services-autowire>` 和
:ref:`自动配置 <services-autoconfigure>` ，那么你已经完工了！
Symfony已经知道并正在使用你的表单类型猜测器。

如果你 **不** 使用自动装配和自动配置，请手动注册你的服务并使用 ``form.type_guesser`` 标签进行标记：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Form\TypeGuesser\PHPDocTypeGuesser:
                tags: [form.type_guesser]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Form\TypeGuesser\PHPDocTypeGuesser">
                    <tag name="form.type_guesser"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Form\TypeGuesser\PHPDocTypeGuesser;

        $container->register(PHPDocTypeGuesser::class)
            ->addTag('form.type_guesser')
        ;

.. sidebar:: 在组件中注册类型猜测器

    如果你使用的表单组件独立于你的PHP项目，那么请使用 ``FormFactoryBuilder`` 的
    :method:`Symfony\\Component\\Form\\FormFactoryBuilder::addTypeGuesser` 或
    :method:`Symfony\\Component\\Form\\FormFactoryBuilder::addTypeGuessers`
    来注册新的类型猜测器::

        use Symfony\Component\Form\Forms;
        use Acme\Form\PHPDocTypeGuesser;

        $formFactory = Forms::createFormFactoryBuilder()
            // ...
            ->addTypeGuesser(new PHPDocTypeGuesser())
            ->getFormFactory();

        // ...
