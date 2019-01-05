.. index::
   single: Serializer
   single: Components; Serializer

Serializer组件
========================

    Serializer组件用于将对象转换为特定格式（XML，JSON，YAML，...），反之亦然。

为此，Serializer组件遵循以下模式。

.. image:: /_images/components/serializer/serializer_workflow.png

如上图所示，一个数组被当做对象和序列化内容之间的中介。
在这中间，编码器只处理特定 **格式** 到 **数组** 的转换，反之亦然。
同样的，Normalizer只处理特定 **对象** 到 **数组** 的转换，反之亦然。

序列化是一个复杂的主题。
此组件可能无法完全覆盖你的所有用例，但它对于开发用于序列化和反序列化你的对象的工具非常有用。

安装
------------

.. code-block:: terminal

    $ composer require symfony/serializer

或者，你可以克隆 `<https://github.com/symfony/serializer>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

如果要使用 ``ObjectNormalizer``， 还必须安装 :doc:`PropertyAccess组件 </components/property_access>`。

用法
-----

.. seealso::

    本文介绍如何在任何PHP应用中将Serializer功能用作独立组件。
    阅读 :doc:`/serializer` 文档，以了解如何在创建Symfony测试时使用它。

要使用Serializer组件，请设置
:class:`Symfony\\Component\\Serializer\\Serializer` 来指定启用哪些编码器和normalizer::

    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Encoder\XmlEncoder;
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    $encoders = array(new XmlEncoder(), new JsonEncoder());
    $normalizers = array(new ObjectNormalizer());

    $serializer = new Serializer($normalizers, $encoders);

首选的normalizer是
:class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer`，但也可以使用其他normalizer。
以下所示的所有示例都使用了 ``ObjectNormalizer``。

序列化对象
---------------------

为了这个例子，假设你的项目中已经存在以下类::

    namespace App\Model;

    class Person
    {
        private $age;
        private $name;
        private $sportsperson;
        private $createdAt;

        // Getters
        public function getName()
        {
            return $this->name;
        }

        public function getAge()
        {
            return $this->age;
        }

        public function getCreatedAt()
        {
            return $this->createdAt;
        }

        // Issers
        public function isSportsperson()
        {
            return $this->sportsperson;
        }

        // Setters
        public function setName($name)
        {
            $this->name = $name;
        }

        public function setAge($age)
        {
            $this->age = $age;
        }

        public function setSportsperson($sportsperson)
        {
            $this->sportsperson = $sportsperson;
        }

        public function setCreatedAt($createdAt)
        {
            $this->createdAt = $createdAt;
        }
    }

现在，如果要将此对象序列化为JSON，则只需使用之前创建的序列化器服务::

    $person = new App\Model\Person();
    $person->setName('foo');
    $person->setAge(99);
    $person->setSportsperson(false);

    $jsonContent = $serializer->serialize($person, 'json');

    // $jsonContent 将包含 {"name":"foo","age":99,"sportsperson":false,"createdAt":null}

    echo $jsonContent; // 或者在一个响应中返回它

在这个例子中，:method:`Symfony\\Component\\Serializer\\Serializer::serialize`
方法的第一个参数是要序列化的对象，第二个参数用于选择合适的编码器，此例使用的是
:class:`Symfony\\Component\\Serializer\\Encoder\\JsonEncoder`。

反序列化对象
-----------------------

你现在将学习如何进行反向操作。这次，``Person`` 类的信息将以XML格式编码::

    use App\Model\Person;

    $data = <<<EOF
    <person>
        <name>foo</name>
        <age>99</age>
        <sportsperson>false</sportsperson>
    </person>
    EOF;

    $person = $serializer->deserialize($data, Person::class, 'xml');

在这个例子中，:method:`Symfony\\Component\\Serializer\\Serializer::deserialize`
方法需要三个参数：

#. 要解码的信息
#. The name of the class this information will be decoded to此信息将被解码为的类的名称
#. 用于将该信息转换为数组的编码器

.. versionadded:: 3.3
    Symfony 3.3中引入了对上下文中的 ``allow_extra_attributes`` 键的支持。

默认情况下，序列化器组件将忽略未映射到denormalized对象的其他属性。
如果你希望在发生这种情况时抛出异常，请将 ``allow_extra_attributes`` 选项设置为
``false``，并在构造normalizer时提供一个实现 ``ClassMetadataFactoryInterface`` 的对象::

    $data = <<<EOF
    <person>
        <name>foo</name>
        <age>99</age>
        <city>Paris</city>
    </person>
    EOF;

    // 这里会抛出一个 Symfony\Component\Serializer\Exception\ExtraAttributesException
    // 因为 "city" 不是 Person 了的一个属性
    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));
    $normalizer = new ObjectNormalizer($classMetadataFactory);
    $serializer = new Serializer(array($normalizer));
    $person = $serializer->deserialize($data, 'Acme\Person', 'xml', array(
        'allow_extra_attributes' => false,
    ));

在现有对象中反序列化
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

序列化器还可用于更新现有对象::

    // ...
    $person = new Person();
    $person->setName('bar');
    $person->setAge(99);
    $person->setSportsperson(true);

    $data = <<<EOF
    <person>
        <name>foo</name>
        <age>69</age>
    </person>
    EOF;

    $serializer->deserialize($data, Person::class, 'xml', array('object_to_populate' => $person));
    // $person = App\Model\Person(name: 'foo', age: '69', sportsperson: true)

在使用一个ORM时，这是一个常见的需求。

.. _component-serializer-attributes-groups:

属性组
-----------------

有时，你希望从你的实体中序列化不同的属性集。组是实现这一需求的便捷方式。
Sometimes, you want to serialize different sets of attributes from your
entities. Groups are a handy way to achieve this need.

假设你有以下普通的PHP对象::

    namespace Acme;

    class MyObj
    {
        public $foo;

        private $bar;

        public function getBar()
        {
            return $this->bar;
        }

        public function setBar($bar)
        {
            return $this->bar = $bar;
        }
    }

可以使用注释、XML或YAML来指定序列化的定义。而normalizer使用的
:class:`Symfony\\Component\\Serializer\\Mapping\\Factory\\ClassMetadataFactory`
必须知道将要使用的格式。
The definition of serialization can be specified using annotations, XML
or YAML.
The :class:`Symfony\\Component\\Serializer\\Mapping\\Factory\\ClassMetadataFactory` that will be used by the normalizer must be aware of the format to use.

以如下所示的方式来初始化
:class:`Symfony\\Component\\Serializer\\Mapping\\Factory\\ClassMetadataFactory`::

    use Symfony\Component\Serializer\Mapping\Factory\ClassMetadataFactory;
    // 对于注释
    use Doctrine\Common\Annotations\AnnotationReader;
    use Symfony\Component\Serializer\Mapping\Loader\AnnotationLoader;
    // 对于XML
    // use Symfony\Component\Serializer\Mapping\Loader\XmlFileLoader;
    // 对于YAML
    // use Symfony\Component\Serializer\Mapping\Loader\YamlFileLoader;

    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));
    // 对于XML
    // $classMetadataFactory = new ClassMetadataFactory(new XmlFileLoader('/path/to/your/definition.xml'));
    // 对于YAML
    // $classMetadataFactory = new ClassMetadataFactory(new YamlFileLoader('/path/to/your/definition.yaml'));

.. _component-serializer-attributes-groups-annotations:

然后，创建你的组定义：

.. configuration-block::

    .. code-block:: php-annotations

        namespace Acme;

        use Symfony\Component\Serializer\Annotation\Groups;

        class MyObj
        {
            /**
             * @Groups({"group1", "group2"})
             */
            public $foo;

            /**
             * @Groups("group3")
             */
            public function getBar() // 同样支持方法
            {
                return $this->bar;
            }

            // ...
        }

    .. code-block:: yaml

        Acme\MyObj:
            attributes:
                foo:
                    groups: ['group1', 'group2']
                bar:
                    groups: ['group3']

    .. code-block:: xml

        <?xml version="1.0" ?>
        <serializer xmlns="http://symfony.com/schema/dic/serializer-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/serializer-mapping
                http://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
        >
            <class name="Acme\MyObj">
                <attribute name="foo">
                    <group>group1</group>
                    <group>group2</group>
                </attribute>

                <attribute name="bar">
                    <group>group3</group>
                </attribute>
            </class>
        </serializer>

你现在能仅序列化所需组中的属性::

    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    $obj = new MyObj();
    $obj->foo = 'foo';
    $obj->setBar('bar');

    $normalizer = new ObjectNormalizer($classMetadataFactory);
    $serializer = new Serializer(array($normalizer));

    $data = $serializer->normalize($obj, null, array('groups' => 'group1'));
    // $data = array('foo' => 'foo');

    $obj2 = $serializer->denormalize(
        array('foo' => 'foo', 'bar' => 'bar'),
        'MyObj',
        null,
        array('groups' => array('group1', 'group3'))
    );
    // $obj2 = MyObj(foo: 'foo', bar: 'bar')

.. include:: /_includes/_annotation_loader_tip.rst.inc

.. _ignoring-attributes-when-serializing:

选择特定属性
-----------------------------

也可以仅序列化一组(set)特定属性::

    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    class User
    {
        public $familyName;
        public $givenName;
        public $company;
    }

    class Company
    {
        public $name;
        public $address;
    }

    $company = new Company();
    $company->name = 'Les-Tilleuls.coop';
    $company->address = 'Lille, France';

    $user = new User();
    $user->familyName = 'Dunglas';
    $user->givenName = 'Kévin';
    $user->company = $company;

    $serializer = new Serializer(array(new ObjectNormalizer()));

    $data = $serializer->normalize($user, null, array('attributes' => array('familyName', 'company' => ['name'])));
    // $data = array('familyName' => 'Dunglas', 'company' => array('name' => 'Les-Tilleuls.coop'));

只有未被忽略（见下文）的属性可用。如果同时还设置了某些序列化组，则只能使用这些组允许的属性。
Only attributes that are not ignored (see below) are available.
If some serialization groups are set, only attributes allowed by those groups can be used.

对于组，可以在序列化和反序列化过程中选择属性。
可以在序列化和反序列化过程中选择组、属性。
As for groups, attributes can be selected during both the serialization and deserialization process.

忽略属性
-------------------

.. note::

    使用属性组而不是
    :method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setIgnoredAttributes`
    方法，被认为是最佳的实践。

作为一个选项，有一种方法可以忽略原始对象的属性。
要移除这些属性，请在normalizer定义中使用
:method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setIgnoredAttributes`
方法::
As an option, there's a way to ignore attributes from the origin object. To remove
those attributes use the
:method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setIgnoredAttributes`
method on the normalizer definition::

    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    $normalizer = new ObjectNormalizer();
    $normalizer->setIgnoredAttributes(array('age'));
    $encoder = new JsonEncoder();

    $serializer = new Serializer(array($normalizer), array($encoder));
    $serializer->serialize($person, 'json'); // 输出: {"name":"foo","sportsperson":false}

.. _component-serializer-converting-property-names-when-serializing-and-deserializing:

在序列化和反序列化时转换属性名称
------------------------------------------------------------

有时，序列化后的属性的名称必须与PHP类的属性或 getter/setter 方法不同。
Sometimes serialized attributes must be named differently than properties
or getter/setter methods of PHP classes.

Serializer组件提供了一种将PHP字段名称转换或映射为已序列化名称的便捷方法：名称转换器系统。
The Serializer component provides a handy way to translate or map PHP field
names to serialized names: The Name Converter System.

鉴于你有以下对象::

    class Company
    {
        public $name;
        public $address;
    }

并且在序列化的表单中，所有属性都必须加上 ``org_`` 前缀，如下所示::

    {"org_name": "Acme Inc.", "org_address": "123 Main Street, Big City"}

一个自定义的名称转换器可以处理此类情况::

    use Symfony\Component\Serializer\NameConverter\NameConverterInterface;

    class OrgPrefixNameConverter implements NameConverterInterface
    {
        public function normalize($propertyName)
        {
            return 'org_'.$propertyName;
        }

        public function denormalize($propertyName)
        {
            // 移除 'org_' 前缀
            return 'org_' === substr($propertyName, 0, 4) ? substr($propertyName, 4) : $propertyName;
        }
    }

该自定义名称转换器可以通过将其作为任何继承
:class:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer`
的类的第二个参数传递来使用，包括
:class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer` 和
:class:`Symfony\\Component\\Serializer\\Normalizer\\PropertyNormalizer`::
The custom name converter can be used by passing it as second parameter of any
class extending :class:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer`,
including :class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer`
and :class:`Symfony\\Component\\Serializer\\Normalizer\\PropertyNormalizer`::

    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $nameConverter = new OrgPrefixNameConverter();
    $normalizer = new ObjectNormalizer(null, $nameConverter);

    $serializer = new Serializer(array($normalizer), array(new JsonEncoder()));

    $company = new Company();
    $company->name = 'Acme Inc.';
    $company->address = '123 Main Street, Big City';

    $json = $serializer->serialize($company, 'json');
    // {"org_name": "Acme Inc.", "org_address": "123 Main Street, Big City"}
    $companyCopy = $serializer->deserialize($json, Company::class, 'json');
    // 与 $company 一样的数据

.. note::

    You can also implement
    :class:`Symfony\\Component\\Serializer\\NameConverter\\AdvancedNameConverterInterface`
    to access to the current class name, format and context.
    你还可以通过实现
    :class:`Symfony\\Component\\Serializer\\NameConverter\\AdvancedNameConverterInterface`
    来访问当前的类名、格式和上下文。

    .. versionadded:: 4.2
        ``AdvancedNameConverterInterface`` 接口在Symfony4.2推出。

.. _using-camelized-method-names-for-underscored-attributes:

驼峰拼写法和蛇形拼写法的转换
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在许多格式中，通常使用下划线来分隔单词（也称为蛇形拼写法）。
但是，在Symfony应用中，使用驼峰拼写法来命名属性是很常见的（即使 `PSR-1标准`_ 并未建议属性名称的任何特定用例）。
In many formats, it's common to use underscores to separate words (also known
as snake_case). However, in Symfony applications is common to use CamelCase to
name properties (even though the `PSR-1标准`_ doesn't recommend any
specific case for property names).

Symfony提供了一个内置的名称转换器，用于在序列化和反序列化过程中在蛇形拼写法和驼峰拼写法样式之间进行转换::
Symfony provides a built-in name converter designed to transform between
snake_case and CamelCased styles during serialization and deserialization
processes::

    use Symfony\Component\Serializer\NameConverter\CamelCaseToSnakeCaseNameConverter;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    $normalizer = new ObjectNormalizer(null, new CamelCaseToSnakeCaseNameConverter());

    class Person
    {
        private $firstName;

        public function __construct($firstName)
        {
            $this->firstName = $firstName;
        }

        public function getFirstName()
        {
            return $this->firstName;
        }
    }

    $kevin = new Person('Kévin');
    $normalizer->normalize($kevin);
    // ['first_name' => 'Kévin'];

    $anne = $normalizer->denormalize(array('first_name' => 'Anne'), 'Person');
    // Person object with firstName: 'Anne'

使用元数据配置名称转换
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在Symfony应用中按照
:ref:`属性组章节 <component-serializer-attributes-groups>`
中的说明启用类元数据工厂来使用此组件时，已经设置了此组件，你只需提供相应配置。
除此以外::
When using this component inside a Symfony application and the class metadata
factory is enabled as explained in the :ref:`属性组章节 <component-serializer-attributes-groups>`,
this is already set up and you only need to provide the configuration. Otherwise::

    // ...
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\NameConverter\MetadataAwareNameConverter;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));

    $metadataAwareNameConverter = new MetadataAwareNameConverter($classMetadataFactory);

    $serializer = new Serializer(
        array(new ObjectNormalizer($classMetadataFactory, $metadataAwareNameConverter)),
        array('json' => new JsonEncoder())
    );

现在配置你的名称转换映射。考虑一个定义了具有 ``firstName`` 属性的 ``Person`` 实体的应用：
Now configure your name conversion mapping. Consider an application that
defines a ``Person`` entity with a ``firstName`` property:

.. configuration-block::

    .. code-block:: php-annotations

        namespace App\Entity;

        use Symfony\Component\Serializer\Annotation\SerializedName;

        class Person
        {
            /**
             * @SerializedName("customer_name")
             */
            private $firstName;

            public function __construct($firstName)
            {
                $this->firstName = $firstName;
            }

            // ...
        }

    .. code-block:: yaml

        App\Entity\Person:
            attributes:
                firstName:
                    serialized_name: customer_name

    .. code-block:: xml

        <?xml version="1.0" ?>
        <serializer xmlns="http://symfony.com/schema/dic/serializer-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/serializer-mapping
                http://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
        >
            <class name="App\Entity\Person">
                <attribute name="firstName" serialized-name="customer_name" />
            </class>
        </serializer>

此自定义映射用于在序列化和反序列化对象时转换属性的名称::

    $serialized = $serializer->serialize(new Person("Kévin"));
    // {"customer_name": "Kévin"}

序列化布尔属性
------------------------------

如果你使用的是isser方法（前缀为 ``is`` 的方法，类似
``App\Model\Person::isSportsperson()``），则序列化器组件将自动检测并使用它来序列化相关属性。
If you are using isser methods (methods prefixed by ``is``, like
``App\Model\Person::isSportsperson()``), the Serializer component will
automatically detect and use it to serialize related attributes.

``ObjectNormalizer`` 还需要照顾以 ``has``、``add`` 和 ``remove`` 开头的方法。

Using Callbacks to Serialize Properties with Object Instances
使用回调来通过对象实例来序列化属性
-------------------------------------------------------------

序列化时，你可以设置一个回调来格式化特定的对象属性::

    use App\Model\Person;
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $encoder = new JsonEncoder();
    $normalizer = new GetSetMethodNormalizer();

    // all callback parameters are optional (you can omit the ones you don't use)
    $callback = function ($innerObject, $outerObject, string $attributeName, string $format = null, array $context = array()) {
        return $innerObject instanceof \DateTime ? $innerObject->format(\DateTime::ISO8601) : '';
    };

    $normalizer->setCallbacks(array('createdAt' => $callback));

    $serializer = new Serializer(array($normalizer), array($encoder));

    $person = new Person();
    $person->setName('cordoval');
    $person->setAge(34);
    $person->setCreatedAt(new \DateTime('now'));

    $serializer->serialize($person, 'json');
    // Output: {"name":"cordoval", "age": 34, "createdAt": "2014-03-22T09:43:12-0500"}

.. versionadded:: 4.2
    The ``$outerObject``, ``$attributeName``, ``$format`` and ``$context``
    parameters of the callback were introduced in Symfony 4.2.
    在$outerObject，$attributeName，$format和$context 回调的参数中的Symfony 4.2推出。

.. _component-serializer-normalizers:

Normalizers
-----------

有几种类型的normalizer可用：

:class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer`
    This normalizer leverages the :doc:`PropertyAccess Component </components/property_access>`
    to read and write in the object. It means that it can access to properties
    directly and through getters, setters, hassers, adders and removers. It supports
    calling the constructor during the denormalization process.
    此normalizer利用 :doc:`PropertyAccess 组件 </components/property_access>`
    来读取和写入对象。
    这意味着它可以直接访问属性，也可以通过getter、setter、hassers、adders和removers访问来属性。
    它支持在denormalization过程中调用构造函数。

    Objects are normalized to a map of property names and values (names are
    generated removing the ``get``, ``set``, ``has`` or ``remove`` prefix from
    the method name and lowercasing the first letter; e.g. ``getFirstName()`` ->
    ``firstName``).
    对象是normalized的为一个属性名称和值的映射（生成的名称移除方法名称中的
    ``get``、``set``、``has``、``remove`` 前缀并小写第一个字母;例如
    ``getFirstName()`` -> ``firstName``）。

    The ``ObjectNormalizer`` is the most powerful normalizer. It is configured by
    default in Symfony applications with the Serializer component enabled.
    ``ObjectNormalizer`` 是最强大的normalizer。
    它在Symfony应用中默认配置，并启用了Serializer组件。

:class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer`
    This normalizer reads the content of the class by calling the "getters"
    (public methods starting with "get"). It will denormalize data by calling
    the constructor and the "setters" (public methods starting with "set").
    此normalizer通过调用“getters”（以“get”开头的公共方法）来读取类的内容。
    它将通过调用构造函数和“setter”（以“set”开头的公共方法）来对数据进行denormalize。

    Objects are normalized to a map of property names and values (names are
    generated removing the ``get`` prefix from the method name and lowercasing
    the first letter; e.g. ``getFirstName()`` -> ``firstName``).
    对象被normalized为一个属性名称和值的映射（生成的名称，从方法名称中移除
    ``get`` 前缀并小写第一个字母;例如 ``getFirstName()`` -> ``firstName``）。

:class:`Symfony\\Component\\Serializer\\Normalizer\\PropertyNormalizer`
    This normalizer directly reads and writes public properties as well as
    **private and protected** properties (from both the class and all of its
    parent classes). It supports calling the constructor during the denormalization process.
    此normalizer直接读取和写入公共属性以及 **私有和受保护** 的属性（来自类及其所有父类）。
    它支持在denormalization过程中调用构造函数。

    Objects are normalized to a map of property names to property values.
    对象被normalized为一个属性名称到属性值的映射。

:class:`Symfony\\Component\\Serializer\\Normalizer\\JsonSerializableNormalizer`
    此normalizer适用于实现了 :phpclass:`JsonSerializable` 的类。

    It will call the :phpmethod:`JsonSerializable::jsonSerialize` method and
    then further normalize the result. This means that nested
    :phpclass:`JsonSerializable` classes will also be normalized.
    它将调用 :phpmethod:`JsonSerializable::jsonSerialize` 方法，然后进一步normalize结果。
    这意味着嵌套的 :phpclass:`JsonSerializable` 类也将被normalized。

    This normalizer is particularly helpful when you want to gradually migrate
    from an existing codebase using simple :phpfunction:`json_encode` to the Symfony
    Serializer by allowing you to mix which normalizers are used for which classes.
    当你希望逐步从使用简单的 :phpfunction:`json_encode` 的现有代码库迁移到Symfony Serializer时，此normalizer特别有用，它允许你将哪些规范化器用于哪些类。

    Unlike with :phpfunction:`json_encode` circular references can be handled.
    与 :phpfunction:`json_encode` 不同，此normalizer可以处理循环引用。

:class:`Symfony\\Component\\Serializer\\Normalizer\\DateTimeNormalizer`
    This normalizer converts :phpclass:`DateTimeInterface` objects (e.g.
    :phpclass:`DateTime` and :phpclass:`DateTimeImmutable`) into strings.
    By default it uses the RFC3339_ format.
    此normalizer将 :phpclass:`DateTimeInterface` 对象（例如 :phpclass:`DateTime` 和
    :phpclass:`DateTimeImmutable`）转换为字符串。默认情况下，它使用 RFC3339_ 格式。

:class:`Symfony\\Component\\Serializer\\Normalizer\\DataUriNormalizer`
    This normalizer converts :phpclass:`SplFileInfo` objects into a data URI
    string (``data:...``) such that files can be embedded into serialized data.
    此normalizer将 :phpclass:`SplFileInfo`
    对象转换为一个data URI格式的字符串（``data:...``），以便可以将文件嵌入到序列化数据中。

:class:`Symfony\\Component\\Serializer\\Normalizer\\DateIntervalNormalizer`
    This normalizer converts :phpclass:`DateInterval` objects into strings.
    By default it uses the ``P%yY%mM%dDT%hH%iM%sS`` format.
    此normalizer将 :phpclass:`DateInterval` 对象转换为字符串。
    默认情况下，它使用 ``P%yY%mM%dDT%hH%iM%sS`` 格式。

:class:`Symfony\\Component\\Serializer\\Normalizer\\ConstraintViolationListNormalizer`
    This normalizer converts objects that implement
    :class:`Symfony\\Component\\Validator\\ConstraintViolationListInterface`
    into a list of errors according to the `RFC 7807`_ standard.
    此normalizer根据 `RFC 7807`_ 标准将实现
    :class:`Symfony\\Component\\Validator\\ConstraintViolationListInterface`
    的对象转换为一个错误列表。

    .. versionadded:: 4.1
        ``ConstraintViolationListNormalizer`` 在Symfony4.1中引入。

.. _component-serializer-encoders:

编码器
--------

编码器将 **数组** 转换为特定 **格式**，反之亦然。
它们的编码（从数组到格式）实现了
:class:`Symfony\\Component\\Serializer\\Encoder\\EncoderInterface`
和解码（从格式到数组）实现了 :class:`Symfony\\Component\\Serializer\\Encoder\\DecoderInterface`。
Encoders turn **arrays** into **formats** and vice versa. They implement
:class:`Symfony\\Component\\Serializer\\Encoder\\EncoderInterface`
for encoding (array to format) and
:class:`Symfony\\Component\\Serializer\\Encoder\\DecoderInterface` for decoding
(format to array).

你可以通过将其传递到序列化器实例的第二个构造函数参数来添加新编码器::
You can add new encoders to a Serializer instance by using its second constructor argument::

    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Encoder\XmlEncoder;
    use Symfony\Component\Serializer\Encoder\JsonEncoder;

    $encoders = array(new XmlEncoder(), new JsonEncoder());
    $serializer = new Serializer(array(), $encoders);

内置编码器
~~~~~~~~~~~~~~~~~

Serializer组件提供了几个内置编码器:

:class:`Symfony\\Component\\Serializer\\Encoder\\JsonEncoder`
    该类以 JSON_ 格式来编码和解码数据。

:class:`Symfony\\Component\\Serializer\\Encoder\\XmlEncoder`
    该类以 XML_ 格式来编码和解码数据。

:class:`Symfony\\Component\\Serializer\\Encoder\\YamlEncoder`
    该类以 YAML_ 格式来编码和解码数据。该编码器需要 :doc:`Yaml组件 </components/yaml>`。

:class:`Symfony\\Component\\Serializer\\Encoder\\CsvEncoder`
    该类以 CSV_ 格式来编码和解码数据。

在Symfony应用中使用Serializer组件时，默认情况下会启用所有这些编码器。

``JsonEncoder``
~~~~~~~~~~~~~~~~~~~

``JsonEncoder`` 基于 :phpfunction:`json_encode` 和 :phpfunction:`json_decode`
函数对JSON字符串进行编码和解码。

``CsvEncoder``
~~~~~~~~~~~~~~~~~~~

``CsvEncoder`` 对CSV进行编码和解码。

你可以传递上下文键 ``as_collection``，以便始终将结果作为一个集合。
You can pass the context key ``as_collection`` in order to have the results
always as a collection.

.. versionadded:: 4.1
    ``as_collection`` 选项在Symfony4.1中引入。

.. versionadded:: 4.2
    Relying on the default value ``false`` is deprecated since Symfony 4.2.
    从Symfony 4.2开始，弃用对默认值 ``false`` 的依赖。

``XmlEncoder``
~~~~~~~~~~~~~~~~~~

该编码器将数组转换为XML，反之亦然。

例如，如下所示将对象normalized::
For example, take an object normalized as following::

    array('foo' => array(1, 2), 'bar' => true);

``XmlEncoder`` 会将该对象编码为这个样子::

    <?xml version="1.0"?>
    <response>
        <foo>1</foo>
        <foo>2</foo>
        <bar>1</bar>
    </response>

请注意，此编码器将考虑以 ``@`` 属性开头的键，并将使用  ``#comment`` 键来编码XML注释(comment)::
Be aware that this encoder will consider keys beginning with ``@`` as attributes, and will use
the key  ``#comment`` for encoding XML comments::

    $encoder = new XmlEncoder();
    $encoder->encode(array('foo' => array('@bar' => 'value'), 'qux' => array('#comment' => 'A comment));
    // will return:
    // <?xml version="1.0"?>
    // <response>
    //     <foo bar="value" />
    //     <qux><!-- A comment --!><qux>
    // </response>

你可以传递上下文键 ``as_collection``，以便始终将结果作为一个集合。
You can pass the context key ``as_collection`` in order to have the results
always as a collection.

.. versionadded:: 4.1
    ``as_collection`` 选项在Symfony4.1中引入。

.. tip::

    XML comments are ignored by default when decoding contents, but this
    behavior can be changed with the optional ``$decoderIgnoredNodeTypes`` argument of
    the ``XmlEncoder`` class constructor.
    解码内容时，默认情况下会忽略XML注释，但可以使用 ``XmlEncoder`` 的类构造函数的可选
    ``$decoderIgnoredNodeTypes`` 参数更改此行为。

    Data with ``#comment`` keys are encoded to XML comments by default. This can be
    changed with the optional ``$encoderIgnoredNodeTypes`` argument of the
    ``XmlEncoder`` class constructor.
    默认情况下，带 ``#comment`` 键的数据被编码为XML注释。
    但可以使用 ``XmlEncoder`` 的类构造函数的可选 ``$encoderIgnoredNodeTypes`` 参数进行更改。

    .. versionadded:: 4.1
        从Symfony 4.1开始，将默认忽略XML注释。

``YamlEncoder``
~~~~~~~~~~~~~~~~~~~

该编码器需要 :doc:`Yaml组件 </components/yaml>` 并将负责Yaml的转换。


跳过 ``null`` 值
------------------------

默认情况下，Serializer将保留包含 ``null`` 值的属性。
你可以通过将 ``skip_null_values`` 上下文选项设置为 ``true`` 来更改此行为::

    $dummy = new class {
        public $foo;
        public $bar = 'notNull';
    };

    $normalizer = new ObjectNormalizer();
    $result = $normalizer->normalize($dummy, 'json', ['skip_null_values' => true]);
    // ['bar' => 'notNull']

.. versionadded:: 4.2
    ``skip_null_values`` 选项在Symfony4.2中引入。

.. _component-serializer-handling-circular-references:

处理循环引用
----------------------------

处理实体关系时，循环(Circular)引用很常见::

    class Organization
    {
        private $name;
        private $members;

        public function setName($name)
        {
            $this->name = $name;
        }

        public function getName()
        {
            return $this->name;
        }

        public function setMembers(array $members)
        {
            $this->members = $members;
        }

        public function getMembers()
        {
            return $this->members;
        }
    }

    class Member
    {
        private $name;
        private $organization;

        public function setName($name)
        {
            $this->name = $name;
        }

        public function getName()
        {
            return $this->name;
        }

        public function setOrganization(Organization $organization)
        {
            $this->organization = $organization;
        }

        public function getOrganization()
        {
            return $this->organization;
        }
    }

为了避免无限循环，
:class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer`
或 :class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer`
在遇到这种情况时会抛出一个
:class:`Symfony\\Component\\Serializer\\Exception\\CircularReferenceException`::

    $member = new Member();
    $member->setName('Kévin');

    $organization = new Organization();
    $organization->setName('Les-Tilleuls.coop');
    $organization->setMembers(array($member));

    $member->setOrganization($organization);

    echo $serializer->serialize($organization, 'json'); // Throws a CircularReferenceException

此normalizer的 ``setCircularReferenceLimit()`` 方法设置在将其视为循环引用之前将序列化同一对象的次数。
它的默认值为 ``1``。
The ``setCircularReferenceLimit()`` method of this normalizer sets the number
of times it will serialize the same object before considering it a circular
reference. Its default value is ``1``.

循环引用也可以由自定义的可调用对象处理，而不是抛出异常。
在序列化具有唯一标识符的实体时，这尤其有用：
Instead of throwing an exception, circular references can also be handled
by custom callables. This is especially useful when serializing entities
having unique identifiers::

    $encoder = new JsonEncoder();
    $normalizer = new ObjectNormalizer();

    // all callback parameters are optional (you can omit the ones you don't use)
    $normalizer->setCircularReferenceHandler(function ($object, string $format = null, array $context = array()) {
        return $object->getName();
    });

    $serializer = new Serializer(array($normalizer), array($encoder));
    var_dump($serializer->serialize($org, 'json'));
    // {"name":"Les-Tilleuls.coop","members":[{"name":"K\u00e9vin", organization: "Les-Tilleuls.coop"}]}

.. versionadded:: 4.2
    ``setCircularReferenceHandler()`` 的
    ``$format`` 和 ``$context`` 参数是在Symfony4.2引入的。

处理序列化深度
----------------------------

Serializer组件能够检测并限制序列化的深度。在序列化大型树时尤其有用。假设以下数据结构：
The Serializer component is able to detect and limit the serialization depth.
It is especially useful when serializing large trees. Assume the following data
structure::

    namespace Acme;

    class MyObj
    {
        public $foo;

        /**
         * @var self
         */
        public $child;
    }

    $level1 = new MyObj();
    $level1->foo = 'level1';

    $level2 = new MyObj();
    $level2->foo = 'level2';
    $level1->child = $level2;

    $level3 = new MyObj();
    $level3->foo = 'level3';
    $level2->child = $level3;

可以将序列化器配置为为给定属性设置最大深度。在这里，我们将 ``$child`` 属性设置为 ``2``：
The serializer can be configured to set a maximum depth for a given property.
Here, we set it to 2 for the ``$child`` property:

.. configuration-block::

    .. code-block:: php-annotations

        use Symfony\Component\Serializer\Annotation\MaxDepth;

        namespace Acme;

        class MyObj
        {
            /**
             * @MaxDepth(2)
             */
            public $child;

            // ...
        }

    .. code-block:: yaml

        Acme\MyObj:
            attributes:
                child:
                    max_depth: 2

    .. code-block:: xml

        <?xml version="1.0" ?>
        <serializer xmlns="http://symfony.com/schema/dic/serializer-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/serializer-mapping
                http://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
        >
            <class name="Acme\MyObj">
                <attribute name="child" max-depth="2" />
            </class>
        </serializer>

必须配置与所选格式对应的元数据加载器才能使用此功能。
在Symfony应用中使用Serializer组件时会自动完成。
使用独立组件时，请参阅 :ref:`组 <component-serializer-attributes-groups>`
章节以了解如何执行此操作。
The metadata loader corresponding to the chosen format must be configured in
order to use this feature. It is done automatically when using the Serializer component
in a Symfony application. When using the standalone component, refer to
:ref:`the groups documentation <component-serializer-attributes-groups>` to
learn how to do that.

只有在序列化器上下文的 ``enable_max_depth`` 键设置为 ``true`` 时，才会进行检查。
在以下示例中，第三级未序列化，因为它比配置的最大深度2更深：
The check is only done if the ``enable_max_depth`` key of the serializer context
is set to ``true``. In the following example, the third level is not serialized
because it is deeper than the configured maximum depth of 2::

    $result = $serializer->normalize($level1, null, array('enable_max_depth' => true));
    /*
    $result = array(
        'foo' => 'level1',
        'child' => array(
                'foo' => 'level2',
                'child' => array(
                        'child' => null,
                    ),
            ),
    );
    */

当达到最大深度时，可以执行自定义可调用对象而不是抛出异常。
在序列化具有唯一标识符的实体时，这尤其有用：
Instead of throwing an exception, a custom callable can be executed when the
maximum depth is reached. This is especially useful when serializing entities
having unique identifiers::

    use Doctrine\Common\Annotations\AnnotationReader;
    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Annotation\MaxDepth;
    use Symfony\Component\Serializer\Mapping\Factory\ClassMetadataFactory;
    use Symfony\Component\Serializer\Mapping\Loader\AnnotationLoader;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    class Foo
    {
        public $id;

        /**
         * @MaxDepth(1)
         */
        public $child;
    }

    $level1 = new Foo();
    $level1->id = 1;

    $level2 = new Foo();
    $level2->id = 2;
    $level1->child = $level2;

    $level3 = new Foo();
    $level3->id = 3;
    $level2->child = $level3;

    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));
    $normalizer = new ObjectNormalizer($classMetadataFactory);
    // all callback parameters are optional (you can omit the ones you don't use)
    $normalizer->setMaxDepthHandler(function ($innerObject, $outerObject, string $attributeName, string $format = null, array $context = array()) {
        return '/foos/'.$innerObject->id;
    });

    $serializer = new Serializer(array($normalizer));

    $result = $serializer->normalize($level1, null, array(ObjectNormalizer::ENABLE_MAX_DEPTH => true));
    /*
    $result = array(
        'id' => 1,
        'child' => array(
            'id' => 2,
            'child' => '/foos/3',
        ),
    );
    */

.. versionadded:: 4.1
    ``setMaxDepthHandler()`` 方法是在Symfony4.1中引入的。

.. versionadded:: 4.2
    ``setMaxDepthHandler()`` 的 ``$outerObject``、``$attributeName``、``$format``
    以及 ``$context`` 参数是在Symfony4.2中引入的。

处理数组
---------------

Serializer组件也能够处理对象数组。序列化数组就像序列化单个对象一样：
The Serializer component is capable of handling arrays of objects as well.
Serializing arrays works just like serializing a single object::

    use Acme\Person;

    $person1 = new Person();
    $person1->setName('foo');
    $person1->setAge(99);
    $person1->setSportsman(false);

    $person2 = new Person();
    $person2->setName('bar');
    $person2->setAge(33);
    $person2->setSportsman(true);

    $persons = array($person1, $person2);
    $data = $serializer->serialize($persons, 'json');

    // $data 包含 [{"name":"foo","age":99,"sportsman":false},{"name":"bar","age":33,"sportsman":true}]

如果要反序列化此类结构，则需要将其添加
:class:`Symfony\\Component\\Serializer\\Normalizer\\ArrayDenormalizer`
到normalizer集。通过附加 ``[]`` 到
:method:`Symfony\\Component\\Serializer\\Serializer::deserialize`
方法的类型参数，可以指示你期望的是数组而不是单个对象。
If you want to deserialize such a structure, you need to add the
:class:`Symfony\\Component\\Serializer\\Normalizer\\ArrayDenormalizer`
to the set of normalizers. By appending ``[]`` to the type parameter of the
:method:`Symfony\\Component\\Serializer\\Serializer::deserialize` method,
you indicate that you're expecting an array instead of a single object.

.. code-block:: php

    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\ArrayDenormalizer;
    use Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $serializer = new Serializer(
        array(new GetSetMethodNormalizer(), new ArrayDenormalizer()),
        array(new JsonEncoder())
    );

    $data = ...; // 上一示例中的序列化数据
    $persons = $serializer->deserialize($data, 'Acme\Person[]', 'json');

``XmlEncoder``
------------------

该编码器将数组转换为XML，反之亦然。例如，如下所示将对象normalized::
This encoder transforms arrays into XML and vice versa. For example, take an
object normalized as following::

    array('foo' => array(1, 2), 'bar' => true);

``XmlEncoder`` 会将该对象编码为这个样子::
The ``XmlEncoder`` encodes this object as follows:

.. code-block:: xml

    <?xml version="1.0"?>
    <response>
        <foo>1</foo>
        <foo>2</foo>
        <bar>1</bar>
    </response>

以 ``@`` 开头的数组键会被当做XML属性::

    array('foo' => array('@bar' => 'value'));

    // is encoded as follows:
    // <?xml version="1.0"?>
    // <response>
    //     <foo bar="value" />
    // </response>

使用特殊 ``#`` 键来定义一个节点的数据::

    array('foo' => array('@bar' => 'value', '#' => 'baz'));

    // is encoded as follows:
    // <?xml version="1.0"?>
    // <response>
    //     <foo bar="value">
    //        baz
    //     </foo>
    // </response>

上下文
~~~~~~~

``encode()`` 方法定义了第三个名为 ``context`` 的可选参数，该参数定义了XmlEncoder和关联数组的配置选项：
The ``encode()`` method defines a third optional parameter called ``context``
which defines the configuration options for the XmlEncoder an associative array::

    $xmlEncoder->encode($array, 'xml', $context);

这些是可用的选项：

``xml_format_output``
    If set to true, formats the generated XML with line breaks and indentation.
    如果设置为 ``true``，则生成的XML的格式使用换行符和缩进。

``xml_version``
    设置XML的版本属性（默认：``1.1``) 。

``xml_encoding``
    设置XML的编码属性（默认值：``utf-8``)。

``xml_standalone``
    Adds standalone attribute in the generated XML (default: ``true``).
    在生成的XML中添加独立(standalone)属性（默认值：``true``) 。

``xml_root_node_name``
    Sets the root node name (default: ``response``).
    设置根节点名称（默认值： ``response``)。

``remove_empty_tags``
    如果设置为 ``true``，则删除生成的XML中的所有空标签。

处理构造函数参数
------------------------------

.. versionadded:: 4.1
    ``default_constructor_arguments`` 选项在Symfony4.1中引入。

如果类构造函数定义了参数，就像 `Value Objects`_ 一样，如果缺少某些参数，序列化器将无法创建对象。
在这些情况下，请使用 ``default_constructor_arguments`` 上下文选项：
If the class constructor defines arguments, as usually happens with
`Value Objects`_, the serializer won't be able to create the object if some
arguments are missing. In those cases, use the ``default_constructor_arguments``
context option::

    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    class MyObj
    {
        private $foo;
        private $bar;

        public function __construct($foo, $bar)
        {
            $this->foo = $foo;
            $this->bar = $bar;
        }
    }

    $normalizer = new ObjectNormalizer($classMetadataFactory);
    $serializer = new Serializer(array($normalizer));

    $data = $serializer->denormalize(
        array('foo' => 'Hello'),
        'MyObj',
        array('default_constructor_arguments' => array(
            'MyObj' => array('foo' => '', 'bar' => ''),
        )
    ));
    // $data = new MyObj('Hello', '');

Recursive Denormalization and Type Safety
递归Denormalization和类型安全
-----------------------------------------

Serializer组件可以使用
:doc:`PropertyInfo组件 </components/property_info>` 来对复杂类型（对象）进行denormalize。
将使用提供的提取器猜测类的属性的类型，并用于递归地反规范化内部数据。
The Serializer component can use the :doc:`PropertyInfo Component </components/property_info>` to denormalize
complex types (objects). The type of the class' property will be guessed using the provided
extractor and used to recursively denormalize the inner data.

在Symfony应用中使用此组件时，所有规范化器都会自动配置为使用已注册的提取器。
当独立使用此组件时，必须将
:class:`Symfony\\Component\\PropertyInfo\\PropertyTypeExtractorInterface`
（通常是一个 :class:`Symfony\\Component\\PropertyInfo\\PropertyInfoExtractor`
实例）的实现作为 ``ObjectNormalizer`` 的第4个参数传递：
When using this component in a Symfony application, all normalizers are automatically configured to use the registered extractors.
When using the component standalone, an implementation of :class:`Symfony\\Component\\PropertyInfo\\PropertyTypeExtractorInterface`,
(usually an instance of :class:`Symfony\\Component\\PropertyInfo\\PropertyInfoExtractor`) must be passed as the 4th
parameter of the ``ObjectNormalizer``::

    use Symfony\Component\PropertyInfo\Extractor\ReflectionExtractor;
    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Normalizer\DateTimeNormalizer;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    namespace Acme;

    class ObjectOuter
    {
        private $inner;
        private $date;

        public function getInner()
        {
            return $this->inner;
        }

        public function setInner(ObjectInner $inner)
        {
            $this->inner = $inner;
        }

        public function setDate(\DateTimeInterface $date)
        {
            $this->date = $date;
        }

        public function getDate()
        {
            return $this->date;
        }
    }

    class ObjectInner
    {
        public $foo;
        public $bar;
    }

    $normalizer = new ObjectNormalizer(null, null, null, new ReflectionExtractor());
    $serializer = new Serializer(array(new DateTimeNormalizer(), $normalizer));

    $obj = $serializer->denormalize(
        array('inner' => array('foo' => 'foo', 'bar' => 'bar'), 'date' => '1988/01/21'),
         'Acme\ObjectOuter'
    );

    dump($obj->getInner()->foo); // 'foo'
    dump($obj->getInner()->bar); // 'bar'
    dump($obj->getDate()->format('Y-m-d')); // '1988-01-21'

当一个 ``PropertyTypeExtractor``
可用时，normalizer还将检查要denormalize的数据是否与属性的类型匹配（即使对于基本类型）。
例如，如果提供了一个 ``string``，但属性的类型是 ``int``，则将抛出一个
:class:`Symfony\\Component\\Serializer\\Exception\\UnexpectedValueException`。
属性的类型强制可以通过设置 ``ObjectNormalizer::DISABLE_TYPE_ENFORCEMENT``
序列化上下文选项为 ``true`` 来禁用。
When a ``PropertyTypeExtractor`` is available, the normalizer will also check that the data to denormalize
matches the type of the property (even for primitive types). For instance, if a ``string`` is provided, but
the type of the property is ``int``, an :class:`Symfony\\Component\\Serializer\\Exception\\UnexpectedValueException`
will be thrown. The type enforcement of the properties can be disabled by setting
the serializer context option ``ObjectNormalizer::DISABLE_TYPE_ENFORCEMENT``
to ``true``.

序列化接口和抽象类
-------------------------------------------

处理非常相似或共享属性的对象时，可以使用接口或抽象类。
Serializer组件允许你使用 *"discriminator class mapping"* 来序列化和反序列化这些对象。
When dealing with objects that are fairly similar or share properties, you may
use interfaces or abstract classes. The Serializer component allows you to
serialize and deserialize these objects using a *"discriminator class mapping"*.

discriminator是用于区分可能对象的字段（在序列化字符串中）。
实际上，在使用Serializer组件时，会将一个
:class:`Symfony\\Component\\Serializer\\Mapping\\ClassDiscriminatorResolverInterface`
实现传递给 :class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer`。
The discriminator is the field (in the serialized string) used to differentiate
between the possible objects. In practice, when using the Serializer component,
pass a :class:`Symfony\\Component\\Serializer\\Mapping\\ClassDiscriminatorResolverInterface`
implementation to the :class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer`.

Serializer组件提供了一个名为
:class:`Symfony\\Component\\Serializer\\Mapping\\ClassDiscriminatorFromClassMetadata`
的 ``ClassDiscriminatorResolverInterface`` 的实现，它使用类元数据工厂和映射配置来序列化和反序列化正确类的对象。
The Serializer component provides an implementation of ``ClassDiscriminatorResolverInterface``
called :class:`Symfony\\Component\\Serializer\\Mapping\\ClassDiscriminatorFromClassMetadata`
which uses the class metadata factory and a mapping configuration to serialize
and deserialize objects of the correct class.

在Symfony应用中使用此组件并按照
:ref:`属性组章节 <component-serializer-attributes-groups>`
中的说明启用类元数据工厂时，已经设置了此组件，你只需提供配置。除此以外：
When using this component inside a Symfony application and the class metadata factory is enabled
as explained in the :ref:`Attributes Groups section <component-serializer-attributes-groups>`,
this is already set up and you only need to provide the configuration. Otherwise::

    // ...
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Mapping\ClassDiscriminatorMapping;
    use Symfony\Component\Serializer\Mapping\ClassDiscriminatorFromClassMetadata;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));

    $discriminator = new ClassDiscriminatorFromClassMetadata($classMetadataFactory);

    $serializer = new Serializer(
        array(new ObjectNormalizer($classMetadataFactory, null, null, null, $discriminator)),
        array('json' => new JsonEncoder())
    );

现在配置你的鉴别器类映射。考虑一个定义扩展的类GitHubCodeRepository 和BitBucketCodeRepository类的应用：
Now configure your discriminator class mapping. Consider an application that
defines an abstract ``CodeRepository`` class extended by ``GitHubCodeRepository``
and ``BitBucketCodeRepository`` classes:

.. configuration-block::

    .. code-block:: php-annotations

        namespace App;

        use Symfony\Component\Serializer\Annotation\DiscriminatorMap;

        /**
         * @DiscriminatorMap(typeProperty="type", mapping={
         *    "github"="App\GitHubCodeRepository",
         *    "bitbucket"="App\BitBucketCodeRepository"
         * })
         */
        interface CodeRepository
        {
            // ...
        }

    .. code-block:: yaml

        App\CodeRepository:
            discriminator_map:
                type_property: type
                mapping:
                    github: 'App\GitHubCodeRepository'
                    bitbucket: 'App\BitBucketCodeRepository'

    .. code-block:: xml

        <?xml version="1.0" ?>
        <serializer xmlns="http://symfony.com/schema/dic/serializer-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/serializer-mapping
                http://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
        >
            <class name="App\CodeRepository">
                <discriminator-map type-property="type">
                    <mapping type="github" class="App\GitHubCodeRepository" />
                    <mapping type="bitbucket" class="App\BitBucketCodeRepository" />
                </discriminator-map>
            </class>
        </serializer>

配置完成后，序列化程序使用映射来选择正确的类：
Once configured, the serializer uses the mapping to pick the correct class::

    $serialized = $serializer->serialize(new GitHubCodeRepository());
    // {"type": "github"}

    $repository = $serializer->deserialize($serialized, CodeRepository::class, 'json');
    // instanceof GitHubCodeRepository

性能
-----------

为了搞清哪些normalizer（或denormalizer）必须被用来处理一个对象，
:class:`Symfony\\Component\\Serializer\\Serializer`
类将在一个循环中调用所有已注册的normalizers（或denormalizers）的
:method:`Symfony\\Component\\Serializer\\Normalizer\\NormalizerInterface::supportsNormalization`
(或 :method:`Symfony\\Component\\Serializer\\Normalizer\\DenormalizerInterface::supportsDenormalization`)。
To figure which normalizer (or denormalizer) must be used to handle an object,
the :class:`Symfony\\Component\\Serializer\\Serializer` class will call the
:method:`Symfony\\Component\\Serializer\\Normalizer\\NormalizerInterface::supportsNormalization`
(or :method:`Symfony\\Component\\Serializer\\Normalizer\\DenormalizerInterface::supportsDenormalization`)
of all registered normalizers (or denormalizers) in a loop.

这些方法的结果可能因序列化的对象、格式和上下文而异。
这就是默认情况下结果 **未缓存** 的原因，并且可能导致严重的性能瓶颈。
The result of these methods can vary depending on the object to serialize, the
format and the context. That's why the result **is not cached** by default and
can result in a significant performance bottleneck.

但是，当对象的类型和格式相同时，大多数normalizer（和denormalizer）总是返回相同的结果，因此可以缓存结果。
为此，请使这些normalizer（和denormalizer）实现
:class:`Symfony\\Component\\Serializer\\Normalizer\\CacheableSupportsMethodInterface`
，并在调用
:method:`Symfony\\Component\\Serializer\\Normalizer\\CacheableSupportsMethodInterface::hasCacheableSupportsMethod`
时返回 ``true``。
However, most normalizers (and denormalizers) always return the same result when
the object's type and the format are the same, so the result can be cached. To
do so, make those normalizers (and denormalizers) implement the
:class:`Symfony\\Component\\Serializer\\Normalizer\\CacheableSupportsMethodInterface`
and return ``true`` when
:method:`Symfony\\Component\\Serializer\\Normalizer\\CacheableSupportsMethodInterface::hasCacheableSupportsMethod`
is called.

 .. note::

    All built-in :ref:`normalizers and denormalizers <component-serializer-normalizers>`
    as well the ones included in `API Platform`_ natively implement this interface.
    所有内置 :ref:`normalizer和denormalizer <component-serializer-normalizers>`
    以及 `API Platform`_ 中包含的规范化器和规范化器本身都实现了此接口。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /serializer

.. seealso::

    Normalizers for the Symfony Serializer Component supporting popular web API formats
    (JSON-LD, GraphQL, HAL and JSONAPI) are available as part of the `API Platform`_ project.
    支持流行的Web API格式（JSON-LD、GraphQL、HAL和JSONAPI）的Symfony Serializer组件的规范化器可作为API Platform项目的一部分提供。

.. seealso::

    A popular alternative to the Symfony Serializer component is the third-party
    library, `JMS serializer`_ (versions before ``v1.12.0`` were released under
    the Apache license, so incompatible with GPLv2 projects).
    Symfony Serializer组件的一个流行替代品是第三方库，JMS序列化器（之前的版本v1.12.0是在Apache许可下发布的，因此与GPLv2项目不兼容）。

.. _`PSR-1标准`: https://www.php-fig.org/psr/psr-1/
.. _`JMS serializer`: https://github.com/schmittjoh/serializer
.. _Packagist: https://packagist.org/packages/symfony/serializer
.. _RFC3339: https://tools.ietf.org/html/rfc3339#section-5.8
.. _JSON: http://www.json.org/
.. _XML: https://www.w3.org/XML/
.. _YAML: http://yaml.org/
.. _CSV: https://tools.ietf.org/html/rfc4180
.. _`RFC 7807`: https://tools.ietf.org/html/rfc7807
.. _`Value Objects`: https://en.wikipedia.org/wiki/Value_object
.. _`API Platform`: https://api-platform.com
