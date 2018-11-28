.. index::
    single: Validation; Group Sequences
    single: Validation; Group Sequence Providers

如何按顺序应用验证组
===========================================

在某些情况下，你希望按步骤验证你的组。为此，你可以使用 ``GroupSequence`` 功能。
在这种情况下，一个对象定义一个组序列，该序列决定应验证的组的顺序。

例如，假设你有一个 ``User``
类，并且只有在所有其他验证通过后才会验证密码是否安全（即密码不能等同于用户名），
这么做是为了避免同时出现多个错误消息。

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/User.php
        namespace App\Entity;

        use Symfony\Component\Security\Core\User\UserInterface;
        use Symfony\Component\Validator\Constraints as Assert;

        /**
         * @Assert\GroupSequence({"User", "Strict"})
         */
        class User implements UserInterface
        {
            /**
             * @Assert\NotBlank
             */
            private $username;

            /**
             * @Assert\NotBlank
             */
            private $password;

            /**
             * @Assert\IsTrue(message="The password cannot match your username", groups={"Strict"})
             */
            public function isPasswordSafe()
            {
                return ($this->username !== $this->password);
            }
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\User:
            group_sequence:
                - User
                - Strict
            getters:
                passwordSafe:
                    - 'IsTrue':
                        message: 'The password cannot match your username'
                        groups: [Strict]
            properties:
                username:
                    - NotBlank: ~
                password:
                    - NotBlank: ~

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\User">
                <property name="username">
                    <constraint name="NotBlank" />
                </property>

                <property name="password">
                    <constraint name="NotBlank" />
                </property>

                <getter property="passwordSafe">
                    <constraint name="IsTrue">
                        <option name="message">The password cannot match your username</option>
                        <option name="groups">
                            <value>Strict</value>
                        </option>
                    </constraint>
                </getter>

                <group-sequence>
                    <value>User</value>
                    <value>Strict</value>
                </group-sequence>
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
                $metadata->addPropertyConstraint('username', new Assert\NotBlank());
                $metadata->addPropertyConstraint('password', new Assert\NotBlank());

                $metadata->addGetterConstraint('passwordSafe', new Assert\IsTrue(array(
                    'message' => 'The password cannot match your first name',
                    'groups'  => array('Strict'),
                )));

                $metadata->setGroupSequence(array('User', 'Strict'));
            }
        }

在此示例中，它将首先验证 ``User`` 组（等同于 ``Default`` 组）中的所有约束。
仅当该组中的所有约束都有效时，才会验证第二个组，即 ``Strict``。

.. caution::

    正如你在 :doc:`/validation/groups` 中所见到的，``Default`` 组和使用类名称（例如 ``User``）的组是等同的。
    但是当使用组序列时，它们不再等同。``Default`` 组这时将引用(reference)组序列，而不是所有不属于任何组的约束。

    这意味着在指定组序列时必须使用 ``{ClassName}`` （例如 ``User``）组。
    如果使用 ``Default``，你将获得无限的递归（因为
    ``Default`` 组引用了组序列，而该序列又包含引用了相同组序列的 ``Default`` 组，...）。

你还可以在 ``validation_groups`` 表单选项中定义一个组序列::

    use Symfony\Component\Validator\Constraints\GroupSequence;
    use Symfony\Component\Form\AbstractType;
    // ...

    class MyType extends AbstractType
    {
        // ...
        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'validation_groups' => new GroupSequence(['First', 'Second']),
            ]);
        }
    }

组序列提供器
------------------------

想象一下可以是普通用户或高级用户的一个 ``User`` 实体。
当它是高级用户时，应该向用户实体添加一些额外的约束（例如信用卡详细信息）。
要动态的确定应激活哪些验证组，你可以创建一个组序列提供器。
首先，创建实体和一个名为 ``Premium`` 的新约束组：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/User.php
        namespace App\Entity;

        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            /**
             * @Assert\NotBlank
             */
            private $name;

            /**
             * @Assert\CardScheme(
             *     schemes={"VISA"},
             *     groups={"Premium"},
             * )
             */
            private $creditCard;

            // ...
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\User:
            properties:
                name:
                    - NotBlank: ~
                creditCard:
                    - CardScheme:
                        schemes: [VISA]
                        groups: [Premium]

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\User">
                <property name="name">
                    <constraint name="NotBlank" />
                </property>

                <property name="creditCard">
                    <constraint name="CardScheme">
                        <option name="schemes">
                            <value>VISA</value>
                        </option>
                        <option name="groups">
                            <value>Premium</value>
                        </option>
                    </constraint>
                </property>

                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/User.php
        namespace App\Entity;

        use Symfony\Component\Validator\Constraints as Assert;
        use Symfony\Component\Validator\Mapping\ClassMetadata;

        class User
        {
            private $name;
            private $creditCard;

            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new Assert\NotBlank());
                $metadata->addPropertyConstraint('creditCard', new Assert\CardScheme(array(
                    'schemes' => array('VISA'),
                    'groups'  => array('Premium'),
                )));
            }
        }

现在，修改 ``User`` 类以实现 :class:`Symfony\\Component\\Validator\\GroupSequenceProviderInterface`
并添加
:method:`Symfony\\Component\\Validator\\GroupSequenceProviderInterface::getGroupSequence`
方法，该方法应返回一个要使用的验证组的数组::

    // src/Entity/User.php
    namespace App\Entity;

    // ...
    use Symfony\Component\Validator\GroupSequenceProviderInterface;

    class User implements GroupSequenceProviderInterface
    {
        // ...

        public function getGroupSequence()
        {
            // 当返回一个简单数组时，如果任何组中存在一个违规，则不再验证其余的组。
            // 例如，如果'User'验证失败，则'Premium'和'Api'不会被验证：
            return array('User', 'Premium', 'Api');

            // 当返回一个嵌套数组时，将验证每个数组中包含的所有组。
            // 例如，如果'User'验证失败，'Premium'还是会被验证（并且你也将会得到它的违规），但'Api'将不会被验证：
            return array(array('User', 'Premium'), 'Api');
        }
    }

最后，你必须告知Validator组件，你的 ``User`` 类提供了一个要验证的组的序列：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/User.php
        namespace App\Entity;

        // ...

        /**
         * @Assert\GroupSequenceProvider
         */
        class User implements GroupSequenceProviderInterface
        {
            // ...
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\User:
            group_sequence_provider: true

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\User">
                <group-sequence-provider />
                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/User.php
        namespace App\Entity;

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;

        class User implements GroupSequenceProviderInterface
        {
            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->setGroupSequenceProvider(true);
                // ...
            }
        }
