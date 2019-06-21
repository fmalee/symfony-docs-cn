.. index::
   single: Tests; Database

如何测试与数据库交互的代码
=================================================

如果你的代码与数据库交互，例如从中读取数据或将数据存储到数据库中，则需要调整测试以将其考虑在内。
有很多方法可以解决这个问题。在单元测试中，你可以创建一个 ``Repository``
的模拟并使用它来返回预期的对象。
在功能测试中，你可能需要准备一个具有预定义值的测试数据库，以确保你的测试始终具有相同的数据。

.. note::

    如果要直接测试数据库查询，请参阅 :doc:`/testing/doctrine`。

.. tip::

    提高与数据库交互的测试的性能的一种流行技术是在每次测试之前启动事务，并在测试完成后回滚。
    这使得无需在每次测试之前重新创建数据库或重新加载 fixture。
    名为 `DoctrineTestBundle`_ 的社区bundle提供了此功能。

在单元测试中模拟 ``Repository``
-----------------------------------------

如果要单独测试依赖于一个Doctrine仓库的代码，则需要模拟 ``Repository``。
但是，通常你是将 ``EntityManager`` 注入到类中并使用它来获取仓库的。
这就使得事情变得更加复杂，因为你需要同时模拟 ``EntityManager`` 和你的仓库类。

.. tip::

    通过将仓库注册为一个
    :doc:`工厂服务 </service_container/factories>`，可以（并且一个好主意）让你直接注入该仓库。
    这需要一些设置工作，但是可以使得测试更容易，因为你只需要模拟该仓库就可以了。

假设你要测试的类如下所示::

    // src/Salary/SalaryCalculator.php
    namespace App\Salary;

    use App\Entity\Employee;
    use Doctrine\Common\Persistence\ObjectManager;

    class SalaryCalculator
    {
        private $objectManager;

        public function __construct(ObjectManager $objectManager)
        {
            $this->objectManager = $objectManager;
        }

        public function calculateTotalSalary($id)
        {
            $employeeRepository = $this->objectManager
                ->getRepository(Employee::class);
            $employee = $employeeRepository->find($id);

            return $employee->getSalary() + $employee->getBonus();
        }
    }

由于 ``EntityManagerInterface`` 通过构造函数被注入到类中，因此可以在测试中传递一个模拟对象::

    // tests/Salary/SalaryCalculatorTest.php
    namespace App\Tests\Salary;

    use App\Entity\Employee;
    use App\Salary\SalaryCalculator;
    use Doctrine\Common\Persistence\ObjectManager;
    use Doctrine\Common\Persistence\ObjectRepository;
    use PHPUnit\Framework\TestCase;

    class SalaryCalculatorTest extends TestCase
    {
        public function testCalculateTotalSalary()
        {
            $employee = new Employee();
            $employee->setSalary(1000);
            $employee->setBonus(1100);

            // 现在，模拟该仓库，以便返回该 employee 的模拟
            $employeeRepository = $this->createMock(ObjectRepository::class);
            // 在 PHPUnit 5.3 或更低版本中使用 getMock()
            // $employeeRepository = $this->getMock(ObjectRepository::class);
            $employeeRepository->expects($this->any())
                ->method('find')
                ->willReturn($employee);

            // 最后，模拟 EntityManager 以返回该仓库的模拟
            $objectManager = $this->createMock(ObjectManager::class);
            // 在 PHPUnit 5.3 或更低版本中使用 getMock()
            // $objectManager = $this->getMock(ObjectManager::class);
            $objectManager->expects($this->any())
                ->method('getRepository')
                ->willReturn($employeeRepository);

            $salaryCalculator = new SalaryCalculator($objectManager);
            $this->assertEquals(2100, $salaryCalculator->calculateTotalSalary(1));
        }
    }

在这个例子中，你是从内到外构建模拟，首先创建一个
`employee`，然后创建返回该 `employee` 的 ``Repository``，最后创建返回该
``Repository`` 的 ``EntityManager``。
通过这样处理，测试中就不会涉及真正的类。

更改功能测试的数据库设置
-----------------------------------------------

如果你有功能测试，那么会希望它们与一个真实的数据库进行交互。
大多数情况下，你希望使用一个专用的数据库连接，以确保不会覆盖在开发应用时输入的数据，同时也可以在每次测试之前清除数据库。

为此，你可以在 ``phpunit.xml.dist`` 中重写 ``DATABASE_URL``
环境变量的值，以便为测试使用不同的数据库：

.. code-block:: xml

    <?xml version="1.0" charset="utf-8" ?>
    <phpunit>
        <php>
            <!-- 该值是DSN格式的Doctrine连接字符串 -->
            <env name="DATABASE_URL" value="mysql://USERNAME:PASSWORD@127.0.0.1/DB_NAME"/>
        </php>
        <!-- ... -->
    </phpunit>

.. _`DoctrineTestBundle`: https://github.com/dmaicher/doctrine-test-bundle
