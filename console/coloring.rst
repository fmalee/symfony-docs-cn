如何为控制台输出设置颜色和样式
=========================================

通过在命令输出中使用颜色，你可以区分不同类型的输出（例如重要消息、标题、注释等）。

.. note::

    默认情况下，Windows命令控制台不支持输出着色。
    控制台组件禁用Windows系统的输出着色，但如果你的命令调用其他发出(emit)颜色序列的脚本，则它们将错误地显示为原始转义字符。
    安装 `Cmder`_、`ConEmu`_、`ANSICON`_、`Mintty`_ （默认情况下在GitBash和Cygwin中使用）或 `Hyper`_ 免费应用，
    为Windows命令控制台添加着色支持。

使用颜色样式
------------------

无论何时输出文本，你都可以使用标签包裹文本以为其输出着色。例如::

    // 绿色文本
    $output->writeln('<info>foo</info>');

    // 黄色文本
    $output->writeln('<comment>foo</comment>');

    // 青色背景的黑色文本
    $output->writeln('<question>foo</question>');

    // 红色背景的白色文本
    $output->writeln('<error>foo</error>');

结束标签可以替换为 ``</>``，它将撤消最后打开的标签建立的所有格式选项。

可以使用 :class:`Symfony\\Component\\Console\\Formatter\\OutputFormatterStyle` 类定义自己的样式::

    use Symfony\Component\Console\Formatter\OutputFormatterStyle;

    // ...
    $outputStyle = new OutputFormatterStyle('red', 'yellow', ['bold', 'blink']);
    $output->getFormatter()->setStyle('fire', $outputStyle);

    $output->writeln('<fire>foo</>');

可用前景色和背景色有：``black``、``red``、``green``、
``yellow``、``blue``、``magenta``、``cyan`` 以及 ``white``。

可用的选项为：``bold``、``underscore``、``blink``、``reverse``
（启用背景和前景颜色交换的“反向视频”模式）以及 ``conceal``
（设定前景颜色为透明，使键入的文本不可见-尽管它可被选择和复制;该选项通常在要求用户键入敏感信息时使用）。

你还可以直接在标签名称中设置这些颜色和选项::

    // 绿色文本
    $output->writeln('<fg=green>foo</>');

    // 青色背景的黑色文本
    $output->writeln('<fg=black;bg=cyan>foo</>');

    // 黄色背景的加粗文本
    $output->writeln('<bg=yellow;options=bold>foo</>');

    // 带下划线的加粗文本
    $output->writeln('<options=bold,underscore>foo</>');

.. note::

    如果需要直接渲染一个标签，请使用反斜杠转义它： ``\<info>``
    或使用 :method:`Symfony\\Component\\Console\\Formatter\\OutputFormatter::escape`
    方法转义给定字符串中包含的所有标签。

显示可点击链接
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. versionadded:: 4.3

    Symfony 4.3中引入了显示可点击链接的特性。

命令可以使用特殊的 ``<href>`` 标签来显示类似于网页的 ``<a>`` 元素的链接::

    $output->writeln('<href=https://symfony.com>Symfony Homepage</>');

如果你的终端属于 `支持链接的终端仿真器列表`_，你可以单击 *"Symfony Homepage"*
文本在默认浏览器中打开其URL。否则，你将看到 *"Symfony Homepage"* 作为常规文本，URL将丢失。

.. _Cmder: http://cmder.net/
.. _ConEmu: https://conemu.github.io/
.. _ANSICON: https://github.com/adoxa/ansicon/releases
.. _Mintty: https://mintty.github.io/
.. _Hyper: https://hyper.is/
.. _`支持链接的终端仿真器列表`: https://gist.github.com/egmontkob/eb114294efbcd5adb1944c9f3cb5feda
