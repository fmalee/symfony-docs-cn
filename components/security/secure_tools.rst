安全地生成随机值
=================================

Symfony Security组件附带了一系列与安全性相关的优秀工具。
这些实用工具由Symfony使用，但如果你想解决它们已解决的问题，也应该使用它们。

.. note::

    本文中描述的函数是在PHP 5.6或PHP 7中引入的。
    对于较旧的PHP版本，`Symfony Polyfill组件`_ 提供了一个 polyfill。

比较字符串
~~~~~~~~~~~~~~~~~

比较两个字符串所需的时间取决于它们的差异。
当两个字符串代表密码时，攻击者可以使用它; 这被称为 `时序攻击`_。

比较两个密码时，你应该使用 :phpfunction:`hash_equals` 函数::

    if (hash_equals($knownString, $userInput)) {
        // ...
    }

生成安全的随机字符串
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

每当你需要生成安全的随机字符串时，强烈建议你使用 :phpfunction:`random_bytes` 函数::

    $random = random_bytes(10);

该函数返回一个适用于加密使用的随机字符串，字节的数量可以作为一个参数传递（在上例中为10）。

.. tip::

    ``random_bytes()`` 函数返回一个可能包含 ``\0`` 字符的二进制字符串。
    这可能会在几种常见情况下造成麻烦，例如将此值存储在数据库中或将其作为URL的一部分包含在内。
    解决方案是使用一个哈希函数（例如 :phpfunction:`md5` 或 :phpfunction:`sha1`）哈希 ``random_bytes()`` 返回的值。

生成安全的随机数字
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果需要生成一个加密的安全的随机整数，则应使用 :phpfunction:`random_int` 函数::

    $random = random_int(1, 10);

.. _`时序攻击`: https://en.wikipedia.org/wiki/Timing_attack
.. _`Symfony Polyfill组件`: https://github.com/symfony/polyfill
