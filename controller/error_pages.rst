.. index::
   single: Controller; Customize error pages
   single: Error pages

如何自定义错误页面
============================

在Symfony应用程序中，所有错误都被视为异常，无论它们只是404 Not Found错误还是由于在代码中抛出异常而触发的致命错误。
In Symfony applications, all errors are treated as exceptions, no matter if they
are just a 404 Not Found error or a fatal error triggered by throwing some
exception in your code.

在开发环境中，Symfony捕获所有异常并显示一个 包含大量调试信息的特殊异常页面，以帮助您发现根问题：
In the :doc:`development environment </configuration/environments>`,
Symfony catches all the exceptions and displays a special **exception page**
with lots of debug information to help you discover the root problem:

.. image:: /_images/controller/error_pages/exceptions-in-dev-environment.png
   :alt: A typical exception page in the development environment
   :align: center
   :class: with-browser

由于这些页面包含许多敏感的内部信息，因此Symfony不会在生产环境中显示它们。相反，它将显示一个简单而通用的错误页面：
Since these pages contain a lot of sensitive internal information, Symfony won't
display them in the production environment. Instead, it'll show a simple and
generic **error page**:

.. image:: /_images/controller/error_pages/errors-in-prod-environment.png
   :alt: A typical error page in the production environment
   :align: center
   :class: with-browser

可以根据您的需要以不同方式自定义生产环境的错误页面：
Error pages for the production environment can be customized in different ways
depending on your needs:

#. If you just want to change the contents and styles of the error pages to match
   the rest of your application, :ref:`override the default error templates <use-default-exception-controller>`;
   如果您只想更改错误页面的内容和样式以匹配应用程序的其余部分，请覆盖默认错误模板 ;

#. If you also want to tweak the logic used by Symfony to generate error pages,
   :ref:`override the default exception controller <custom-exception-controller>`;
   如果您还想调整Symfony使用的逻辑来生成错误页面，请 覆盖默认的异常控制器 ;

#. If you need total control of exception handling to execute your own logic
   :ref:`use the kernel.exception event <use-kernel-exception-event>`.
   如果需要完全控制异常处理来执行自己的逻辑，请 使用kernel.exception事件。

.. _use-default-exception-controller:
.. _using-the-default-exceptioncontroller:

重写默认的错误模板
--------------------------------------

加载错误页面时，内部ExceptionController 用于呈现Twig模板以显示用户。
When the error page loads, an internal :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController`
is used to render a Twig template to show the user.

.. _controller-error-pages-by-status-code:

此控制器使用HTTP状态代码，请求格式和以下逻辑来确定模板文件名：
This controller uses the HTTP status code, the request format and the following
logic to determine the template filename:

#. Look for a template for the given format and status code (like ``error404.json.twig``
   or ``error500.html.twig``);
   寻找给定格式和状态代码的模板（如error404.json.twig 或error500.html.twig）;

#. If the previous template doesn't exist, discard the status code and look for
   a generic template for the given format (like ``error.json.twig`` or
   ``error.xml.twig``);
   如果以前的模板不存在，则丢弃状态代码并查找给定格式的通用模板（如error.json.twig或 error.xml.twig）;

#. If none of the previous templates exist, fall back to the generic HTML template
   (``error.html.twig``).
   如果不存在以前的模板，则回退到通用HTML模板（error.html.twig）。

.. _overriding-or-adding-templates:

要覆盖这些模板，请依赖标准的Symfony方法来 覆盖生活在bundle中的模板并将它们放在templates/bundles/TwigBundle/Exception/目录中。
To override these templates, rely on the standard Symfony method for
:ref:`overriding templates that live inside a bundle <override-templates>` and
put them in the ``templates/bundles/TwigBundle/Exception/`` directory.

返回HTML和JSON页面的典型项目可能如下所示：
A typical project that returns HTML and JSON pages might look like this:

.. code-block:: text

    templates/
    └─ bundles/
       └─ TwigBundle/
          └─ Exception/
             ├─ error404.html.twig
             ├─ error403.html.twig
             ├─ error.html.twig      # All other HTML errors (including 500)
             ├─ error404.json.twig
             ├─ error403.json.twig
             └─ error.json.twig      # All other JSON errors (including 500)

404错误模板示例
--------------------------

要覆盖HTML页面的404错误模板，请创建error404.html.twig位于以下位置的新 模板templates/bundles/TwigBundle/Exception/：
To override the 404 error template for HTML pages, create a new
``error404.html.twig`` template located at ``templates/bundles/TwigBundle/Exception/``:

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

如果您需要它们，则ExceptionController通过分别存储HTTP状态代码和消息的变量status_code和status_text变量将一些信息传递给错误模板。
In case you need them, the ``ExceptionController`` passes some information to
the error template via the ``status_code`` and ``status_text`` variables that
store the HTTP status code and message respectively.

.. tip::

    You can customize the status code by implementing
    :class:`Symfony\\Component\\HttpKernel\\Exception\\HttpExceptionInterface`
    and its required ``getStatusCode()`` method. Otherwise, the ``status_code``
    will default to ``500``.
    您可以通过实现HttpExceptionInterface 及其所需getStatusCode()方法来自定义状态代码 。否则，status_code 默认为500。

.. note::

    The exception pages shown in the development environment can be customized
    in the same way as error pages. Create a new ``exception.html.twig`` template
    for the standard HTML exception page or ``exception.json.twig`` for the JSON
    exception page.
    可以使用与错误页面相同的方式自定义开发环境中显示的异常页面。exception.html.twig为标准HTML异常页面或exception.json.twigJSON异常页面创建新模板。

安全 & 404页
--------------------

由于加载路由和安全性的顺序，404页面上将不提供安全信息 。这意味着它看起来好像您的用户已在404页面上注销（它将在测试时起作用，但不会在生产中起作用）。
Due to the order of how routing and security are loaded, security information will
*not* be available on your 404 pages. This means that it will appear as if your
user is logged out on the 404 page (it will work while testing, but not on production).

.. _testing-error-pages:

在开发期间测试错误页面
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

当您处于开发环境中时，Symfony会显示大异常 页面而不是您闪亮的新自定义错误页面。那么，你怎么看到它的样子并进行调试呢？
While you're in the development environment, Symfony shows the big *exception*
page instead of your shiny new customized error page. So, how can you see
what it looks like and debug it?

幸运的是，默认ExceptionController允许您在开发期间预览 错误页面。
Fortunately, the default ``ExceptionController`` allows you to preview your
*error* pages during development.

要使用此功能，您需要加载TwigBundle提供的一些特殊路由（如果应用程序使用Symfony Flex，则在安装Twig支持时会自动加载它们）：
To use this feature, you need to load some special routes provided by TwigBundle
(if the application uses :doc:`Symfony Flex </setup/flex>` they are loaded
automatically when installing Twig support):

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
                http://symfony.com/schema/routing/routing-1.0.xsd">

            <import resource="@TwigBundle/Resources/config/routing/errors.xml"
                prefix="/_error" />
        </routes>

    .. code-block:: php

        // config/routes/dev/twig.php
        use Symfony\Component\Routing\RouteCollection;

        $routes = new RouteCollection();
        $routes->addCollection(
            $loader->import('@TwigBundle/Resources/config/routing/errors.xml')
        );
        $routes->addPrefix("/_error");

        return $routes;

添加此路由后，您可以使用这些URL来预览给定状态代码的错误页面，如HTML或给定的状态代码和格式。
With this route added, you can use URLs like these to preview the *error* page
for a given status code as HTML or for a given status code and format.

.. code-block:: text

     http://localhost/index.php/_error/{statusCode}
     http://localhost/index.php/_error/{statusCode}.{format}

.. _custom-exception-controller:
.. _replacing-the-default-exceptioncontroller:

重写默认的ExceptionController
------------------------------------------

如果除了覆盖模板之外还需要更多的灵活性，那么您可以更改呈现错误页面的控制器。例如，您可能需要将一些其他变量传递到模板中。
If you need a little more flexibility beyond just overriding the template,
then you can change the controller that renders the error page. For example,
you might need to pass some additional variables into your template.

为此，在应用程序的任何位置创建一个新控制器，并将twig.exception_controller 配置选项设置为指向它：
To do this, create a new controller anywhere in your application and set
the :ref:`twig.exception_controller <config-twig-exception-controller>`
configuration option to point to it:

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
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                http://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:exception-controller>App\Controller\ExceptionController::showException</twig:exception-controller>
            </twig:config>

        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', array(
            'exception_controller' => 'App\Controller\ExceptionController::showException',
            // ...
        ));

在ExceptionListener 使用的TwigBundle作为一个监听器类kernel.exception事件创建将被分派到控制器的要求。另外，你的控制器将传递两个参数：
The :class:`Symfony\\Component\\HttpKernel\\EventListener\\ExceptionListener`
class used by the TwigBundle as a listener of the ``kernel.exception`` event creates
the request that will be dispatched to your controller. In addition, your controller
will be passed two parameters:

``exception``
    A :class:`\\Symfony\\Component\\Debug\\Exception\\FlattenException`
    instance created from the exception being handled.
    FlattenException 从处理的异常创建的实例。

``logger``
    A :class:`\\Symfony\\Component\\HttpKernel\\Log\\DebugLoggerInterface`
    instance which may be ``null`` in some circumstances.
    一个DebugLoggerInterface 可能是例如null在某些情况下。

您可以扩展默认值，而不是从头开始创建新的异常控制器ExceptionController。在这种情况下，你可能要重写的一个或两个showAction()和  findTemplate()方法。后者定位要使用的模板。
Instead of creating a new exception controller from scratch you can also extend
the default :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController`.
In that case, you might want to override one or both of the ``showAction()`` and
``findTemplate()`` methods. The latter one locates the template to be used.

.. note::

    In case of extending the
    :class:`Symfony\\Bundle\\TwigBundle\\Controller\\ExceptionController` you
    may configure a service to pass the Twig environment and the ``debug`` flag
    to the constructor.
    在扩展的情况下，  ExceptionController您可以配置服务以将Twig环境和debug标志传递给构造函数。

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            services:
                _defaults:
                    # ... be sure autowiring is enabled
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
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <services>
                    <!-- ... be sure autowiring is enabled -->
                    <defaults autowire="true" />
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

    The :ref:`error page preview <testing-error-pages>` also works for
    your own controllers set up this way.
    该错误页面预览也适用于您自己的控制器设置了这种方式。

.. _use-kernel-exception-event:

使用 ``kernel.exception`` 事件
-------------------------------------------

抛出异常时，HttpKernel 类会捕获它并调度一个kernel.exception事件。这使您能够以Response几种不同的方式将异常转换为a 。
When an exception is thrown, the :class:`Symfony\\Component\\HttpKernel\\HttpKernel`
class catches it and dispatches a ``kernel.exception`` event. This gives you the
power to convert the exception into a ``Response`` in a few different ways.

使用此事件实际上比之前解释的要强大得多，但也需要彻底了解Symfony内部。假设您的代码抛出了对您的应用程序域具有特定含义的特殊异常。
Working with this event is actually much more powerful than what has been explained
before, but also requires a thorough understanding of Symfony internals. Suppose
that your code throws specialized exceptions with a particular meaning to your
application domain.

为kernel.exception事件编写自己的事件侦听器可以让您仔细查看异常并根据它采取不同的操作。这些操作可能包括记录异常，将用户重定向到另一个页面或呈现专门的错误页面。
:doc:`Writing your own event listener </event_dispatcher>`
for the ``kernel.exception`` event allows you to have a closer look at the exception
and take different actions depending on it. Those actions might include logging
the exception, redirecting the user to another page or rendering specialized
error pages.

.. note::

    If your listener calls ``setResponse()`` on the
    :class:`Symfony\\Component\\HttpKernel\\Event\\GetResponseForExceptionEvent`,
    event, propagation will be stopped and the response will be sent to
    the client.
    如果你的听众来电setResponse()的  GetResponseForExceptionEvent，事件的传播将停止，响应将被发送到客户端。

这种方法允许您创建集中式和分层的错误处理：不是一次又一次地在各种控制器中捕获（和处理）相同的异常，
您可以只有一个（或几个）侦听器处理它们。
This approach allows you to create centralized and layered error handling:
instead of catching (and handling) the same exceptions in various controllers
time and again, you can have just one (or several) listeners deal with them.

.. tip::

    See :class:`Symfony\\Component\\Security\\Http\\Firewall\\ExceptionListener`
    class code for a real example of an advanced listener of this type. This
    listener handles various security-related exceptions that are thrown in
    your application (like :class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`)
    and takes measures like redirecting the user to the login page, logging them
    out and other things.
    请参阅ExceptionListener 类代码以获取此类高级侦听器的真实示例。此侦听器处理应用程序中抛出的各种与安全相关的异常（例如AccessDeniedException），并采取措施，例如将用户重定向到登录页面，将其记录下来以及其他内容。

.. _`WebfactoryExceptionsBundle`: https://github.com/webfactory/exceptions-bundle
.. _`Symfony Standard Edition`: https://github.com/symfony/symfony-standard/
.. _`ExceptionListener`: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Security/Http/Firewall/ExceptionListener.php
