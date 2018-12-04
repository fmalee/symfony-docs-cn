.. index::
   single: Finder
   single: Components; Finder

Finder组件
====================

    Finder组件通过直观的流畅界面查找文件和目录。

安装
------------

.. code-block:: terminal

    $ composer require symfony/finder

或者，你可以克隆 `<https://github.com/symfony/finder>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

用法
-----

:class:`Symfony\\Component\\Finder\\Finder` 类查找的文件或目录::

    use Symfony\Component\Finder\Finder;

    $finder = new Finder();
    $finder->files()->in(__DIR__);

    foreach ($finder as $file) {
        // 转储绝对路径
        var_dump($file->getRealPath());

        // 转储文件的相对路径，省略文件名
        var_dump($file->getRelativePath());

        // 转储文件的相对路径，包括文件名
        var_dump($file->getRelativePathname());
    }

``$file`` 是一个 :class:`Symfony\\Component\\Finder\\SplFileInfo`
(继承自PHP自己的 :phpclass:`SplFileInfo`)实例，以提供使用相对路径的方法。

上面的代码以递归方式打印当前目录中的所有文件的名称。
Finder类使用流式接口，因此所有方法都返回该Finder实例。

.. tip::

    Finder实例是一个PHP :phpclass:`IteratorAggregate`。
    因此，除了使用 ``foreach`` 迭代Finder之外，你还可以使用 :phpfunction:`iterator_to_array`
    函数将其转换为数组，或者使用 :phpfunction:`iterator_count` 来获取该项的数量。

.. caution::

    ``Finder`` 对象不会自动重置其内部状态。这意味着如果你不希望得到混合(mixed)结果，则需要创建一个新实例。

.. caution::

    在搜索被传递给 :method:`Symfony\\Component\\Finder\\Finder::in`
    方法的多个位置时，会在内部为每个位置创建一个单独的迭代器。
    这意味着我们会将多个结果集聚合为一个。由于 :phpfunction:`iterator_to_array`
    默认情况下使用结果集的键，因此在转换为数组时，某些键可能会被复制并且其值会被覆盖。
    通过将 ``false`` 传递给 :phpfunction:`iterator_to_array` 的第二个参数可以避免这种情况。

条件
--------

有很多方法可以过滤和排序你的结果。
你还可以使用 :method:`Symfony\\Component\\Finder\\Finder::hasResults`
方法检查是否存在与搜索条件匹配的文件或目录。

位置
~~~~~~~~

位置是唯一的强制性条件。它告诉finder用于搜索的目录::

    $finder->in(__DIR__);

通过链式调用 :method:`Symfony\\Component\\Finder\\Finder::in` 来搜索多个位置::

    // 在*两个*目录里面搜索
    $finder->in(array(__DIR__, '/elsewhere'));

    // 与上述相同
    $finder->in(__DIR__)->in('/elsewhere');

使用通配符来搜索匹配一个模式的目录::

    $finder->in('src/Symfony/*/*/Resources');

每个模式必须解析为至少一个目录路径。

使用 :method:`Symfony\\Component\\Finder\\Finder::exclude` 方法排除匹配的目录::

    // 作为参数传递的目录必须相对于使用 in() 方法定义的目录
    $finder->in(__DIR__)->exclude('ruby');

也可以忽略你无权读取的目录::

    $finder->ignoreUnreadableDirs()->in(__DIR__);

由于Finder使用PHP迭代器，你可以使用支持的 `协议`_ 来传递任何URL::

    // 在FTP根目录中查找时始终添加一个尾斜杠
    $finder->in('ftp://example.com/');

    // 你也可以在FTP目录中查找
    $finder->in('ftp://example.com/pub/');

它也适用于用户定义的流::

    use Symfony\Component\Finder\Finder;

    $s3 = new \Zend_Service_Amazon_S3($key, $secret);
    $s3->registerStreamWrapper('s3');

    $finder = new Finder();
    $finder->name('photos*')->size('< 100K')->date('since 1 hour ago');
    foreach ($finder->in('s3://bucket-name') as $file) {
        // ... 对文件做一些事情
    }

.. note::

    阅读 `Streams`_ 文档以了解如何创建自己的流。

文件和目录
~~~~~~~~~~~~~~~~~~~~

默认情况下，Finder返回文件和目录; 但可以使用
:method:`Symfony\\Component\\Finder\\Finder::files` 和
:method:`Symfony\\Component\\Finder\\Finder::directories` 方法来控制改行为::

    $finder->files();

    $finder->directories();

如果要关注链接，请使用 ``followLinks()`` 方法::

    $finder->files()->followLinks();

默认情况下，迭代器忽略流行的VCS文件。但可以通过 ``ignoreVCS()`` 方法更改::

    $finder->ignoreVCS(false);

排序
~~~~~~~

按名称或类型（首先是目录，然后是文件）对结果进行排序::

    $finder->sortByName();

    $finder->sortByType();

.. tip::

    默认情况下，``sortByName()`` 方法使用 :phpfunction:`strcmp`
    PHP函数（例如 ``file1.txt``、``file10.txt``、``file2.txt``）。
    可以通过传递 ``true`` 作为它的参数来使用PHP的 `自然排序`_
    算法（例如 ``file1.txt``、``file2.txt``、``file10.txt``）。

    .. versionadded:: 4.2
        Symfony 4.2中引入了使用自然排序顺序的选项。

按上次访问、变更或修改的时间对文件和目录进行排序::

    $finder->sortByAccessedTime();

    $finder->sortByChangedTime();

    $finder->sortByModifiedTime();

你还可以使用 ``sort()`` 方法定义自己的排序算法::

    $finder->sort(function (\SplFileInfo $a, \SplFileInfo $b) {
        return strcmp($a->getRealPath(), $b->getRealPath());
    });

你可以使用 ``reverseSorting()`` 方法反转任何排序::

    // 结果将被排序为“Z到A”而不是默认的“A到Z”
    $finder->sortByName()->reverseSorting();

.. versionadded:: 4.2
    ``reverseSorting()`` 方法是在Symfony 4.2中引入的。

.. note::

    请注意，这些 ``sort*`` 方法需要获取所有匹配的元素以完成它们的工作。对于大型迭代器，它会很慢。

文件名称
~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::name` 方法按名称限制文件::

    $finder->files()->name('*.php');

``name()`` 方法接受globs、字符串、正则表达式或数组形式的globs、字符串、正则表达式::

    $finder->files()->name('/\.php$/');

可以通过链式调用或传递一个数组来定义多个文件名::

    $finder->files()->name('*.php')->name('*.twig');

    // 与上述相同
    $finder->files()->name(array('*.php', '*.twig'));

``notName()`` 方法排除匹配一个模式的文件::

    $finder->files()->notName('*.rb');

可以通过链式调用或传递一个数组来排除多个文件名::

    $finder->files()->notName('*.rb')->notName('*.py');

    // 与上述相同
    $finder->files()->notName(array('*.rb', '*.py'));

.. versionadded:: 4.2
    在Symfony 4.2引入了对将数组传递到 ``name()`` 和 ``notName()`` 的支持。

文件内容
~~~~~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::contains` 方法来按内容限制文件::

    $finder->files()->contains('lorem ipsum');

``contains()`` 方法接受字符串或正则表达式::

    $finder->files()->contains('/lorem\s+ipsum$/i');

``notContains()`` 方法排除包含一个给定模式的文件::

    $finder->files()->notContains('dolor sit amet');

路径
~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::path` 方法来按路径限制文件和目录::

    // 匹配其路径（文件或目录）中的任何位置包含“data”的文件
    $finder->path('data');
    // 例如，如果它们存在的话，则匹配 data/*.xml 和 data.xml
    $finder->path('data')->name('*.xml');

在所有平台上，应该使用斜杠（即 ``/``）作为目录的分隔符。

``path()`` 方法接受字符串、正则表达式或字符串、正则表达式数组::

    $finder->path('foo/bar');
    $finder->path('/^foo\/bar/');

可以通过链式调用或传递一个数组来定义多个路径::

    $finder->path('data')->path('foo/bar');

    // 与上述相同
    $finder->path(array('data', 'foo/bar'));

.. versionadded:: 4.2
    在Symfony 4.2中引入了对传递数组到 ``path()`` 的支持

在内部，通过转义斜杠和添加分隔符来将字符串转换为正则表达式：

.. code-block:: text

    dirname    ===>    /dirname/
    a/b/c      ===>    /a\/b\/c/

:method:`Symfony\\Component\\Finder\\Finder::notPath` 方法按路径排除文件::

    $finder->notPath('other/dir');

链式调用或传递一个数组可以排除多个路径::

    $finder->notPath('first/dir')->notPath('other/dir');

    // 与上述相同
    $finder->notPath(array('first/dir', 'other/dir'));

.. versionadded:: 4.2
    在Symfony 4.2中引入了对传递数组到 ``notPath()`` 的支持

文件大小
~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::size` 方法来按大小(size)限制文件::

    $finder->files()->size('< 1.5K');

通过链式调用或传递一个数组来限制大小范围::

    $finder->files()->size('>= 1K')->size('<= 2K');

    // 与上述相同
    $finder->files()->size(array('>= 1K', '<= 2K'));

.. versionadded:: 4.2
    在Symfony 4.2中引入了对传递数组到 ``size()`` 的支持

比较运算符可以是下列任何一项： ``>``, ``>=``, ``<``, ``<=``, ``==``, ``!=``。

目标值可以使用千字节（``k``、``ki``），兆字节（``m``、``mi``）或千兆字节（``g``、``gi``）的大小。
后缀为 ``i`` 的话，会根据 `IEC标准`_ 来使用适当的 ``2**n`` 版本。

文件日期
~~~~~~~~~

使用 :method:`Symfony\\Component\\Finder\\Finder::date` 方法来按最后修改日期限制文件::

    $finder->date('since yesterday');

通过链式调用或传递一个数组来限制日期范围::

    $finder->date('>= 2018-01-01')->size('<= 2018-12-31');

    // 与上述相同
    $finder->date(array('>= 2018-01-01', '<= 2018-12-31'));

.. versionadded:: 4.2
    在Symfony 4.2中引入了对传递数组到 ``date()`` 的支持

比较运算符可以是下列任何一项：``>``, ``>=``, ``<``, ``<=``, ``==``。
你还可以使用 ``since`` 或 ``after`` 作为 ``>``
的别名，使用 ``until`` 或 ``before`` 作为 ``<`` 别名。

目标值可以是 `strtotime`_ 函数支持的任何日期。

目录深度
~~~~~~~~~~~~~~~

默认情况下，Finder以递归方式遍历目录。通过使用
:method:`Symfony\\Component\\Finder\\Finder::depth` 方法来限制遍历的深度::

    $finder->depth('== 0');
    $finder->depth('< 3');

通过链式调用或传递一个数组来限制深度范围::

    $finder->depth('> 2')->depth('< 5');

    // 与上述相同
    $finder->depth(array('> 2', '< 5'));

.. versionadded:: 4.2
    在Symfony 4.2中引入了对传递数组到 ``depth()`` 的支持

自定义过滤
~~~~~~~~~~~~~~~~

要使用你自己的策略来限制匹配文件，请使用
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

读取返回文件的内容
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

返回文件的内容可以通过使用
:method:`Symfony\\Component\\Finder\\SplFileInfo::getContents` 方法来读取::

    use Symfony\Component\Finder\Finder;

    $finder = new Finder();
    $finder->files()->in(__DIR__);

    foreach ($finder as $file) {
        $contents = $file->getContents();

        // ...
    }

.. _strtotime:          https://php.net/manual/en/datetime.formats.php
.. _协议:               https://php.net/manual/en/wrappers.php
.. _Streams:            https://php.net/streams
.. _IEC标准:            https://physics.nist.gov/cuu/Units/binary.html
.. _Packagist:          https://packagist.org/packages/symfony/finder
.. _`自然排序`: https://en.wikipedia.org/wiki/Natural_sort_order
