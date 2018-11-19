.. index::
   single: Security

Security组件
======================

    Security组件为Web应用提供了完整的安全系统。
    它附带了使用HTTP基本身份验证，交互式表单登录或X.509证书登录进行认证的功能，还允许你实现自己的认证策略。
    此外，该组件还提供了根据角色来授权已认证用户的方法。

安装
------------

.. code-block:: terminal

    $ composer require symfony/security

或者，你可以克隆 `<https://github.com/symfony/security>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

安全组件分为四个较小的可以单独使用的子组件：

``symfony/security-core``
    它提供了所有常见的安全功能，从认证到授权，从编码密码到加载用户。

``symfony/security-http``
    它将核心子组件与HTTP协议集成在一起，以处理HTTP请求和响应。

``symfony/security-csrf``
    它可以防止 `CSRF攻击`_。

.. seealso::

    本文介绍如何在任何PHP应用中将安全功能用作独立组件。
    阅读 :doc:`/security` 文档，了解如何在Symfony应用中使用它。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /components/security/*
    /security
    /security/*
    /reference/configuration/security
    /reference/constraints/UserPassword

.. _Packagist: https://packagist.org/packages/symfony/security
.. _`CSRF攻击`: https://en.wikipedia.org/wiki/Cross-site_request_forgery
