.. index::
   single: kernel, performance

如何使用多个内核创建Symfony应用
========================================================

.. caution::

    Symfony不再推荐使用多个内核来创建应用。如果有需求，可以考虑创建多个小应用。

在大多数Symfony应用中，传入请求由 ``public/index.php``
前端控制器处理，该控制器实例化 ``src/Kernel.php``
类以创建加载bundle的应用内核，并处理请求以生成响应。

这种单内核方法是一种方便的默认方法，但Symfony应用可以定义任意数量的内核。
:doc:`环境 </configuration/environments>`
使用不同的配置来执行同一个应用，而内核可以执行同一应用的不同部分。

以下是创建多个内核的一些常见用例：
These are some of the common use cases for creating multiple kernels:

* 定义API的应用可以出于性能原因定义两个内核。
  第一个内核将为常规应用提供服务，第二个内核仅响应API请求，加载较少的bundle并启用较少的功能;
* 高度敏感的应用可以定义两个内核。第一个只会加载与应用公开的部分相匹配的路由。
  第二个内核将加载应用的其余部分，其访问权限将受Web服务器的保护;
* 一个面向微服务的应用可以定义几个内核来启用/禁用服务，有选择地将传统的整体应用转换为几个微应用。

向应用添加新内核
--------------------------------------

在Symfony应用中创建新内核的过程分为三个步骤：

1. 创建一个新的前端控制器来加载新内核;
2. 创建新的内核类;
3. 定义新内核将要加载的配置。

以下示例展示如何为给定Symfony应用的API创建一个新内核。

步骤 1) 创建一个新的前端控制器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

不需要从头开始创建新的前端控制器，直接复制现有的前端控制器将更容易。
例如，依照 ``public/index.php`` 来创建 ``public/api.php``。

然后，更新新前端控制器的代码以实例化新的内核类而不是常用的 ``Kernel`` 类::

    // public/api.php
    // ...
    $kernel = new ApiKernel(
        $_SERVER['APP_ENV'] ?? 'dev',
        $_SERVER['APP_DEBUG'] ?? ('prod' !== ($_SERVER['APP_ENV'] ?? 'dev'))
    );
    // ...

.. tip::

    另一种方法是保留现有的 ``index.php`` 前端控制器，但添加一个 ``if``
    语句以根据URL来加载不同的内核（例如，如果URL以 ``/api`` 开头，则使用 ``ApiKernel``）。

步骤 2) 创建新的内核类
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

现在你需要定义新前端控制器使用的 ``ApiKernel`` 类。
最简单的方法是复制现有  ``src/Kernel.php`` 文件并进行必要的更改。

在此示例中，``ApiKernel`` 将加载比默认内核更少的bundle。
请务必同时更改缓存、日志和配置文件的位置，以免它们与 ``src/Kernel.php`` 下的文件发生冲突::

    // src/ApiKernel.php
    use Symfony\Component\Config\Loader\LoaderInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\Kernel;

    class ApiKernel extends Kernel
    {
        // ...

        public function registerBundles()
        {
            // 仅加载API需要的bundle...
        }

        public function getCacheDir()
        {
            return dirname(__DIR__).'/var/cache/api/'.$this->getEnvironment();
        }

        public function getLogDir()
        {
            return dirname(__DIR__).'/var/log/api';
        }

        public function configureContainer(ContainerBuilder $container, LoaderInterface $loader)
        {
            // 仅加载API需要的配置文件
            $confDir = $this->getProjectDir().'/config';
            $loader->load($confDir.'/api/*'.self::CONFIG_EXTS, 'glob');
            if (is_dir($confDir.'/api/'.$this->environment)) {
                $loader->load($confDir.'/api/'.$this->environment.'/**/*'.self::CONFIG_EXTS, 'glob');
            }
        }
    }

步骤 3) 定义内核配置
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

最后，定义新 ``ApiKernel`` 要加载的配置文件。
根据上面的代码，这个配置将存在于一个或多个存储在 ``config/api/`` 和
``config/api/ENVIRONMENT_NAME/`` 目录中的文件中。

当你仅加载少数几个bundle时，可以从头开始创建新配置文件，因为它会很小。
否则，更好的做法是，复制现有 ``config/packages/`` 中的配置文件，然后导入它们并重写所需的选项。

用不同的内核执行命令
------------------------------------------

用于运行Symfony命令的 ``bin/console`` 脚本始终使用默认 ``Kernel`` 类来构建应用并加载命令。
如果需要使用新内核执行控制台命令，请复制 ``bin/console`` 脚本并重命名（例如 ``bin/api``）。

然后，通过你自己的内核实例化（例如 ``ApiKernel``）来替换 ``Kernel``
实例化，现在你可以使用新内核执行命令（例如 ``php bin/api cache:clear``）。

.. note::

    每个控制台脚本（例如 ``bin/console`` 和
    ``bin/api``）的可用命令可能不同，因为它们依赖于每个内核中被启用的bundle，而这些bundle可能并不一样。

在不同内核中定义的渲染模板
-------------------------------------------------

如果你遵循Symfony最佳实践，默认内核的模板将存储在 ``templates/``。
尝试在不同的内核中渲染这些模板将导致 *There are no registered paths for namespace
"__main__"* 错误。

要解决此问题，请将以下配置添加到你的内核：

.. code-block:: yaml

    # config/api/twig.yaml
    twig:
        paths:
            # 允许在 ApiKernel 中使用 api/templates/ 目录
            "%kernel.project_dir%/api/templates": ~

使用不同的内核来运行测试
--------------------------------------

在Symfony应用中，功能测试默认从
:class:`Symfony\\Bundle\\FrameworkBundle\\Test\\WebTestCase` 类中继承。
在该类中，一个名为 ``getKernelClass()`` 的方法尝试查找在测试期间用于运行应用的内核类。
此方法的逻辑不支持多个内核应用，因此你的测试将无法使用正确的内核。

解决方案是为功能测试创建一个继承 ``WebTestCase`` 类的自定义基类，该类重写 ``getKernelClass()``
方法以返回要使用的内核的完全限定类名::

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    // 需要 ApiKernel 才能运行的测试，
    // 现在必须继承这个 ApiTestCase 类而不是默认的 WebTestCase 类
    class ApiTestCase extends WebTestCase
    {
        protected static function getKernelClass()
        {
            return 'App\ApiKernel';
        }

        // 这是必需的，因为 KernelTestCase 类在其静态 $kernel 属性中保留对先前创建的内核的引用。
        // 因此，如果你的功能测试不在隔离的进程中运行，
        // 则对不同内核的后续运行测试将重用先前创建的实例，该实例指向一个不同的内核。
        protected function tearDown()
        {
            parent::tearDown();

            static::$class = null;
        }
    }

向应用添加更多内核
--------------------------------------

如果你的应用非常复杂并且你创建了多个内核，最好将它们存储在各自的目录中，而不是在默认
``src/`` 目录中，从而产生一大堆的混乱文件：

.. code-block:: text

    project/
    ├─ src/
    │  ├─ ...
    │  └─ Kernel.php
    ├─ api/
    │  ├─ ...
    │  └─ ApiKernel.php
    ├─ ...
    └─ public/
        ├─ ...
        ├─ api.php
        └─ index.php
