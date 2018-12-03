使用MicroKernelTrait构建自己的框架
=====================================================

Symfony应用中包含的默认``Kernel`` 类，该类使用一个
:class:`Symfony\\Bundle\\FrameworkBundle\\Kernel\\MicroKernelTrait`
来在同一类中配置bundle、路由和服务容器。

这种微内核方法非常灵活，允许你控制应用的结构和功能。

单文件Symfony应用
---------------------------------

从一个完全空目录开始，并通过Composer安装这些Symfony组件：

.. code-block:: bash

    $ composer require symfony/config symfony/http-kernel \
      symfony/http-foundation symfony/routing \
      symfony/dependency-injection symfony/framework-bundle

接下来，创建一个定义内核类并执行它的 ``index.php`` 文件::

    use Symfony\Bundle\FrameworkBundle\Kernel\MicroKernelTrait;
    use Symfony\Component\Config\Loader\LoaderInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpFoundation\JsonResponse;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;
    use Symfony\Component\Routing\RouteCollectionBuilder;

    require __DIR__.'/vendor/autoload.php';

    class Kernel extends BaseKernel
    {
        use MicroKernelTrait;

        public function registerBundles()
        {
            return array(
                new Symfony\Bundle\FrameworkBundle\FrameworkBundle()
            );
        }

        protected function configureContainer(ContainerBuilder $c, LoaderInterface $loader)
        {
            // 相当于 config/packages/framework.yaml
            $c->loadFromExtension('framework', array(
                'secret' => 'S0ME_SECRET'
            ));
        }

        protected function configureRoutes(RouteCollectionBuilder $routes)
        {
            // kernel是指向此类的一个服务
            // 可选的第三个参数是路由名称
            $routes->add('/random/{limit}', 'Kernel::randomNumber');
        }

        public function randomNumber($limit)
        {
            return new JsonResponse(array(
                'number' => random_int(0, $limit),
            ));
        }
    }

    $kernel = new Kernel('dev', true);
    $request = Request::createFromGlobals();
    $response = $kernel->handle($request);
    $response->send();
    $kernel->terminate($request, $response);

仅此而已！要测试它，你可以启动内置Web服务器：

.. code-block:: terminal

    $ php -S localhost:8000

然后在浏览器中查看该JSON响应：

    http://localhost:8000/random/10

“微”内核的方法
-------------------------------

当你使用 ``MicroKernelTrait`` 时，你的内核需要有三个方法来定义你的bundle、服务和路由：

**registerBundles()**
    这与你在普通内核中看到的 ``registerBundles()`` 相同。

**configureContainer(ContainerBuilder $c, LoaderInterface $loader)**
    此方法用于构建和配置容器。在实践中，你将使用 ``loadFromExtension``
    来配置不同的bundle（这相当于你在普通 ``config/packages/*`` 文件中看到的）。
    你还可以直接在PHP中注册服务或加载外部配置文件（如下所示）。

**configureRoutes(RouteCollectionBuilder $routes)**
    你在此方法中的工作是向应用添加路由。``RouteCollectionBuilder``
    具有使在PHP中添加路由更有趣的方法。你还可以加载外部路由文件（如下所示）。

高级示例：Twig、注释以及Web调试工具栏
-------------------------------------------------------------

``MicroKernelTrait`` 的目标 *不是* 创建一个单文件的应用。相反，它的目标是让你有权选择bundle和结构。

首先，你可能希望将PHP类放在 ``src/`` 目录中。配置 ``composer.json`` 文件以定义要加载的位置：

.. code-block:: json

    {
        "require": {
            "...": "..."
        },
        "autoload": {
            "psr-4": {
                "App\\": "src/"
            }
        }
    }

然后，运行 ``composer dump-autoload`` 以转储新的自动加载配置。

现在，假设你要使用Twig并通过注释加载路由。
现在不再将 *一切* 都放入 ``index.php``，创建一个新的 ``src/Kernel.php`` 来保存内核。
现在它看起来像这样::

    // src/Kernel.php
    namespace App;

    use Symfony\Bundle\FrameworkBundle\Kernel\MicroKernelTrait;
    use Symfony\Component\Config\Loader\LoaderInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;
    use Symfony\Component\Routing\RouteCollectionBuilder;

    class Kernel extends BaseKernel
    {
        use MicroKernelTrait;

        public function registerBundles()
        {
            $bundles = array(
                new \Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
                new \Symfony\Bundle\TwigBundle\TwigBundle(),
            );

            if ($this->getEnvironment() == 'dev') {
                $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            }

            return $bundles;
        }

        protected function configureContainer(ContainerBuilder $c, LoaderInterface $loader)
        {
            $loader->load(__DIR__.'/../config/framework.yaml');

            // 仅在启用了WebProfilerBundle时才配置该bundle
            if (isset($this->bundles['WebProfilerBundle'])) {
                $c->loadFromExtension('web_profiler', array(
                    'toolbar' => true,
                    'intercept_redirects' => false,
                ));
            }
        }

        protected function configureRoutes(RouteCollectionBuilder $routes)
        {
            // 仅在启用了WebProfilerRoutes时才导入该bundle
            if (isset($this->bundles['WebProfilerBundle'])) {
                $routes->import('@WebProfilerBundle/Resources/config/routing/wdt.xml', '/_wdt');
                $routes->import('@WebProfilerBundle/Resources/config/routing/profiler.xml', '/_profiler');
            }

            // 加载注释路由
            $routes->import(__DIR__.'/../src/Controller/', '/', 'annotation');
        }

        // 可选项，使用标准的Symfony缓存目录
        public function getCacheDir()
        {
            return __DIR__.'/../var/cache/'.$this->getEnvironment();
        }

        // 可选项，使用标准的Symfony日志目录
        public function getLogDir()
        {
            return __DIR__.'/../var/log';
        }
    }

在继续之前，运行此命令以添加对新依赖的支持：

.. code-block:: terminal

    $ composer require symfony/yaml symfony/twig-bundle symfony/web-profiler-bundle doctrine/annotations

与之前的内核不同，这会加载一个 ``config/framework.yaml`` 外部文件，因为配置开始变多：

.. configuration-block::

    .. code-block:: yaml

        # config/framework.yaml
        framework:
            secret: S0ME_SECRET
            profiler: { only_exceptions: false }

    .. code-block:: xml

        <!-- config/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config secret="S0ME_SECRET">
                <framework:profiler only-exceptions="false" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/framework.php
        $container->loadFromExtension('framework', array(
            'secret' => 'S0ME_SECRET',
            'profiler' => array(
                'only_exceptions' => false,
            ),
        ));

这也会从一个包含一个文件的 ``src/Controller/`` 目录中加载注释路由::

    // src/Controller/MicroController.php
    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Routing\Annotation\Route;

    class MicroController extends AbstractController
    {
        /**
         * @Route("/random/{limit}")
         */
        public function randomNumber($limit)
        {
            $number = random_int(0, $limit);

            return $this->render('micro/random.html.twig', array(
                'number' => $number,
            ));
        }
    }

模板文件应该位于项目根目录的 ``templates/`` 目录中。
此模板位于 ``templates/micro/random.html.twig``：

.. code-block:: html+twig

    <!-- templates/micro/random.html.twig -->
    <!DOCTYPE html>
    <html>
        <head>
            <title>Random action</title>
        </head>
        <body>
            <p>{{ number }}</p>
        </body>
    </html>

最后，你需要一个前端控制器来启动和运行该应用。创建一个``public/index.php``::

    // public/index.php

    use App\Kernel;
    use Doctrine\Common\Annotations\AnnotationRegistry;
    use Symfony\Component\HttpFoundation\Request;

    $loader = require __DIR__.'/../vendor/autoload.php';
    // 自动加载注释
    AnnotationRegistry::registerLoader(array($loader, 'loadClass'));

    $kernel = new Kernel('dev', true);
    $request = Request::createFromGlobals();
    $response = $kernel->handle($request);
    $response->send();
    $kernel->terminate($request, $response);

仅此而已！``/random/10`` URL将会生效，Twig将被渲染，你甚至可以将Web调试工具栏显示在底部。
最终结构如下所示：

.. code-block:: text

    your-project/
    ├─ config/
    │  └─ framework.yaml
    ├─ public/
    |  └─ index.php
    ├─ src/
    |  ├─ Controller
    |  |  └─ MicroController.php
    |  └─ Kernel.php
    ├─ templates/
    |  └─ micro/
    |     └─ random.html.twig
    ├─ var/
    |  ├─ cache/
    │  └─ log/
    ├─ vendor/
    │  └─ ...
    ├─ composer.json
    └─ composer.lock

和之前一样，你可以使用PHP内置服务器：

.. code-block:: terminal

    cd public/
    $ php -S localhost:8000 -t public/

然后在浏览器中查看该网页：

    http://localhost:8000/random/10
