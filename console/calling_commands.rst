如何调用其他命令
==========================

如果命令依赖于在其之前运行的另一个命令，你不能要求用户记住执行顺序，而是自己直接调用它。
如果你想创建一个只运行一堆其他命令的“meta”命令
（例如，当生产服务器上的项目代码发生变化时需要运行的所有命令：清除缓存、生成Doctrine2代理、输出web资产...），
调用其他命令将会派上用场。

在命令中调用另一个命令很简单::

    use Symfony\Component\Console\Input\ArrayInput;
    // ...

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        $command = $this->getApplication()->find('demo:greet');

        $arguments = array(
            'command' => 'demo:greet',
            'name'    => 'Fabien',
            '--yell'  => true,
        );

        $greetInput = new ArrayInput($arguments);
        $returnCode = $command->run($greetInput, $output);

        // ...
    }

首先，通过传递命令名称来 :method:`Symfony\\Component\\Console\\Application::find` 要执行的命令。
然后，你需要使用要传递给该命令的参数和选项来创建一个新的 :class:`Symfony\\Component\\Console\\Input\\ArrayInput`。

最终，调用 ``run()`` 方法实际上执行该命令并从返回该命令返回的代码（从命令的 ``execute()`` 方法返回值 ）。

.. tip::

    如果要抑制已执行命令的输出，请将 :class:`Symfony\\Component\\Console\\Output\\NullOutput`
    作为第二个参数传递给 ``$command->run()``。

.. caution::

    请注意，所有命令都将在同一进程中运行，而Symfony的某些内置命令可能无法以这种方式运行。
    例如，``cache:clear`` 和 ``cache:warmup`` 命令会更改某些类定义，因此在它们之后运行的命令可能会出错。

.. note::

    大多数情况下，在代码中而不是在命令行中调用命令不是一个好主意。
    主要原因是命令的输出针对控制台进行了优化，而不是传递给其他命令。
