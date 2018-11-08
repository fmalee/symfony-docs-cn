.. index::
   single: Controller; As Services

如何将控制器定义为服务
=====================================

在Symfony中，控制器就 *不* 需要注册为服务。
但是，如果你使用的是
:ref:`默认services.yaml配置 <service-container-services-load-example>`，
你的控制器都 *已经* 被注册为服务。这意味着你可以像任何其他普通服务一样使用依赖注入。

从路由引用你的服务
-------------------------------------

将控制器注册为服务是第一步，但你还需要更新路由配置以正确引用服务，以便Symfony知道如何使用它。

使用 ``service_id::method_name`` 语法来引用控制器方法。
如果服务ID是控制器的完全限定类名（FQCN），正如Symfony建议的那样，那么该语法与控制器不是服务的语法相同
：``App\Controller\HelloController::index``：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Controller/HelloController.php

        use Symfony\Component\Routing\Annotation\Route;

        class HelloController
        {
            /**
             * @Route("/hello", name="hello")
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
            defaults: { _controller: App\Controller\HelloController::index }

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="hello" path="/hello">
                <default key="_controller">App\Controller\HelloController::index</default>
            </route>

        </routes>

    .. code-block:: php

        // config/routes.php
        $collection->add('hello', new Route('/hello', array(
            '_controller' => 'App\Controller\HelloController::index',
        )));

.. _controller-service-invoke:

Invokable控制器
---------------------

如果你的控制器实现了一个受Action-Domain-Response（ADR）模式的欢迎的 ``__invoke()`` 方法，
那么你可以在没有方法的情况下引用服务ID
（例如 ``App\Controller\HelloController``）。

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
                array('name' => $name)
            );

            return new Response($content);
        }
    }

你还可以使用特殊的 :ref:`基于动作的依赖项注入 <controller-accessing-services>`
来接收服务作为控制器动作方法的参数。

基础控制器方法及其服务替换
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

了解如何替换基础 ``Controller`` 便捷方法的最佳方法是查看保存其逻辑的 `ControllerTrait`_。

如果你想知道每个服务使用哪种类型约束，请参阅 `AbstractController`_ 中的 ``getSubscribedServices()`` 方法。

.. _`控制器类源代码`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`base Controller class`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`ControllerTrait`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`AbstractController`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/AbstractController.php
