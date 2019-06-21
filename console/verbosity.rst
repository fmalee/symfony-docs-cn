冗余度级别
================

控制台命令具有不同的冗余度(verbosity)级别，用于确定其输出中显示的消息。
默认情况下，命令仅显示最有用的消息，但你可以使用 ``-q`` 和 ``-v`` 选项控制其详细程度：

.. code-block:: terminal

    # 不输出任何消息 (甚至没有该命令的最终消息)
    $ php bin/console some-command -q
    $ php bin/console some-command --quiet

    # 正常行为, 没有引入选项 (只显示有用的消息)
    $ php bin/console some-command

    # 增加消息的冗余程度
    $ php bin/console some-command -v

    # 显示非必要的消息
    $ php bin/console some-command -vv

    # 显示所有消息 (用于调试错误)
    $ php bin/console some-command -vvv

可以使用 ``SHELL_VERBOSITY`` 环境变量来全局控制所有命令的冗余度级别
（ ``-q`` 和 ``-v`` 选项仍然优先于 ``SHELL_VERBOSITY`` 值）：

=====================  =========================  ===========================================
控制台选项                ``SHELL_VERBOSITY`` 值     等价的PHP常量
=====================  =========================  ===========================================
``-q`` or ``--quiet``  ``-1``                     ``OutputInterface::VERBOSITY_QUIET``
(无)                    ``0``                      ``OutputInterface::VERBOSITY_NORMAL``
``-v``                 ``1``                      ``OutputInterface::VERBOSITY_VERBOSE``
``-vv``                ``2``                      ``OutputInterface::VERBOSITY_VERY_VERBOSE``
``-vvv``               ``3``                      ``OutputInterface::VERBOSITY_DEBUG``
=====================  =========================  ===========================================

可以在命令中仅为特定的冗余度级别打印消息。例如::

    // ...
    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    class CreateUserCommand extends Command
    {
        // ...

        public function execute(InputInterface $input, OutputInterface $output)
        {
            $user = new User(...);

            $output->writeln([
                'Username: '.$input->getArgument('username'),
                'Password: '.$input->getArgument('password'),
            ]);

            // 可用方法: ->isQuiet(), ->isVerbose(), ->isVeryVerbose(), ->isDebug()
            if ($output->isVerbose()) {
                $output->writeln('User class: '.get_class($user));
            }

            // alternatively you can pass the verbosity level PHP constant to writeln()
            $output->writeln(
                'Will only be printed in verbose mode or higher',
                OutputInterface::VERBOSITY_VERBOSE
            );
        }
    }

使用静默级别时，所有输出都会被抑制，因为默认的
:method:`Symfony\\Component\\Console\\Output\\Output::write` 方法返回时没有实际打印。

.. tip::

    MonologBridge提供了一个 :class:`Symfony\\Bridge\\Monolog\\Handler\\ConsoleHandler` 类，
    允许你在控制台上显示消息。这比在条件中封装输出调用更简洁。
    有关在Symfony Framework中使用的示例，请参阅 :doc:`/logging/monolog_console`。

.. tip::

    如果使用 ``VERBOSITY_VERBOSE`` 级别或更高级别，则打印完整的异常堆栈跟踪。
