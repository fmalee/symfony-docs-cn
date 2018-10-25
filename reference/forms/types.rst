.. index::
   single: Forms; Types Reference

表单类型参考
====================

.. toctree::
   :maxdepth: 1
   :hidden:

   types/text
   types/textarea
   types/email
   types/integer
   types/money
   types/number
   types/password
   types/percent
   types/search
   types/url
   types/range
   types/tel
   types/color

   types/choice
   types/entity
   types/country
   types/language
   types/locale
   types/timezone
   types/currency

   types/date
   types/dateinterval
   types/datetime
   types/time
   types/birthday

   types/checkbox
   types/file
   types/radio

   types/collection
   types/repeated

   types/hidden

   types/button
   types/reset
   types/submit

   types/form

表单由 *字段* 组成，每个字段都是在字段 *类型* 的帮助下构建的（例如 ``TextType``、``ChoiceType`` 等）。 
Symfony标配了可在你的应用中直接使用的大量字段类型。

支持的表单类型
---------------------

以下字段类型在Symfony中是直接可用的：

.. include:: /reference/forms/types/map.rst.inc
