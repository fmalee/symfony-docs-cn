.. index::
   single: Doctrine; Multiple entity managers

如何使用多个实体管理器和连接
=========================================================

你可以在Symfony应用中使用多个Doctrine实体管理器或连接。
如果你使用不同的数据库甚至是具有完全不同的实体集的供应商(vendor)，则这是必要的。
换句话说，连接到一个数据库的一个实体管理器将处理一些实体，而连接到另一个数据库的另一个实体管理器可以处理其余的实体。

.. note::

    使用多个实体管理器配置并不复杂，但更高级，通常不需要。
    在添加这一复杂层之前，请确保你确实需要多个实体管理器。

.. caution::

    实体无法定义不同实体管理器之间的关联。如果你需要，有几种需要一些自定义设置的
    `替代方案 <https://stackoverflow.com/a/11494543/2804294>`_。

以下配置代码显示了如何配置两个实体管理器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            dbal:
                default_connection: default
                connections:
                    default:
                        # 针对你的数据库服务的一些配置
                        url: '%env(DATABASE_URL)%'
                        driver: 'pdo_mysql'
                        server_version: '5.7'
                        charset: utf8mb4
                    customer:
                        #  针对你的数据库服务的一些配置
                        url: '%env(DATABASE_CUSTOMER_URL)%'
                        driver: 'pdo_mysql'
                        server_version: '5.7'
                        charset: utf8mb4

            orm:
                default_entity_manager: default
                entity_managers:
                    default:
                        connection: default
                        mappings:
                            Main:
                                is_bundle: false
                                type: annotation
                                dir: '%kernel.project_dir%/src/Entity/Main'
                                prefix: 'App\Entity\Main'
                                alias: Main
                    customer:
                        connection: customer
                        mappings:
                            Customer:
                                is_bundle: false
                                type: annotation
                                dir: '%kernel.project_dir%/src/Entity/Customer'
                                prefix: 'App\Entity\Customer'
                                alias: Customer

    .. code-block:: xml

        <!-- config/packages/doctrine.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                https://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:dbal default-connection="default">
                    <!-- configure these for your database server -->
                    <doctrine:connection name="default"
                        url="%env(DATABASE_URL)%"
                        driver="pdo_mysql"
                        server_version="5.7"
                        charset="utf8mb4"
                    />

                    <!-- configure these for your database server -->
                    <doctrine:connection name="customer"
                        url="%env(DATABASE_CUSTOMER_URL)%"
                        driver="pdo_mysql"
                        server_version="5.7"
                        charset="utf8mb4"
                    />
                </doctrine:dbal>

                <doctrine:orm default-entity-manager="default">
                    <doctrine:entity-manager name="default" connection="default">
                        <doctrine:mapping
                            name="Main"
                            is_bundle="false"
                            type="annotation"
                            dir="%kernel.project_dir%/src/Entity/Main"
                            prefix="App\Entity\Main"
                            alias="Main"
                        />
                    </doctrine:entity-manager>

                    <doctrine:entity-manager name="customer" connection="customer">
                        <doctrine:mapping
                            name="Customer"
                            is_bundle="false"
                            type="annotation"
                            dir="%kernel.project_dir%/src/Entity/Customer"
                            prefix="App\Entity\Customer"
                            alias="Customer"
                        />
                    </doctrine:entity-manager>
                </doctrine:orm>
            </doctrine:config>
        </container>

    .. code-block:: php

        // config/packages/doctrine.php
        $container->loadFromExtension('doctrine', [
            'dbal' => [
                'default_connection' => 'default',
                'connections' => [
                    // configure these for your database server
                    'default' => [
                        'url'            => '%env(DATABASE_URL)%',
                        'driver'         => 'pdo_mysql',
                        'server_version' => '5.7',
                        'charset'        => 'utf8mb4',
                    ],
                    // configure these for your database server
                    'customer' => [
                        'url'            => '%env(DATABASE_CUSTOMER_URL)%',
                        'driver'         => 'pdo_mysql',
                        'server_version' => '5.7',
                        'charset'        => 'utf8mb4',
                    ],
                ],
            ],

            'orm' => [
                'default_entity_manager' => 'default',
                'entity_managers' => [
                    'default' => [
                        'connection' => 'default',
                        'mappings'   => [
                            'Main'  => [
                                is_bundle => false,
                                type => 'annotation',
                                dir => '%kernel.project_dir%/src/Entity/Main',
                                prefix => 'App\Entity\Main',
                                alias => 'Main',
                            ]
                        ],
                    ],
                    'customer' => [
                        'connection' => 'customer',
                        'mappings'   => [
                            'Customer'  => [
                                is_bundle => false,
                                type => 'annotation',
                                dir => '%kernel.project_dir%/src/Entity/Customer',
                                prefix => 'App\Entity\Customer',
                                alias => 'Customer',
                            ]
                        ],
                    ],
                ],
            ],
        ]);

在这个例子中，你定义了两个实体管理器，并将他们命名为 ``default`` 和 ``customer``。
``default`` 实体管理器管理的实体在 ``src/Entity/Main`` 目录，
而 ``customer`` 实体管理器的管理的实体在 ``src/Entity/Customer`` 目录。
你还定义了两个连接，每个实体管理器一个连接。

.. caution::

    使用多个连接和实体管理器时，应明确说明所需的配置。
    如果你 *省略* 了连接或实体管理器的名称，默认值（即 ``default``）被使用。

    如果使用与默认实体管理器 ``default`` 所不同的名称，则还需要在 ``prod``
    环境配置中重新定义默认实体管理器：

    .. code-block:: yaml

        # config/packages/prod/doctrine.yaml
        doctrine:
            orm:
                default_entity_manager: 'your default entity manager name'

        # ...

使用多个连接创建数据库时：

.. code-block:: terminal

    # 仅使用“default”连接
    $ php bin/console doctrine:database:create

    # 仅使用“customer”连接
    $ php bin/console doctrine:database:create --connection=customer

使用多个实体管理器生成迁移时：

.. code-block:: terminal

    # 仅使用“default”映射
    $ php bin/console doctrine:migrations:diff
    $ php bin/console doctrine:migrations:migrate

    # 仅使用“customer”映射
    $ php bin/console doctrine:migrations:diff --em=customer
    $ php bin/console doctrine:migrations:migrate --em=customer

当要求实体管理器时，如果你省略了它的名称，将返回默认的实体管理器（即``default``）::

    // ...

    use Doctrine\ORM\EntityManagerInterface;

    class UserController extends AbstractController
    {
        public function index(EntityManagerInterface $entityManager)
        {
            // 这些方法同样都返回默认的实体管理器，
            // 但是最好通过在动作方法中注入 EntityManagerInterface 来获取它
            $entityManager = $this->getDoctrine()->getManager();
            $entityManager = $this->getDoctrine()->getManager('default');
            $entityManager = $this->get('doctrine.orm.default_entity_manager');

            // 两个都返回“customer”实体管理器
            $customerEntityManager = $this->getDoctrine()->getManager('customer');
            $customerEntityManager = $this->get('doctrine.orm.customer_entity_manager');
        }
    }

你现在可以像以前一样使用Doctrine -
使用 ``default`` 实体管理器来持久化并获取它管理的实体，
并且使用 ``customer`` 实体管理器持久化并获取它们的实体。

这同样适用于仓库调用::

    use AcmeStoreBundle\Entity\Customer;
    use AcmeStoreBundle\Entity\Product;
    // ...

    class UserController extends AbstractController
    {
        public function index()
        {
            // 检索一个由“default”实体管理器管理的仓库
            $products = $this->getDoctrine()
                ->getRepository(Product::class)
                ->findAll()
            ;

            // 显式的使用"default"实体管理器的方式
            $products = $this->getDoctrine()
                ->getRepository(Product::class, 'default')
                ->findAll()
            ;

            // 检索一个由“customer”实体管理器管理的仓库
            $customers = $this->getDoctrine()
                ->getRepository(Customer::class, 'customer')
                ->findAll()
            ;
        }
    }
