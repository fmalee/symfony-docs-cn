.. index::
   single: Doctrine

数据库 & Doctrine
==============================

.. admonition:: Screencast
    :class: screencast

    更喜欢视频教程? 可以观看 `Doctrine screencast series`_ 系列录像.

Symfony框架并未整合任何需要使用数据库的组件，但是却紧密集成了一个名为 `Doctrine`_ 的三方类库。

.. note::

    本章讲的全部是Doctrine ORM。
    如果你倾向于使用数据库的原始查询，可参考 ":doc:`/doctrine/dbal`" 一文的讲解。

    你也可以使用Doctrine ODM类库将数据持久化到 `MongoDB`_ 。
    参考 "`DoctrineMongoDBBundle`_" 以了解更多信息。

安装 Doctrine
-------------------

首先，通过 "ORM pack" 以及 MakerBundle 安装Doctrine支持，这将有助于生成一些代码：

.. code-block:: terminal

    $ composer require symfony/orm-pack
    $ composer require symfony/maker-bundle --dev

配置数据库
~~~~~~~~~~~~~~~~~~~~~~~~

数据库连接信息存储在名为 ``DATABASE_URL`` 的环境变量中。
在开发时，你可以在 ``.env`` 中找到并自定义它：

.. code-block:: text

    # .env (或覆盖 .env.local 中的 DATABASE_URL 以避免提交更改)

    # 自定义这一行!
    DATABASE_URL="mysql://db_user:db_password@127.0.0.1:3306/db_name"

    # 使用 sqlite:
    # DATABASE_URL="sqlite:///%kernel.project_dir%/var/app.db"

.. caution::

    如果URI中的用户名、密码、主机或数据库名称包含特殊的任何字符（例如 ``!``, ``@``, ``$``, ``#``, ``/``），
    则必须对它们进行编码。有关保留字符的完整列表请参阅 `RFC 3986`_ ，
    或使用 :phpfunction:`urlencode` 函数对其进行编码。
    在这种情况下，你需要删除 ``config/packages/doctrine.yaml`` 中的 ``resolve:`` 前缀以避免错误：
    ``url: '%env(resolve:DATABASE_URL)%'``

既然已经设置好了连接参数，Doctrine可以为你创建 ``db_name`` 数据库了：

.. code-block:: terminal

    $ php bin/console doctrine:database:create

你可以配置 ``config/packages/doctrine.yaml`` 中的更多选项，
包括可能会影响Doctrine的运行方式 ``server_version`` 选项（例如5.7，如果你使用的是MySQL 5.7）。

.. tip::

    还有许多其他 Doctrine 命令。运行 ``php bin/console list doctrine`` 以查看完整列表。

创建实体类
------------------------

假设你正构建一套程序，其中有些产品需要展示。即使不考虑Doctrine或者数据库，
你也已经知道你需要一个 ``Product`` 对象来呈现这些产品。

.. _doctrine-adding-mapping:

你可以使用 ``make:entity`` 命令创建此类以及所需的任何字段。
该命令会问你一些问题 - 回答如下：

.. code-block:: terminal

    $ php bin/console make:entity

    Class name of the entity to create or update:
    > Product

    New property name (press <return> to stop adding fields):
    > name

    Field type (enter ? to see all types) [string]:
    > string

    Field length [255]:
    > 255

    Can this field be null in the database (nullable) (yes/no) [no]:
    > no

    New property name (press <return> to stop adding fields):
    > price

    Field type (enter ? to see all types) [string]:
    > integer

    Can this field be null in the database (nullable) (yes/no) [no]:
    > no

    New property name (press <return> to stop adding fields):
    >
    (press enter again to finish)

.. versionadded:: 1.3
    ``make:entity`` 命令的交互行为是在MakerBundle 1.3中引入的。

Woh！你现在有了一个新的 ``src/Entity/Product.php`` 文件::

    // src/Entity/Product.php
    namespace App\Entity;

    use Doctrine\ORM\Mapping as ORM;

    /**
     * @ORM\Entity(repositoryClass="App\Repository\ProductRepository")
     */
    class Product
    {
        /**
         * @ORM\Id
         * @ORM\GeneratedValue
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string", length=255)
         */
        private $name;

        /**
         * @ORM\Column(type="integer")
         */
        private $price;

        public function getId()
        {
            return $this->id;
        }

        // ... getter 和 setter 方法
    }

.. note::

    困惑为什么价格是整数？别担心：这只是一个例子。
    但是，将价格存储为整数（例如100 = 1美元）可以避免舍入(rounding)问题。

.. caution::

    在MySQL 5.6及更早版本中使用InnoDB表时，会碰到 `索引键前缀的限制为767字节`_ 问题，
    具有255个字节长度和 ``utf8mb4`` 编码的字符串列会超过该限制。
    这意味着任何 ``string`` 类型和 ``unique=true`` 的列必须将其最大 ``length`` 设置为 ``190``。
    否则，你将看到此错误：*"[PDOException] SQLSTATE[42000]: Syntax error or access violation:
    1071 Specified key was too long; max key length is 767 bytes"*。

该类称为“实体”。很快，你将能够将 ``Product`` 对象保存并查询到数据库中的 ``product`` 表。
``Product`` 实体中的每个属性都可以映射到该表中的列。
这通常使用注释完成：你在每个属性上方看到的 ``@ORM\...`` 注释：

.. image:: /_images/doctrine/mapping_single_entity.png
   :align: center

``make:entity`` 命令是一种很便捷的工具。
但这是你的代码：添加/删除字段、添加/删除方法或更新配置都随你的意。

Doctrine 支持各种字段类型，每种类型都有自己的选项。
要查看完整列表，请查看 `Doctrine的映射类型文档`_。
如果要使用XML而不是注释，
请将 ``type: xml`` 和 ``dir: '%kernel.project_dir%/config/doctrine'`` 添加到
``config/packages/doctrine.yaml`` 文件中的实体映射中。

.. caution::

    注意不要将保留的SQL关键字用作表或列名（例如 ``GROUP`` 或 ``USER``）。
    有关如何转义这些内容的详细信息，请参阅Doctrine的 `SQL关键字保留文档`_。
    或者，使用类上方的 ``@ORM\Table(name="groups")`` 更改表名，使用 ``name="group_name"`` 选项配置列名。

.. _doctrine-creating-the-database-tables-schema:

迁移：创建数据库的表/模式
-----------------------------------------------

``Product`` 类已完全配置并准备保存到一个 ``product`` 表。
如果你刚刚定义了这个类，你的数据库实际上还没有 ``product`` 表。
要添加它，你可以利用已经安装的 `DoctrineMigrationsBundle`_：

.. code-block:: terminal

    $ php bin/console make:migration

如果一切正常，你应该看到这样的消息::

    SUCCESS!

    Next: Review the new migration "src/Migrations/Version20180207231217.php"
    Then: Run the migration with php bin/console doctrine:migrations:migrate

如果你打开此文件，它将包含更新数据库所需的SQL语句！
要运行该SQL，请执行迁移：

.. code-block:: terminal

    $ php bin/console doctrine:migrations:migrate

此命令将执行尚未在你的数据库运行过的所有迁移文件。
你应该在部署生产时运行此命令，以使生产的数据库保持最新。

.. _doctrine-add-more-fields:

迁移 & 添加更多字段
-------------------------------

如果你需要向 ``Product`` 添加一个新的字段属性，如 ``description``，该怎么办？
你可以编辑该类以添加新属性。，但是，你也可以再次使用 ``make:entity``：

.. code-block:: terminal

    $ php bin/console make:entity

    Class name of the entity to create or update
    > Product

    New property name (press <return> to stop adding fields):
    > description

    Field type (enter ? to see all types) [string]:
    > text

    Can this field be null in the database (nullable) (yes/no) [no]:
    > no

    New property name (press <return> to stop adding fields):
    >
    (press enter again to finish)

执行的结果会添加新的 ``description`` 属性和 ``getDescription()`` 以及 ``setDescription()`` 方法：

.. code-block:: diff

    // src/Entity/Product.php
    // ...

    class Product
    {
        // ...

    +     /**
    +      * @ORM\Column(type="text")
    +      */
    +     private $description;

        // getDescription() & setDescription() 同样已经添加
    }

新的属性已映射，但在 ``product`` 表中尚不存在。
小问题！生成一个新的迁移：

.. code-block:: terminal

    $ php bin/console make:migration

这次，生成的文件中的SQL将如下所示：

.. code-block:: sql

    ALTER TABLE product ADD description LONGTEXT NOT NULL

迁移系统很 *聪明*。它将所有实体与数据库的当前状态进行比较，并生成同步它们所需的SQL语句！
与之前一样，执行迁移：

.. code-block:: terminal

    $ php bin/console doctrine:migrations:migrate

该命令只会执行 *一个* 新的迁移文件，因为 DoctrineMigrationsBundle 知道第一次迁移已经在之前执行过。
在幕后，它管理着一个 ``migration_versions`` 表来跟踪迁移信息。

每次更改模式(schema)后，运行这两个命令以生成迁移，然后执行它。
确保提交迁移文件并在部署时执行它们。

.. _doctrine-generating-getters-and-setters:

.. tip::

    如果你希望手动添加新属性，``make:entity`` 命令可以为你生成 getter 和 setter 方法：

    .. code-block:: terminal

        $ php bin/console make:entity --regenerate

    如果进行了一些修改并想要重新生成 *所有* 的 getter/setter 方法，还要传递 ``--overwrite`` 参数。

持久化对象到数据库
----------------------------------

是时候将一个 ``Product`` 对象保存到数据库了！
让我们创建一个新的控制器进行实验：

.. code-block:: terminal

    $ php bin/console make:controller ProductController

在控制器内部，你可以创建一个新的 ``Product`` 对象，接着给它添加数据，然后进行保存！

.. code-block:: php

    // src/Controller/ProductController.php
    namespace App\Controller;

    // ...
    use App\Entity\Product;

    class ProductController extends AbstractController
    {
        /**
         * @Route("/product", name="product")
         */
        public function index()
        {
            // 可以使用 $this->getDoctrine() 方法获取 EntityManager
            // 或者添加一个参数到你的动作上：index(EntityManagerInterface $entityManager)
            $entityManager = $this->getDoctrine()->getManager();

            $product = new Product();
            $product->setName('Keyboard');
            $product->setPrice(1999);
            $product->setDescription('Ergonomic and stylish!');

            // 告诉Doctrine你希望（最终）存储 Product 对象（还没有语句被执行）
            $entityManager->persist($product);

            // 真正执行语句（如，INSERT 查询）
            $entityManager->flush();

            return new Response('Saved new product with id '.$product->getId());
        }
    }

试试看！

    http://localhost:8000/product

恭喜！你刚刚在 ``product`` 表中创建了第一行。
为了证明这一点，你可以直接查询数据库：

.. code-block:: terminal

    $ php bin/console doctrine:query:sql 'SELECT * FROM product'

    # 在不使用Powershell的Windows系统上，请运行此命令：
    # php bin/console doctrine:query:sql "SELECT * FROM product"

深入分析一下前面的例子：

.. _doctrine-entity-manager:

* **16行** ``$this->getDoctrine()->getManager()`` 方法获取Doctrine的 *实体管理器* 对象，这是Doctrine中最重要的对象。
  它负责将对象保存到数据库并从中提取对象。

* **18-21行** 在本节中，你将像任何其他普通PHP对象一样实例化和使用 ``$product`` 对象。

* **24行** 调用 ``persist($product)`` 告诉Doctrine去 "管理" ``$product`` 对象。
  它 *没有* 引发对数据库的请求。

* **27行** 当 ``flush()`` 方法被调用时，Doctrine会遍历它管理的所有对象以确定是否需要被持久化到数据库。
  本例中，``$product`` 对象的数据在数据库中并不存在，
  因此实体管理器要执行 ``INSERT`` 查询，在 ``product`` 表中创建一个新行。

.. note::

    如果 ``flush()`` 调用失败，则抛出 ``Doctrine\ORM\ORMException`` 异常。
    请参阅 `事务和并发`_。

无论你是创建还是更新对象，工作流始终都是相同的：
Doctrine足够聪明，可以知道它应该是 *插入* 还是 *更新* 你的实体。

从数据库中获取对象
----------------------------------

从数据库中取回对象更加容易。
假设你希望能够转到 ``/product/1`` 查看你的新产品::

    // src/Controller/ProductController.php
    // ...

    /**
     * @Route("/product/{id}", name="product_show")
     */
    public function show($id)
    {
        $product = $this->getDoctrine()
            ->getRepository(Product::class)
            ->find($id);

        if (!$product) {
            throw $this->createNotFoundException(
                'No product found for id '.$id
            );
        }

        return new Response('Check out this great product: '.$product->getName());

        // 也可以渲染一个模板
        // 在模板中，使用 {{ product.name }} 输出内容
        // return $this->render('product/show.html.twig', ['product' => $product]);
    }

试试看！

    http://localhost:8000/product/1

查询特定类型的对象时，始终使用所谓的“仓库”。
你可以将仓库视为PHP类，其唯一的工作是帮助你获取某个类的实体。

拥有仓库对象后，你会获得许多辅助方法::

    $repository = $this->getDoctrine()->getRepository(Product::class);

    // 通过主键（通常是id）查询一件产品
    $product = $repository->find($id);

    // 基于产品名称的值来找到一件产品
    $product = $repository->findOneBy(['name' => 'Keyboard']);
    // 或是同时使用 产品名称 和 价格
    $product = $repository->findOneBy([
        'name' => 'Keyboard',
        'price' => 1999,
    ]);

    // 通过产品名称检索多个产品对象，并按照价格进行排序
    $products = $repository->findBy(
        ['name' => 'Keyboard'],
        ['price' => 'ASC']
    );

    // 查出 *全部* 产品对象
    $products = $repository->findAll();

你还可以为更复杂的查询添加 *自定义* 方法！
稍后将在 :ref:`doctrine-queries` 部分中进行更多介绍。

.. tip::

    在渲染HTML页面后，页面底部的Web调试工具栏将显示本次查询的数量和执行它们所花费的时间：

    .. image:: /_images/doctrine/doctrine_web_debug_toolbar.png
       :align: center
       :class: with-browser

    如果数据库查询的数量太多，图标将变为黄色以指示某些内容可能不正确。
    单击图标以打开Symfony分析器并查看已执行的确切查询。
    如果你没有看到Web调试工具栏，请尝试运行 ``composer require --dev symfony/profiler-pack`` 来安装它。

自动获取对象 (ParamConverter)
-----------------------------------------------

在许多情况下，你可以使用 `SensioFrameworkExtraBundle`_ 自动为你执行查询！
首先，如果你还没有安装该bundle，请先安装：

.. code-block:: terminal

    $ composer require sensio/framework-extra-bundle

现在，简化你的控制器::

    // src/Controller/ProductController.php

    use App\Entity\Product;
    // ...

    /**
     * @Route("/product/{id}", name="product_show")
     */
    public function show(Product $product)
    {
        // 使用该产品对象!
        // ...
    }

仅此而已！该bundle使用路由中的 ``{id}`` 来按 ``id`` 列查询 ``Product``。
如果找不到相应记录，则生成404页面。

你还可以使用更多的选项。具体请阅读有关 `ParamConverter`_ 的章节。

更新对象
------------------

一旦从Doctrine中获取了一个对象，就可以像使用任何PHP模型一样与它进行交互::

    /**
     * @Route("/product/edit/{id}")
     */
    public function update($id)
    {
        $entityManager = $this->getDoctrine()->getManager();
        $product = $entityManager->getRepository(Product::class)->find($id);

        if (!$product) {
            throw $this->createNotFoundException(
                'No product found for id '.$id
            );
        }

        $product->setName('New product name!');
        $entityManager->flush();

        return $this->redirectToRoute('product_show', [
            'id' => $product->getId()
        ]);
    }

使用Doctrine编辑现有产品包含三个步骤：

#. 从Doctrine中取出对象;
#. 修改该对象;
#. 调用实体管理器的 ``flush()`` 方法。

你仍可以调用 ``$entityManager->persist($product)`` 方法，但这是不必要的：
因为 Doctrine 已经在“管理”(watching)你的对象了。

删除对象
------------------

删除一个对象十分类似，但需要从实体管理器调用 ``remove()`` 方法::

    $entityManager->remove($product);
    $entityManager->flush();

和你想象的一样：``remove()`` 方法通知Doctrine你想从数据库中删除指定的实体，
但真正的 ``DELETE`` 查询不会被真正执行，直到 ``flush()`` 方法被调用。

.. _doctrine-queries:

对象查询：仓库
-------------------------

你已经看到仓库对象是如何让你执行一些基本查询而毋须做其他任何工作了::

    // 从控制器内部
    $repository = $this->getDoctrine()->getRepository(Product::class);

    $product = $repository->find($id);

但是，如果需要更复杂的查询呢？
使用 ``make:entity`` 生成实体时，该命令 *还* 会生成一个 ``ProductRepository`` 类::

    // src/Repository/ProductRepository.php
    namespace App\Repository;

    use App\Entity\Product;
    use Doctrine\Bundle\DoctrineBundle\Repository\ServiceEntityRepository;
    use Symfony\Bridge\Doctrine\RegistryInterface;

    class ProductRepository extends ServiceEntityRepository
    {
        public function __construct(RegistryInterface $registry)
        {
            parent::__construct($registry, Product::class);
        }
    }

当你获取一个仓库（即 ``->getRepository(Product::class)``）时，它 *实际上* 是 *这个* 对象的一个​​实例！
这是因为生成在 ``Product`` 实体类顶部的 ``repositoryClass`` 配置。

假设你要查询价格高于特定值的所有产品对象。为你的仓库添加一个新方法::

    // src/Repository/ProductRepository.php

    // ...
    class ProductRepository extends ServiceEntityRepository
    {
        public function __construct(RegistryInterface $registry)
        {
            parent::__construct($registry, Product::class);
        }

        /**
         * @param $price
         * @return Product[]
         */
        public function findAllGreaterThanPrice($price): array
        {
            // 会智能的选择 Product 表
            // “p”是你将在查询的其余部分中使用的别名
            $qb = $this->createQueryBuilder('p')
                ->andWhere('p.price > :price')
                ->setParameter('price', $price)
                ->orderBy('p.price', 'ASC')
                ->getQuery();

            return $qb->execute();

            // 只获取一个结果:
            // $product = $qb->setMaxResults(1)->getOneOrNullResult();
        }
    }

该示例使用的是Doctrine的 `查询生成器`_：一种非常强大且对用户友好的编写自定义查询的方式。
现在，你可以在仓库中调用此方法::

    // 从控制器内部
    $minPrice = 1000;

    $products = $this->getDoctrine()
        ->getRepository(Product::class)
        ->findAllGreaterThanPrice($minPrice);

    // ...

如果你在一个  :ref:`services-constructor-injection` 中，
你可以类型约束 ``ProductRepository`` 类并像平常一样注入它。

有关更多详细信息，请参阅 `查询生成器`_ 文档。

用 DQL/SQL 进行查询
------------------------

除了查询生成器之外，你还可以使用 `Doctrine查询语言`_ 进行查询::

    // src/Repository/ProductRepository.php
    // ...

    public function findAllGreaterThanPrice($price): array
    {
        $entityManager = $this->getEntityManager();

        $query = $entityManager->createQuery(
            'SELECT p
            FROM App\Entity\Product p
            WHERE p.price > :price
            ORDER BY p.price ASC'
        )->setParameter('price', $price);

        // 返回一个数组形式的产品对象
        return $query->execute();
    }

或者直接使用 SQL::

    // src/Repository/ProductRepository.php
    // ...

    public function findAllGreaterThanPrice($price): array
    {
        $conn = $this->getEntityManager()->getConnection();

        $sql = '
            SELECT * FROM product p
            WHERE p.price > :price
            ORDER BY p.price ASC
            ';
        $stmt = $conn->prepare($sql);
        $stmt->execute(['price' => $price]);

        // 返回一个数组形式的数组（即原始数据集）
        return $stmt->fetchAll();
    }

使用SQL，你将获得原始数据，而不是对象（除非你使用 `NativeQuery`_ 功能）。

配置
-------------

参阅 :doc:`Doctrine 配置参考 </reference/configuration/doctrine>`.

关系和关联
------------------------------

Doctrine提供了管理数据库关系（也称为关联）所需的所有功能，包括ManyToOne，OneToMany，OneToOne和ManyToMany关系。

更多信息，请参阅 :doc:`/doctrine/associations`。

.. _doctrine-fixtures:

填充测试数据
-------------------

Doctrine提供了一个库，允许你以编程方式将测试数据加载到项目中（即“fixture data”）。
先安装它：

.. code-block:: terminal

    $ composer require doctrine/doctrine-fixtures-bundle --dev

然后，使用 ``make:fixtures`` 命令生成一个空 fixture 类：

.. code-block:: terminal

    $ php bin/console make:fixtures

    The class name of the fixtures to create (e.g. AppFixtures):
    > ProductFixture

定制该类以将 ``Product`` 对象加载到Doctrine ::

    // src/DataFixtures/ProductFixture.php
    namespace App\DataFixtures;

    use Doctrine\Bundle\FixturesBundle\Fixture;
    use Doctrine\Common\Persistence\ObjectManager;

    class ProductFixture extends Fixture
    {
        public function load(ObjectManager $manager)
        {
            $product = new Product();
            $product->setName('Priceless widget!');
            $product->setPrice(14.50);
            $product->setDescription('Ok, I guess it *does* have a price');
            $manager->persist($product);

            // 添加更多产品

            $manager->flush();
        }
    }

清空数据库并重新加载 *所有* fixture 类：

.. code-block:: terminal

    $ php bin/console doctrine:fixtures:load

更多详情，请参阅 "`DoctrineFixturesBundle`_" 文档。

扩展阅读
----------------------

.. toctree::
    :maxdepth: 1

    doctrine/associations
    doctrine/common_extensions
    doctrine/lifecycle_callbacks
    doctrine/event_listeners_subscribers
    doctrine/registration_form
    doctrine/custom_dql_functions
    doctrine/dbal
    doctrine/multiple_entity_managers
    doctrine/pdo_session_storage
    doctrine/mongodb_session_storage
    doctrine/resolve_target_entity
    doctrine/reverse_engineering

* `DoctrineFixturesBundle`_

.. _`Doctrine`: http://www.doctrine-project.org/
.. _`RFC 3986`: https://www.ietf.org/rfc/rfc3986.txt
.. _`MongoDB`: https://www.mongodb.org/
.. _`Doctrine的映射类型文档`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html
.. _`查询生成器`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/query-builder.html
.. _`Doctrine查询语言`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/dql-doctrine-query-language.html
.. _`Mapping Types Documentation`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#property-mapping
.. _`SQL关键字保留文档`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/basic-mapping.html#quoting-reserved-words
.. _`DoctrineMongoDBBundle`: https://symfony.com/doc/current/bundles/DoctrineMongoDBBundle/index.html
.. _`DoctrineFixturesBundle`: https://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html
.. _`事务和并发`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/transactions-and-concurrency.html
.. _`DoctrineMigrationsBundle`: https://github.com/doctrine/DoctrineMigrationsBundle
.. _`NativeQuery`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/native-sql.html
.. _`SensioFrameworkExtraBundle`: http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/index.html
.. _`ParamConverter`: http://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`索引键前缀的限制为767字节`: https://dev.mysql.com/doc/refman/5.6/en/innodb-restrictions.html
.. _`Doctrine screencast series`: https://symfonycasts.com/screencast/symfony-doctrine
