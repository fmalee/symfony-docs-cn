.. index::
    single: Doctrine; Associations

如何使用Doctrine关联关系
==================================================

.. admonition:: Screencast
    :class: screencast

    更喜欢视频教程? 可以观看 `Mastering Doctrine Relations`_ 系列录像.

有 **两种** 主要的关系/关联类型：

``ManyToOne`` / ``OneToMany``
    最常见的关系，使用外键列（例如，``product`` 表中的 ``category_id`` 列）映射到数据库中。
    这实际上只是 *一种* 关联类型，但可以从关系的两个不同面来分开处理。

``ManyToMany``
    当关系的两面可以都具有许多的另一面时，需要使用一个连接表。
    （例如“学生”和“班级”：每个学生在许多班级中，并且每个班级有许多学生）。

首先，你需要确定要使用的关系。
如果关系的两个方面都包含许多另一方（例如“学生”和“班级”），则需要一个 ``ManyToMany`` 关系。
否则，你可能需要一个 ``ManyToOne``。

.. tip::

    还有一个 ``OneToOne`` 关系（例如，一个用户有一个用户资料，反之亦然）。
    在实践中，会将它当做 ``ManyToOne`` 处理。

ManyToOne / OneToMany
-------------------------------------

假设你的应用中的每个产品都属于一个类别。
在这种情况下，你将需要一个 ``Category`` 类，以及将 ``Product`` 对象与 ``Category`` 对象相关联的方法。

首先创建一个包含 ``name`` 字段的 ``Category`` 实体：

.. code-block:: terminal

    $ php bin/console make:entity Category

    New property name (press <return> to stop adding fields):
    > name

    Field type (enter ? to see all types) [string]:
    > string

    Field length [255]:
    > 255

    Can this field be null in the database (nullable) (yes/no) [no]:
    > no

    New property name (press <return> to stop adding fields):
    >
    (press enter again to finish)

这将生成新的实体类::

    // src/Entity/Category.php
    // ...

    class Category
    {
        /**
         * @ORM\Id
         * @ORM\GeneratedValue
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string")
         */
        private $name;

        // ... getters and setters
    }

映射ManyToOne关系
----------------------------------

在此示例中，每个类别可以与 *许多* 产品相关联。但是，每个产品只能与 *一个* 类别相关联。
这种关系可以概括为：*许多* 产品属于 *一个* 类别（或等效地， *一个* 类别拥有 *多个* 产品）。

从 ``Product`` 实体的角度来看，这是一种多对一的关系。
从 ``Category`` 实体的角度来看，这是一种一对多的关系。

要映射它，首先使用 ``ManyToOne`` 注释在 ``Product`` 类上创建一个 ``category`` 属性。
你可以手动执行此操作，也可以使用 ``make:entity`` 命令执行此操作，该命令会向你询问有关你的关系的几个问题。
如果你不确定答案，请不要担心！你可以随时更改该设置：

.. code-block:: terminal

    $ php bin/console make:entity

    Class name of the entity to create or update (e.g. BraveChef):
    > Product

    New property name (press <return> to stop adding fields):
    > category

    Field type (enter ? to see all types) [string]:
    > relation

    What class should this entity be related to?:
    > Category

    Relation type? [ManyToOne, OneToMany, ManyToMany, OneToOne]:
    > ManyToOne

    Is the Product.category property allowed to be null (nullable)? (yes/no) [yes]:
    > no

    Do you want to add a new property to Category so that you can access/update
    Product objects from it - e.g. $category->getProducts()? (yes/no) [yes]:
    > yes

    New field name inside Category [products]:
    > products

    Do you want to automatically delete orphaned App\Entity\Product objects
    (orphanRemoval)? (yes/no) [no]:
    > no

    New property name (press <return> to stop adding fields):
    >
    (press enter again to finish)

这使得 *两个* 实体发生了改变。首先，它为 ``Product`` 实体添加了一个新的 ``category``
属性（以及getter和setter方法）：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Product.php

        // ...
        class Product
        {
            // ...

            /**
             * @ORM\ManyToOne(targetEntity="App\Entity\Category", inversedBy="products")
             */
            private $category;

            public function getCategory(): ?Category
            {
                return $this->category;
            }

            public function setCategory(?Category $category): self
            {
                $this->category = $category;

                return $this;
            }
        }

    .. code-block:: yaml

        # src/Resources/config/doctrine/Product.orm.yml
        App\Entity\Product:
            type: entity
            # ...
            manyToOne:
                category:
                    targetEntity: App\Entity\Category
                    inversedBy: products
                    joinColumn:
                        nullable: false

    .. code-block:: xml

        <!-- src/Resources/config/doctrine/Product.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="App\Entity\Product">
                <!-- ... -->
                <many-to-one
                    field="category"
                    target-entity="App\Entity\Category"
                    inversed-by="products">
                    <join-column nullable="false" />
                </many-to-one>
            </entity>
        </doctrine-mapping>

该 ``ManyToOne`` 映射是必需的。
它告诉Doctrine使用 ``product`` 表上的 ``category_id`` 列将该表中的每条记录与 ``category`` 表中的记录相关联。

接下来，由于 *一个* ``Category`` 对象将涉及 *许多* ``Product`` 对象，
因此 ``make:entity`` 命令 *还* 向将保存这些对象的 ``Category`` 类添加了一个 ``products`` 属性：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Category.php

        // ...
        use Doctrine\Common\Collections\ArrayCollection;
        use Doctrine\Common\Collections\Collection;

        class Category
        {
            // ...

            /**
             * @ORM\OneToMany(targetEntity="App\Entity\Product", mappedBy="category")
             */
            private $products;

            public function __construct()
            {
                $this->products = new ArrayCollection();
            }

            /**
             * @return Collection|Product[]
             */
            public function getProducts(): Collection
            {
                return $this->products;
            }

            // 同时还添加了 addProduct() 和 removeProduct()
        }

    .. code-block:: yaml

        # src/Resources/config/doctrine/Category.orm.yml
        App\Entity\Category:
            type: entity
            # ...
            oneToMany:
                products:
                    targetEntity: App\Entity\Product
                    mappedBy: category
        # Don't forget to initialize the collection in
        # the __construct() method of the entity

    .. code-block:: xml

        <!-- src/Resources/config/doctrine/Category.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="App\Entity\Category">
                <!-- ... -->
                <one-to-many
                    field="products"
                    target-entity="App\Entity\Product"
                    mapped-by="category" />

                <!--
                    don't forget to init the collection in
                    the __construct() method of the entity
                -->
            </entity>
        </doctrine-mapping>

前面展示的 ``ManyToOne`` 映射是 *必需* 的，而这次的 ``OneToMany`` 映射是 *可选* 的：
只有在你希望能够访问与类别相关的产品时才添加它（这是 ``make:entity`` 的问题之一）。
在这个例子中，在需要调用 ``$category->getProducts()`` 时，该关系就派上用场了。
如果你不想要它，那么你也不需要配置 ``inversedBy`` 或 ``mappedBy``。

.. sidebar:: 什么是ArrayCollection？

    ``__construct()`` 内部的代码很重要：
    ``$products`` 属性必须是实现Doctrine的 ``Collection`` 接口的一个集合对象。
    在这个例子中，使用的是 ``ArrayCollection`` 对象。
    它的行为和外观几乎 *完全* 像一个数组，但多了一些额外的灵活性。
    想象一下它就是一个 ``array`` 并且你将会处于良好状态。

你的数据库已设置好！现在，像往常一样执行迁移：

.. code-block:: terminal

    $ php bin/console doctrine:migrations:diff
    $ php bin/console doctrine:migrations:migrate

由于这种关系，它将会在 ``product`` 表上创建一个 ``category_id`` 外键列。
Doctrine已经准备好持久化我们的关系！

保存相关实体
-----------------------

现在你可以看到这个新代码的实际应用！想象一下你的控制器内部::

    // ...

    use App\Entity\Category;
    use App\Entity\Product;
    use Symfony\Component\HttpFoundation\Response;

    class ProductController extends AbstractController
    {
        /**
         * @Route("/product", name="product")
         */
        public function index()
        {
            $category = new Category();
            $category->setName('Computer Peripherals');

            $product = new Product();
            $product->setName('Keyboard');
            $product->setPrice(19.99);
            $product->setDescription('Ergonomic and stylish!');

            // 将产品关联到类别
            $product->setCategory($category);

            $entityManager = $this->getDoctrine()->getManager();
            $entityManager->persist($category);
            $entityManager->persist($product);
            $entityManager->flush();

            return new Response(
                'Saved new product with id: '.$product->getId()
                .' and new category with id: '.$category->getId()
            );
        }
    }

当你浏览 ``/product`` 时，它会同时向 ``category`` 表和 ``product`` 表添加一行。
新产品的 ``product.category_id`` 列已经设置为新类别的 ``id`` 列的值。
Doctrine为你管理这种关系的持久性：

.. image:: /_images/doctrine/mapping_relations.png
    :align: center

如果你是ORM的新手，这是 *最难* 的概念：你需要停止考虑你的数据库，而 *只* 考虑你的对象。
你需要将整个 ``Category`` *对象* （而不是类别的id）设置到 ``Product``。
保存时，Doctrine会处理其余的事情。

.. sidebar:: 从从属方更新关系

    可以通过调用 ``$category->addProduct()`` 来改变关系吗？
    可以的，因为 ``make:entity`` 命令帮助了我们实现了它。
    有关更多详细信息，请参阅：:ref:`associations-inverse-side`。

获取相关对象
------------------------

当你需要获取关联对象时，你的工作流程就和以前一样。
首先，获取一个 ``$product`` 对象，然后访问与其相关的 ``Category`` 对象::

    use App\Entity\Product;
    // ...

    public function show($id)
    {
        $product = $this->getDoctrine()
            ->getRepository(Product::class)
            ->find($id);

        // ...

        $categoryName = $product->getCategory()->getName();

        // ...
    }

在此示例中，你首先基于 ``Product`` 对象查询产品的 ``id``。
这样就可以只查询产品数据并生成(hydrates) ``$product``。
接下来，当你调用 ``$product->getCategory()->getName()`` 时，
Doctrine默默地进行第二次查询以找到与 ``Product`` 相关的 ``Category``。
并准备好 ``$category`` 对象，以将其返回给你。

.. image:: /_images/doctrine/mapping_relations_proxy.png
    :align: center

重要的是你可以轻松访问产品的相关类别，但在你需要类别之前，实际上并未检索对应的类别数据（即“延迟加载”）。

因为我们映射了可选的 ``OneToMany`` 侧，你还可以从另一个方向进行查询::

    public function showProducts($id)
    {
        $category = $this->getDoctrine()
            ->getRepository(Category::class)
            ->find($id);

        $products = $category->getProducts();

        // ...
    }

在这个例子中，会发生同样的事情：首先查询单个 ``Category`` 对象。
然后，只有当你访问产品时，Doctrine才会进行第二次查询以检索相关的 ``Product`` 对象。
通过添加连接可以避免这种额外的查询。

.. sidebar:: 关系和代理类

    这种“延迟加载”是可能的，因为在必要时，Doctrine返回一个“代理”对象来代替真实对象。再看一下上面的例子::

        $product = $this->getDoctrine()
            ->getRepository(Product::class)
            ->find($id);

        $category = $product->getCategory();

        // 打印 "Proxies\AppEntityCategoryProxy"
        dump(get_class($category));
        die();

    此代理对象继承了真实的 ``Category`` 对象，其外观和行为与其完全相同。
    不同之处在于，通过使用代理对象，Doctrine可以延迟查询实际的 ``Category`` 数据，
    直到你确实需要该数据（例如，``$category->getName()`` 被调用）。

    该代理类由Doctrine生成并存储在缓存目录中。
    你可能永远不会注意到你的 ``$category`` 对象实际上是一个代理对象。

    在下一节中，当你一次性检索产品和类别数据时（通过 *join*），
    因为不需要延迟加载任何内容，Doctrine将返回 *真实* 的 ``Category`` 对象，。

连接相关记录
-----------------------

在上面的示例中，进行了两个查询 - 一个用于原始对象（例如 ``Category``），一个用于相关对象（例如 ``Product`` 对象）。

.. tip::

    请记住，你可以通过Web调试工具栏查看请求期间发出的所有查询。

如果你事先就知道需要访问这两个对象，则可以通过在原始查询中发出一个连接来避免第二个查询。
将以下方法添加到 ``ProductRepository`` 类中::

    // src/Repository/ProductRepository.php
    public function findOneByIdJoinedToCategory($productId)
    {
        return $this->createQueryBuilder('p')
            // p.category是指产品上的“category”属性
            ->innerJoin('p.category', 'c')
            // select 所有类别数据以避免再次查询
            ->addSelect('c')
            ->andWhere('p.id = :id')
            ->setParameter('id', $productId)
            ->getQuery()
            ->getOneOrNullResult();
    }

这 *仍然* 会返回一个 ``Product`` 对象数组。
但现在，当你调用 ``$product->getCategory()`` 并使用该数据时，不会再进行第二次查询。

现在，你可以在控制器中使用此方法来查询一个 ``Product`` 对象以及相关的 ``Category``，并且只有一个查询::

    public function show($id)
    {
        $product = $this->getDoctrine()
            ->getRepository(Product::class)
            ->findOneByIdJoinedToCategory($id);

        $category = $product->getCategory();

        // ...
    }

.. _associations-inverse-side:

从从属方设置信息
-----------------------------------------

到目前为止，你已通过调用 ``$product->setCategory($category)`` 更新了这段关系。
这不是偶然的！每个关系具有两个方面：
在这个例子中，``Product.category`` 是 *拥有* 方，而 ``Category.products`` 是 *从属* 方。

要更新数据库中的关系，*必须* 在 *拥有* 方设置该关系。
拥有方总是设置 ``ManyToOne`` 映射的那侧（对于 ``ManyToMany`` 关系，你可以选择哪一方是拥有方）。

这是否意味着无法调用 ``$category->addProduct()`` 或 ``$category->removeProduct()`` 来更新数据库？
事实上，由于 ``make:entity`` 命令生成了一些聪明的代码，所以这种调用是可行的::

    // src/Entity/Category.php

    // ...
    class Category
    {
        // ...

        public function addProduct(Product $product): self
        {
            if (!$this->products->contains($product)) {
                $this->products[] = $product;
                $product->setCategory($this);
            }

            return $this;
        }
    }

*关键* 是 ``$product->setCategory($this)``，它设置了 *拥有* 方。
因此，当你保存时，该关系 *将* 在数据库中更新。

怎么样在 ``Category`` 中移除一个 ``Product``？
为此，``make:entity`` 命令还生成了一个 ``removeProduct()`` 方法::

    // src/Entity/Category.php

    // ...
    class Category
    {
        // ...

        public function removeProduct(Product $product): self
        {
            if ($this->products->contains($product)) {
                $this->products->removeElement($product);
                // 将拥有方设置为null（除非已经更改）
                if ($product->getCategory() === $this) {
                    $product->setCategory(null);
                }
            }

            return $this;
        }
    }

得益于此，如果你调用了 ``$category->removeProduct($product)``，
``Product`` 上的 ``category_id`` 将在数据库中被设置为 ``null``。

但是，区别于设置 ``category_id`` 为空，如果你想要在 ``Product`` 变得“孤立”（即没有 ``Category``）时删除它？
则请使用 ``Category`` 内部的 `orphanRemoval`_ 选项::

    // src/Entity/Category.php

    // ...

    /**
     * @ORM\OneToMany(targetEntity="App\Entity\Product", mappedBy="category", orphanRemoval=true)
     */
    private $products;

得益于此，如果 ``Product`` 从 ``Category`` 中删除，它将从数据库中完全删除。

关于关联的更多信息
--------------------------------

本节介绍了一种常见的实体关系，即一对多关系。
有关如何使用其他类型关系的更多高级详细信息和示例（例如，一对一，多对多），请参阅Doctrine的 `关联映射文档`_。

.. note::

    如果你正在使用注释，则需要在所有注释前面添加 ``@ORM\`` （例如 ``@ORM\OneToMany``），这在Doctrine的文档中没有反映出来。

.. _`关联映射文档`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/association-mapping.html
.. _`orphanRemoval`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/working-with-associations.html#orphan-removal
.. _`Mastering Doctrine Relations`: https://symfonycasts.com/screencast/doctrine-relations
