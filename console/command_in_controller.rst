.. index::
   single: Console; How to Call a Command from a controller

如何从控制器调用命令
=======================================

:doc:`控制台组件文档 </components/console>`
介绍了如何创建一个控制台命令。本文将介绍如何直接在控制器中使用控制台命令。

你可能需要执行仅在控制台命令中可用的某些功能。
通常，你应该重构命令并将一些逻辑移动到可以在控制器中重用的服务中。
但是，当命令是第三方库的一部分时，你并不希望修改或复制其代码。
如此，你可以直接执行该命令。

.. caution::

    与从控制台直接调用相比，由于请求堆栈开销，从控制器调用命令会对性能产生轻微影响。

想象一下，你想通过 :doc:`使用 swiftmailer:spool:send 命令 </email>` 发送假脱机(spooled)的Swift Mailer消息。
通过以下方式从控制器内部运行此命令::

    // src/Controller/SpoolController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Console\Application;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Console\Input\ArrayInput;
    use Symfony\Component\Console\Output\BufferedOutput;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpKernel\KernelInterface;

    class SpoolController extends AbstractController
    {
        public function sendSpool($messages = 10, KernelInterface $kernel)
        {
            $application = new Application($kernel);
            $application->setAutoExit(false);

            $input = new ArrayInput([
                'command' => 'swiftmailer:spool:send',
                // (可选)定义命令参数的值
                'fooArgument' => 'barValue',
                // (可选) 传递选择给命令
                '--message-limit' => $messages,
            ]);

            // 如果不需要任何输出，可以使用 NullOutput()
            $output = new BufferedOutput();
            $application->run($input, $output);

            // 如果没有使用 NullOutput()，返回该输出
            $content = $output->fetch();

            // 如果使用 NullOutput()，则是 return new Response("")
            return new Response($content);
        }
    }

显示彩色命令输出
--------------------------------

通过通过第二个参数告诉 ``BufferedOutput`` 进行装饰，它将返回Ansi颜色编码的内容。
`SensioLabs AnsiToHtml converter`_ 可用于将其转换为丰富多彩的HTML。

首先，安装该软件包：

.. code-block:: terminal

    $ composer require sensiolabs/ansi-to-html

现在，在你的控制器中使用它::

    // src/Controller/SpoolController.php
    namespace App\Controller;

    use SensioLabs\AnsiConverter\AnsiToHtmlConverter;
    use Symfony\Component\Console\Output\BufferedOutput;
    use Symfony\Component\Console\Output\OutputInterface;
    use Symfony\Component\HttpFoundation\Response;
    // ...

    class SpoolController extends AbstractController
    {
        public function sendSpool($messages = 10)
        {
            // ...
            $output = new BufferedOutput(
                OutputInterface::VERBOSITY_NORMAL,
                true // true 意味着开始装饰
            );
            // ...

            // 返回该输出
            $converter = new AnsiToHtmlConverter();
            $content = $output->fetch();

            return new Response($converter->convert($content));
        }
    }

``AnsiToHtmlConverter`` 也可以注册 `为Twig扩展`_，并支持可选的主题。

.. _`SensioLabs AnsiToHtml converter`: https://github.com/sensiolabs/ansi-to-html
.. _`为Twig扩展`: https://github.com/sensiolabs/ansi-to-html#twig-integration
