.. index::
   single: Serializer; Custom normalizers

如何创建自定义规范化器
====================================

:doc:`Serializer组件 </components/serializer>` 使用规范化器将任何数据变换成一个数组。
该组件提供了几个 :doc:`内置规范化器 </serializer/normalizers>`，
但你可能需要创建自己的规范化器来转换不支持的数据结构。

创建新的规范化器
-------------------------

想象一下，你希望在序列化过程中添加、修改或删除某些属性。
为此，你必须创建自己的规范化器。
但通常最好让Symfony来规范化对象，然后挂钩到规范化以定制规范化数据。
为此，利用 ``ObjectNormalizer``::

    namespace App\Serializer;

    use App\Entity\Topic;
    use Symfony\Component\Routing\Generator\UrlGeneratorInterface;
    use Symfony\Component\Serializer\Normalizer\NormalizerInterface;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    class TopicNormalizer implements NormalizerInterface
    {
        private $router;
        private $normalizer;

        public function __construct(UrlGeneratorInterface $router, ObjectNormalizer $normalizer)
        {
            $this->router = $router;
            $this->normalizer = $normalizer;
        }

        public function normalize($topic, $format = null, array $context = [])
        {
            $data = $this->normalizer->normalize($topic, $format, $context);

            // 在这里添加、编辑、删除数据:
            $data['href']['self'] = $this->router->generate('topic_show', [
                'id' => $topic->getId(),
            ], UrlGeneratorInterface::ABSOLUTE_URL);

            return $data;
        }

        public function supportsNormalization($data, $format = null)
        {
            return $data instanceof Topic;
        }
    }

在应用中注册
----------------------------------

在Symfony应用中使用此规范化器之前，必须将其注册为服务并 :doc:`标记 </service_container/tags>`
为 ``serializer.normalizer``。
如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，则注册会自动完成！
