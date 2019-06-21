防止多次执行控制台命令
================================================

防止在单个服务器中多次执行同一命令的简单但有效的方法是使用 `锁`_。
:doc:`Lock组件 </components/lock>` 提供多个类来创建基于
文件系统（:ref:`FlockStore <lock-store-flock>`）、共享内存（:ref:`SemaphoreStore <lock-store-semaphore>`）、甚至数据库和Redis的服务器的锁。

此外，Console组件提供了一个名为 ``LockableTrait`` 的PHP trait，它添加了两个方便的方法来锁定和释放命令::

    // ...
    use Symfony\Component\Console\Command\Command;
    use Symfony\Component\Console\Command\LockableTrait;
    use Symfony\Component\Console\Input\InputInterface;
    use Symfony\Component\Console\Output\OutputInterface;

    class UpdateContentsCommand extends Command
    {
        use LockableTrait;

        // ...

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            if (!$this->lock()) {
                $output->writeln('The command is already running in another process.');

                return 0;
            }

            // 如果你希望等到该锁被释放，请使用：
            // $this->lock(null, true);

            // ...

            // 如果没有显式释放，Symfony会在命令执行结束时自动释放锁
            $this->release();
        }
    }

.. _`锁`: https://en.wikipedia.org/wiki/Lock_(computer_science)
