验证约束参考
================================

.. toctree::
   :maxdepth: 1
   :hidden:

   constraints/NotBlank
   constraints/Blank
   constraints/NotNull
   constraints/IsNull
   constraints/IsTrue
   constraints/IsFalse
   constraints/Type

   constraints/Email
   constraints/Length
   constraints/Url
   constraints/Regex
   constraints/Ip
   constraints/Uuid

   constraints/EqualTo
   constraints/NotEqualTo
   constraints/IdenticalTo
   constraints/NotIdenticalTo
   constraints/LessThan
   constraints/LessThanOrEqual
   constraints/GreaterThan
   constraints/GreaterThanOrEqual
   constraints/Range
   constraints/DivisibleBy

   constraints/Date
   constraints/DateTime
   constraints/Time

   constraints/Choice
   constraints/Collection
   constraints/Count
   constraints/UniqueEntity
   constraints/Language
   constraints/Locale
   constraints/Country

   constraints/File
   constraints/Image

   constraints/CardScheme
   constraints/Currency
   constraints/Luhn
   constraints/Iban
   constraints/Bic
   constraints/Isbn
   constraints/Issn

   constraints/Callback
   constraints/Expression
   constraints/All
   constraints/UserPassword
   constraints/Valid
   constraints/Traverse

验证器旨在根据 *约束* 验证对象。
在现实生活中，一个约束可能是：“蛋糕不能被烧掉”。在Symfony中，约束类似：它们是条件为真的断言。

支持的约束
---------------------

Symfony中可以使用以下约束：

.. include:: /reference/constraints/map.rst.inc
