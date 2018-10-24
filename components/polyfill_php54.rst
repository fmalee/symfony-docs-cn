.. index::
    single: Polyfill
    single: PHP
    single: Components; Polyfill

Symfony Polyfill / PHP 5.4 组件
========================================

    This component provides some PHP 5.4 features to applications using earlier
    PHP versions.

安装
------------

.. code-block:: terminal

    $ composer require symfony/polyfill-php54

Alternatively, you can clone the `<https://github.com/symfony/polyfill-php54>`_ repository.

.. include:: /components/require_autoload.rst.inc

用法
-----

Once this component is installed in your application, you can use the following
functions, no matter if your PHP version is earlier than PHP 5.4.

Provided Functions
~~~~~~~~~~~~~~~~~~

* :phpfunction:`class_uses`
* :phpfunction:`hex2bin`
* :phpfunction:`session_register_shutdown`
* :phpfunction:`trait_exists`
