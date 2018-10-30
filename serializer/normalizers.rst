.. index::
   single: Serializer, Normalizers

规范化器
===========

规范化器将\ *对象*\转换为\ *数组*\，反之亦然。
它们实现 :class:`Symfony\\Component\\Serializer\\Normalizers\\NormalizerInterface` 来规范化（对象到数组）和
:class:`Symfony\\Component\\Serializer\\Normalizers\\DenormalizerInterface` 来反规范化（数组到对象）。

在序列化器中启用规范化器，将它们作为第一个参数传递::

    use Symfony\Component\Serializer\Serializer;
    use Symfony\Component\Serializer\Normalizer\ObjectNormalizer;

    $normalizers = array(new ObjectNormalizer());
    $serializer = new Serializer($normalizers);

内置规范化器
--------------------

Symfony包括以下规范化器，但你也可以 :doc:`创建自己的规范化器 </serializer/custom_normalizer>`：

* :class:`Symfony\\Component\\Serializer\\Normalizer\\ObjectNormalizer` 使用
  :doc:`PropertyAccessor组件 </components/property_access>` 规范化PHP对象;
* :class:`Symfony\\Component\\Serializer\\Normalizer\\CustomNormalizer` 使用实现
  ``:class:`Symfony\\Component\\Serializer\\Normalizer\\NormalizableInterface``
  的一个对象来规范化PHP对象
* :class:`Symfony\\Component\\Serializer\\Normalizer\\GetSetMethodNormalizer`
  使用对象的getter和setter方法来规范化PHP对象;
* :class:`Symfony\\Component\\Serializer\\Normalizer\\PropertyNormalizer` 使用
  `PHP反射`_ 来规范化PHP对象。

.. _`PHP反射`: https://php.net/manual/en/book.reflection.php
