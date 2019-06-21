.. index::
   pair: Doctrine; DBAL

如何使用Doctrine DBAL
========================

.. note::

    本文是关于Doctrine DBAL的。
    通常，你将使用更高级别的Doctrine ORM层，它在幕后实际是使用DBAL与数据库进行通信。
    要阅读有关Doctrine ORM的更多信息，请参阅 ":doc:`/doctrine`"。

`Doctrine`_ 数据库抽象层（DBAL）是位于的顶部一个抽象层 `PDO`_，
并提供了一个直观而灵活的API与最流行的关系数据库进行通信。
DBAL库允许你独立于ORM模型来编写查询，例如用于构建报告或直接数据操作。

.. tip::

    阅读官方Doctrine `DBAL文档`_，了解Doctrine DBAL库的所有细节和功能。

首先，安装Doctrine ORM包：

.. code-block:: terminal

    $ composer require symfony/orm-pack

然后配置在 ``.env`` 中的 ``DATABASE_URL`` 环境变量：

.. code-block:: text

    # .env (或覆盖 .env.local 中的 DATABASE_URL 以避免提交更改)

    # 自定义这一行!
    DATABASE_URL="mysql://db_user:db_password@127.0.0.1:3306/db_name"

可以在 ``config/packages/doctrine.yaml`` 配置其他内容 -
请参阅 :ref:`reference-dbal-configuration`。
如果你 *不* 愿意使用Doctrine ORM，请删除该文件中的 ``orm`` 键。

然后，你可以通过自动装配 ``Connection`` 对象来访问Doctrine DBAL连接::

    use Doctrine\DBAL\Driver\Connection;

    class UserController extends AbstractController
    {
        public function index(Connection $connection)
        {
            $users = $connection->fetchAll('SELECT * FROM users');

            // ...
        }
    }

这将会给你传递 ``database_connection`` 服务。

注册自定义映射类型
--------------------------------

你可以通过Symfony的配置注册自定义映射类型。它们将添加到所有已配置的连接中。
有关自定义映射类型的更多信息，请阅读Doctrine文档中的 `自定义映射类型`_ 部分。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            dbal:
                types:
                    custom_first:  App\Type\CustomFirst
                    custom_second: App\Type\CustomSecond

    .. code-block:: xml

        <!-- config/packages/doctrine.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                https://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal>
                    <doctrine:type name="custom_first" class="App\Type\CustomFirst"/>
                    <doctrine:type name="custom_second" class="App\Type\CustomSecond"/>
                </doctrine:dbal>
            </doctrine:config>
        </container>

    .. code-block:: php

        // config/packages/doctrine.php
        use App\Type\CustomFirst;
        use App\Type\CustomSecond;

        $container->loadFromExtension('doctrine', [
            'dbal' => [
                'types' => [
                    'custom_first'  => CustomFirst::class,
                    'custom_second' => CustomSecond::class,
                ],
            ],
        ]);

在SchemaTool中注册自定义映射类型
--------------------------------------------------

SchemaTool用于检查数据库以对比模式(schema)。
要完成此任务，需要知道每种数据库类型需要使用哪种映射类型。
可以通过配置完成新类型的注册。

现在，映射 ENUM 类型（默认情况下不受DBAL支持）为 ``string`` 映射类型：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            dbal:
                mapping_types:
                    enum: string

    .. code-block:: xml

        <!-- config/packages/doctrine.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                https://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal>
                    <doctrine:mapping-type name="enum">string</doctrine:mapping-type>
                </doctrine:dbal>
            </doctrine:config>
        </container>

    .. code-block:: php

        // config/packages/doctrine.php
        $container->loadFromExtension('doctrine', [
            'dbal' => [
                'mapping_types' => [
                    'enum'  => 'string',
                ],
            ],
        ]);

.. _`PDO`:           https://php.net/pdo
.. _`Doctrine`:      http://www.doctrine-project.org
.. _`DBAL文档`: http://docs.doctrine-project.org/projects/doctrine-dbal/en/latest/index.html
.. _`自定义映射类型`: http://docs.doctrine-project.org/projects/doctrine-dbal/en/latest/reference/types.html#custom-mapping-types
