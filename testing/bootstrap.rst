如何在运行测试之前自定义引导过程
===========================================================

有时在运行测试时，你需要在运行这些测试之前执行额外的引导工作。
例如，如果你正在运行一个功能测试并已引入新的翻译资源，则需要在运行这些测试之前清除缓存。

为此，首先添加一个执行引导工作的文件::

    // tests/bootstrap.php
    if (isset($_ENV['BOOTSTRAP_CLEAR_CACHE_ENV'])) {
        // 执行 "php bin/console cache:clear" 命令
        passthru(sprintf(
            'APP_ENV=%s php "%s/../bin/console" cache:clear --no-warmup',
            $_ENV['BOOTSTRAP_CLEAR_CACHE_ENV'],
            __DIR__
        ));
    }

    require __DIR__.'/../vendor/autoload.php';

然后，配置 ``phpunit.xml.dist`` 以在运行测试之前执行此 ``bootstrap.php`` 文件：

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <?xml version="1.0" encoding="UTF-8"?>
    <phpunit
        bootstrap="tests/bootstrap.php"
    >
        <!-- ... -->
    </phpunit>

现在，你可以在你的 ``phpunit.xml.dist`` 文件中定义在要在哪个环境中清除缓存：

.. code-block:: xml

    <!-- phpunit.xml.dist -->
    <?xml version="1.0" encoding="UTF-8"?>
    <phpunit>
        <!-- ... -->

        <php>
            <env name="BOOTSTRAP_CLEAR_CACHE_ENV" value="test"/>
        </php>
    </phpunit>

现在，该定义成为自定义引导程序文件（``tests/bootstrap.php``）中可用的环境变量（即 ``$_ENV``）。
