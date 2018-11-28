.. index::
    single: Validation; Error Levels
    single: Validation; Payload

如何处理不同的错误级别
====================================

有时，你可能希望根据某些规则以不同的方式来显示约束的验证错误消息。
例如，你有一个新用户的注册表，他们输入一些个人信息并选择他们的认证凭据。
他们必须选择一个用户名和一个安全密码，但提供银行帐户信息是可选的。
尽管如此，你仍希望确保这些可选字段（如果已输入的话）仍然有效，但以不同的方式显示其错误。


实现此行为的过程包括两个步骤：

#. 将不同的错误级别应用于验证约束;
#. 根据配置的错误级别来自定义错误消息。

1. 分配错误级别
----------------------------

使用 ``payload`` 选项为每个约束配置错误级别：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/User.php
        namespace App\Entity;

        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            /**
             * @Assert\NotBlank(payload={"severity"="error"})
             */
            protected $username;

            /**
             * @Assert\NotBlank(payload={"severity"="error"})
             */
            protected $password;

            /**
             * @Assert\Iban(payload={"severity"="warning"})
             */
            protected $bankAccountNumber;
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\User:
            properties:
                username:
                    - NotBlank:
                        payload:
                            severity: error
                password:
                    - NotBlank:
                        payload:
                            severity: error
                bankAccountNumber:
                    - Iban:
                        payload:
                            severity: warning

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\User">
                <property name="username">
                    <constraint name="NotBlank">
                        <option name="payload">
                            <value key="severity">error</value>
                        </option>
                    </constraint>
                </property>
                <property name="password">
                    <constraint name="NotBlank">
                        <option name="payload">
                            <value key="severity">error</value>
                        </option>
                    </constraint>
                </property>
                <property name="bankAccountNumber">
                    <constraint name="Iban">
                        <option name="payload">
                            <value key="severity">warning</value>
                        </option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/User.php
        namespace App\Entity;

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('username', new Assert\NotBlank(array(
                    'payload' => array('severity' => 'error'),
                )));
                $metadata->addPropertyConstraint('password', new Assert\NotBlank(array(
                    'payload' => array('severity' => 'error'),
                )));
                $metadata->addPropertyConstraint('bankAccountNumber', new Assert\Iban(array(
                    'payload' => array('severity' => 'warning'),
                )));
            }
        }

2. 自定义错误消息模板
---------------------------------------

当验证 ``User`` 对象失败时，你可以使用
:method:`Symfony\\Component\\Validator\\ConstraintViolation::getConstraint`
方法来检索导致一个特定失败的约束。每个约束都将附加的 ``payload`` 暴露为一个公共属性::

    // 一个验证失败约束
    // Symfony\Component\Validator\ConstraintViolation 的实例
    $constraintViolation = ...;
    $constraint = $constraintViolation->getConstraint();
    $severity = isset($constraint->payload['severity']) ? $constraint->payload['severity'] : null;

例如，你可以利用此选项来自定义 ``form_errors`` 区块，以便将 ``severity`` 值添加为一个额外的HTML类：

.. code-block:: html+jinja

    {%- block form_errors -%}
        {%- if errors|length > 0 -%}
        <ul>
            {%- for error in errors -%}
                <li class="{{ error.cause.constraint.payload.severity ?? '' }}">{{ error.message }}</li>
            {%- endfor -%}
        </ul>
        {%- endif -%}
    {%- endblock form_errors -%}

.. seealso::

    有关自定义表单渲染的详细信息，请参阅 :doc:`/form/form_customization`。
