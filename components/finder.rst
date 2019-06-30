.. index::
   single: Finder
   single: Components; Finder

Finder组件
====================

    Finder组件通过一个直观流式的接口来根据不同的标准（名称，文件大小，修改时间等）查找文件和目录。

安装
------------

.. code-block:: terminal

    $ composer require symfony/finder

.. include:: /components/require_autoload.rst.inc

用法
-----

:class:`Symfony\\Component\\Finder\\Finder` 类查找的文件或目录::

    use Symfony\Component\Finder\Finder;

    $finder = new Finder();
    // 找到当前目录中的所有文件
    $finder->files()->in(__DIR__);

    // 检查是否有任何搜索结果
    if ($finder->hasResults()) {
        // ...
    }

    foreach ($finder as $file) {
        $absoluteFilePath = $file->getRealPath();
        $fileNameWithExtension = $file->getRelativePathname();

        // ...
    }

``$file`` 变量是一个扩展PHP的 :phpclass:`SplFileInfo` 的一个
:class:`Symfony\\Component\\Finder\\SplFileInfo` 实例的，以提供处理相对路径的方法。

.. caution::

    ``Finder`` 对象不会自动重置其内部状态。这意味着如果你不希望得到混合(mixed)结果，则需要创建一个新实例。

搜索文件和目录
-----------------------------------

该组件提供了许多方法来定义搜索条件。它们都可以链式操作，因为它们实现了一个 `流式接口`_。

位置
~~~~~~~~

位置是唯一的强制性条件。它告诉finder用于搜索的目录::

    $finder->in(__DIR__);

通过链式调用 :method:`Symfony\\Component\\Finder\\Finder::in` 来搜索多个位置::

    // 在*两个*目录里面搜索
    $finder->in([__DIR__, '/elsewhere']);

    // 与上述相同
    $finder->in(__DIR__)->in('/elsewhere');

使用 ``*`` 作为通配符来搜索匹配一个模式的目录（每个模式必须解析为至少一个目录路径）::

    $finder->in('src/Symfony/*/*/Resources');

使用 :method:`Symfony\\Component\\Finder\\Finder::exclude`
方法来排除匹配的目录::

    // 作为参数传递的目录必须相对于使用 in() 方法定义的目录
    $finder->in(__DIR__)->exclude('ruby');

也可以忽略你无权读取的目录::

    $finder->ignoreUnreadableDirs()->in(__DIR__);

由于Finder使用PHP迭代器，你可以使用支持的 ``PHP wrapper for URL-style protocols`_
(``ftp://``、``zlib://`` 等等)来传递任何URL::

    // 在FTP根目录中查找时始终添加一个尾斜杠
    $finder->in('ftp://example.com/');

    // 你也可以在FTP目录中查找
    $finder->in('ftp://example.com/pub/');

它也适用于用户定义的流::

    use Symfony\Component\Finder\Finder;

    // register a 's3://' wrapper with the official AWS SDK
    $s3Client = new Aws\S3\S3Client([/* config options */]);
    $s3Client->registerStreamWrapper();

    $finder = new Finder();
    $finder->name('photos*')->size('< 100K')->date('since 1 hour ago');
    foreach ($finder->in('s3://bucket-name') as $file) {
        // ... 对文件做一些事情
    }

.. seealso::

    阅读 `PHP流`_ 文档以了解如何创建自己的流。

文件和目录
~~~~~~~~~~~~~~~~~~~~

默认情况下，Finder返回文件和目录; 但可以使用
:method:`Symfony\\Component\\Finder\\Finder::files` 和
:method:`Symfony\\Component\\Finder\\Finder::directories` 方法来控制该行为::

    // 只查找文件; 忽略目录
    $finder->files();

    // 只查找目录; 忽略文件
    $finder->directories();

如果要关注 `软连接`_，请使用 ``followLinks()`` 方法::

    $finder->files()->followLinks();

版本控制文件
~~~~~~~~~~~~~~~~~~~~~

`版本控制系统`_（简称“VCS”），例如Git和Mercurial，会创建一些特殊文件来存储其元数据。
查找文件和目录时，默认情况下会忽略这些文件，但你可以使用 ``ignoreVCS()`` 方法更改此模式::

    $finder->ignoreVCS(false);

如果搜索目录包含一个 ``.gitignore`` 件，你可以重用这些规则，使用
:method:`Symfony\\Component\\Finder\\Finder::ignoreVCSIgnored` 
方法从结果中排除文件和目录::

    // 排除与 .gitignore 模式匹配的文件/目录
    $finder->ignoreVCSIgnored(true);

.. versionadded:: 4.3

    Symfony 4.3中引入了 ``ignoreVCSIgnored()`` 方法。

文件名称
~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::name` 方法按名称限制文件::

    $finder->files()->name('*.php');

``name()`` 方法接受globs、字符串、正则表达式或数组形式的globs、字符串、正则表达式::

    $finder->files()->name('/\.php$/');

可以通过链式调用或传递一个数组来定义多个文件名::

    $finder->files()->name('*.php')->name('*.twig');

    // 与上述相同
    $finder->files()->name(['*.php', '*.twig']);

``notName()`` 方法排除匹配一个模式的文件::

    $finder->files()->notName('*.rb');

可以通过链式调用或传递一个数组来排除多个文件名::

    $finder->files()->notName('*.rb')->notName('*.py');

    // 与上述相同
    $finder->files()->notName(['*.rb', '*.py']);

文件内容
~~~~~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::contains`
方法来按内容查找文件::

    $finder->files()->contains('lorem ipsum');

``contains()`` 方法接受字符串或正则表达式::

    $finder->files()->contains('/lorem\s+ipsum$/i');

``notContains()`` 方法排除包含一个给定模式的文件::

    $finder->files()->notContains('dolor sit amet');

路径
~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::path
方法来按路径查找文件和目录::

    // 匹配其路径（文件或目录）中的任何位置包含“data”的文件
    $finder->path('data');
    // 例如，如果它们存在的话，则匹配 data/*.xml 和 data.xml
    $finder->path('data')->name('*.xml');

在所有平台（包括Windows）上使用正斜杠（即 ``/``）作为目录分隔符。该组件会在内部进行必要的转换。

``path()`` 方法接受字符串、正则表达式、一个字符串、常量表达式的数组::

    $finder->path('foo/bar');
    $finder->path('/^foo\/bar/');

可以通过链式调用或传递一个数组来定义多个路径::

    $finder->path('data')->path('foo/bar');

    // 与上述相同
    $finder->path(['data', 'foo/bar']);

在内部，通过转义斜杠和添加分隔符来将字符串转换为正则表达式：

=====================  =======================
原始给定字符串           使用正则表达式
=====================  =======================
``dirname``            ``/dirname/``
``a/b/c``              ``/a\/b\/c/``
=====================  =======================

:method:`Symfony\\Component\\Finder\\Finder::notPath` 方法按路径排除文件::

    $finder->notPath('other/dir');

链式调用或传递一个数组可以排除多个路径::

    $finder->notPath('first/dir')->notPath('other/dir');

    // 与上述相同
    $finder->notPath(['first/dir', 'other/dir']);

.. versionadded:: 4.2

    在Symfony 4.2中引入了对传递数组到 ``notPath()`` 的支持

文件大小
~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::size`
方法来按大小查找文件::

    $finder->files()->size('< 1.5K');

通过链式调用或传递一个数组来限制大小范围::

    $finder->files()->size('>= 1K')->size('<= 2K');

    // 与上述相同
    $finder->files()->size(['>= 1K', '<= 2K']);

比较运算符可以是下列任何一项：``>``、``>=``、``<``、``<=``、``==``、``!=``。

目标值可以使用千字节（``k``、``ki``）、兆字节（``m``、``mi``）或千兆字节（``g``、``gi``）的大小。
后缀为 ``i`` 的话，会根据 `IEC标准`_ 来使用适当的 ``2**n`` 版本。

文件日期
~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::date`
方法按上次修改日期来查找文件::

    $finder->date('since yesterday');

通过链式调用或传递一个数组来限制日期范围::

    $finder->date('>= 2018-01-01')->date('<= 2018-12-31');

    // 与上述相同
    $finder->date(['>= 2018-01-01', '<= 2018-12-31']);

比较运算符可以是下列任何一项：``>``、``>=``、``<``、``<=``、``==``。
你还可以使用 ``since`` 或 ``after`` 作为 ``>`` 的别名；使用
``until`` 或 ``before`` 作为 ``<`` 的别名。

目标值可以是 :phpfunction:`strtotime` 支持的任何日期。

目录深度
~~~~~~~~~~~~~~~

默认情况下，Finder以递归方式遍历目录。可以通过
:method:`Symfony\\Component\\Finder\\Finder::depth` 方法来限制遍历的深度::

    $finder->depth('== 0');
    $finder->depth('< 3');

通过链式调用或传递一个数组来限制深度范围::

    $finder->depth('> 2')->depth('< 5');

    // 与上述相同
    $finder->depth(['> 2', '< 5']);

自定义过滤
~~~~~~~~~~~~~~~~

要使用你自己的策略来过滤结果，请使用
:method:`Symfony\\Component\\Finder\\Finder::filter`::

    $filter = function (\SplFileInfo $file)
    {
        if (strlen($file) > 10) {
            return false;
        }
    };

    $finder->files()->filter($filter);

``filter()`` 方法使用一个闭包作为其参数。
对于每个匹配的文件，使用该文件作为 :class:`Symfony\\Component\\Finder\\SplFileInfo`
实例来调用它。如果闭包返回 ``false``，则从结果集中排除该文件。

排序结果
---------------

按名称或类型（首先是目录，然后是文件）对结果进行排序::

    $finder->sortByName();

    $finder->sortByType();

.. tip::

    默认情况下，``sortByName()`` 方法使用 :phpfunction:`strcmp` PHP函数（例如
    ``file1.txt``、``file10.txt``、``file2.txt``）。
    传递 ``true`` 作为它的参数可以使用PHP的 `自然排序`_
    算法来代替（例如 ``file1.txt``、``file2.txt``、``file10.txt``）。

按上次访问、更改或编辑的时间对文件和目录进行排序::

    $finder->sortByAccessedTime();

    $finder->sortByChangedTime();

    $finder->sortByModifiedTime();

你还可以使用 ``sort()`` 方法来定义自己的排序算法::

    $finder->sort(function (\SplFileInfo $a, \SplFileInfo $b) {
        return strcmp($a->getRealPath(), $b->getRealPath());
    });

你可以使用 ``reverseSorting()`` 方法来反转任何排序::

    // 结果将被排序为“Z到A”而不是默认的“A到Z”
    $finder->sortByName()->reverseSorting();

.. note::

    请注意，这些 ``sort*`` 方法需要获取所有匹配的元素以完成它们的工作。对于大型迭代器，它会很慢。

将结果转换为数组
--------------------------------

Finder实例是一个 :phpclass:`IteratorAggregate` PHP类。因此，除了使用 ``foreach``
来迭代Finder结果之外，你还可以使用 :phpfunction:`iterator_to_array`
函数将其转换为一个数组，或者使用 :phpfunction:`iterator_count` 来获取项目数量。

如果你调用 :method:`Symfony\\Component\\Finder\\Finder::in`
方法在多个位置进行搜索不止一次，传递 ``false`` 作为 :phpfunction:`iterator_to_array` 的第二个参数，以避免出现问题（每个位置都会创建一个单独的迭代器，如果你不传递 ``false`` 到 :phpfunction:`iterator_to_array`，得到的结果集的其中一些键可能是重复的并且它们的值已被覆盖）。

读取返回文件的内容
----------------------------------

返回文件的内容可以通过使用
:method:`Symfony\\Component\\Finder\\SplFileInfo::getContents` 方法来读取::

    use Symfony\Component\Finder\Finder;

    $finder = new Finder();
    $finder->files()->in(__DIR__);

    foreach ($finder as $file) {
        $contents = $file->getContents();

        // ...
    }

.. _`流式接口`: https://en.wikipedia.org/wiki/Fluent_interface
.. _`软连接`: https://en.wikipedia.org/wiki/Symbolic_link
.. _`版本控制系统`: https://en.wikipedia.org/wiki/Version_control
.. _`PHP wrapper for URL-style protocols`: https://php.net/manual/en/wrappers.php
.. _`PHP流`: https://php.net/streams
.. _`IEC标准`: https://physics.nist.gov/cuu/Units/binary.html
.. _`Packagist`: https://packagist.org/packages/symfony/finder
.. _`自然排序`: https://en.wikipedia.org/wiki/Natural_sort_order
