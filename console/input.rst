控制台输入（参数和选项）
===================================

命令中最有趣的部分是你可以使用的参数和选项。这些参数和选项允许你将动态信息从终端传递到该命令。

使用命令参数
-----------------------

参数是字符串 - 由空格分隔 - 位于命令名称本身之后。它们是有序的，可以是可选的或必需的。
例如，要为命令添加一个 ``last_name`` 可选参数并使 ``name`` 参数成为必需参数::

    // ...
    use Symfony\Component\Console\Input\InputArgument;

    class GreetCommand extends Command
    {
        // ...

        protected function configure()
        {
            $this
                // ...
                ->addArgument('name', InputArgument::REQUIRED, 'Who do you want to greet?')
                ->addArgument('last_name', InputArgument::OPTIONAL, 'Your last name?')
            ;
        }
    }

你现在可以访问命令中的 ``last_name`` 参数::

    // ...
    class GreetCommand extends Command
    {
        // ...

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $text = 'Hi '.$input->getArgument('name');

            $lastName = $input->getArgument('last_name');
            if ($lastName) {
                $text .= ' '.$lastName;
            }

            $output->writeln($text.'!');
        }
    }

该命令现在可以通过以下任一方式使用：

.. code-block:: terminal

    $ php bin/console app:greet Fabien
    Hi Fabien!

    $ php bin/console app:greet Fabien Potencier
    Hi Fabien Potencier!

也可以让参数采用一个值列表（想象你想要问候所有的朋友）。只有 *最后* 一个参数可以是一个列表::

    $this
        // ...
        ->addArgument(
            'names',
            InputArgument::IS_ARRAY,
            'Who do you want to greet (separate multiple names with a space)?'
        );

要使用它，请根据需要指定任意数量的名称：

.. code-block:: terminal

    $ php bin/console app:greet Fabien Ryan Bernhard

你可以将 ``names`` 参数作为一个数组访问::

    $names = $input->getArgument('names');
    if (count($names) > 0) {
        $text .= ' '.implode(', ', $names);
    }

你可以使用三种参数变体：

``InputArgument::REQUIRED``
    表示这个参数是强制性的。如果未提供该参数，则命令不会运行;

``InputArgument::OPTIONAL``
    表示该参数是可选的，因此可以省略。这是参数的默认行为;

``InputArgument::IS_ARRAY``
    表示该参数可以包含任意数量的值。因此，必须在参数列表的末尾使用它。

你可以像这样结合 ``IS_ARRAY`` 来使用 ``REQUIRED`` 和 ``OPTIONAL``::

    $this
        // ...
        ->addArgument(
            'names',
            InputArgument::IS_ARRAY | InputArgument::REQUIRED,
            'Who do you want to greet (separate multiple names with a space)?'
        );

使用命令选项
---------------------

与参数不同，选项不是有序的（意味着你可以按任何顺序指定它们），并且使用两个破折号（例如 ``--yell``）来指定。
选项 *总是* 可选的，可以设置为接受一个值（例如 ``--dir=src``）或简单地作为没有值的布尔表示（例如 ``--yell``）。

例如，在命令中添加一个新选项，可用于指定消息应打印的行数::

    // ...
    use Symfony\Component\Console\Input\InputOption;

    $this
        // ...
        ->addOption(
            'iterations',
            null,
            InputOption::VALUE_REQUIRED,
            'How many times should the message be printed?',
            1
        );

接下来，在命令中使用此选项多次打印消息::

    for ($i = 0; $i < $input->getOption('iterations'); $i++) {
        $output->writeln($text);
    }

现在，当你运行该命令时，你可以选择指定一个 ``--iterations`` 标识：

.. code-block:: terminal

    # 不提供 --iterations，则使用默认值(1)
    $ php bin/console app:greet Fabien
    Hi Fabien!

    $ php bin/console app:greet Fabien --iterations=5
    Hi Fabien
    Hi Fabien
    Hi Fabien
    Hi Fabien
    Hi Fabien

    # 选项的排序并不重要
    $ php bin/console app:greet Fabien --iterations=5 --yell
    $ php bin/console app:greet Fabien --yell --iterations=5
    $ php bin/console app:greet --yell --iterations=5 Fabien

.. tip::

     你还可以使用单个短划线调用来声明一个单字母快捷方式，例如 ``-i``::

        $this
            // ...
            ->addOption(
                'iterations',
                'i',
                InputOption::VALUE_REQUIRED,
                'How many times should the message be printed?',
                1
            );

请注意，为了符合 `docopt标准`_，长选项可以在空格或 ``=`` 符号（例如 ``--iterations 5`` 或
``--iterations=5``）之后指定它们的值，但短选项只能使用空格或根本不使用分隔符（例如 ``-i 5`` 或 ``-i5``）。

.. caution::

    虽然可以使用空格将选项与其值分开，但如果选项出现在命令名称之前，则使用此表单会导致歧义。
    例如 ``php bin/console --iterations 5 app:greet Fabien``
    就含糊不清：Symfony会将 ``5`` 解释为命令名称。
    要避免这种情况，请始终将选项放置到命令名称后面，或者避免使用空格将选项名称与其值分开。

你可以使用以下四种选项：

``InputOption::VALUE_IS_ARRAY``
    此选项接受多个值（例如 ``--dir=/foo --dir=/bar``）；

``InputOption::VALUE_NONE``
    此选项不接受输入（例如 ``--yell``）。这是选项的默认行为；

``InputOption::VALUE_REQUIRED``
    该选项的值是必需的（例如 ``--iterations=5`` or ``-i5``），但选项本身仍然是可选的；

``InputOption::VALUE_OPTIONAL``
    此选项的值是可选的（例如 ``--yell`` 或 ``--yell=loud``）。

你可以像这样结合 ``VALUE_IS_ARRAY`` 来使用 ``VALUE_REQUIRED`` 或 ``VALUE_OPTIONAL``::

    $this
        // ...
        ->addOption(
            'colors',
            null,
            InputOption::VALUE_REQUIRED | InputOption::VALUE_IS_ARRAY,
            'Which colors do you like?',
            array('blue', 'red')
        );

带可选参数的选项
-------------------------------

没有什么可以禁止你创建一个带有可选的并接受值的选项的命令，但这有点棘手。考虑这个例子::

    // ...
    use Symfony\Component\Console\Input\InputOption;

    $this
        // ...
        ->addOption(
            'yell',
            null,
            InputOption::VALUE_OPTIONAL,
            'Should I yell while greeting?'
        );

此选项可以通过三种方式使用：``--yell``，``yell=louder``，以及不传递该选项。
但是，很难在传递了选项但未传递值（``greet --yell``）和根本没有传递选项（``greet``）之间进行区分。

要解决此问题，你必须将该选项的默认值设置为 ``false``::

    // ...
    use Symfony\Component\Console\Input\InputOption;

    $this
        // ...
        ->addOption(
            'yell',
            null,
            InputOption::VALUE_OPTIONAL,
            'Should I yell while greeting?',
            false // 这是新的默认值，而不是null
        );

现在检查该选项的值，并记住 ``false !== null``::

    $optionValue = $input->getOption('yell');
    $yell = ($optionValue !== false);
    $yellLouder = ($optionValue === 'louder');

.. _`docopt标准`: http://docopt.org/
