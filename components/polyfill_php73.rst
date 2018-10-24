.. index::
    single: Polyfill
    single: PHP
    single: Components; Polyfill

Symfony Polyfill / PHP 7.3 组件
========================================

    This component provides some PHP 7.3 features to applications using earlier
    PHP versions.

安装
------------

.. code-block:: terminal

    $ composer require symfony/polyfill-php73

Alternatively, you can clone the `<https://github.com/symfony/polyfill-php73>`_ repository.

.. include:: /components/require_autoload.rst.inc

用法
-----

Once this component is installed in your application, you can use the following
functions, no matter if your PHP version is earlier than PHP 7.3.

Provided Functions
~~~~~~~~~~~~~~~~~~

* :phpfunction:`is_countable`
