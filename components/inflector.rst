.. index::
   single: Inflector
   single: Components; Inflector

Inflector组件
=======================

    Inflector组件在单数和复数形式之间转换英语单词。

安装
------------

.. code-block:: terminal

    $ composer require symfony/inflector

.. include:: /components/require_autoload.rst.inc

当你需要一个转换器时
------------------------------

在某些场景中，例如代码生成和代码内省，通常需要将单词在单复数之间转换。
例如，如果你需要知道哪个属性与一个 *adder*
方法相关联，则必须从复数转换为单数（``addStories()`` 方法 -> ``$story`` 属性）。

虽然大多数人类语言都定义了简单的复数规则，但它们也定义了许多例外。
例如，英语的一般规则是在单词的末尾添加一个``s``（``book`` ->
``books``），但即使是常用单词也有很多例外（``woman`` -> ``women``、``life`` ->
``lives``、``news`` -> ``news``、``radius`` -> ``radii`` 等等）。

此组件抽象所有这些复数规则，以便你可以放心地在单复数之间转换。
但是，由于人类语言的复杂性，该组件仅提供对英语的支持。

用法
-----

Inflector组件提供两种静态方法，用于在单复数之间进行转换::

    use Symfony\Component\Inflector\Inflector;

    Inflector::singularize('alumni');   // 'alumnus'
    Inflector::singularize('knives');   // 'knife'
    Inflector::singularize('mice');     // 'mouse'

    Inflector::pluralize('grandchild'); // 'grandchildren'
    Inflector::pluralize('news');       // 'news'
    Inflector::pluralize('bacterium');  // 'bacteria'

有时，无法确定给定单词的唯一单数/复数形式。在这种情况下，方法会返回一个包含所有可能形式的数组::

    use Symfony\Component\Inflector\Inflector;

    Inflector::singularize('indices'); // ['index', 'indix', 'indice']
    Inflector::singularize('leaves');  // ['leaf', 'leave', 'leaff']

    Inflector::pluralize('matrix');    // ['matricies', 'matrixes']
    Inflector::pluralize('person');    // ['persons', 'people']
