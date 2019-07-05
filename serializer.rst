.. index::
   single: Serializer

序列化
=========================

Symfony提供了一个序列化器，用于序列化/反序列化对象和不同格式（例如JSON或XML）。
在使用它之前，请阅读 :doc:`Serializer组件文档 </components/serializer>` 以熟悉其理念以及规范化器和编码器术语。

.. _activating_the_serializer:

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令以在使用之前安装序列化支持：

.. code-block:: terminal

    $ composer require symfony/serializer-pack

使用序列化服务
----------------------------

启用后，序列化器服务可以在你需要的任何服务中注入，也可以在控制器中使用::

    // src/Controller/DefaultController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Serializer\SerializerInterface;

    class DefaultController extends AbstractController
    {
        public function index(SerializerInterface $serializer)
        {
            // 继续阅读用法示例
        }
    }

添加规范化器和编码器
-------------------------------

启用后，``serializer`` 服务将在容器中可用。
它配备了一组有用的 :ref:`编码器 <component-serializer-encoders>` 和
:ref:`规范器 <component-serializer-normalizers>`。

支持以下格式的编码器已启用：

* JSON: :class:`Symfony\\Component\\Serializer\\Encoder\\JsonEncoder`
* XML: :class:`Symfony\\Component\\Serializer\\Encoder\\XmlEncoder`
* CSV: :class:`Symfony\\Component\\Serializer\\Encoder\\CsvEncoder`
* YAML: :class:`Symfony\\Component\\Serializer\\Encoder\\YamlEncoder`

以及以下的规范化器：

* :class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer` 处理典型的数据对象
* :class:`Symfony\\Component\\Serializer\\Normalizer\\DateTimeNormalizer`
  处理实现了 :phpclass:`DateTimeInterface` 接口的对象
* :class:`Symfony\\Component\\Serializer\\Normalizer\\DataUriNormalizer`
  可以在 `Data URIs`_ 中转换 :phpclass:`SplFileInfo` 对象
* :class:`Symfony\\Component\\Serializer\\Normalizer\\JsonSerializableNormalizer`
  处理实现了 :phpclass:`JsonSerializable` 接口的对象
* :class:`Symfony\\Component\\Serializer\\Normalizer\\ArrayDenormalizer`
  使用像 `MyObject[]` （注意 `[]` 后缀）这样的格式来反规范化对象的数组

自定义的 规范化器 和/或 编码器 也可以通过将它们标记为
:ref:`serializer.normalizer <reference-dic-tags-serializer-normalizer>` 和
:ref:`serializer.encoder <reference-dic-tags-serializer-encoder>` 来加载。
也可以设置标签的优先级以决定匹配顺序。

下面是一个关于如何加载 :class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer` 的示例，
当数据对象总是使用getter（``getXxx()``）、issers（``isXxx()``）或hassers（``hasXxx()``
）来读取属性，以及用setter（``setXxx()``）来改变属性时，它是 `ObjectNormalizer` 的更快替代品：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            get_set_method_normalizer:
                class: Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer
                public: false
                tags: [serializer.normalizer]

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="get_set_method_normalizer" class="Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer" public="false">
                    <tag name="serializer.normalizer"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\Serializer\Normalizer\GetSetMethodNormalizer;

        $container->register('get_set_method_normalizer', GetSetMethodNormalizer::class)
            ->setPublic(false)
            ->addTag('serializer.normalizer')
        ;

.. _serializer-using-serialization-groups-annotations:

使用序列化组注释
--------------------------------------

要使用注释，首先通过 SensioFrameworkExtraBundle 添加对它们的支持：

.. code-block:: terminal

    $ composer require sensio/framework-extra-bundle

接下来，将 :ref:`@Groups 注释 <component-serializer-attributes-groups-annotations>`
添加到你的类中，并选择序列化时要使用的组::

    $json = $serializer->serialize(
        $someObject,
        'json', ['groups' => 'group1']
    );

.. tip::

    ``groups`` 键的值可以是单个字符串，也可以是字符串数组。

除 ``@Groups`` 注释外，Serializer组件还支持YAML或XML文件。
存储在以下位置之一的这些文件会自动加载：

* 在 ``config/serializer/`` 目录中的所有 ``*.yaml`` 和 ``*.xml`` 文件。
* Bundle 的 ``Resources/config/`` 目录中的 ``serialization.yaml`` 或 ``serialization.xml`` 文件;
* Bundle 的 ``Resources/config/serialization/`` 目录中的所有 ``*.yaml`` 和 ``*.xml`` 文件。

.. _serializer-enabling-metadata-cache:

配置元数据缓存
------------------------------

序列化器的元数据会被自动缓存，以提高应用的性能。
默认情况下，序列化器使用 ``cache.system`` 缓存池，
该缓存池使用 :ref:`cache.system <reference-cache-system>` 选项配置。

启用名称转换器
-------------------------

可以使用 :ref:`name_converter <reference-serializer-name_converter>` 选项在配置中定义
:ref:`名称转换器 <component-serializer-converting-property-names-when-serializing-and-deserializing>` 服务的使用。

可以使用 ``serializer.name_converter.camel_case_to_snake_case`` 值启用内置的
:ref:`CamelCase 到 snake_case 名称转换器 <using-camelized-method-names-for-underscored-attributes>`：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            # ...
            serializer:
                name_converter: 'serializer.name_converter.camel_case_to_snake_case'

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <framework:config>
            <!-- ... -->
            <framework:serializer name-converter="serializer.name_converter.camel_case_to_snake_case"/>
        </framework:config>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', [
            // ...
            'serializer' => [
                'name_converter' => 'serializer.name_converter.camel_case_to_snake_case',
            ],
        ]);

扩展阅读
---------------------------------

`Api Platform`_ 提供支持以下格式的API系统：

* `JSON-LD`_ along with the `Hydra Core Vocabulary`_
* `OpenAPI`_ v2 (formerly Swagger) and v3
* `GraphQL`_
* `JSON:API`_
* `HAL`_
* JSON
* XML
* YAML
* CSV

它构建于Symfony Framework及其Serializer组件之上。
它提供自定义规范化器和自定义编码器，自定义元数据和缓存系统。

如果你想充分利用Symfony Serializer组件的全部功能，请参阅该Bundle的工作原理。

.. toctree::
    :maxdepth: 1

    serializer/normalizers
    serializer/custom_encoders
    serializer/custom_normalizer

.. _`APCu`: https://github.com/krakjoe/apcu
.. _`API Platform`: https://api-platform.com
.. _`JSON-LD`: http://json-ld.org
.. _`Hydra Core Vocabulary`: http://hydra-cg.com
.. _`OpenAPI`: https://www.openapis.org
.. _`GraphQL`: https://graphql.org
.. _`JSON:API`: https://jsonapi.org
.. _`HAL`: http://stateless.co/hal_specification.html
.. _`Data URIs`: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Data_URIs
