.. index::
   single: Form; Form testing

如何对表单进行单元测试
===========================

Form组件由3个核心对象组成：一个表单类型（实现了
:class:`Symfony\\Component\\Form\\FormTypeInterface`）、
:class:`Symfony\\Component\\Form\\Form` 和
:class:`Symfony\\Component\\Form\\FormView`。

通常由程序员操作的唯一类是作为一个表单蓝图的表单类型类。它用于生成 ``Form`` 和 ``FormView``。
你可以通过模拟它与工厂的交互来直接测试它，但这样会很复杂。最好将它传递给表单工厂，就像在实际应用中完成一样。
它可以很简单的来引导(bootstrap)，你可以信任Symfony组件以将它们用作测试基础。

已经有一个类可以从简单的表单类型测试中受益：
:class:`Symfony\\Component\\Form\\Test\\TypeTestCase`。
它用于测试核心类型，你也可以使用它来测试你的类型。

.. note::

    根据你安装Symfony或Symfony Form组件的方式，测试可能无法下载。
    如果是这种情况，请使用Composer是添加 ``--prefer-source`` 选项。

基础知识
----------

最简单的 ``TypeTestCase`` 实现如下所示::

    // tests/Form/Type/TestedTypeTest.php
    namespace App\Tests\Form\Type;

    use App\Form\Type\TestedType;
    use App\Model\TestObject;
    use Symfony\Component\Form\Test\TypeTestCase;

    class TestedTypeTest extends TypeTestCase
    {
        public function testSubmitValidData()
        {
            $formData = array(
                'test' => 'test',
                'test2' => 'test2',
            );

            $objectToCompare = new TestObject();
            // $objectToCompare 将从表单提交中检索数据; 并将它作为第二个参数传递
            $form = $this->factory->create(TestedType::class, $objectToCompare);

            $object = new TestObject();
            // ...使用 $formData 中存储的数据填充 $object 属性

            // 直接将数据提交到表单
            $form->submit($formData);

            $this->assertTrue($form->isSynchronized());

            // 检查表单提交时是否按预期修改了 $objectToCompare
            $this->assertEquals($object, $objectToCompare);

            $view = $form->createView();
            $children = $view->children;

            foreach (array_keys($formData) as $key) {
                $this->assertArrayHasKey($key, $children);
            }
        }
    }

那么，它测试了什么？这里有一个详细的解释。

首先验证 ``FormType`` 是否编译。这包括基类继承，``buildForm()`` 函数和选项的解析。
这应该是你写的第一个测试::

    $form = $this->factory->create(TestedType::class, $objectToCompare);

此测试检查表单使用的所有数据转换器有没有失败的。如果一个数据转换器抛出异常，则
:method:`Symfony\\Component\\Form\\FormInterface::isSynchronized`
方法返回 ``false``::

    $form->submit($formData);
    $this->assertTrue($form->isSynchronized());

.. note::

    不要测试验证：它被应用在一个在测试用例中不激活的监听器上，并且它依赖验证配置。
    相反，你可以直接对自定义约束进行单元测试。

接下来，验证表单的提交和映射。下面的测试测试检查了所有的字段是否正确被指定::

    $this->assertEquals($object, $objectToCompare);

最后，检查 ``FormView`` 的创建。你应该检查要显示的所有部件是否在子属性中可用::

    $view = $form->createView();
    $children = $view->children;

    foreach (array_keys($formData) as $key) {
        $this->assertArrayHasKey($key, $children);
    }

.. tip::

    通过 :ref:`PHPUnit数据提供器 <testing-data-providers>`
    使用相同的测试代码来测试多个表单条件。

从服务容器测试类型
-----------------------------------------

你的表单可能被用作一个服务，因为它依赖于一些其他服务（例如Doctrine实体管理器）。
在这些情况下，使用上面的代码将不起作用，因为Form组件只是实例化表单类型而不会给构造函数传递任何参数。

要解决这个问题，你必须模拟注入的依赖，然后实例化你自己的表单类型，并使用
:class:`Symfony\\Component\\Form\\PreloadedExtension`
来确保 ``FormRegistry`` 使用了已创建的实例::

    // tests/Form/Type/TestedTypeTest.php
    namespace App\Tests\Form\Type;

    use App\Form\Type\TestedType;
    use Doctrine\Common\Persistence\ObjectManager;
    use Symfony\Component\Form\PreloadedExtension;
    use Symfony\Component\Form\Test\TypeTestCase;
    // ...

    class TestedTypeTest extends TypeTestCase
    {
        private $objectManager;

        protected function setUp()
        {
            // 模拟任何依赖
            $this->objectManager = $this->createMock(ObjectManager::class);

            parent::setUp();
        }

        protected function getExtensions()
        {
            // 使用模拟的依赖来创建一个类型实例
            $type = new TestedType($this->objectManager);

            return array(
                // 使用 PreloadedExtension 注册该类型实例
                new PreloadedExtension(array($type), array()),
            );
        }

        public function testSubmitValidData()
        {
            // 不直接创建一个新实例，而是使用在 getExtensions() 中创建的实例。
            $form = $this->factory->create(TestedType::class);

            // ... 你的测试
        }
    }

添加自定义扩展
------------------------

在使用 :doc:`表单扩展 </form/create_form_type_extension>` 时经常会添加的一些选项。
其中一个案例是使用 ``invalid_message`` 选项的 ``ValidatorExtension``。
``TypeTestCase`` 只加载核心的表单扩展，这意味着如果你尝试测试一个依赖其他扩展的类，那么
:class:`Symfony\\Component\\OptionsResolver\\Exception\\InvalidOptionsException`
将可能被触发。
:method:`Symfony\\Component\\Form\\Test\\TypeTestCase::getExtensions`
方法允许你返回一个扩展列表并去注册它们::

    // tests/Form/Type/TestedTypeTest.php
    namespace App\Tests\Form\Type;

    // ...
    use App\Form\Type\TestedType;
    use Symfony\Component\Form\Extension\Validator\ValidatorExtension;
    use Symfony\Component\Form\Form;
    use Symfony\Component\Validator\ConstraintViolationList;
    use Symfony\Component\Validator\Mapping\ClassMetadata;
    use Symfony\Component\Validator\Validator\ValidatorInterface;

    class TestedTypeTest extends TypeTestCase
    {
        private $validator;

        protected function getExtensions()
        {
            $this->validator = $this->createMock(ValidatorInterface::class);
            // 在 PHPUnit 5.3 或更低版本中使用 getMock()
            // $this->validator = $this->getMock(ValidatorInterface::class);
            $this->validator
                ->method('validate')
                ->will($this->returnValue(new ConstraintViolationList()));
            $this->validator
                ->method('getMetadataFor')
                ->will($this->returnValue(new ClassMetadata(Form::class)));

            return array(
                new ValidatorExtension($this->validator),
            );
        }

        // ... 你的测试
    }

另外，也可以分别使用
:method:`Symfony\\Component\\Form\\Test\\FormIntegrationTestCase::getTypes`、
:method:`Symfony\\Component\\Form\\Test\\FormIntegrationTestCase::getTypeExtensions`
和 :method:`Symfony\\Component\\Form\\Test\\FormIntegrationTestCase::getTypeGuessers`
方法来加载自定义的表单类型、表单类型扩展以及类型猜测器。
