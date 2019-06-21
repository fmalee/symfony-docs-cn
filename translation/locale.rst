.. index::
    single: Translation; Locale

如何使用用户的语言环境
==================================

当前用户的语言环境存储在请求中，可通过 ``Request`` 对象访问::

    use Symfony\Component\HttpFoundation\Request;

    public function index(Request $request)
    {
        $locale = $request->getLocale();
    }

要设置用户的语言环境，你可能需要创建一个自定义事件监听器，以便在系统（即翻译器）的任何其他部分需要它之前设置它::

        public function onKernelRequest(GetResponseEvent $event)
        {
            $request = $event->getRequest();

            // 一些决定 $locale 的逻辑
            $request->setLocale($locale);
        }

.. note::

    自定义监听器必须在 ``LocaleListener`` 之前调用，因为 ``LocaleListener``
    基于当前请求的语言环境进行初始化。
    为此，请将该监听器优先级设置为高于 ``LocaleListener``
    优先级（可以运行 ``debug:event kernel.request`` 命令获得）的值。

阅读 :doc:`/session/locale_sticky_session`，以获取有关将用户的语言环境“粘滞”到其会话的更多信息。

.. note::

    在控制器中使用 ``$request->setLocale()`` 来设置语言环境为时已晚，已经无法影响翻译器。
    可以通过监听器（如上所述）、URL（请参阅下一章节）或直接在``translator``
    服务中调用 ``setLocale()`` 来设置语言环境。

请参阅 :ref:`translation-locale-url` 章节，以了解如何通过路由来设置语言环境。

.. _translation-locale-url:

语言环境和URL
----------------------

由于你可以在会话中存储用户的语言环境，因此可能可以基于用户的语言环境在不同的语言中，使用相同的URL来显示一个资源。
例如，``http://www.example.com/contact`` 可以为一个用户显示英语内容，为另一个用户显示法语内容。
不幸的是，这违反了Web的一个通用规则：特定的URL要对用户返回相同的资源，不管是什么（语种的）用户。
进一步搞砸问题的是，哪个版本的内容可以被搜索引擎检索到？

更好的策略是在URL中包含语言环境。路由系统使用特殊的 ``_locale`` 参数完全支持此功能：

.. configuration-block::

    .. code-block:: yaml

        # config/routes.yaml
        contact:
            path:       /{_locale}/contact
            controller: App\Controller\ContactController::index
            requirements:
                _locale: en|fr|de

    .. code-block:: xml

        <!-- config/routes.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing
                https://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="contact" path="/{_locale}/contact">
                controller="App\Controller\ContactController::index">
                <requirement key="_locale">en|fr|de</requirement>
            </route>
        </routes>

    .. code-block:: php

        // config/routes.php
        use App\Controller\ContactController;
        use Symfony\Component\Routing\Loader\Configurator\RoutingConfigurator;

        return function (RoutingConfigurator $routes) {
            $routes->add('contact', '/{_locale}/contact')
                ->controller([ContactController::class, 'index'])
                ->requirements([
                    '_locale' => 'en|fr|de',
                ])
            ;
        };

在路由中使用特殊的 ``_locale`` 参数时，匹配的语言环境会 *自动设置到请求中*，并可通过
:method:`Symfony\\Component\\HttpFoundation\\Request::getLocale` 方法检索。
换句话说，如果用户访问 ``/fr/contact`` URI，则将自动设置为当前请求的语言环境为 ``fr``。

你现在可以使用语言环境来创建到应用中其他已翻译页面的路由。

.. tip::

    阅读 :doc:`/routing/service_container_parameters`，以了解如何避免在所有路由中硬编码
    ``_locale`` 要求。

.. index::
    single: Translations; Fallback and default locale

.. _translation-default-locale:

设置默认的语言环境
------------------------

如果用户的语言环境尚未确定怎么办？你可以通过为框架定义一个 ``default_locale``
来保证为每个用户的请求设置一个语言环境：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/translation.yaml
        framework:
            default_locale: en

    .. code-block:: xml

        <!-- config/packages/translation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config default-locale="en"/>
        </container>

    .. code-block:: php

        // config/packages/translation.php
        $container->loadFromExtension('framework', [
            'default_locale' => 'en',
        ]);
