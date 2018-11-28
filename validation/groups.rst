.. index::
    single: Validation; Groups

如何仅应用所有验证约束的子集（验证组）
=================================================================================

默认情况下，在验证对象时，将检查该类的所有约束是否实际通过。
但是，在某些情况下，你需要仅针对该类的 *某些* 约束来验证对象。
为此，你可以将每个约束组织到一个或多个“验证组”中，然后仅针对其中一组约束应用验证。

例如，假设你有一个 ``User`` 类，该类在用户注册时和用户稍后更新其联系信息时使用：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/User.php
        namespace App\Entity;

        use Symfony\Component\Security\Core\User\UserInterface;
        use Symfony\Component\Validator\Constraints as Assert;

        class User implements UserInterface
        {
            /**
             * @Assert\Email(groups={"registration"})
             */
            private $email;

            /**
             * @Assert\NotBlank(groups={"registration"})
             * @Assert\Length(min=7, groups={"registration"})
             */
            private $password;

            /**
             * @Assert\Length(min=2)
             */
            private $city;
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\User:
            properties:
                email:
                    - Email: { groups: [registration] }
                password:
                    - NotBlank: { groups: [registration] }
                    - Length: { min: 7, groups: [registration] }
                city:
                    - Length:
                        min: 2

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="
                http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd
            ">

            <class name="App\Entity\User">
                <property name="email">
                    <constraint name="Email">
                        <option name="groups">
                            <value>registration</value>
                        </option>
                    </constraint>
                </property>

                <property name="password">
                    <constraint name="NotBlank">
                        <option name="groups">
                            <value>registration</value>
                        </option>
                    </constraint>
                    <constraint name="Length">
                        <option name="min">7</option>
                        <option name="groups">
                            <value>registration</value>
                        </option>
                    </constraint>
                </property>

                <property name="city">
                    <constraint name="Length">
                        <option name="min">7</option>
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
                $metadata->addPropertyConstraint('email', new Assert\Email(array(
                    'groups' => array('registration'),
                )));

                $metadata->addPropertyConstraint('password', new Assert\NotBlank(array(
                    'groups' => array('registration'),
                )));
                $metadata->addPropertyConstraint('password', new Assert\Length(array(
                    'min'    => 7,
                    'groups' => array('registration'),
                )));

                $metadata->addPropertyConstraint('city', new Assert\Length(array(
                    "min" => 3,
                )));
            }
        }

使用此配置，将会有三个验证组：

``Default``
    包含当前类和所有引用类的不属于任何其他组的约束。

``User``
    相当于 ``Default`` 组中的 ``User`` 对象的所有约束。它始终是类的名称。
    这与 ``Default`` 之间的区别在 :doc:`/validation/sequence_provider` 中进行了解释。

``registration``
    仅包含 ``email`` 和 ``password`` 字段的约束。

一个类的 ``Default`` 组中的约束包括没有显式的配置为组的约束，以及配置为等于类名称或是 ``Default`` 字符串的组。

.. caution::

    *仅* 验证User对象时，``Default`` 组和 ``User`` 组之间没有区别。
    但是如果 ``User`` 嵌入了对象，则会有所不同 。
    例如，``User`` 有一个包含某个 ``Address`` 对象的 ``address`` 属性，并且你已将
    :doc:`/reference/constraints/Valid` 约束添加到此属性，以便在验证 ``User`` 对象时对其进行验证。

    如果你使用 ``Default`` 组来验证 ``User`` 类，那么任何属于
    ``Address`` 类的 ``Default`` 组的约束 *将* 被使用。
    但是，如果 ``User`` 类使用 ``User`` 验证组进行验证，则仅验证
    ``Address`` 类的 ``User`` 组的约束。

    换句话说，``Default`` 组和类名称组（例如
    ``User``）是相同的，除非该类嵌入在另一个实际上将被验证的对象中。

    如果你有继承（例如 ``User extends BaseUser``）并且使用子类的类名（即
    ``User``）进行验证，则将验证 ``User`` 和 ``BaseUser`` 中的所有约束。
    但是，如果使用基类（即 ``BaseUser``）进行验证，则只验证 ``BaseUser`` 类中的默认约束。

要告知验证器使用一个特定组，请传递一个或多个组的名称到 ``validate()`` 方法的第三个参数::

    $errors = $validator->validate($author, null, array('registration'));

如果未指定任何组，则将应用属于 ``Default`` 组的所有约束。

在全栈的Symfony项目中，你通常会通过表单库来间接进行验证。
有关如何在表单内使用验证组的信息，请参阅 :doc:`/form/validation_groups`。
