.. index::
   single: Controller; As Services

如何将控制器定义为服务
=====================================

在Symfony的，控制器就不会需要注册为服务。但是，如果您使用的是默认services.yaml配置，你的控制器都已经被注册为服务。这意味着您可以像任何其他普通服务一样使用依赖注入。
In Symfony, a controller does *not* need to be registered as a service. But if you're
using the :ref:`default services.yaml configuration <service-container-services-load-example>`,
your controllers *are* already registered as services. This means you can use dependency
injection like any other normal service.

从路由引用你的服务
-------------------------------------

将控制器注册为服务是第一步，但您还需要更新路由配置以正确引用服务，以便Symfony知道使用它。
Registering your controller as a service is the first step, but you also need to
update your routing config to reference the service properly, so that Symfony
knows to use it.

使用service_id::method_name语法来引用控制器方法。如果服务标识是控制器的完全限定类名（FQCN），正如Symfony建议的那样，则语法与控制器不是如下服务的语法相同App\Controller\HelloController::index：
Use the ``service_id::method_name`` syntax to refer to the controller method.
If the service id is the fully-qualified class name (FQCN) of your controller,
as Symfony recommends, then the syntax is the same as if the controller was not
a service like: ``App\Controller\HelloController::index``:

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

如果您的控制器实现了 ``__invoke()`` 方法 - 受Action-Domain-Response（ADR）模式的欢迎，您可以在没有方法的情况下引用服务ID
（例如 ``App\Controller\HelloController``）。
If your controller implements the ``__invoke()`` method - popular with the
Action-Domain-Response (ADR) pattern, you can refer to the service id
without the method (``App\Controller\HelloController`` for example).

基础控制器方法的替代方案
---------------------------------------

使用定义为服务的控制器时，您仍然可以扩展 AbstractController基本控制器 并使用其快捷方式。但是，你不需要！您可以选择不扩展任何内容，并使用依赖注入来访问不同的服务。
When using a controller defined as a service, you can still extend the
:ref:`AbstractController base controller <the-base-controller-class-services>`
and use its shortcuts. But, you don't need to! You can choose to extend *nothing*,
and use dependency injection to access different services.

基本Controller类源代码是查看如何完成常见任务的好方法。例如，$this->render()通常用于呈现Twig模板并返回Response。但是，您也可以直接执行此操作：
The base `Controller class source code`_ is a great way to see how to accomplish
common tasks. For example, ``$this->render()`` is usually used to render a Twig
template and return a Response. But, you can also do this directly:

在定义为服务的控制器中，您可以注入twig 服务并直接使用它：
In a controller that's defined as a service, you can instead inject the ``twig``
service and use it directly::

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

您还可以使用特殊的基于操作的依赖项注入 来接收服务作为控制器操作方法的参数。
You can also use a special :ref:`action-based dependency injection <controller-accessing-services>`
to receive services as arguments to your controller action methods.

基础控制器方法及其服务替换
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

了解如何替换基础Controller便捷方法的最佳方法是查看保存其逻辑的ControllerTrait。
The best way to see how to replace base ``Controller`` convenience methods is to
look at the `ControllerTrait`_ that holds its logic.

如果您想知道每个服务使用哪种类型约束，请参阅 `AbstractController`_ 中的 ``getSubscribedServices()`` 方法。

.. _`Controller class source code`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`base Controller class`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`ControllerTrait`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/ControllerTrait.php
.. _`AbstractController`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/AbstractController.php
