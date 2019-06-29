.. index::
   single: Serializer
   single: Components; Serializer

Serializer组件
========================

    Serializer组件用于将对象转换为特定格式（XML，JSON，YAML，...），反之亦然。

为此，Serializer组件遵循以下模式。

.. raw:: html

    <object data="../_images/components/serializer/serializer_workflow.svg" type="image/svg+xml"></object>

如上图所示，一个数组被当做对象和序列化内容之间的中介。
在这中间，编码器只处理特定 **格式** 到 **数组** 的转换，反之亦然。
同样的，Normalizer只处理特定 **对象** 到 **数组** 的转换，反之亦然。

序列化是一个复杂的主题。
此组件可能无法完全覆盖你的所有用例，但它对于开发用于序列化和反序列化你的对象的工具非常有用。

安装
------------

.. code-block:: terminal

    $ composer require symfony/serializer

.. include:: /components/require_autoload.rst.inc

如果要使用 ``ObjectNormalizer``， 还必须安装 :doc:`PropertyAccess组件 </components/property_access>`。

用法
-----

.. seealso::

    本文解释了Serializer的原理，让你熟悉normalizer和编码器的概念。
    代码示例假定你将Serializer用作独立组件。
    如果你在Symfony应用中使用Serializer，请在完成本文后阅读:doc:`/serializer`。

要使用Serializer组件，请设置
:class:`Symfony\\Component\\Serializer\\Serializer` 来指定启用哪些编码器和normalizer::

    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Encoder\XmlEncoder;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $encoders = [new XmlEncoder(), new JsonEncoder()];
    $normalizers = [new ObjectNormalizer()];

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
#. 此信息将被解码成的类的名称
#. 用于将该信息转换为数组的编码器

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
    $serializer = new Serializer([$normalizer]);
    $person = $serializer->deserialize($data, 'App\Model\Person', 'xml', [
        'allow_extra_attributes' => false,
    ]);

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

    $serializer->deserialize($data, Person::class, 'xml', ['object_to_populate' => $person]);
    // $person = App\Model\Person(name: 'foo', age: '69', sportsperson: true)

在使用一个ORM时，这是一个常见的需求。

.. _component-serializer-attributes-groups:

属性组
-----------------

有时，你希望从你的实体中序列化不同的属性集。组是实现这一需求的便捷方式。

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

以下代码展示了如何为每种格式初始化
:class:`Symfony\\Component\\Serializer\\Mapping\\Factory\\ClassMetadataFactory`：

* PHP文件中的注释::

    use Doctrine\Common\Annotations\AnnotationReader;
    use Symfony\Component\Serializer\Mapping\Factory\ClassMetadataFactory;
    use Symfony\Component\Serializer\Mapping\Loader\AnnotationLoader;

    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));

* YAML文件::

    use Symfony\Component\Serializer\Mapping\Factory\ClassMetadataFactory;
    use Symfony\Component\Serializer\Mapping\Loader\YamlFileLoader;

    $classMetadataFactory = new ClassMetadataFactory(new YamlFileLoader('/path/to/your/definition.yaml'));

* XML文件::

    use Symfony\Component\Serializer\Mapping\Factory\ClassMetadataFactory;
    use Symfony\Component\Serializer\Mapping\Loader\XmlFileLoader;

    $classMetadataFactory = new ClassMetadataFactory(new XmlFileLoader('/path/to/your/definition.xml'));

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
                https://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
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

    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $obj = new MyObj();
    $obj->foo = 'foo';
    $obj->setBar('bar');

    $normalizer = new ObjectNormalizer($classMetadataFactory);
    $serializer = new Serializer([$normalizer]);

    $data = $serializer->normalize($obj, null, ['groups' => 'group1']);
    // $data = ['foo' => 'foo'];

    $obj2 = $serializer->denormalize(
        ['foo' => 'foo', 'bar' => 'bar'],
        'MyObj',
        null,
        ['groups' => ['group1', 'group3']]
    );
    // $obj2 = MyObj(foo: 'foo', bar: 'bar')

.. include:: /_includes/_annotation_loader_tip.rst.inc

.. _ignoring-attributes-when-serializing:

选择特定属性
-----------------------------

也可以仅序列化一组(set)特定属性::

    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

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

    $serializer = new Serializer([new ObjectNormalizer()]);

    $data = $serializer->normalize($user, null, ['attributes' => ['familyName', 'company' => ['name']]]);
    // $data = ['familyName' => 'Dunglas', 'company' => ['name' => 'Les-Tilleuls.coop']];

只有未被忽略（见下文）的属性可用。如果还同时设置了某些序列化组，则只能使用这些组允许的属性。

对于组来说，可以在序列化和反序列化过程中选择属性。

忽略属性
-------------------

作为一种选择，有一种方法可以忽略原始对象的属性。要移除这些属性，请通过所需序列化方法的
``context`` 参数中的``ignored_attributes`` 键来提供一个数组::

    use Acme\Person;
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $person = new Person();
    $person->setName('foo');
    $person->setAge(99);

    $normalizer = new ObjectNormalizer();
    $encoder = new JsonEncoder();

    $serializer = new Serializer([$normalizer], [$encoder]);
    $serializer->serialize($person, 'json', ['ignored_attributes' => ['age']]); // 输出: {"name":"foo"}

.. deprecated:: 4.2

    在Symfony 4.2中弃用使用
    :method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setIgnoredAttributes`
    方法，可以使用 ``ignored_attributes`` 选项作为替代方法。

.. _component-serializer-converting-property-names-when-serializing-and-deserializing:

在序列化和反序列化时转换属性名称
------------------------------------------------------------

有时，序列化后的属性的名称必须与PHP类的属性或 getter/setter 方法不同。

Serializer组件提供了一种将PHP字段名称转换或映射为已序列化名称的便捷方法：名称转换器系统。

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

自定义名称转换器可以通过将其作为任何继承
:class:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer`
的类的第二个参数传递来使用，包括
:class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer` 和
:class:`Symfony\\Component\\Serializer\\Normalizer\\PropertyNormalizer`::

    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $nameConverter = new OrgPrefixNameConverter();
    $normalizer = new ObjectNormalizer(null, $nameConverter);

    $serializer = new Serializer([$normalizer], [new JsonEncoder()]);

    $company = new Company();
    $company->name = 'Acme Inc.';
    $company->address = '123 Main Street, Big City';

    $json = $serializer->serialize($company, 'json');
    // {"org_name": "Acme Inc.", "org_address": "123 Main Street, Big City"}
    $companyCopy = $serializer->deserialize($json, Company::class, 'json');
    // 与 $company 一样的数据

.. note::

    你还可以通过实现
    :class:`Symfony\\Component\\Serializer\\NameConverter\\AdvancedNameConverterInterface`
    来访问当前的类名、格式和上下文。

.. _using-camelized-method-names-for-underscored-attributes:

驼峰拼写法和蛇形拼写法的转换
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在许多格式中，通常使用下划线来分隔单词（也称为蛇形拼写法）。
但是，在Symfony应用中，使用驼峰拼写法来命名属性是很常见的（即使 `PSR-1标准`_ 并未建议属性名称的任何特定用例）。

Symfony提供了一个内置的名称转换器，用于在序列化和反序列化过程中在蛇形拼写法和驼峰拼写法样式之间进行转换::

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

    $anne = $normalizer->denormalize(['first_name' => 'Anne'], 'Person');
    // 附有 firstName 为 'Anne' 的 Person 对象

使用元数据配置名称转换
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如 :ref:`属性组章节 <component-serializer-attributes-groups>`
中所述，在Symfony应用中使用此组件并且类元数据工厂被启用时，这些已经设置好了，你只需要提供配置。否则::

    // ...
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\NameConverter\MetadataAwareNameConverter;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));

    $metadataAwareNameConverter = new MetadataAwareNameConverter($classMetadataFactory);

    $serializer = new Serializer(
        [new ObjectNormalizer($classMetadataFactory, $metadataAwareNameConverter)],
        ['json' => new JsonEncoder()]
    );

现在配置你的名称转换映射。考虑一个定义了具有 ``firstName`` 属性的 ``Person`` 实体的应用：

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
                https://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
        >
            <class name="App\Entity\Person">
                <attribute name="firstName" serialized-name="customer_name"/>
            </class>
        </serializer>

此自定义映射用于在序列化和反序列化对象时转换属性的名称::

    $serialized = $serializer->serialize(new Person("Kévin"));
    // {"customer_name": "Kévin"}

序列化布尔属性
------------------------------

如果你使用的是isser方法（前缀为 ``is`` 的方法，类似于
``App\Model\Person::isSportsperson()``），则序列化器组件将自动检测并使用它来序列化相关属性。

``ObjectNormalizer`` 还需要照顾以 ``has``、``add`` 和 ``remove`` 开头的方法。

使用回调来通过对象实例来序列化属性
-------------------------------------------------------------

.. deprecated:: 4.2

    自Symfony 4.2起弃用
    :method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setCallbacks`
    方法。改为使用上下文的 ``callbacks`` 键。

序列化时，你可以设置一个回调来格式化特定的对象属性::

    use App\Model\Person;
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $encoder = new JsonEncoder();

    // all callback parameters are optional (you can omit the ones you don't use)
    $dateCallback = function ($innerObject, $outerObject, string $attributeName, string $format = null, array $context = []) {
        return $innerObject instanceof \DateTime ? $innerObject->format(\DateTime::ISO8601) : '';
    };

    $defaultContext = [
        AbstractNormalizer::CALLBACKS => [
            'createdAt' => $callback,
        ],
    ];
    
    $normalizer = new GetSetMethodNormalizer(null, null, null, null, null, $defaultContext);

    $serializer = new Serializer([$normalizer], [$encoder]);

    $person = new Person();
    $person->setName('cordoval');
    $person->setAge(34);
    $person->setCreatedAt(new \DateTime('now'));

    $serializer->serialize($person, 'json');
    // 输出: {"name":"cordoval", "age": 34, "createdAt": "2014-03-22T09:43:12-0500"}

.. deprecated:: 4.2

    自Symfony 4.2起弃用
    :method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setCallbacks`
    方法。改为使用上下文的 ``callbacks`` 键。

.. _component-serializer-normalizers:

Normalizers
-----------

有几种类型的normalizer可用：

:class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer`
    此normalizer利用 :doc:`PropertyAccess组件 </components/property_access>`
    来读取和写入对象。
    这意味着它可以直接访问属性，也可以通过getter、setter、hasser、adder和remover访问来属性。
    它支持在反规范化过程中调用构造函数。

    对象被规范化为属性名和值的映射（生成的名称从方法名中删除了 ``get``、``set``、``has``、``is``
    或 ``remove`` 前缀，并将第一个字母小写；例如 ``getFirstName()`` -> ``firstName``）。

    ``ObjectNormalizer`` 是最强大的normalizer。
    在启用了Serializer组件的Symfony应用中默认配置了它。

:class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer`
    此normalizer通过调用“getters”（以“get”开头的公共方法）来读取类的内容。
    它将通过调用构造函数和“setter”（以“set”开头的公共方法）来对数据进行反规范化。

    对象被规范化为属性名称和值的映射（生成名称时从方法名称中删除了 ``get``
    前缀，并将第一个字母小写；例如，``getFirstName()`` -> ``firstName``）。

:class:`Symfony\\Component\\Serializer\\Normalizer\\PropertyNormalizer`
    此normalizer直接读取和写入公共属性以及 **私有和受保护** 的属性（来自类及其所有父类）。
    它支持在反规范化过程中调用构造函数。

    对象被规范化为属性名到属性值的映射。

:class:`Symfony\\Component\\Serializer\\Normalizer\\JsonSerializableNormalizer`
    此normalizer适用于实现了 :phpclass:`JsonSerializable` 的类。

    它将调用 :phpmethod:`JsonSerializable::jsonSerialize` 方法，然后进一步规范化结果。
    这意味着嵌套的 :phpclass:`JsonSerializable` 类也将被规范化。

    当你希望逐步从使用简单的 :phpfunction:`json_encode`
    的现有代码库迁移到Symfony Serializer时，此normalizer特别有用，因为它允许你混合(mix)哪些规范化器用于哪些类。

    与 :phpfunction:`json_encode` 不同，此normalizer可以处理循环引用。

:class:`Symfony\\Component\\Serializer\\Normalizer\\DateTimeNormalizer`
    此normalizer将 :phpclass:`DateTimeInterface` 对象（例如 :phpclass:`DateTime` 和
    :phpclass:`DateTimeImmutable`）转换为字符串。默认情况下，它使用 RFC3339_ 格式。

:class:`Symfony\\Component\\Serializer\\Normalizer\\DataUriNormalizer`
    此normalizer将 :phpclass:`SplFileInfo`
    对象转换为一个data URI格式的字符串（``data:...``），以便可以将文件嵌入到序列化数据中。

:class:`Symfony\\Component\\Serializer\\Normalizer\\DateIntervalNormalizer`
    此normalizer将 :phpclass:`DateInterval` 对象转换为字符串。
    默认情况下，它使用 ``P%yY%mM%dDT%hH%iM%sS`` 格式。

:class:`Symfony\\Component\\Serializer\\Normalizer\\ConstraintViolationListNormalizer`
    此normalizer根据 `RFC 7807`_ 标准将实现
    :class:`Symfony\\Component\\Validator\\ConstraintViolationListInterface`
    的对象转换为一个错误列表。

.. _component-serializer-encoders:

编码器
--------

编码器将 **数组** 转换为特定 **格式**，反之亦然。
它们实现了用于编码（从数组到格式）的
:class:`Symfony\\Component\\Serializer\\Encoder\\EncoderInterface`
和用于解码（从格式到数组）的
:class:`Symfony\\Component\\Serializer\\Encoder\\DecoderInterface`。

通过使用序列化器实例的第二个构造函数参数，可以将新的编码器添加到该实例::

    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Encoder\XmlEncoder;
    use Symfony\Component\Serializer\Serializer;

    $encoders = [new XmlEncoder(), new JsonEncoder()];
    $serializer = new Serializer([], $encoders);

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

.. deprecated:: 4.2

    从Symfony 4.2开始，弃用对默认值 ``false`` 的依赖。

``XmlEncoder``
~~~~~~~~~~~~~~~~~~

该编码器将数组转换为XML，反之亦然。

例如，如下所示将一个对象规范化（normalized）::

    ['foo' => [1, 2], 'bar' => true];

``XmlEncoder`` 会将该对象编码为这个样子::

    <?xml version="1.0"?>
    <response>
        <foo>1</foo>
        <foo>2</foo>
        <bar>1</bar>
    </response>

请注意，此编码器将把以 ``@`` 开头的键视为属性，并将使用 ``#comment`` 键来编码XML注释::

    $encoder = new XmlEncoder();
    $encoder->encode([
        'foo' => ['@bar' => 'value'],
        'qux' => ['#comment' => 'A comment'],
    ], 'xml');
    // will return:
    // <?xml version="1.0"?>
    // <response>
    //     <foo bar="value"/>
    //     <qux><!-- A comment --!><qux>
    // </response>

你可以传递上下文键 ``as_collection``，以便始终将结果作为一个集合。

.. tip::

    解码内容时，默认情况下会忽略XML注释，但可以使用 ``XmlEncoder`` 的类构造函数的可选
    ``$decoderIgnoredNodeTypes`` 参数来更改此行为。

    默认情况下，带 ``#comment`` 键的数据会被编码为XML注释。但可以使用 ``XmlEncoder``
    的类构造函数的可选 ``$encoderIgnoredNodeTypes`` 参数进行更改。

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
    $organization->setMembers([$member]);

    $member->setOrganization($organization);

    echo $serializer->serialize($organization, 'json'); // Throws a CircularReferenceException

默认上下文中的 ``circular_reference_limit``
键设置在将同一对象视为循环引用之前将序列化该对象的次数。默认值为“1”。

循环引用也可以由自定义的可调用对象处理，而不是抛出异常。
在序列化具有唯一标识符的实体时，这尤其有用::

    $encoder = new JsonEncoder();
    $defaultContext = [
        AbstractNormalizer::CIRCULAR_REFERENCE_HANDLER => function ($object, $format, $context) {
            return $object->getName();
        },
    ];
    $normalizer = new ObjectNormalizer(null, null, null, null, null, null, $defaultContext);

    $serializer = new Serializer([$normalizer], [$encoder]);
    var_dump($serializer->serialize($org, 'json'));
    // {"name":"Les-Tilleuls.coop","members":[{"name":"K\u00e9vin", organization: "Les-Tilleuls.coop"}]}

.. deprecated:: 4.2

    自Symfony 4.2起弃用
    :method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setCircularReferenceHandler`
    方法。改为使用上下文的 ``circular_reference_handler`` 键。

处理序列化深度
----------------------------

Serializer组件能够检测并限制序列化的深度。在序列化大型树时尤其有用。假设以下数据结构::

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

序列化器可以为一个给定属性设置一个最大深度。在这里，我们将
``$child`` 属性设置为 ``2``：

.. configuration-block::

    .. code-block:: php-annotations

        namespace Acme;

        use Symfony\Component\Serializer\Annotation\MaxDepth;

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
                https://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
        >
            <class name="Acme\MyObj">
                <attribute name="child" max-depth="2"/>
            </class>
        </serializer>

必须配置与所选格式对应的元数据加载器才能使用此功能。
在使用Serializer组件的Symfony应用中会自动完成此配置。
使用独立组件时，请参阅 :ref:`组 <component-serializer-attributes-groups>`
章节以了解如何执行此操作。

只有在序列化器上下文的 ``enable_max_depth`` 键设置为 ``true`` 时，才会进行检查。
在以下示例中，第三级未序列化，因为它比配置的最大深度 2 更深::

    $result = $serializer->normalize($level1, null, ['enable_max_depth' => true]);
    /*
    $result = [
        'foo' => 'level1',
        'child' => [
            'foo' => 'level2',
            'child' => [
                'child' => null,
            ],
        ],
    ];
    */

当达到最大深度时，可以执行自定义可调用对象而不是抛出异常。
在序列化具有唯一标识符的实体时，这尤其有用::

    use Doctrine\Common\Annotations\AnnotationReader;
    use Symfony\Component\Serializer\Annotation\MaxDepth;
    use Symfony\Component\Serializer\Mapping\Factory\ClassMetadataFactory;
    use Symfony\Component\Serializer\Mapping\Loader\AnnotationLoader;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

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

    // 所有回调参数都是可选的（可以省略不使用的参数）
    $maxDepthHandler = function ($innerObject, $outerObject, string $attributeName, string $format = null, array $context = []) {
        return '/foos/'.$innerObject->id;
    };

    $defaultContext = [
        AbstractObjectNormalizer::MAX_DEPTH_HANDLER => $maxDepthHandler,
    ];
    $normalizer = new ObjectNormalizer($classMetadataFactory, null, null, null, null, null, $defaultContext);

    $serializer = new Serializer([$normalizer]);

    $result = $serializer->normalize($level1, null, [ObjectNormalizer::ENABLE_MAX_DEPTH => true]);
    /*
    $result = [
        'id' => 1,
        'child' => [
            'id' => 2,
            'child' => '/foos/3',
        ],
    ];
    */

.. deprecated:: 4.2

    自Symfony 4.2起弃用
    :method:`Symfony\\Component\\Serializer\\Normalizer\\AbstractNormalizer::setMaxDepthHandler`
    方法。改为使用上下文的 ``max_depth_handler`` 键。

处理数组
---------------

Serializer组件也能够处理对象数组。序列化数组就像序列化单个对象一样::

    use Acme\Person;

    $person1 = new Person();
    $person1->setName('foo');
    $person1->setAge(99);
    $person1->setSportsman(false);

    $person2 = new Person();
    $person2->setName('bar');
    $person2->setAge(33);
    $person2->setSportsman(true);

    $persons = [$person1, $person2];
    $data = $serializer->serialize($persons, 'json');

    // $data 包含 [{"name":"foo","age":99,"sportsman":false},{"name":"bar","age":33,"sportsman":true}]

如果要反序列化此类结构，则需要将
:class:`Symfony\\Component\\Serializer\\Normalizer\\ArrayDenormalizer`
添加到一组规范化器中。通过附加 ``[]`` 到
:method:`Symfony\\Component\\Serializer\\Serializer::deserialize`
方法的类型参数，可以表示你期望的是数组而不是单个对象::

    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Normalizer\ArrayDenormalizer;
    use Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $serializer = new Serializer(
        [new GetSetMethodNormalizer(), new ArrayDenormalizer()],
        [new JsonEncoder()]
    );

    $data = ...; // 上一示例中的序列化数据
    $persons = $serializer->deserialize($data, 'Acme\Person[]', 'json');

``XmlEncoder``
------------------

该编码器将数组转换为XML，反之亦然。例如，如下所示将对象规范化（normalized）::

    ['foo' => [1, 2], 'bar' => true];

``XmlEncoder`` 会将该对象编码为这个样子：

.. code-block:: xml

    <?xml version="1.0"?>
    <response>
        <foo>1</foo>
        <foo>2</foo>
        <bar>1</bar>
    </response>

以 ``@`` 开头的数组键会被当做XML属性::

    ['foo' => ['@bar' => 'value']];

    // is encoded as follows:
    // <?xml version="1.0"?>
    // <response>
    //     <foo bar="value"/>
    // </response>

使用特殊 ``#`` 键来定义一个节点的数据::

    ['foo' => ['@bar' => 'value', '#' => 'baz']];

    // is encoded as follows:
    // <?xml version="1.0"?>
    // <response>
    //     <foo bar="value">
    //        baz
    //     </foo>
    // </response>

上下文
~~~~~~~

``encode()`` 方法定义了第三个名为 ``context``
的可选参数，该参数定义了XmlEncoder的配置选项的关联数组::

    $xmlEncoder->encode($array, 'xml', $context);

这些是可用的选项：

``xml_format_output``
    如果设置为 ``true``，则生成的XML的格式将使用换行符和缩进。

``xml_version``
    设置XML的版本属性（默认：``1.1``) 。

``xml_encoding``
    设置XML的编码属性（默认值：``utf-8``)。

``xml_standalone``
    在生成的XML中添加独立(standalone)属性（默认值：``true``) 。

``xml_root_node_name``
    设置根节点名称（默认值：``response``)。

``remove_empty_tags``
    如果设置为 ``true``，则删除生成的XML中的所有空标签。

处理构造函数参数
------------------------------

如果类构造函数定义了参数，就像 `值对象`_ 一样，如果缺少某些参数，序列化器将无法创建对象。
在这些情况下，请使用 ``default_constructor_arguments`` 上下文选项::

    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

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
    $serializer = new Serializer([$normalizer]);

    $data = $serializer->denormalize(
        ['foo' => 'Hello'],
        'MyObj',
        ['default_constructor_arguments' => [
            'MyObj' => ['foo' => '', 'bar' => ''],
        ]]
    );
    // $data = new MyObj('Hello', '');

递归反规范化和类型安全
-----------------------------------------

Serializer组件可以使用
:doc:`PropertyInfo组件 </components/property_info>` 来对复杂类型（对象）进行反规范化。
将使用提供的提取器来猜测类属性的类型，并用于递归地反规范化内部数据。

在Symfony应用中使用此组件时，所有规范化器都会自动配置为使用已注册的提取器。
当独立使用此组件时，必须将
:class:`Symfony\\Component\\PropertyInfo\\PropertyTypeExtractorInterface`
的实现（通常是一个
:class:`Symfony\\Component\\PropertyInfo\\PropertyInfoExtractor` 实例）作为
``ObjectNormalizer`` 的第4个参数传递::

    namespace Acme;

    use Symfony\Component\PropertyInfo\Extractor\ReflectionExtractor;
    use Symfony\Component\Serializer\Normalizer\DateTimeNormalizer;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

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
    $serializer = new Serializer([new DateTimeNormalizer(), $normalizer]);

    $obj = $serializer->denormalize(
        ['inner' => ['foo' => 'foo', 'bar' => 'bar'], 'date' => '1988/01/21'],
        'Acme\ObjectOuter'
    );

    dump($obj->getInner()->foo); // 'foo'
    dump($obj->getInner()->bar); // 'bar'
    dump($obj->getDate()->format('Y-m-d')); // '1988-01-21'

当 ``PropertyTypeExtractor``
可用时，规范化器还将检查要反规范化的数据是否与属性的类型匹配（即使对于基元类型也是如此）。
例如，如果提供了一个 ``string``，但属性的类型是 ``int``，则将抛出一个
:class:`Symfony\\Component\\Serializer\\Exception\\UnexpectedValueException`。
属性的类型强制可以通过设置 ``ObjectNormalizer::DISABLE_TYPE_ENFORCEMENT``
序列化上下文选项为 ``true`` 来禁用。
属性的类型强制（enforcement）可以通过将序列化器的上下文选项
``ObjectNormalizer::DISABLE_TYPE_ENFORCEMENT`` 设置为 ``true`` 来禁用。

序列化接口和抽象类
-------------------------------------------

当处理相当相似或共享属性的对象时，可以使用接口或抽象类。
Serializer组件允许你使用 *“鉴别器类映射”* 来序列化和反序列化这些对象。

鉴别器是用于区分可能对象的字段（在已序列化字符串中）。
实际上，在使用Serializer组件时，会将一个
:class:`Symfony\\Component\\Serializer\\Mapping\\ClassDiscriminatorResolverInterface`
的实现传递给 :class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer`。

Serializer组件提供了一个名为
:class:`Symfony\\Component\\Serializer\\Mapping\\ClassDiscriminatorFromClassMetadata`
的 ``ClassDiscriminatorResolverInterface`` 的实现，它使用类元数据工厂和映射配置来序列化和反序列化正确类的对象。

如 :ref:`属性组章节 <component-serializer-attributes-groups>`
中所述，在Symfony应用中使用此组件并且类元数据工厂被启用时，这些已经设置好了，你只需要提供配置。否则::

    // ...
    use Symfony\Component\Serializer\Encoder\JsonEncoder;
    use Symfony\Component\Serializer\Mapping\ClassDiscriminatorFromClassMetadata;
    use Symfony\Component\Serializer\Mapping\ClassDiscriminatorMapping;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;
    use Symfony\Component\Serializer\Serializer;

    $classMetadataFactory = new ClassMetadataFactory(new AnnotationLoader(new AnnotationReader()));

    $discriminator = new ClassDiscriminatorFromClassMetadata($classMetadataFactory);

    $serializer = new Serializer(
        [new ObjectNormalizer($classMetadataFactory, null, null, null, $discriminator)],
        ['json' => new JsonEncoder()]
    );

现在配置你的鉴别器类映射。思考一个应用，该应用定义了一个被 ``GitHubCodeRepository``
和 ``BitBucketCodeRepository`` 类扩展的抽象 ``CodeRepository`` 类：

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
                https://symfony.com/schema/dic/serializer-mapping/serializer-mapping-1.0.xsd"
        >
            <class name="App\CodeRepository">
                <discriminator-map type-property="type">
                    <mapping type="github" class="App\GitHubCodeRepository"/>
                    <mapping type="bitbucket" class="App\BitBucketCodeRepository"/>
                </discriminator-map>
            </class>
        </serializer>

配置完成后，序列化器使用映射来选择正确的类::

    $serialized = $serializer->serialize(new GitHubCodeRepository());
    // {"type": "github"}

    $repository = $serializer->deserialize($serialized, CodeRepository::class, 'json');
    // GitHubCodeRepository的实例

性能
-----------

为了搞清哪些normalizer（或denormalizer）必须被用来处理一个对象，
:class:`Symfony\\Component\\Serializer\\Serializer`
类将在一个循环中调用所有已注册的normalizers（或denormalizers）的
:method:`Symfony\\Component\\Serializer\\Normalizer\\NormalizerInterface::supportsNormalization`
(或 :method:`Symfony\\Component\\Serializer\\Normalizer\\DenormalizerInterface::supportsDenormalization`)。

这些方法的结果可能因序列化的对象、格式和上下文而异。
这就是默认情况下结果 **不被缓存** 的原因，并且可能导致严重的性能瓶颈。

但是，当对象的类型和格式相同时，大多数normalizer（和denormalizer）总是返回相同的结果，因此可以缓存结果。
为此，请使这些normalizer（和denormalizer）实现
:class:`Symfony\\Component\\Serializer\\Normalizer\\CacheableSupportsMethodInterface`
，并在调用
:method:`Symfony\\Component\\Serializer\\Normalizer\\CacheableSupportsMethodInterface::hasCacheableSupportsMethod`
时返回 ``true``。

.. note::

    所有内置的以及 `API Platform`_ 中包含
    :ref:`normalizer 和 denormalizer <component-serializer-normalizers>`
    本身都实现了此接口。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /serializer

.. seealso::

    支持流行的Web API格式（JSON-LD、GraphQL、OpenAPI、HAL、JSONAPI）的
    Symfony Serializer 组件的Normalizer器可作为 `API Platform`_ 项目的一部分提供。

.. seealso::

    Symfony Serializer组件的一个流行替代品是 `JMS serializer`_
    第三方库（``v1.12.0`` 之前的版本是在Apache许可下发布的，因此与GPLv2项目不兼容）。

.. _`PSR-1标准`: https://www.php-fig.org/psr/psr-1/
.. _`JMS serializer`: https://github.com/schmittjoh/serializer
.. _Packagist: https://packagist.org/packages/symfony/serializer
.. _RFC3339: https://tools.ietf.org/html/rfc3339#section-5.8
.. _JSON: http://www.json.org/
.. _XML: https://www.w3.org/XML/
.. _YAML: http://yaml.org/
.. _CSV: https://tools.ietf.org/html/rfc4180
.. _`RFC 7807`: https://tools.ietf.org/html/rfc7807
.. _`值对象`: https://en.wikipedia.org/wiki/Value_object
.. _`API Platform`: https://api-platform.com
