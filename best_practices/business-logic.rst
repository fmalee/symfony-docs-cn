组织你的业务逻辑
==============================

在计算机软件领域， **业务逻辑** 或域逻辑（domain logic）指的是“程序中用于处理现实世界中决定数据被创建、显示、存储和改变的业务规则那部分内容”。（参考 `full definition`_）

Symfony程序中，业务逻辑是指你为自己的不与框架本身重合（比如路由或控制器）的程序所写的全部定制代码。
域类(Domain classes)，Doctrine 实体以及被当作服务来使用的常规PHP类，都是业务逻辑的好样板。

对于多数项目来说，你应该把所有代码放到 ``src/`` 中。在这里，你可以创建任意目录，用来组织内容：

.. code-block:: text

    symfony-project/
    ├─ config/
    ├─ public/
    ├─ src/
    │  └─ Utils/
    │     └─ MyClass.php
    ├─ tests/
    ├─ var/
    └─ vendor/

.. _services-naming-and-format:

服务: 命名和配置
----------------------------------

.. best-practice::

    使用自动装配来自动配置应用的服务。

:doc:`Service autowiring </service_container/autowiring>` 是Symfony的服务容器提供的一项功能，
用于以最少的配置来管理服务。
它读取构造函数（或其他方法）上的类型提示，并自动将正确的服务传递给每个方法。
它还可以向需要它们的服务添加 :doc:`service tags </service_container/tags>`，例如Twig扩展，事件订阅者等。

博客应用需要一个工具，该工具可以将帖子标题（例如“Hello World”）转换为slug（例如“hello-world”）以将其作为帖子URL的一部分。
让我们在 ``src/Utils/`` 中创建一个新的 ``Slugger`` 类::

    // src/Utils/Slugger.php
    namespace App\Utils;

    class Slugger
    {
        public function slugify(string $value): string
        {
            // ...
        }
    }

如果你使用的是默认的 :ref:`default services.yaml configuration <service-container-services-load-example>`，
则此类将自动注册为ID为 ``App\Utils\Slugger`` 的服务（如果已在你的代码中导入该类，则只需简介的 ``Slugger::class``）。

.. best-practice::

    应用的服务id应该等于它们的类名，除非你为同一个类配置了多个服务（在这种情况下，使用蛇形命名(snake case)id）。

现在，你可以在任何其他服务或控制器类中使用自定义的slugger，例如 ``AdminController`` ::

    use App\Utils\Slugger;

    public function create(Request $request, Slugger $slugger)
    {
        // ...

        if ($form->isSubmitted() && $form->isValid()) {
            $slug = $slugger->slugify($post->getTitle());
            $post->setSlug($slug);

            // ...
        }
    }

服务也可以是 :ref:`public or private <container-public>`。
如果使用 :ref:`default services.yaml configuration <service-container-services-load-example>`，
则默认情况下所有服务都是私有的。

.. best-practice::

    服务应尽可能是 ``private``。这可以阻止通过 ``$container->get()`` 来访问该服务。
    取而代之的是你必须使用依赖注入。

服务的格式：YAML
--------------------

如果使用 :ref:`default services.yaml configuration <service-container-services-load-example>`，
则将自动配置大多数服务。但是，在某些边缘情况下，你需要手动配置服务（或其中的一部分）。

.. best-practice::

    使用 YAML 格式来配置你的服务。

这是有争议的，而且在我们的实验中，YAML 和 XML 的使用即便在开发者中亦存在争议，YAML略微占先。
两种格式拥有相同的性能，所以使用谁最终取决于个人口味。

我们推荐 YAML 是因为它对初学者友好且简洁。你当然可以选择你喜欢的格式。

使用持久层
-------------------------

Symfony 是一个HTTP框架，它只关心为每一个HTTP请求生成一个HTTP响应。
这就是为何 Symfony 不提供用于持久层（如数据库，外部API）通信的方法的原因。
对此，你可以选择自己的类库或策略来达到目的。

实际上，很多 Symfony 程序使用依赖于独立的 `Doctrine project`_ 的实体和仓库来定义其模型。
就像在业务逻辑中建议的那样，我们推荐把 Doctrine 实体存放在 ``src/Entity/`` 目录下。

我们的示例博客应用中定义的三个实体就是一个很好的例子：

.. code-block:: text

    symfony-project/
    ├─ ...
    └─ src/
       └─ Entity/
          ├─ Comment.php
          ├─ Post.php
          └─ User.php

Doctrine映射信息
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Doctrine实体是你存储在某个“数据库”中的原生PHP对象。
Doctrine只能通过你配置在模型类中元数据（metadata）来获知这个实体。Doctrine支持四种元数据格式：YAML，XML，PHP和注释。

.. best-practice::

    使用注释来定义 Doctrine 实体的映射信息。

到目前为止，注释是设置和查找映射信息最方便，最敏捷的方法::

    namespace App\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Doctrine\Common\Collections\ArrayCollection;

    /**
     * @ORM\Entity
     */
    class Post
    {
        const NUMBER_OF_ITEMS = 10;

        /**
         * @ORM\Id
         * @ORM\GeneratedValue
         * @ORM\Column(type="integer")
         */
        private $id;

        /**
         * @ORM\Column(type="string")
         */
        private $title;

        /**
         * @ORM\Column(type="string")
         */
        private $slug;

        /**
         * @ORM\Column(type="text")
         */
        private $content;

        /**
         * @ORM\Column(type="string")
         */
        private $authorEmail;

        /**
         * @ORM\Column(type="datetime")
         */
        private $publishedAt;

        /**
         * @ORM\OneToMany(
         *      targetEntity="Comment",
         *      mappedBy="post",
         *      orphanRemoval=true
         * )
         * @ORM\OrderBy({"publishedAt"="ASC"})
         */
        private $comments;

        public function __construct()
        {
            $this->publishedAt = new \DateTime();
            $this->comments = new ArrayCollection();
        }

        // getters and setters ...
    }

所有格式都具有相同的性能，因此这再一次最终成为品味问题。

Data Fixtures
~~~~~~~~~~~~~

由于 fixtures 功能并未在Symfony中默认开启，你应该执行下述命令来安装 Doctrine fixtures bundle：

.. code-block:: terminal

    $ composer require "doctrine/doctrine-fixtures-bundle"

然后，该 bundle 会自动启用，但仅适用于 ``dev`` 和 ``test`` 环境::

    // config/bundles.php

    return [
        // ...
        Doctrine\Bundle\FixturesBundle\DoctrineFixturesBundle::class => ['dev' => true, 'test' => true],
    ];

为了简化操作，我们推荐仅创建*一个* `fixture class`_，但如果类中的内容过长的话你也可以创建更多类。

假设你至少有一个 fixtures 类，而且数据库连接信息已被正确配置，通过以下命令即可加载你的 fixtures：:

.. code-block:: terminal

    $ php bin/console doctrine:fixtures:load

    Careful, database will be purged. Do you want to continue Y/N ? Y
      > purging database
      > loading App\DataFixtures\ORM\LoadFixtures

编码标准
----------------

Symfony源代码遵循 `PSR-1`_ 和 `PSR-2`_ 代码书写规范，它们是由PHP社区制定的。
你可以从 :doc:`the Symfony Coding standards </contributing/code/standards>` 了解更多，
甚至使用`PHP-CS-Fixer`_，这是一个命令行工具，它可以“秒完成”地修复整个代码库的编码标准。
Symfony源代码遵循PHP社区定义的 `PSR-1`_ 和 `PSR-2`_ 编码标准。
你可以了解 :doc:`the Symfony Coding standards </contributing/code/standards>` 的更多信息，
甚至可以使用 `PHP-CS-Fixer`_，它是一个命令行工具，可以在几秒钟内修复整个代码库的编码标准。

----

下一章: :doc:`/best_practices/controllers`

.. _`full definition`: https://en.wikipedia.org/wiki/Business_logic
.. _`Doctrine project`: http://www.doctrine-project.org/
.. _`fixture class`: https://symfony.com/doc/current/bundles/DoctrineFixturesBundle/index.html#writing-simple-fixtures
.. _`PSR-1`: https://www.php-fig.org/psr/psr-1/
.. _`PSR-2`: https://www.php-fig.org/psr/psr-2/
.. _`PHP-CS-Fixer`: https://github.com/FriendsOfPHP/PHP-CS-Fixer
