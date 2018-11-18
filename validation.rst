.. index::
   single: Validation

验证
==========

验证是Web应用中非常常见的任务。在表单中输入的数据需要进行验证。
在将数据写入数据库或传递给Web服务之前，还需要验证数据。

Symfony提供了一个 `Validator`_ 组件，使该任务变得简单透明。
该组件基于 `JSR303 Bean Validation specification`_。

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令以在使用之前安装验证器：

.. code-block:: terminal

    $ composer require symfony/validator doctrine/annotations

.. index::
   single: Validation; The basics

基础验证
------------------------

理解验证的最佳方法是看它的实际效果。首先，假设你已经创建了一个普通的PHP对象，你需要在应用中的某个位置使用它::

    // src/Entity/Author.php
    namespace App\Entity;

    class Author
    {
        public $name;
    }

到目前为止，这只是一个普通的类，在你的应用中有一些用途。验证的目的是告诉你对象的数据是否有效。
为此，你将配置对象必须遵循的规则列表（称为 :ref:`约束 <validation-constraints>`）才能生效。
这些规则通常使用PHP代码或注释定义，但它们也可以在 ``config/validator/``
目录中定义为 ``validation.yaml`` 或 ``validation.xml`` 文件：

例如，要保证 ``$name`` 属性不为空，请添加以下内容：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank
             */
            public $name;
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\Author:
            properties:
                name:
                    - NotBlank: ~

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\Author">
                <property name="name">
                    <constraint name="NotBlank" />
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;

        class Author
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank());
            }
        }

.. tip::

    还可以验证受保护的属性和私有属性，以及“getter”方法（请参阅 :ref:`validator-constraint-targets`）。

.. index::
   single: Validation; Using the validator

使用 ``validator`` 服务
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

接下来，要实际验证 ``Author`` 对象，请在 ``validator`` 服务
（类 :class:`Symfony\\Component\\Validator\\Validator`）上使用 ``validate()`` 方法。
``validator`` 的工作就是读取类的约束（即规则）并验证对象上的数据是否满足这些约束。
如果验证失败，则返回非空的错误列表（类 :class:`Symfony\\Component\\Validator\\ConstraintViolationList`）。
从控制器内部获取这个简单示例::

    // ...
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Validator\Validator\ValidatorInterface;
    use App\Entity\Author;

    // ...
    public function author(ValidatorInterface $validator)
    {
        $author = new Author();

        // ... 对 $author 对象执行某些操作

        $errors = $validator->validate($author);

        if (count($errors) > 0) {
            /*
             * 在 $errors 变量上使用 __toString 方法，该变量是 ConstraintViolationList 对象。
             * 这为我们提供了一个很好的调试字符串。
             */
            $errorsString = (string) $errors;

            return new Response($errorsString);
        }

        return new Response('The author is valid! Yes!');
    }

如果 ``$name`` 属性为空，你将看到以下错误消息：

.. code-block:: text

    App\Entity\Author.name:
        This value should not be blank

如果在 ``name`` 属性中插入值，则会显示愉快的成功消息。

.. tip::

    大多数情况下，你不会直接与 ``validator`` 服务交互，也不必担心打印出错误。
    大多数情况下，你在处理提交的表单数据时会间接使用验证。
    有关更多信息，请参阅 :ref:`forms-form-validation`。

你还可以将错误集合传递到模板中::

    if (count($errors) > 0) {
        return $this->render('author/validation.html.twig', array(
            'errors' => $errors,
        ));
    }

在模板内部，你可以根据需要输出错误列表：

.. code-block:: html+twig

    {# templates/author/validation.html.twig #}
    <h3>The author has the following errors</h3>
    <ul>
    {% for error in errors %}
        <li>{{ error.message }}</li>
    {% endfor %}
    </ul>

.. note::

    每个验证错误（称为“约束违规”）由 :class:`Symfony\\Component\\Validator\\ConstraintViolation` 对象表示。

.. index::
   pair: Validation; Configuration

配置
-------------

在使用Symfony验证器之前，请确保在主配置文件中启用它：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            validation: { enabled: true }

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:validation enabled="true" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            'validation' => array(
                'enabled' => true,
            ),
        ));

此外，如果你计划使用注释来配置验证，请使用以下内容替换以前的配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            validation: { enable_annotations: true }

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:validation enable-annotations="true" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            'validation' => array(
                'enable_annotations' => true,
            ),
        ));

.. index::
   single: Validation; Constraints

.. _validation-constraints:

约束
-----------

``validator`` 旨在根据 *约束* （即规则）验证对象。
为了验证一个对象，只需将一个或多个约束映射到其类，然后将其传递给 ``validator`` 服务。

在幕后，约束只是一个PHP对象，它产生一个断言语句。在现实生活中，一个约束可能是：'蛋糕不能被烧掉'。
在Symfony中，约束类似：它们是条件为真的断言。给定一个值，约束将告诉你该值是否符合约束规则。

支持的约束
~~~~~~~~~~~~~~~~~~~~~

Symfony封装了很多最常用的约束：

.. include:: /reference/constraints/map.rst.inc

您还可以创建自己的自定义约束。:doc:`/validation/custom_constraint` 文档中介绍了此主题。

.. index::
   single: Validation; Constraints configuration

.. _validation-constraint-configuration:

约束配置
~~~~~~~~~~~~~~~~~~~~~~~~

一些约束（如 :doc:`NotBlank </reference/constraints/NotBlank>`）很简单，而其他约束
（如 :doc:`Choice </reference/constraints/Choice>` 约束）有几个可用的配置选项。
假设 ``Author`` 类有另一个名为 ``genre`` 的属性，它定义了与作者相关的主要文学类型，可以设置为“小说”或“非小说”：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice(
             *     choices = { "fiction", "non-fiction" },
             *     message = "Choose a valid genre."
             * )
             */
            public $genre;

            // ...
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\Author:
            properties:
                genre:
                    - Choice: { choices: [fiction, non-fiction], message: Choose a valid genre. }
                # ...

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\Author">
                <property name="genre">
                    <constraint name="Choice">
                        <option name="choices">
                            <value>fiction</value>
                            <value>non-fiction</value>
                        </option>
                        <option name="message">Choose a valid genre.</option>
                    </constraint>
                </property>

                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            public $genre;

            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                // ...

                $metadata->addPropertyConstraint('genre', new Assert\Choice(array(
                    'choices' => array('fiction', 'non-fiction'),
                    'message' => 'Choose a valid genre.',
                )));
            }
        }

.. _validation-default-option:

约束的选项始终可以作为数组传递。但是，某些约束还允许你传递一个 “*default*” 选项的值来代替数组。
在本例中的 ``Choice`` 约束， ``choices`` 可以以这种方式指定：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice({"fiction", "non-fiction"})
             */
            protected $genre;

            // ...
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\Author:
            properties:
                genre:
                    - Choice: [fiction, non-fiction]
                # ...

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\Author">
                <property name="genre">
                    <constraint name="Choice">
                        <value>fiction</value>
                        <value>non-fiction</value>
                    </constraint>
                </property>

                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            protected $genre;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                // ...

                $metadata->addPropertyConstraint(
                    'genre',
                    new Assert\Choice(array('fiction', 'non-fiction'))
                );
            }
        }

这纯粹是为了使约束的最常见选项的配置更短更快。

如果你不确定如何指定选项，请检查约束的 :namespace:`Symfony\\Component\\Validator\\Constraints`
或通过传入一个选项数组（上面显示的第一种方法）来安全地使用它。

表单类中的约束
---------------------------

通过表单字段的 ``constraints`` 选项构建表单时可以定义约束::

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder
            ->add('myField', TextType::class, array(
                'required' => true,
                'constraints' => array(new Length(array('min' => 3)))
            ))
        ;
    }

.. index::
   single: Validation; Constraint targets

.. _validator-constraint-targets:

约束目标
------------------

约束可以应用于类属性（例如 ``name``）、公共getter方法（例如 ``getFullName()``）或整个类。
属性约束是最常见且易于使用的。Getter约束允许你指定更复杂的验证规则。最后，类约束适用于要将类作为整体进行验证的场景。

.. index::
   single: Validation; Property constraints

.. _validation-property-target:

属性
~~~~~~~~~~

验证类属性是最基本的验证技术。Symfony允许你验证私有，受保护或公共属性。
下一个清单显示如何将 ``Author`` 类的 ``$firstName`` 属性配置为至少包含3个字符。

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank
             * @Assert\Length(min=3)
             */
            private $firstName;
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\Author:
            properties:
                firstName:
                    - NotBlank: ~
                    - Length:
                        min: 3

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\Author">
                <property name="firstName">
                    <constraint name="NotBlank" />
                    <constraint name="Length">
                        <option name="min">3</option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            private $firstName;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('firstName', new Assert\NotBlank());
                $metadata->addPropertyConstraint(
                    'firstName',
                    new Assert\Length(array("min" => 3))
                );
            }
        }

.. index::
   single: Validation; Getter constraints

Getter
~~~~~~~

约束也可以应用于一个方法的返回值。Symfony允许你向名称以“get”，“is”或“has”开头的任何公有方法添加约束。
在本指南中，这些类型的方法称为“getters”。

这种技术的好处是它允许你动态验证你的对象。例如，假设你要确保密码字段与用户的名字不匹配（出于安全原因）。
你可以通过创建 ``isPasswordSafe()`` 方法，然后断言此方法必须返回 ``true`` 来执行此操作：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\IsTrue(message="The password cannot match your first name")
             */
            public function isPasswordSafe()
            {
                // ... 返回 true 或 false
            }
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\Author:
            getters:
                passwordSafe:
                    - 'IsTrue': { message: 'The password cannot match your first name' }

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\Author">
                <getter property="passwordSafe">
                    <constraint name="IsTrue">
                        <option name="message">The password cannot match your first name</option>
                    </constraint>
                </getter>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addGetterConstraint('passwordSafe', new Assert\IsTrue(array(
                    'message' => 'The password cannot match your first name',
                )));
            }
        }

现在，创建 ``isPasswordSafe()`` 方法并包含你需要的逻辑::

    public function isPasswordSafe()
    {
        return $this->firstName !== $this->password;
    }

.. note::

    你们中的敏锐眼睛会注意到在YAML，XML和PHP格式的映射中省略了getter的前缀（“get”，“is”或“has”）。
    这允许你稍后将约束移动到具有相同名称的属性上（反之亦然），而无需更改验证逻辑。

.. _validation-class-target:

类
~~~~~~~

某些约束适用于要验证的整个类。例如，:doc:`Callback </reference/constraints/Callback>`
约束是一个应用于类本身的泛型约束。验证该类时，将简单地执行该约束指定的方法，以便每个方法都可以提供更多的自定义验证。

总结
--------------

Symfony ``validator`` 是一个功能强大的工具，可用于保证任何对象的数据“有效”。
验证背后的力量在于“约束”，这些规则可以应用于对象的属性或getter方法。
虽然你在使用表单时最常间接使用验证框架，但请记住，它可以用于在任何地方验证任何对象。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /validation/*

.. _Validator: https://github.com/symfony/validator
.. _JSR303 Bean Validation specification: http://jcp.org/en/jsr/detail?id=303
