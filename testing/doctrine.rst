.. index::
   single: Tests; Doctrine

如何测试Doctrine仓库
=================================

不建议对Symfony项目中的Doctrine仓库进行单元测试。
当你处理一个仓库时，你是在真实的处理那些东西，因为你正在对一个真实的数据库连接进行测试。

幸运的是，你可以轻松地针对一个真实数据库来测试查询。

功能测试
------------------

如果你需要真实的执行一个查询，则需要引导内核以获得一个有效连接。
在这种情况下，你将继承 ``KernelTestCase`` 以使Symfony环境可用::

    // tests/Repository/ProductRepositoryTest.php
    namespace App\Tests\Repository;

    use App\Entity\Product;
    use Symfony\Bundle\FrameworkBundle\Test\KernelTestCase;

    class ProductRepositoryTest extends KernelTestCase
    {
        /**
         * @var \Doctrine\ORM\EntityManager
         */
        private $entityManager;

        /**
         * {@inheritDoc}
         */
        protected function setUp()
        {
            $kernel = self::bootKernel();

            $this->entityManager = $kernel->getContainer()
                ->get('doctrine')
                ->getManager();
        }

        public function testSearchByCategoryName()
        {
            $products = $this->entityManager
                ->getRepository(Product::class)
                ->searchByCategoryName('foo')
            ;

            $this->assertCount(1, $products);
        }

        /**
         * {@inheritDoc}
         */
        protected function tearDown()
        {
            parent::tearDown();

            $this->entityManager->close();
            $this->entityManager = null; // avoid memory leaks
        }
    }
