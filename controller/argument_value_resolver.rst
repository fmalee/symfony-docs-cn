.. index::
   single: Controller; Argument Value Resolvers

扩展动作参数的解析
===================================

在 :doc:`控制器指南 </controller>` 中，你已经了解到可以通过控制器中的参数获取
:class:`Symfony\\Component\\HttpFoundation\\Request` 对象。
该参数必须由 ``Request`` 类进行类型约束才能被识别。
这是通过 :class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentResolver` 完成的。
通过创建和注册自定义的参数值解析器，你可以扩展此功能。

HttpKernel附带的功能
-----------------------------------------

Symfony在HttpKernel组件中附带五个值解析器：

:class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentResolver\\RequestAttributeValueResolver`
    尝试查找一个与参数名称匹配的请求属性。

:class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentResolver\\RequestValueResolver`
    如果类型约束的是 ``Request`` 类或该类的扩展，则注入当前 ``Request``。

:class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentResolver\\ServiceValueResolver`
    如果类型约束是有效的服务类或接口，则注入对应服务。
    这就像 :doc:`自动装配 </service_container/autowiring>` 一样。

:class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentResolver\\SessionValueResolver`
    如果类型约束的是 ``SessionInterface`` 类或该类的扩展，
    则注入继承自  ``SessionInterface``  的已配置的会话类。

:class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentResolver\\DefaultValueResolver`
    将参数设置为默认值（如果该默认值存在且该参数是可选的）。

:class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentResolver\\VariadicValueResolver`
    验证请求数据是否为数组，并将所有数据添加到参数列表中。
    调用该动作时，最后一个（可变参数）参数将包含此数组的所有值。

添加自定义值解析器
------------------------------

在下一个示例中，只要控制器方法使用 ``User`` 类来类型约束参数，
你就会创建一个值解析器来注入表示当前用户的对象::

    namespace App\Controller;

    use App\Entity\User;
    use Symfony\Component\HttpFoundation\Response;

    class UserController
    {
        public function index(User $user)
        {
            return new Response('Hello '.$user->getUsername().'!');
        }
    }

请注意，此功能已由SensioFrameworkExtraBundle中的 `@ParamConverter`_ 注释提供。
如果你在项目中安装了该软件包，请添加此配置以禁用类型约束方法参数的自动转换：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/sensio_framework_extra.yaml
        sensio_framework_extra:
            request:
                converters: true
                auto_convert: false

    .. code-block:: xml

        <!-- config/packages/sensio_framework_extra.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:sensio-framework-extra="http://symfony.com/schema/dic/symfony_extra"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <sensio-framework-extra:config>
                <request converters="true" auto-convert="false" />
            </sensio-framework-extra:config>
        </container>

    .. code-block:: php

        // config/packages/sensio_framework_extra.php
        $container->loadFromExtension('sensio_framework_extra', array(
            'request' => array(
                'converters' => true,
                'auto_convert' => false,
            ),
        ));

添加新的值解析器需要创建一个实现
:class:`Symfony\\Component\\HttpKernel\\Controller\\ArgumentValueResolverInterface`
类并将其定义为服务。该接口定义了两种方法：

``supports()``
    此方法用于检查值解析器是否支持给定的参数。``resolve()`` 只有在返回 ``true`` 时才会执行。
``resolve()``
    此方法将解析参数的实际值。一旦值被解析，必须 `yield`_ 该值到 ``ArgumentResolver``。

两种方法都获取 ``Request`` 对象，即当前请求和
:class:`Symfony\\Component\\HttpKernel\\ControllerMetadata\\ArgumentMetadata` 实例。
此对象包含从当前参数的方法签名中检索的所有信息。

现在你知道该怎么做了，你可以实现此接口。要获取当前的 ``User``，你需要当前的安全令牌。
可以从令牌存储中检索此令牌::

    // src/ArgumentResolver/UserValueResolver.php
    namespace App\ArgumentResolver;

    use App\Entity\User;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpKernel\Controller\ArgumentValueResolverInterface;
    use Symfony\Component\HttpKernel\ControllerMetadata\ArgumentMetadata;
    use Symfony\Component\Security\Core\Security;

    class UserValueResolver implements ArgumentValueResolverInterface
    {
        private $security;

        public function __construct(Security $security)
        {
            $this->security = $security;
        }

        public function supports(Request $request, ArgumentMetadata $argument)
        {
            if (User::class !== $argument->getType()) {
                return false;
            }

            return $this->security->getUser() instanceof User;
        }

        public function resolve(Request $request, ArgumentMetadata $argument)
        {
            yield $this->security->getUser();
        }
    }

为了在参数中获取到实际 ``User`` 对象，给定值必须满足以下要求：

* 参数必须在动作方法签名中使用 ``User`` 类型提示;
* 该值必须是 ``User`` 类的实例。

当满足所有这些要求并返回 ``true`` 时，``ArgumentResolver`` 调用具有与调用 ``supports()`` 的值相同值的 ``resolve()`` 。

仅此而已！现在，你只需添加服务容器的配置即可。
这可以通过将服务标记为 ``controller.argument_value_resolver`` 并添加优先级来完成。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            _defaults:
                # ... be sure autowiring is enabled
                autowire: true
            # ...

            App\ArgumentResolver\UserValueResolver:
                tags:
                    - { name: controller.argument_value_resolver, priority: 50 }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-Instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... be sure autowiring is enabled -->
                <defaults autowire="true" />
                <!-- ... -->

                <service id="App\ArgumentResolver\UserValueResolver">
                    <tag name="controller.argument_value_resolver" priority="50" />
                </service>
            </services>

        </container>

    .. code-block:: php

        // config/services.php
        use App\ArgumentResolver\UserValueResolver;

        $container->autowire(UserValueResolver::class)
            ->addTag('controller.argument_value_resolver', array('priority' => 50));

虽然添加优先级是可选的，但建议添加一个以确保注入预期值。
负责从 ``Request`` 提取属性的 ``RequestAttributeValueResolver`` 优先级为100，
所以建议以较低优先级触发你的自定义值解析器。
这可确保在属性存在时不会触发该参数解析器。
例如在用子请求传递用户时。

.. tip::

    正如你在 ``UserValueResolver::supports()`` 方法中看到的那样，
    用户可能无法使用（例如，当控制器不在防火墙后面时）的情况。
    在这些情况下，不会执行该解析器。如果没有参数值被解析，则抛出异常。

    为防止这种情况，你可以在控制器中添加一个默认值（例如 ``User $user = null``）。
    ``DefaultValueResolver`` 会作为最后的解析器执行，如果没有值被解析，将使用默认值。

.. _`@ParamConverter`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`yield`: http://php.net/manual/en/language.generators.syntax.php
