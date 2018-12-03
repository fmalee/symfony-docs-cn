.. index::
    single: Sessions, saving locale

在用户会话期间保存(Sticky)语言环境
==================================================

Symfony将语言环境设置存储在请求中，这意味着此设置不会跨请求自动保存（“sticky”）。
但是，你 *可以* 将语言环境存储在会话中，以便在后续请求中使用它。

.. _creating-a-LocaleSubscriber:

创建LocaleSubscriber
---------------------------

创建一个 :ref:`新的事件订阅器 <events-subscriber>`。
通常，``_locale`` 被用作表示语言环境的路由参数，但你可以根据需要确定正确的语言环境::

    // src/EventSubscriber/LocaleSubscriber.php
    namespace App\EventSubscriber;

    use Symfony\Component\HttpKernel\Event\GetResponseEvent;
    use Symfony\Component\HttpKernel\KernelEvents;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;

    class LocaleSubscriber implements EventSubscriberInterface
    {
        private $defaultLocale;

        public function __construct($defaultLocale = 'en')
        {
            $this->defaultLocale = $defaultLocale;
        }

        public function onKernelRequest(GetResponseEvent $event)
        {
            $request = $event->getRequest();
            if (!$request->hasPreviousSession()) {
                return;
            }

            // 尝试查看是否已将语言环境设置为一个 _locale 路由参数
            if ($locale = $request->attributes->get('_locale')) {
                $request->getSession()->set('_locale', $locale);
            } else {
                // 如果未在此请求上显式的设置语言环境，则使用会话中的语言环境
                $request->setLocale($request->getSession()->get('_locale', $this->defaultLocale));
            }
        }

        public static function getSubscribedEvents()
        {
            return array(
                // 必须在默认的语言环境监听器之前（即一个更高的优先级）注册
                KernelEvents::REQUEST => array(array('onKernelRequest', 20)),
            );
        }
    }

如果你使用
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，那么你已经完工了！
Symfony将自动了解该事件订阅者并在每个请求上调用 ``onKernelRequest`` 方法。

要查看它是否正常工作，请手动（例如，通过某些“更改区域设置”的路由和控制器）设置会话上的
``_locale`` 键，或创建具有 :ref:`默认 _locale <translation-locale-url>` 的路由。

.. sidebar:: 显式的配置订阅器

    你也可以显式配置它，以便传入
    :ref:`default_locale <config-framework-default_locale>`：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            services:
                # ...

                App\EventSubscriber\LocaleSubscriber:
                    arguments: ['%kernel.default_locale%']
                    # 如果你不使用自动配置，请取消下一行的注释
                    # tags: [kernel.event_subscriber]

        .. code-block:: xml

            <!-- config/services.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <services>
                    <service id="App\EventSubscriber\LocaleSubscriber">
                        <argument>%kernel.default_locale%</argument>

                        <!-- uncomment the next line if you are not using autoconfigure -->
                        <!-- <tag name="kernel.event_subscriber" /> -->
                    </service>
                </services>
            </container>

        .. code-block:: php

            // config/services.php
            use App\EventSubscriber\LocaleSubscriber;

            $container->register(LocaleSubscriber::class)
                ->addArgument('%kernel.default_locale%')
                // uncomment the next line if you are not using autoconfigure
                // ->addTag('kernel.event_subscriber');

仅此而已！现在可以通过更改用户的语言环境，然后就能在整个请求中看到它是自动保存（sticky）的。

请记住，要获取用户的语言环境，请始终使用 :method:`Request::getLocale <Symfony\\Component\\HttpFoundation\\Request::getLocale>` 方法::

    // from a controller...
    use Symfony\Component\HttpFoundation\Request;

    public function index(Request $request)
    {
        $locale = $request->getLocale();
    }

根据用户的首选项设置语言环境
--------------------------------------------------

你可能希望进一步改进此技术，并根据登录用户的用户实体来定义语言环境。
但是，由于 ``LocaleSubscriber`` 在 ``FirewallListener``（负责在
``TokenStorage`` 中处理认证和设置用户令牌）之前被调用，因此你无权在该监听器中访问登录的用户。

假设你的 ``User`` 实体上有一个 ``locale`` 属性，并希望使用它来保持给定用户的语言环境。
要实现此目的，你可以挂钩到登录进程，并在它们被重定向之前使用此语言环境值更新用户的会话。

为此，你需要一个事件订阅器来订阅 ``security.interactive_login``::

    // src/EventSubscriber/UserLocaleSubscriber.php
    namespace App\EventSubscriber;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\HttpFoundation\Session\SessionInterface;
    use Symfony\Component\Security\Http\Event\InteractiveLoginEvent;
    use Symfony\Component\Security\Http\SecurityEvents;

    /**
     * 在登录后，在会话中存储用户的语言环境。
     * 稍后会由LocaleSubscriber使用。
     */
    class UserLocaleSubscriber implements EventSubscriberInterface
    {
        private $session;

        public function __construct(SessionInterface $session)
        {
            $this->session = $session;
        }

        public function onInteractiveLogin(InteractiveLoginEvent $event)
        {
            $user = $event->getAuthenticationToken()->getUser();

            if (null !== $user->getLocale()) {
                $this->session->set('_locale', $user->getLocale());
            }
        }

        public static function getSubscribedEvents()
        {
            return array(
                SecurityEvents::INTERACTIVE_LOGIN => 'onInteractiveLogin',
            );
        }
    }

.. caution::

    为了在用户更改语言首选项后立即更新对应语言，你还需要在更改 ``User`` 实体时更新会话。
