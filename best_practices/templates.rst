模板
=========

当PHP于20年前被创建的时候，开发者喜欢它的简洁以及可以混同HTML生成动态内容。时过境迁，其他模板语言 - 比如 `Twig`_ - 正令模板创建过程变得更好。

.. best-practice::

    使用Twig格式的模板作为你的模板引擎。

一般来说，PHP模板比Twig模板更冗长，因为它们缺少对模板所需的大量现代功能的原生支持，如继承，自动转义和过滤器和函数的命名参数。

Twig是Symfony中的默认模板格式，并且拥有所有非PHP模板引擎的最大社区支持（它被用于高级项目，如Drupal 8）。

模板位置
------------------

.. best-practice::

    将应用模板存储在项目根目录的 ``templates/`` 目录中。

将模板集中在一个位置可以简化设计人员的工作。
此外，使用此目录简化了引用模板时使用的符号（例如 ``$this->render('admin/post/show.html.twig')``，而不是 ``$this->render('@SomeTwigNamespace/Admin/Posts/show.html.twig')``）。

.. best-practice::

    对目录和模板名称，使用蛇型拼写法的小写格式。

此建议与Twig最佳实践一致，其中变量和模板名称也使用小写的蛇形命名
（例如使用 ``user_profile`` 替代 ``userProfile``，使用 ``edit_form.html.twig`` 替代 ``EditForm.html.twig``）。

.. best-practice::

    对模板名称中的部分模板使用带前缀的下划线。

您经常希望使用 ``include`` 函数重用模板代码以避免冗余代码。
要在文件系统中轻松确定这些部分，您应该为部分和任何其他模板添加前缀，而不使用HTML正文或​​使用单个下划线的 ``extends`` 标记。

Twig 扩展
---------------

.. best-practice::

    在 ``src/Twig/`` 目录中定义Twig扩展。你的应用将自动检测并配置它们。

我们的应用需要自定义一个 ``md2html`` Twig过滤器，以便我们可以将每个帖子的Markdown内容转换为HTML。
为此，请创建一个新的 ``Markdown`` 类，稍后将由Twig扩展使用。
它需要定义一个方法来将Markdown内容转换为HTML::

    namespace App\Utils;

    class Markdown
    {
        // ...

        public function toHtml(string $text): string
        {
            return $this->parser->text($text);
        }
    }

接下来，创建一个新的Twig扩展，并使用 ``Twig\TwigFilter`` 类定义一个名为 ``md2html`` 的过滤器。
在Twig扩展的构造函数中注入新定义的 ``Markdown`` 类::

    namespace App\Twig;

    use App\Utils\Markdown;
    use Twig\Extension\AbstractExtension;
    use Twig\TwigFilter;

    class AppExtension extends AbstractExtension
    {
        private $parser;

        public function __construct(Markdown $parser)
        {
            $this->parser = $parser;
        }

        public function getFilters()
        {
            return [
                new TwigFilter('md2html', [$this, 'markdownToHtml'], [
                    'is_safe' => ['html'],
                    'pre_escape' => 'html',
                ]),
            ];
        }

        public function markdownToHtml($content)
        {
            return $this->parser->toHtml($content);
        }
    }

就是这些!

如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么这就完工了！Symfony将自动探知你的新服务并将其标记为一个Twig扩展。

----

下一章: :doc:`/best_practices/forms`

.. _`Twig`: https://twig.symfony.com/
.. _`Parsedown`: http://parsedown.org/
