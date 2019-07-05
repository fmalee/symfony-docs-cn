.. index::
   single: Validation; Custom constraints

如何创建自定义验证约束
============================================

你可以通过继承 :class:`Symfony\\Component\\Validator\\Constraint`
基本约束类来创建自定义约束。
作为示例，你将创建一个简单的验证器，用于检查一个字符串是否仅包含字母、数字(alphanumeric)字符。

创建约束类
-----------------------------

首先，你需要创建一个约束类并继承 :class:`Symfony\\Component\\Validator\\Constraint`::

    // src/Validator/Constraints/ContainsAlphanumeric.php
    namespace App\Validator\Constraints;

    use Symfony\Component\Validator\Constraint;

    /**
     * @Annotation
     */
    class ContainsAlphanumeric extends Constraint
    {
        public $message = 'The string "{{ string }}" contains an illegal character: it can only contain letters or numbers.';
    }

.. note::

    ``@Annotation`` 注释是必需的，这是为了使这个新的约束能通过注释的方式在类中使用。
    约束的选项在约束类中使用公共属性来表示。

创建验证器本身
-----------------------------

如你所见，该约束类相当小。因为实际的验证由另一个“约束验证器”类来执行。
约束验证器类通过该约束的 ``validatedBy()`` 方法指定，该方法包括一些简单的默认逻辑::

    // 在 Symfony\Component\Validator\Constraint 基类内部
    public function validatedBy()
    {
        return \get_class($this).'Validator';
    }

换句话说，如果你创建了一个自定义 ``Constraint``（例如
``MyConstraint``），Symfony将在实际执行验证时自动查找另一个类，如 ``MyConstraintValidator``。

验证器类也很简单，只有一个必需的 ``validate()`` 方法::

    // src/Validator/Constraints/ContainsAlphanumericValidator.php
    namespace App\Validator\Constraints;

    use Symfony\Component\Validator\Constraint;
    use Symfony\Component\Validator\ConstraintValidator;
    use Symfony\Component\Validator\Exception\UnexpectedTypeException;
    use Symfony\Component\Validator\Exception\UnexpectedValueException;

    class ContainsAlphanumericValidator extends ConstraintValidator
    {
        public function validate($value, Constraint $constraint)
        {
            if (!$constraint instanceof ContainsAlphanumeric) {
                throw new UnexpectedTypeException($constraint, ContainsAlphanumeric::class);
            }

            // 自定义约束应该忽略null和空值，以允许其他约束（NotBlank、NotNull等）处理
            if (null === $value || '' === $value) {
                return;
            }

            if (!is_string($value)) {
                // 如果你的验证器无法处理传递的类型，则可以抛出此异常以将其标记为无效
                throw new UnexpectedValueException($value, 'string');

                // 使用管道分隔多个类型
                // throw new UnexpectedValueException($value, 'string|int');
            }

            if (!preg_match('/^[a-zA-Z0-9]+$/', $value, $matches)) {
                $this->context->buildViolation($constraint->message)
                    ->setParameter('{{ string }}', $value)
                    ->addViolation();
            }
        }
    }

在 ``validate`` 内部，你不需要返回一个值。
但是，你需要将违规信息添加到验证器的 ``context`` 属性中，如果一个值未导致违规，则该值被视为有效。
``buildViolation()`` 方法将错误消息作为其参数并返回一个
:class:`Symfony\\Component\\Validator\\Violation\\ConstraintViolationBuilderInterface`
实例。最后调用 ``addViolation()`` 方法增加了违规的上下文。

使用新的验证器
-----------------------

你可以像使用Symfony自身提供的验证器那样使用自定义验证器：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/AcmeEntity.php
        use App\Validator\Constraints as AcmeAssert;
        use Symfony\Component\Validator\Constraints as Assert;

        class AcmeEntity
        {
            // ...

            /**
             * @Assert\NotBlank
             * @AcmeAssert\ContainsAlphanumeric
             */
            protected $name;

            // ...
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\AcmeEntity:
            properties:
                name:
                    - NotBlank: ~
                    - App\Validator\Constraints\ContainsAlphanumeric: ~

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping https://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\AcmeEntity">
                <property name="name">
                    <constraint name="NotBlank"/>
                    <constraint name="App\Validator\Constraints\ContainsAlphanumeric"/>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/AcmeEntity.php
        use App\Validator\Constraints\ContainsAlphanumeric;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Mapping\ClassMetadata;

        class AcmeEntity
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank());
                $metadata->addPropertyConstraint('name', new ContainsAlphanumeric());
            }
        }

如果你的约束包含选项，则它们应该是你之前创建的自定义约束类的公共属性。
这些选项可以像核心Symfony约束的选项那样配置。

使用依赖的约束验证器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你使用的是 :ref:`默认的services.yaml配置 <service-container-services-load-example>`
，那么你的验证器已经注册为服务并 :doc:`标记 </service_container/tags>`
为必要的 ``validator.constraint_validator``。这意味着你可以像任何其他服务一样
:ref:`注入服务或配置 <services-constructor-injection>`。

类约束验证器
~~~~~~~~~~~~~~~~~~~~~~~~~~

除了验证单个属性外，约束还可以将整个类作为其范围（scope）。你只需要将其添加到 ``Constraint`` 类中::

    public function getTargets()
    {
        return self::CLASS_CONSTRAINT;
    }

对应于此，验证器的 ``validate()`` 方法将获取一个对象作为其第一个参数::

    class ProtocolClassValidator extends ConstraintValidator
    {
        public function validate($protocol, Constraint $constraint)
        {
            if ($protocol->getFoo() != $protocol->getBar()) {
                $this->context->buildViolation($constraint->message)
                    ->atPath('foo')
                    ->addViolation();
            }
        }
    }

.. tip::

    ``atPath()`` 方法定义了与验证错误相关联的属性。
    可以使用任何 :doc:`有效的PropertyAccess语法 </components/property_access>` 来定义该属性。

一个类约束验证器应该用于类本身，而不是属性：

.. configuration-block::

    .. code-block:: php-annotations

        /**
         * @AcmeAssert\ProtocolClass
         */
        class AcmeEntity
        {
            // ...
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\AcmeEntity:
            constraints:
                - App\Validator\Constraints\ProtocolClass: ~

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <class name="App\Entity\AcmeEntity">
            <constraint name="App\Validator\Constraints\ProtocolClass"/>
        </class>
