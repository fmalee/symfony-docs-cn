.. index::
   single: Controller; Customize error pages
   single: Error pages

如何自定义错误页面
============================

在Symfony应用中，所有错误都被视为异常，无论它们只是 “404 Not Found”
错误还是由于在代码中抛出异常而触发的致命错误。

如果你的应用安装了 `TwigBundle`_，则一个特殊控制器会处理这些异常。
此控制器显示错误的调试信息并允许自定义错误页面，因此请运行此命令以确保安装了该软件包：

.. code-block:: terminal

    $ composer require twig

在 :ref:`开发环境 </configuration/environments>` 中，
Symfony捕获所有异常并显示一个包含大量调试信息的特殊 **异常页面**，以帮助你发现问题的症结：

.. image:: /_images/controller/error_pages/exceptions-in-dev-environment.png
   :alt: A typical exception page in the development environment
   :align: center
   :class: with-browser

由于这些页面包含许多敏感的内部信息，因此Symfony不会在生产环境中显示它们。
相反，它将显示一个简单而通用的 **错误页面**：

.. image:: /_images/controller/error_pages/errors-in-prod-environment.png
   :alt: A typical error page in the production environment
   :align: center
   :class: with-browser

可以根据你的需要以不同方式自定义生产环境的错误页面：

#. 如果你只想更改错误页面的内容和样式以匹配应用的其余部分，
   请 :ref:`重写默认错误模板 <use-default-exception-controller>`;

#. 如果你还想调整Symfony使用的逻辑来生成错误页面，
   请 :ref:`重写默认异常控制器 <custom-exception-controller>`;

#. 如果需要完全控制异常处理来执行自己的逻辑，
   请 :ref:`使用kernel.exception事件 <use-kernel-exception-event>`。

.. _use-default-exception-controller:
.. _using-the-default-exceptioncontroller:

重写默认的错误模板
--------------------------------------

加载错误页面时，内部 :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController`
用于渲染Twig模板以呈现给用户。

.. _controller-error-pages-by-status-code:

此控制器使用HTTP状态代码、请求格式和以下逻辑来确定模板文件名：

#. 寻找给定格式和状态代码的模板（如 ``error404.json.twig`` 或 ``error500.html.twig``）;

#. 如果之前的模板不存在，则丢弃状态代码并查找给定格式的通用模板
   （如 ``error.json.twig`` 或 ``error.xml.twig``）;

#. 如果之前的模板不存在，则回退到通用HTML模板（``error.html.twig``）。

.. _overriding-or-adding-templates:

要重写这些模板，请依赖标准的Symfony方法来 :ref:`重写位于bundle中的模板 <override-templates>`
并将它们放在 ``templates/bundles/TwigBundle/Exception/`` 目录中。

返回HTML和JSON页面的典型项目可能如下所示：

.. code-block:: text

    templates/
    └─ bundles/
       └─ TwigBundle/
          └─ Exception/
             ├─ error404.html.twig
             ├─ error403.html.twig
             ├─ error.html.twig      # 其他所有的HTML错误 (包括 500)
             ├─ error404.json.twig
             ├─ error403.json.twig
             └─ error.json.twig      # 其他所有的JSON错误 (包括 500)

404错误模板示例
--------------------------

要重写HTML页面的404错误模板，请在 ``templates/bundles/TwigBundle/Exception/``
下创建 ``error404.html.twig``:

.. code-block:: html+twig

    {# templates/bundles/TwigBundle/Exception/error404.html.twig #}
    {% extends 'base.html.twig' %}

    {% block body %}
        <h1>Page not found</h1>

        <p>
            The requested page couldn't be located. Checkout for any URL
            misspelling or <a href="{{ path('homepage') }}">return to the homepage</a>.
        </p>
    {% endblock %}

如果有需要，则可以使用 ``ExceptionController`` 传递一些信息到错误模板，
它们分别是存储HTTP状态代码和消息的 ``status_code`` 和 ``status_text`` 变量。

.. tip::

    你可以通过实现
    :class:`Symfony\\Component\\HttpKernel\\Exception\\HttpExceptionInterface`
    及其所需 ``getStatusCode()`` 方法来自定义状态代码。
    否则， ``status_code`` 默认为 ``500``。

.. note::

    可以使用与错误页面相同的方式来自定义开发环境中显示的异常页面。
    为标准HTML异常页面创建新的 ``exception.html.twig`` 模板，
    或为JSON异常页面创建新的 ``exception.json.twig`` 模板。

安全 & 404页
--------------------

由于加载路由和安全的顺序，404页面上 *不会* 有可用的安全信息。
这意味着它看起来好像你的用户已在404页面上注销（它将在测试时起作用，但不会在生产中起作用）。

.. _testing-error-pages:

在开发期间测试错误页面
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

当你处于开发环境中时，Symfony会显示详细 *异常* 页面而不是你闪亮的新自定义错误页面。
那么，你怎么看到它的样子并进行调试呢？

幸运的是，默认的 ``ExceptionController`` 允许你在开发期间预览 *错误* 页面。

要使用此功能，你需要加载TwigBundle提供的一些特殊路由（如果应用使用 :doc:`Symfony Flex </setup/flex>`，则在安装Twig支持时会自动加载它们）：

.. configuration-block::

    .. code-block:: yaml

        # config/routes/dev/twig.yaml
        _errors:
            resource: '@TwigBundle/Resources/config/routing/errors.xml'
            prefix:   /_error

    .. code-block:: xml

        <!-- config/routes/dev/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@TwigBundle/Resources/config/routing/errors.xml" prefix="/_error"/>
        </routes>

    .. code-block:: php

        // config/routes/dev/twig.php
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->import('@TwigBundle/Resources/config/routing/errors.xml')
                ->prefix('/_error')
            ;
        };

添加此路由后，你可以使用这些URL来预览给定状态代码的HTML或给定状态代码和格式的 *错误* 页面。

.. code-block:: text

     http://localhost/index.php/_error/{statusCode}
     http://localhost/index.php/_error/{statusCode}.{format}

.. _custom-exception-controller:
.. _replacing-the-default-exceptioncontroller:

重写默认的ExceptionController
------------------------------------------

如果除了重写模板之外还需要更多的灵活性，那么你可以更改渲染错误页面的控制器。
例如，你可能需要将一些额外的变量传递到模板中。

为此，在应用的任何位置创建一个新控制器，
并将 :ref:`twig.exception_controller <config-twig-exception-controller>` 
配置选项设置为指向该控制器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            exception_controller: App\Controller\ExceptionController::showException

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:exception-controller>App\Controller\ExceptionController::showException</twig:exception-controller>
            </twig:config>

        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'exception_controller' => 'App\Controller\ExceptionController::showException',
            // ...
        ]);

TwigBundle使用一个监听 ``kernel.exception`` 事件的
:class:`Symfony\\Component\\HttpKernel\\EventListener\\ExceptionListener`
类来创建将被分派到控制器的请求。
另外，你的控制器还将接收两个参数：

``exception``
    从处理的异常创建的
    :class:`\\Symfony\\Component\\Debug\\Exception\\FlattenException` 实例。

``logger``
    一个在某些情况下可能是 ``null`` 的
    :class:`\\Symfony\\Component\\HttpKernel\\Log\\DebugLoggerInterface`。

你可以继承默认的 :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController`
，而不是从头开始创建新的异常控制器。
在这个示例中，你可能要重写 ``showAction()`` 和 ``findTemplate()`` 方法中的一个或两个。
而后者用于定位要使用的模板。

.. note::

    在继承 :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController`
    的情况下，你可以配置一个服务以将Twig环境和 ``debug`` 标志传递给构造函数。

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            services:
                _defaults:
                    # ... 确认自动装配已经启用
                    autowire: true
                # ...

                App\Controller\CustomExceptionController:
                    public: true
                    arguments:
                        $debug: '%kernel.debug%'

        .. code-block:: xml

            <!-- config/services.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd">

                <services>
                    <!-- ... be sure autowiring is enabled -->
                    <defaults autowire="true"/>
                    <!-- ... -->

                    <service id="App\Controller\CustomExceptionController" public="true">
                        <argument key="$debug">%kernel.debug%</argument>
                    </service>
                </services>

            </container>

        .. code-block:: php

            // config/services.php
            use App\Controller\CustomExceptionController;

            $container->autowire(CustomExceptionController::class)
                ->setArgument('$debug', '%kernel.debug%');

.. tip::

    :ref:`错误页面预览 <testing-error-pages>` 也适用于设置了这种方式的你自己的控制器。

.. _use-kernel-exception-event:

使用 ``kernel.exception`` 事件
-------------------------------------------

抛出异常时，:class:`Symfony\\Component\\HttpKernel\\HttpKernel`
类会捕获它并调度一个 ``kernel.exception`` 事件。
这使你能够以几种不同的方式将异常转换为一个 ``Response``。

使用此事件实际上比之前解释的要强大得多，但也需要彻底了解Symfony内部。
比如你的代码抛出了对你的应用域具有特定含义的特殊异常。

``kernel.exception`` 事件可以让你 :doc:`编写自己的事件监听器 </event_dispatcher>`
，让你仔细查看异常并根据它采取不同的操作。
这些操作可能包括记录异常，将用户重定向到另一个页面或渲染专门的错误页面。

.. note::

    如果你的监听器调用
    :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseForExceptionEvent`
    上的 ``setResponse()``，事件的传播将停止，响应将被发送到客户端。

这种方法允许你创建集中式和分层的错误处理：不是一次又一次地在各种控制器中捕获（和处理）相同的异常，
你可以只有一个（或几个）监听器来处理它们。

.. tip::

    请参阅 :class:`Symfony\\Component\\Security\\Http\\Firewall\\ExceptionListener`
    类的代码以获取此类高级监听器的真实示例。
    此监听器处理应用中抛出的各种与安全相关的异常
    （例如 :class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`），
    并采取措施，例如将用户重定向到登录页面，将其记录下来以及其他事情。

.. _`TwigBundle`: https://github.com/symfony/twig-bundle
.. _`WebfactoryExceptionsBundle`: https://github.com/webfactory/exceptions-bundle
.. _`Symfony Standard Edition`: https://github.com/symfony/symfony-standard/
.. _`ExceptionListener`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Security/Http/Firewall/ExceptionListener.php
