.. index::
   single: Filesystem

Filesystem组件
========================

    Filesystem组件为文件系统提供基本的工具。

安装
------------

.. code-block:: terminal

    $ composer require symfony/filesystem

或者，你可以克隆 `<https://github.com/symfony/filesystem>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

用法
-----

:class:`Symfony\\Component\\Filesystem\\Filesystem` 类是文件系统操作的唯一端点::

    use Symfony\Component\Filesystem\Filesystem;
    use Symfony\Component\Filesystem\Exception\IOExceptionInterface;

    $fileSystem = new Filesystem();

    try {
        $fileSystem->mkdir(sys_get_temp_dir().'/'.random_int(0, 1000));
    } catch (IOExceptionInterface $exception) {
        echo "An error occurred while creating your directory at ".$exception->getPath();
    }

.. note::

    :method:`Symfony\\Component\\Filesystem\\Filesystem::mkdir`、
    :method:`Symfony\\Component\\Filesystem\\Filesystem::exists`、
    :method:`Symfony\\Component\\Filesystem\\Filesystem::touch`、
    :method:`Symfony\\Component\\Filesystem\\Filesystem::remove`、
    :method:`Symfony\\Component\\Filesystem\\Filesystem::chmod`、
    :method:`Symfony\\Component\\Filesystem\\Filesystem::chown` 以及
    :method:`Symfony\\Component\\Filesystem\\Filesystem::chgrp`
    方法能够接收一个字符串、一个数组或任何实现 :phpclass:`Traversable` 的对象作为其目标参数。

mkdir
~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::mkdir` 以递归方式创建目录。
在POSIX文件系统上，使用默认的 `0777` 模式值来创建目录。你可以使用第二个参数来设置自己的模式::

    $fileSystem->mkdir('/tmp/photos', 0700);

.. note::

    你可以将数组或任何 :phpclass:`Traversable` 对象作为第一个参数传递。

.. note::

    此函数忽略已存在的目录。

.. note::

    目录的权限受当前 `umask`_ 的影响。
    要为你的网络服务器设置umask，请使用PHP的 :phpfunction:`umask` 函数或
    在创建目录后使用 :phpfunction:`chmod` 函数。

exists
~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::exists`
检查是否存在一个或多个文件或目录，如果缺少任何文件或目录则返回 ``false``::

    // 如果此绝对目录存在，则返回 true
    $fileSystem->exists('/tmp/photos');

    // 如果 rabbit.jpg 存在，但 bottle.png 不存在，则返回 false
    // 非绝对路径是相对于存储正在运行的PHP脚本的目录
    $fileSystem->exists(array('rabbit.jpg', 'bottle.png'));

.. note::

    你可以将数组或任何 :phpclass:`Traversable` 对象作为第一个参数传递。

copy
~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::copy`
创建单个文件的一个副本（使用 :method:`Symfony\\Component\\Filesystem\\Filesystem::mirror` 来复制目录）。
如果目标已存在，则仅在源修改日期晚于目标时才会复制文件。传递布尔类型的第三个参数可以重写此行为::

    // 只有 image-ICC 在 image.jpg 之后修改时才有效
    $fileSystem->copy('image-ICC.jpg', 'image.jpg');

    // image.jpg 会被重写
    $fileSystem->copy('image-ICC.jpg', 'image.jpg', true);

touch
~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::touch` 设置文件的访问时间和修改时间。
默认情况下使用当前时间。
你可以使用第二个参数来设置修改时间。第三个参数是访问时间::

    // 将修改时间设置为当前时间戳
    $fileSystem->touch('file.txt');
    // 将修改时间设置为当前时间的后10秒
    $fileSystem->touch('file.txt', time() + 10);
    // 将访问时间设置为当前时间的前10秒
    $fileSystem->touch('file.txt', time(), time() - 10);

.. note::

    你可以将数组或任何 :phpclass:`Traversable` 对象作为第一个参数传递。

chown
~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::chown` 修改文件的所有者。
第三个参数是一个布尔类型的递归选项::

    // 将 lolcat 视频的所有者设置为 www-data
    $fileSystem->chown('lolcat.mp4', 'www-data');
    // 以递归方式更改 video 目录的所有者
    $fileSystem->chown('/video', 'www-data', true);

.. note::

    你可以将数组或任何 :phpclass:`Traversable` 对象作为第一个参数传递。

chgrp
~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::chgrp` 修改文件组。
第三个参数是一个布尔类型的递归选项::

    // 将 lolcat 视频的组设置为 nginx
    $fileSystem->chgrp('lolcat.mp4', 'nginx');
    // 以递归方式更改 video 目录的组
    $fileSystem->chgrp('/video', 'nginx', true);

.. note::

    你可以将数组或任何 :phpclass:`Traversable` 对象作为第一个参数传递。

chmod
~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::chmod` 修改文件的模式或权限。
第四个参数是一个布尔类型的递归选项::

    // 将视频的模式设置为0600
    $fileSystem->chmod('video.ogg', 0600);
    // 以递归方式更改 src 目录的模式
    $fileSystem->chmod('src', 0700, 0000, true);

.. note::

    你可以将数组或任何 :phpclass:`Traversable` 对象作为第一个参数传递。

remove
~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::remove` 删除文件、目录和符号链接::

    $fileSystem->remove(array('symlink', '/path/to/directory', 'activity.log'));

.. note::

    你可以将数组或任何 :phpclass:`Traversable` 对象作为第一个参数传递。

rename
~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::rename` 修改单个文件或目录的名称::

    // 重命名一个文件
    $fileSystem->rename('/tmp/processed_video.ogg', '/path/to/store/video_647.ogg');
    // 重命名一个目录
    $fileSystem->rename('/tmp/files', '/path/to/store/files');

symlink
~~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::symlink` 创建从源到目标的符号链接。
如果文件系统不支持符号链接，则可以使用第三个布尔类型的参数::

    // 创建一个符号链接
    $fileSystem->symlink('/path/to/source', '/path/to/destination');
    // 如果文件系统不支持符号链接，则复制源目录
    $fileSystem->symlink('/path/to/source', '/path/to/destination', true);

readlink
~~~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::readlink` 读取链接目标。

PHP的 ``readlink()`` 函数返回符号链接的目标。
但是，它在Windows和Unix下的行为完全不同。
在Windows系统上，``readlink()`` 递归解析该链接的子链接，直到找到最终目标。
在基于Unix的系统上，``readlink()`` 只能解析下一个链接。

Filesystem组件提供的
:method:`Symfony\\Component\\Filesystem\\Filesystem::readlink` 方法始终以相同的方式运行::

    // 返回链接的下一个直接目标，而不考虑目标是否存在
    $fileSystem->readlink('/path/to/link');

    // 返回目标的绝对完全解析的最终版本（如果有嵌套链接，则解析它们）
    $fileSystem->readlink('/path/to/link', true);

其行为如下::

    public function readlink($path, $canonicalize = false)

* 当 ``$canonicalize`` 为 ``false``:
    * 如果 ``$path`` 不存在或不是链接，则返回 ``null``。
    * 如果``$path`` 是链接，则返回链接的下一个直接目标，而不考虑目标是否存在。

* 当 ``$canonicalize`` 为 ``true``:
    * 如果 ``$path`` 不存在，则返回 ``null``。
    * 如果 ``$path`` 存在，则返回其绝对完全解析的最终版本。

makePathRelative
~~~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::makePathRelative`
采用两个绝对路径并返回从第二个路径到第一个路径的相对路径::

    // 返回 '../'
    $fileSystem->makePathRelative(
        '/var/lib/symfony/src/Symfony/',
        '/var/lib/symfony/src/Symfony/Component'
    );
    // 返回 'videos/'
    $fileSystem->makePathRelative('/tmp/videos', '/tmp')

mirror
~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::mirror`
将源目录的所有内容复制到目标目录（使用
:method:`Symfony\\Component\\Filesystem\\Filesystem::copy` 方法复制单个文件）::

    $fileSystem->mirror('/path/to/source', '/path/to/target');

isAbsolutePath
~~~~~~~~~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::isAbsolutePath`
如果给定路径是绝对路径则返回 ``true``，否则返回 ``false``::

    // 返回 true
    $fileSystem->isAbsolutePath('/tmp');
    // 返回 true
    $fileSystem->isAbsolutePath('c:\\Windows');
    // 返回 false
    $fileSystem->isAbsolutePath('tmp');
    // 返回 false
    $fileSystem->isAbsolutePath('../dir');

dumpFile
~~~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::dumpFile` 将给定内容保存到文件中。
它以原子方式执行该操作：它首先将内容写入到一个临时文件，写入完成后再将其移动到该新文件的位置。
这意味着用户将始终看到完整的旧文件和新文件（但不会有一个只写入一个部分的文件）::

    $fileSystem->dumpFile('file.txt', 'Hello World');

``file.txt`` 文件现在包含 ``Hello World``。

appendToFile
~~~~~~~~~~~~

:method:`Symfony\\Component\\Filesystem\\Filesystem::appendToFile` 在某个文件的末尾添加新内容::

    $fileSystem->appendToFile('logs.txt', 'Email sent to user@example.com');

如果对应文件或包含它的目录不存在，则该方法会在附加内容之前创建它们。

错误处理
--------------

每当出现错误时，都会抛出一个实现
:class:`Symfony\\Component\\Filesystem\\Exception\\ExceptionInterface` 或
:class:`Symfony\\Component\\Filesystem\\Exception\\IOExceptionInterface`
的异常。

.. note::

    如果目录创建失败，则抛出一个
    :class:`Symfony\\Component\\Filesystem\\Exception\\IOException`。

.. _`Packagist`: https://packagist.org/packages/symfony/filesystem
.. _`umask`: https://en.wikipedia.org/wiki/Umask
