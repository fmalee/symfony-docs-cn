.. index::
   single: Serializer; Custom encoders

如何创建自定义编码器
=================================

在 :doc:`Serializer组件 </components/serializer>` 使用正规化来将任何数据转换成一个数组。
然后，通过利用 *编码器*，可以将该数据转换为任何数据结构（例如JSON）。

该组件提供了几个内置编码器，这些编码器在 :doc:`serializer组件 </components/serializer>`
中有描述 ，但你可能希望使用另一个不受支持的结构。

创建新编码器
----------------------

想象一下，你想序列化和反序列化YAML。
为此，你必须创建自己的使用 :doc:`Yaml组件 </components/yaml>` 的编码器::

    namespace App\Serializer;

    use Symfony\Component\Serializer\Encoder\DecoderInterface;
    use Symfony\Component\Serializer\Encoder\EncoderInterface;
    use Symfony\Component\Yaml\Yaml;

    class YamlEncoder implements EncoderInterface, DecoderInterface
    {
        public function encode($data, $format, array $context = [])
        {
            return Yaml::dump($data);
        }

        public function supportsEncoding($format)
        {
            return 'yaml' === $format;
        }

        public function decode($data, $format, array $context = [])
        {
            return Yaml::parse($data);
        }

        public function supportsDecoding($format)
        {
            return 'yaml' === $format;
        }
    }

.. tip::

    如果你需要在你的 ``supportsDecoding`` 或 ``supportsEncoding`` 方法中访问
    ``$context``，请确保其相应的实现了
    ``Symfony\Component\Serializer\Encoder\ContextAwareDecoderInterface``
    或 ``Symfony\Component\Serializer\Encoder\ContextAwareEncoderInterface``。

在应用中注册
--------------------------

如果你使用Symfony框架。那么你可能想在你的应用中将此编码器注册为服务。
如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，则注册将会自动完成！

.. tip::

    如果你没有使用 :ref:`自动配置 <service_autoconfigure>`，
    请务必将你的类注册为服务并标记为 ``serializer.encoder``。

现在，你将能够序列化和反序列化YAML！

.. _tracker: https://github.com/symfony/symfony/issues
