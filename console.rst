.. index::
   single: Console; Create commands

控制台
================

Symfony框架通过 ``bin/console`` 脚本(如，广为人知的 ``bin/console cache:clear`` 命令)提供了大量命令。
这些命令是通过 :doc:`Console组件 </components/console>` 被创建的。
你也可以使用它创建自己的命令。

控制台: APP_ENV & APP_DEBUG
---------------------------------

控制台命令在 ``.env`` 文件的 ``APP_ENV`` 变量中定义的 :ref:`环境 <config-dot-env>` 中运行，
默认情况下为 ``dev``。
它还读取 ``APP_DEBUG`` 值以开启或关闭“调试”模式（默认为 ``1``，即开启）。

要在其他环境或调试模式下运行该命令，请编辑 ``APP_ENV`` 和 ``APP_DEBUG`` 的值。

创建命令
------------------

命令在继承 :class:`Symfony\\Component\\Console\\Command\\Command` 的类中定义。
例如，你可能需要一个命令来创建用户::

    // src/Command/CreateUserCommand.php
    namespace App\Command;

    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    class CreateUserCommand extends Command
    {
        // 命令的名称（"bin/console"之后的部分）
        protected static $defaultName = 'app:create-user';

        protected function configure()
        {
            // ...
        }

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            // ...
        }
    }

配置命令
-----------------------

首先，你必须在 ``configure()`` 方法中配置命令的名称。
然后，你可以选择定义帮助信息以及 :doc:`输入选项和参数 </console/input>`::

    // ...
    protected function configure()
    {
        $this
            // 运行 "php bin/console list" 后展示的简短介绍
            ->setDescription('Creates a new user.')

            // 使用“--help”选项运行命令时显示的完整的命令说明
            ->setHelp('This command allows you to create a user...')
        ;
    }

``configure()`` 方法会在该命令的构造函数的末尾自动调用。
如果你的命令定义了自己的构造函数，请先设置属性然后再调用父构造函数，
以使这些属性在 ``configure()`` 方法中生效::

    class CreateUserCommand extends Command
    {
        // ...

        public function __construct(bool $requirePassword = false)
        {
            // 最佳实践建议首先调用父构造函数，然后设置自己的属性。
            // 但在这例子中不起作用，因为 configure() 需要在此构造函数中设置的属性
            $this->requirePassword = $requirePassword;

            parent::__construct();
        }

        public function configure()
        {
            $this
                // ...
                ->addArgument('password', $this->requirePassword ? InputArgument::OPTIONAL : InputArgument::REQUIRED, 'User password')
            ;
        }
    }

注册命令
-----------------------

必须将Symfony命令注册为服务并使用 ``console.command`` 标签进行 :doc:`标记 </service_container/tags>`。
如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
自动配置已经为你完成这些工作了。

执行命令
---------------------

配置并注册命令后，就可以在终端中执行：

.. code-block:: terminal

    $ php bin/console app:create-user

正如你所料，此命令将不执行任何操作，因为你尚未编写任何逻辑。
请在 ``execute()`` 方法中添加自己的逻辑。

控制台输出
--------------

``execute()`` 方法可以访问输出流以将消息写入控制台::

    // ...
    protected function execute(InputInterface $input, OutputInterface $output)
    {
        // 输出多行到控制台（在每行末尾添加“\n”）
        $output->writeln([
            'User Creator',
            '============',
            '',
        ]);

        // someMethod() 返回的值可以是迭代器（https://secure.php.net/iterator）
        // 生成并返回带有 'yield' PHP关键字的消息
        $output->writeln($this->someMethod());

        // 输出一条消息后跟着“\n”
        $output->writeln('Whoa!');

        // 输出一条消息而不在行尾添加“\n”
        $output->write('You are about to ');
        $output->write('create a user.');
    }

.. versionadded:: 4.1
    Symfony 4.1中引入了 ``write()`` 和 ``writeln()`` 方法中PHP迭代器的支持。

现在，尝试执行命令：

.. code-block:: terminal

    $ php bin/console app:create-user
    User Creator
    ============

    Whoa!
    You are about to create a user.

.. _console-output-sections:

输出切片
~~~~~~~~~~~~~~~

.. versionadded:: 4.1
    输出切片在Symfony 4.1中引入。

常规控制台输出可以分割为多个独立区域，称为“输出切片”。
需要清除和覆盖输出信息时，可以创建一个或多个切片。

Section 可以通过使用 :method:`Symfony\\Component\\Console\\Output\\ConsoleOutput::section` 方法创建，
该方法返回一个 :class:`Symfony\\Component\\Console\\Output\\ConsoleSectionOutput` 实例::

    class MyCommand extends Command
    {
        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $section1 = $output->section();
            $section2 = $output->section();
            $section1->writeln('Hello');
            $section2->writeln('World!');
            // 输出呈现 "Hello\nWorld!\n"

            // overwrite() 用给定的内容替换所有现有的切片内容
            $section1->overwrite('Goodbye');
            // 输出呈现 "Goodbye\nWorld!\n"

            // clear() 清除所有的切片内容...
            $section2->clear();
            // 输出呈现 "Goodbye\n"

            // ...但你也可以删除给定数量的行
            // (此示例删除该切片的最后两行)
            $section1->clear(2);
            // 输出现在完全空了！
        }
    }

.. note::

    在切片中显示信息时会自动添加新行。

输出切片允许你以高级方式操作控制台输出，
例如独立显示更新的 :ref:`多个进度条 <console-multiple-progress-bars>`
以及在已经渲染的表格中将 :ref:`附加行到表格 <console-modify-rendered-tables>`。

控制输入
-------------

使用输入选项或参数将信息传递给命令::

    use Symfony\Component\Console\Input\InputArgument;

    // ...
    protected function configure()
    {
        $this
            // 配置一个参数
            ->addArgument('username', InputArgument::REQUIRED, 'The username of the user.')
            // ...
        ;
    }

    // ...
    public function execute(InputInterface $input, OutputInterface $output)
    {
        $output->writeln([
            'User Creator',
            '============',
            '',
        ]);

        // 使用 getArgument() 获取参数值
        $output->writeln('Username: '.$input->getArgument('username'));
    }

现在，你可以将用户名传递给命令了：

.. code-block:: terminal

    $ php bin/console app:create-user Wouter
    User Creator
    ============

    Username: Wouter

.. seealso::

    有关控制台选项和参数的更多信息，请阅读 :doc:`/console/input`。

从服务容器中获取服务
-------------------------------------------

要实际创建新用户，该命令必须访问某些 :doc:`服务 </service_container>`。
由于你的命令已注册为服务，因此可以使用常规的依赖项注入。
想象一下，你有一个要将访问的 ``App\Service\UserManager`` 服务::

    // ...
    use Symfony\Component\Console\Command\Command;
    use App\Service\UserManager;

    class CreateUserCommand extends Command
    {
        private $userManager;

        public function __construct(UserManager $userManager)
        {
            $this->userManager = $userManager;

            parent::__construct();
        }

        // ...

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            // ...

            $this->userManager->create($input->getArgument('username'));

            $output->writeln('User successfully generated!');
        }
    }

命令的生命周期
-----------------

命令有三个生命周期方法，在运行命令时调用：

:method:`Symfony\\Component\\Console\\Command\\Command::initialize` *(可选)*
    此方法在  ``interact()`` 和 ``execute()`` 方法之前执行。
    其主要目的是初始化要在其余命令方法中使用的变量。

:method:`Symfony\\Component\\Console\\Command\\Command::interact` *(可选)*
    此方法在 ``initialize()`` 之后和 ``execute()`` 之前执行。
    其目的是检查是否缺少某些选项/参数，并以交互方式询问用户这些缺失的值。
    这是你可以询问缺少的选项/参数的最后一个地方。
    执行此命令后，如果还缺少选项/参数，将会导致错误。

:method:`Symfony\\Component\\Console\\Command\\Command::execute` *(必须)*
    此方法在 ``interact()`` 和 ``initialize()`` 之后执行。
    它包含你希望命令执行的逻辑。

.. _console-testing-commands:

测试命令
----------------

Symfony提供了几种工具来帮助你测试命令。
最有用的是 :class:`Symfony\\Component\\Console\\Tester\\CommandTester` 类。
它无需真正的控制台，使用的是特殊的输入和输出类来简化测试::

    // tests/Command/CreateUserCommandTest.php
    namespace App\Tests\Command;

    use App\Command\CreateUserCommand;
    use Symfony\Bundle\FrameworkBundle\Console\Application;
    use Symfony\Bundle\FrameworkBundle\Test\KernelTestCase;
    use Symfony\Component\Console\Tester\CommandTester;

    class CreateUserCommandTest extends KernelTestCase
    {
        public function testExecute()
        {
            $kernel = static::createKernel();
            $application = new Application($kernel);

            $command = $application->find('app:create-user');
            $commandTester = new CommandTester($command);
            $commandTester->execute(array(
                'command'  => $command->getName(),

                // 传递参数给该辅助方法
                'username' => 'Wouter',

                // 传递选项时，键前缀为两个破折号，
                // 例如: '--some-option' => 'option_value',
            ));

            // 控制台中命令的输出
            $output = $commandTester->getDisplay();
            $this->assertContains('Username: Wouter', $output);

            // ...
        }
    }

.. tip::

    你还可以使用 :class:`Symfony\\Component\\Console\\Tester\\ApplicationTester` 测试整个控制台应用。

.. note::

    在独立项目中使用Console组件时，
    请使用 :class:`Symfony\\Component\\Console\\Application <Symfony\\Component\\Console\\Application>`
    并继承常规的 ``\PHPUnit\Framework\TestCase``。

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    console/*

控制台组件还包含一组“辅助程序” - 不同的小工具，可以帮助你完成不同的任务：

* :doc:`/components/console/helpers/questionhelper`: 以交互方式询问用户信息
* :doc:`/components/console/helpers/formatterhelper`: 自定义输出着色
* :doc:`/components/console/helpers/progressbar`: 显示进度条
* :doc:`/components/console/helpers/table`: 将表格数据显示为表格
