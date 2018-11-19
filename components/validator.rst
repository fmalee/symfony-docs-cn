.. index::
   single: Validator
   single: Components; Validator

Validator组件
=======================

    Validator组件提供了验证 `JSR-303 Bean验证规范`_ 后面的值的工具 。

安装
------------

.. code-block:: terminal

    $ composer require symfony/validator

或者，你可以克隆 `<https://github.com/symfony/validator>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

用法
-----

.. seealso::

    阅读 :doc:`/validation` 文档，了解如何验证Symfony应用中的数据和实体。

Validator组件行为基于两个概念：

* 约束，定义要验证的规则;
* 验证器，它是包含实际验证逻辑的类。

以下示例显示如何验证字符串的长度至少为10个字符::

    use Symfony\Component\Validator\Validation;
    use Symfony\Component\Validator\Constraints\Length;
    use Symfony\Component\Validator\Constraints\NotBlank;

    $validator = Validation::createValidator();
    $violations = $validator->validate('Bernhard', array(
        new Length(array('min' => 10)),
        new NotBlank(),
    ));

    if (0 !== count($violations)) {
        // 有错误，现在你可以显示它们
        foreach ($violations as $violation) {
            echo $violation->getMessage().'<br>';
        }
    }

``validate()`` 方法返回违规的列表作为一个实现
:class:`Symfony\\Component\\Validator\\ConstraintViolationListInterface` 的对象。
如果你有许多验证错误，可以通过错误代码过滤它们::

    use Symfony\Bridge\Doctrine\Validator\Constraints\UniqueEntity;

    $violations = $validator->validate(...);
    if (0 !== count($violations->findByCodes(UniqueEntity::NOT_UNIQUE_ERROR))) {
        // 处理此特定错误（显示一些消息，发送邮件等）
    }

.. versionadded:: 3.3
    ``findByCodes()`` 方法是在Symfony 3.3中引入的。

检索一个验证器实例
-------------------------------

:class:`Symfony\\Component\\Validator\\Validator` 类是Validator组件的主要接入点。
要创建此类的新实例，建议使用 :class:`Symfony\\Component\\Validator\\Validation` 类::

    use Symfony\Component\Validator\Validation;

    $validator = Validation::createValidator();

此 ``$validator`` 对象可以验证简单变量，如字符串、数字和数组，但不能验证对象。
为此，请按照下一节中的说明配置 ``Validator`` 类。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /components/validator/*
    /validation
    /validation/*

.. _`JSR-303 Bean验证规范`: http://jcp.org/en/jsr/detail?id=303
.. _Packagist: https://packagist.org/packages/symfony/validator
