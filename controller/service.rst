.. index::
   single: Controller; As Services

如何将控制器定义为服务
=====================================

在Symfony中，*不* 需要将控制器注册为服务。
但是，如果你使用的是
:ref:`默认services.yaml配置 <service-container-services-load-example>`，
你的控制器都 *已经* 被注册为服务。这意味着你可以像任何其他普通服务一样使用依赖注入。

从路由引用你的服务
-------------------------------------

将控制器注册为服务是第一步，但你还需要更新路由配置以正确引用服务，以便Symfony知道如何使用它。

现在可以使用 ``service_id::method_name`` 语法来引用该控制器方法。
如果正如Symfony建议的那样，该控制器的服务ID是其完全限定类名（FQCN），那么该语法与控制器不是服务时的语法相同
：``App\Controller\HelloController::index``：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/HelloController.php
        use Symfony\Component\Routing\Annotation\Route;

        class HelloController
        {
            /**
             * @Route("/hello", name="hello", methods={"GET"})
             */
            public function index()
            {
                // ...
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        hello:
            path:     /hello
            controller: App\Controller\HelloController::index
            methods: GET

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" path="/hello" controller="App\Controller\HelloController::index" methods="GET"/>

        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\HelloController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('hello', '/hello')
                ->controller([HelloController::class, 'index'])
                ->methods(['GET'])
            ;
        };

.. _controller-service-invoke:

Invokable控制器
---------------------

控制器还可以使用 ``__invoke()`` 方法来定义单个动作，这是遵循
`ADR模式`_（Action-Domain-Responder）时的常见做法：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/Hello.php
        use Symfony\Component\HttpFoundation\Response;
        use Symfony\Component\Routing\Annotation\Route;

        /**
         * @Route("/hello/{name}", name="hello")
         */
        class Hello
        {
            public function __invoke($name = 'World')
            {
                return new Response(sprintf('Hello %s!', $name));
            }
        }

    .. code-block:: yaml

        # config/routes.yaml
        hello:
            path:     /hello/{name}
            defaults: { _controller: app.hello_controller }

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" path="/hello/{name}">
                <default key="_controller">app.hello_controller</default>
            </route>

        </routes>

    .. code-block:: php

        // app/config/routing.php
        $collection->add('hello', new Route('/hello', [
            '_controller' => 'app.hello_controller',
        ]));

基础控制器方法的替代方案
---------------------------------------

使用定义为服务的控制器时，你仍然可以继承
:ref:`AbstractController 基础控制器 <the-base-controller-class-services>`
并使用其快捷方式。
但是，你可以不需要如此！你可以选择不继承任何内容，并使用依赖注入来访问不同的服务。

基础 `控制器类源代码`_ 是查看如何完成常见任务的好方法。
例如 ``$this->render()`` 通常用于渲染Twig模板并返回响应。但是，你也可以直接执行此操作：

在定义为服务的控制器中，你可以注入 ``twig`` 服务并直接使用它::

    // src/Controller/HelloController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\Response;
    use Twig\Environment;

    class HelloController
    {
        private $twig;

        public function __construct(Environment $twig)
        {
            $this->twig = $twig;
        }

        public function index($name)
        {
            $content = $this->twig->render(
                'hello/index.html.twig',
                ['name' => $name]
            );

            return new Response($content);
        }
    }

你还可以使用特殊的 :ref:`基于动作的依赖项注入 <controller-accessing-services>`
来接收服务作为控制器动作方法的参数。

基础控制器方法及其服务替换
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

了解如何替换基础 ``Controller`` 便捷方法的最佳方法是查看保存其逻辑的 `ControllerTrait`_。

如果你想知道使用哪种类型约束来获取每个服务，请参阅 `AbstractController`_ 中的 ``getSubscribedServices()`` 方法。

.. _`控制器类源代码`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`base Controller class`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`ControllerTrait`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`AbstractController`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/AbstractController.php
.. _`ADR模式`: https://en.wikipedia.org/wiki/Action%E2%80%93domain%E2%80%93responder
