.. index::
   single: Twig extensions

如何编写自定义Twig扩展
====================================

如果你需要创建自定义Twig函数、过滤器、测试等，则需要创建Twig扩展。
你可以在Twig文档中阅读有关 `Twig扩展`_ 的更多信息。

.. tip::

    在编写自己的Twig扩展之前，请检查你所需的过滤器/功能是否已在Symfony
    :doc:`Twig扩展 </reference/twig_reference>` 中实现。
    还可以查看 `官方的Twig扩展`_，它同样可以安装到你的应用中，如下所示：

    .. code-block:: terminal

        $ composer require twig/extensions

创建扩展类
--------------------------

假设你要创建一个名为 ``price`` 的用于格式化数字的新过滤器：

.. code-block:: twig

    {{ product.price|price }}

    {# 传递3个可选参数 #}
    {{ product.price|price(2, ',', '.') }}

创建一个继承 ``AbstractExtension`` 的类并填充逻辑::

    // src/Twig/AppExtension.php
    namespace App\Twig;

    use Twig\Extension\AbstractExtension;
    use Twig\TwigFilter;

    class AppExtension extends AbstractExtension
    {
        public function getFilters()
        {
            return [
                new TwigFilter('price', [$this, 'formatPrice']),
            ];
        }

        public function formatPrice($number, $decimals = 0, $decPoint = '.', $thousandsSep = ',')
        {
            $price = number_format($number, $decimals, $decPoint, $thousandsSep);
            $price = '$'.$price;

            return $price;
        }
    }

如果要创建函数而不是过滤器，请定义 ``getFunctions()`` 方法::

    // src/Twig/AppExtension.php
    namespace App\Twig;

    use Twig\Extension\AbstractExtension;
    use Twig\TwigFunction;

    class AppExtension extends AbstractExtension
    {
        public function getFunctions()
        {
            return [
                new TwigFunction('area', [$this, 'calculateArea']),
            ];
        }

        public function calculateArea(int $width, int $length)
        {
            return $width * $length;
        }
    }

.. tip::

    除了自定义过滤器和函数，你还可以注册 `全局变量`_。

将扩展注册为服务
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

接下来，将你的类注册为服务并使用 ``twig.extension`` 标签。
如果你使用 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么你已经完工了！Symfony会自动了解你的新服务并添加相应的标签。

（可选操作）执行此命令以确认你的新过滤器是否成功注册：

.. code-block:: terminal

    $ php bin/console debug:twig --filter=price

你现在可以在任何Twig模板中使用这个过滤器。

.. tip::

    运行以下命令以验证你的扩展所创建的过滤器和函数是否已成功注册：

    .. code-block:: terminal

        $ php bin/console debug:twig

.. _lazy-loaded-twig-extensions:

创建延迟加载的Twig扩展
------------------------------------

.. versionadded:: 1.26

    Twig 1.26中引入了对延迟加载扩展的支持。

在Twig扩展类中包含自定义过滤器/函数的代码是创建扩展的最简单方法。
但是，Twig必须在渲染任何模板之前初始化所有扩展，即使该模板不使用该扩展。

如果该扩展没有定义依赖关系（即，如果你不在其中注入服务），性能不会受到影响。
但是，如果扩展定义了许多复杂的依赖关系（例如建立数据库连接），那么性能损失可能很大。

这就是为什么Twig允许将扩展定义与它的实现分离的原因。
按照与之前相同的示例，第一个更改是从扩展中删除 ``formatPrice()`` 方法并更新在
``getFilters()`` 中定义的可调用(callable)PHP代码::

    // src/Twig/AppExtension.php
    namespace App\Twig;

    use App\Twig\AppRuntime;
    use Twig\Extension\AbstractExtension;
    use Twig\TwigFilter;

    class AppExtension extends AbstractExtension
    {
        public function getFilters()
        {
            return [
                // 此过滤器的逻辑现在在不同的类中实现
                new TwigFilter('price', [AppRuntime::class, 'formatPrice']),
            ];
        }
    }

然后，创建新的 ``AppRuntime`` 类（``Runtime`` 不是必需的，但这些类按惯例使用该后缀）
并包含之前的 ``formatPrice()`` 方法的逻辑::

    // src/Twig/AppRuntime.php
    namespace App\Twig;

    use Twig\Extension\RuntimeExtensionInterface;

    class AppRuntime implements RuntimeExtensionInterface
    {
        public function __construct()
        {
            // 这个简单的例子没有定义任何依赖，
            // 但在你自己的扩展中，你需要使用这个构造函数注入服务
        }

        public function formatPrice($number, $decimals = 0, $decPoint = '.', $thousandsSep = ',')
        {
            $price = number_format($number, $decimals, $decPoint, $thousandsSep);
            $price = '$'.$price;

            return $price;
        }
    }

如果你使用默认 ``services.yaml`` 配置，这就已经有效！
否则，请为这个类 :ref:`创建一个服务 <service-container-creating-service>`，
并使用 ``twig.runtime`` :doc:`标记该服务 </service_container/tags>`。

.. _`官方的Twig扩展`: https://github.com/twigphp/Twig-extensions
.. _`全局变量`: https://twig.symfony.com/doc/2.x/advanced.html#id1
.. _`functions`: https://twig.symfony.com/doc/2.x/advanced.html#id2
.. _`Twig扩展`: https://twig.symfony.com/doc/2.x/advanced.html#creating-an-extension
