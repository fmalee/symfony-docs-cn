.. index::
   single: DependencyInjection; Compiler passes
   single: Service Container; Compiler passes

如何使用编译器传递
================================

编译器传递(Compiler Passes)使你有机会操作已向服务容器注册的其他
:doc:`服务定义 </service_container/definitions>`。
你可以在 ":ref:`components-di-separate-compiler-passes`" 组件章节中阅读有关如何创建它们的信息。

编译器传递在应用内核的 ``build()`` 方法中注册::

    // src/Kernel.php
    namespace App;

    use App\DependencyInjection\Compiler\CustomPass;
    use Symfony\Bundle\FrameworkBundle\Kernel\MicroKernelTrait;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;

    class Kernel extends BaseKernel
    {
        use MicroKernelTrait;

        // ...

        protected function build(ContainerBuilder $container): void
        {
            $container->addCompilerPass(new CustomPass());
        }
    }

编译器传递的最常见用例之一是用来 :doc:`标记服务 </service_container/tags>`。
在这些情况下，你可以使内核实现
:class:`Symfony\\Component\\DependencyInjection\\Compiler\\CompilerPassInterface`
并处理 ``process()`` 方法内的服务，而不是创建一个编译器传递::

    // src/Kernel.php
    namespace App;

    use Symfony\Bundle\FrameworkBundle\Kernel\MicroKernelTrait;
    use Symfony\Component\DependencyInjection\Compiler\CompilerPassInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;

    class Kernel extends BaseKernel implements CompilerPassInterface
    {
        use MicroKernelTrait;

        // ...

        public function process(ContainerBuilder $container)
        {
            // 在此方法中，你可以操作服务容器
            // 例如，更改某些容器服务：
            $container->getDefinition('app.some_private_service')->setPublic(true);

            // 或处理已标记的服务:
            foreach ($container->findTaggedServiceIds('some_tag') as $id => $tags) {
                // ...
            }
        }
    }

在Bundles中使用编译器传递
---------------------------------------

:doc:`Bundles </bundles>` 可以在主bundle类的 ``build()``
方法中定义编译器传递（在扩展中实现 ``process()`` 方法时不需要这样做）::

    // src/MyBundle/MyBundle.php
    namespace App\MyBundle;

    use App\DependencyInjection\Compiler\CustomPass;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\Bundle\Bundle;

    class MyBundle extends Bundle
    {
        public function build(ContainerBuilder $container)
        {
            parent::build($container);

            $container->addCompilerPass(new CustomPass());
        }
    }

如果你按照惯例使用Bundle中的自定义 :doc:`服务标签 </service_container/tags>`，
则标签名称由Bundle的名称（小写，以下划线作为分隔符），后跟一个点，最后是“真实”名称组成。
例如，如果要在AcmeMailerBundle中引入某种“transport”标签，则应用 ``acme_mailer.transport`` 调用它。
