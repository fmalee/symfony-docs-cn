.. index::
    single: Session; Database Storage

如何使用PdoSessionHandler在数据库中存储会话
==============================================================

默认的Symfony会话存储会将会话信息写入文件。
大多数中型到大型网站使用数据库来存储会话值而不是文件，因为数据库更易于使用并可在多个Web服务器环境中进行扩展。

Symfony有一个内置的数据库会话存储解决方案：
:class:`Symfony\\Component\\HttpFoundation\\Session\\Storage\\Handler\\PdoSessionHandler`。
要使用它，首先注册一个新的处理器服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler:
                arguments:
                    - 'mysql:dbname=mydatabase; host=myhost; port=myport'
                    - { db_username: myuser, db_password: mypassword }

                    # 如果你正在使用Doctrine并希望重用该连接，那么：
                    # 注释上面的2行并取消下行的注释
                    # - !service { class: PDO, factory: ['@database_connection', 'getWrappedConnection'] }
                    # 如果你遇到事务问题（例如登录后），请取消下行的注释
                    # - { lock_mode: 1 }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <services>
                <service id="Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler" public="false">
                    <argument>mysql:dbname=mydatabase, host=myhost</argument>
                    <argument type="collection">
                        <argument key="db_username">myuser</argument>
                        <argument key="db_password">mypassword</argument>
                    </argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler;

        $storageDefinition = $container->autowire(PdoSessionHandler::class)
            ->setArguments([
                'mysql:dbname=mydatabase; host=myhost; port=myport',
                ['db_username' => 'myuser', 'db_password' => 'mypassword'],
            ])
        ;

.. tip::

    :doc:`使用配置文件中的环境变量 </configuration/environment_variables>`
    来配置数据库凭据，让你的应用更安全。

接下来，告诉Symfony将你的服务用作会话处理器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            session:
                # ...
                handler_id: Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <framework:config>
            <!-- ... -->
            <framework:session handler-id="Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler" cookie-lifetime="3600" auto-start="true"/>
        </framework:config>

    .. code-block:: php

        // config/packages/framework.php
        use Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler;

        // ...
        $container->loadFromExtension('framework', [
            // ...
            'session' => [
                // ...
                'handler_id' => PdoSessionHandler::class,
            ],
        ]);

配置表名和列名
--------------------------------------

这将期望(expect)一个包含许多不同列的 ``sessions`` 表。
可以通过将第二个数组参数传递给 ``PdoSessionHandler`` 来配置表名和所有列名：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler:
                arguments:
                    - 'mysql:dbname=mydatabase; host=myhost; port=myport'
                    - { db_table: 'sessions', db_username: 'myuser', db_password: 'mypassword' }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler" public="false">
                    <argument>mysql:dbname=mydatabase, host=myhost</argument>
                    <argument type="collection">
                        <argument key="db_table">sessions</argument>
                        <argument key="db_username">myuser</argument>
                        <argument key="db_password">mypassword</argument>
                    </argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\HttpFoundation\Session\Storage\Handler\PdoSessionHandler;
        // ...

        $container->autowire(PdoSessionHandler::class)
            ->setArguments([
                'mysql:dbname=mydatabase; host=myhost; port=myport',
                ['db_table' => 'sessions', 'db_username' => 'myuser', 'db_password' => 'mypassword']
            ])
        ;

这些是你可以配置的参数：

``db_table`` (默认 ``sessions``)：
    数据库中会话表的名称;

``db_id_col`` (默认 ``sess_id``)：
    会话表中id列的名称（VARCHAR（128））;

``db_data_col`` (默认 ``sess_data``):
    会话表中值列的名称（BLOB）;

``db_time_col`` (默认 ``sess_time``)：
    会话表中的时间列的名称（INTEGER）;

``db_lifetime_col`` (默认 ``sess_lifetime``)：
    会话表中的生命周期列的名称（INTEGER）。

.. _example-sql-statements:

准备数据库来存储会话
----------------------------------------

在数据库中存储会话之前，必须创建存储信息的表。
会话处理器提供了一个
:method:`Symfony\\Component\\HttpFoundation\\Session\\Storage\\Handler::createTable`
方法，根据使用的数据库引擎为你设置该表::

    try {
        $sessionHandlerService->createTable();
    } catch (\PDOException $exception) {
        // 由于某种原因无法创建表
    }

如果你更喜欢自己设置表，那么这些是根据你的特定数据库引擎可能使用的SQL语句的一些示例。

在生产中运行它的一个好方法是生成一个空迁移，然后在里面添加这个SQL：

.. code-block:: terminal

    $ php bin/console doctrine:migrations:generate

在下面找到正确的SQL并将其放在该文件中。然后执行它：

.. code-block:: terminal

    $ php bin/console doctrine:migrations:migrate

MySQL
~~~~~

.. code-block:: sql

    CREATE TABLE `sessions` (
        `sess_id` VARCHAR(128) NOT NULL PRIMARY KEY,
        `sess_data` BLOB NOT NULL,
        `sess_time` INTEGER UNSIGNED NOT NULL,
        `sess_lifetime` MEDIUMINT NOT NULL
    ) COLLATE utf8_bin, ENGINE = InnoDB;

.. note::

    一个 ``BLOB`` 列类型仅可以存储最多64 KB。
    如果存储在用户会话中的数据超过此值，则可能会抛出异常或者会话将以静默方式重置。
    如果你需要更多空间，请考虑使用 ``MEDIUMBLOB``。

PostgreSQL
~~~~~~~~~~

.. code-block:: sql

    CREATE TABLE sessions (
        sess_id VARCHAR(128) NOT NULL PRIMARY KEY,
        sess_data BYTEA NOT NULL,
        sess_time INTEGER NOT NULL,
        sess_lifetime INTEGER NOT NULL
    );

Microsoft SQL Server
~~~~~~~~~~~~~~~~~~~~

.. code-block:: sql

    CREATE TABLE [dbo].[sessions](
        [sess_id] [nvarchar](255) NOT NULL,
        [sess_data] [ntext] NOT NULL,
        [sess_time] [int] NOT NULL,
        [sess_lifetime] [int] NOT NULL,
        PRIMARY KEY CLUSTERED(
            [sess_id] ASC
        ) WITH (
            PAD_INDEX  = OFF,
            STATISTICS_NORECOMPUTE  = OFF,
            IGNORE_DUP_KEY = OFF,
            ALLOW_ROW_LOCKS  = ON,
            ALLOW_PAGE_LOCKS  = ON
        ) ON [PRIMARY]
    ) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

.. caution::

    如果会话数据不适合该数据列，则可能会被数据库引擎截断。
    更糟糕的是，当会话数据被破坏时，PHP会忽略该数据而不会发出警告。

    如果应用存储大量会话数据，则可以通过增加列大小（使用 ``BLOB`` 甚至是 ``MEDIUMBLOB``）来解决此问题。
    使用MySQL作为数据库引擎时，你还可以启用 `严格的SQL模式`_，以便在发生此类错误时收到通知。

.. _`严格的SQL模式`: https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html
