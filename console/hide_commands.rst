如何隐藏控制台命令
============================

默认情况下，不附带参数执行控制台应用脚本或使用 ``list`` 命令时，会列出所有控制台命令。

但是，有时命令并不是设计给最终用户执行的; 例如，应用的遗留部分的命令、通过计划任务专门执行的命令等。

在这些情况下，你可以通过在命令配置中设置 ``setHidden()`` 方法为 ``true`` 来 **隐藏** 该命令::

    // src/Command/LegacyCommand.php
    namespace App\Command;

    use Symfony\Component\Console\Command\Command;

    class LegacyCommand extends Command
    {
        protected function configure()
        {
            $this
                ->setName('app:legacy')
                ->setHidden(true)
                // ...
            ;
        }
    }

隐藏的命令与普通命令的行为相同，但它们不再显示在命令列表中，因此最终用户不知道它们的存在。

.. note::

    使用JSON或XML描述符仍然可以获取到隐藏命令。
