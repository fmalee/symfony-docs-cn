.. index::
    single: DependencyInjection; Autowiring

自动定义服务依赖（自动装配）
=========================================================

自动装配允许你以最少的配置管理容器中的服务。
它读取构造函数（或其他方法）上的类型约束，并自动将正确的服务传递给每个方法。
Symfony的自动装配被设计为可预测的：如果不清楚应该传递哪个依赖，你将看到一个可操作的异常。

.. tip::

    感谢Symfony的编译容器，使用自动装配没有运行时开销。

自动装配示例
---------------------

想象一下，你正在构建一个API，以便在Twitter上发布状态，并使用 `ROT13`_
进行模糊处理，ROT13是一个有趣的编码器，可以将字母表中的所有字符向前移动13个字母。

首先创建一个ROT13转换器类::

    namespace App\Util;

    class Rot13Transformer
    {
        public function transform($value)
        {
            return str_rot13($value);
        }
    }

现在是一个使用这个转换器的Twitter客户端::

    namespace App\Service;

    use App\Util\Rot13Transformer;

    class TwitterClient
    {
        private $transformer;

        public function __construct(Rot13Transformer $transformer)
        {
            $this->transformer = $transformer;
        }

        public function tweet($user, $key, $status)
        {
            $transformedStatus = $this->transformer->transform($status);

            // ... 连接到Twitter并发送已编码的状态
        }
    }

如果你使用的是
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，则
**这两个类都已自动注册为服务并配置为自动装配**。
这意味着你可以立即使用它们而无需 *任何* 配置。

但是，为了更好地了解自动装配，以下示例显式的配置了这两个服务。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            _defaults:
                autowire: true
                autoconfigure: true
                public: false
            # ...

            App\Service\TwitterClient:
                # 其余的得感谢_defaults，但每个服务的值都可以被重写
                autowire: true

            App\Util\Rot13Transformer:
                autowire: true

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <defaults autowire="true" autoconfigure="true" public="false"/>
                <!-- ... -->

                <service id="App\Service\TwitterClient" autowire="true"/>

                <service id="App\Util\Rot13Transformer" autowire="true"/>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Service\TwitterClient;
        use App\Util\Rot13Transformer;

        // ...

        // the autowire method is new in Symfony 3.3
        // in earlier versions, use register() and then call setAutowired(true)
        $container->autowire(TwitterClient::class);

        $container->autowire(Rot13Transformer::class)
            ->setPublic(false);

现在，你可以立即在控制器中使用 ``TwitterClient`` 服务了::

    namespace App\Controller;

    use App\Service\TwitterClient;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Routing\Annotation\Route;

    class DefaultController extends AbstractController
    {
        /**
         * @Route("/tweet", methods={"POST"})
         */
        public function tweet(TwitterClient $twitterClient)
        {
            // 从POST过来的数据获取 $user, $key, $status

            $twitterClient->tweet($user, $key, $status);

            // ...
        }
    }

该服务已自动生效！容器知道在创建 ``TwitterClient`` 服务时将 ``Rot13Transformer`` 服务作为第一个参数传递过去。

.. _autowiring-logic-explained:

自动装配逻辑阐述
--------------------------

自动装配通过在 ``TwitterClient`` 中读取 ``Rot13Transformer`` *类型提示* 来工作::

    // ...
    use App\Util\Rot13Transformer;

    class TwitterClient
    {
        // ...

        public function __construct(Rot13Transformer $transformer)
        {
            $this->transformer = $transformer;
        }
    }

自动装配系统会查找其id与类型约束完全匹配的服务：例如 ``App\Util\Rot13Transformer``。
在这个例子中，该服务是存在的！配置 ``Rot13Transformer`` 服务时，你使用其完全限定的类名作为其ID。
自动装配不是魔术：它只是寻找一个id与类型约束相匹配的服务。
如果是 :ref:`自动的加载服务 <service-container-services-load-example>`，则每个服务的id都是其类名称。

如果 *没有* 一个其id与类型完全匹配的服务，则会抛出一个明确的异常。

自动装配是自动化配置的好方法，Symfony会尽可能地 *可预测* 和清晰。

.. _service-autowiring-alias:

使用别名来启用自动装配
----------------------------------

配置自动装配的主要方法是创建一个id与其类完全匹配的服务。
在前面的示例中，该服务的id是 ``App\Util\Rot13Transformer``，从而允许我们自动自动装配此类型。

也可以通过使用 :ref:`别名 <services-alias>` 来完成此操作。
假设由于某种原因，该服务的id变成 ``app.rot13.transformer``。
在这种情况下，任何带有类名称（``App\Util\Rot13Transformer``）的类型提示的参数都不能再自动装配了。

没关系！要解决此问题，你可以通过添加一个服务别名来 *创建* 一个id与类匹配的服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            # 该id不是一个类，因此不会用于自动装配
            app.rot13.transformer:
                class: App\Util\Rot13Transformer
                # ...

            # 但在这里解决了这个问题！
            # 当检测到 ``App\Util\Rot13Transformer`` 类型约束时，
            # 将注入 ``app.rot13.transformer`` 服务。
            App\Util\Rot13Transformer: '@app.rot13.transformer'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->

                <service id="app.rot13.transformer" class="App\Util\Rot13Transformer" autowire="true"/>
                <service id="App\Util\Rot13Transformer" alias="app.rot13.transformer"/>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Util\Rot13Transformer;

        // ...

        $container->autowire('app.rot13.transformer', Rot13Transformer::class)
            ->setPublic(false);
        $container->setAlias(Rot13Transformer::class, 'app.rot13.transformer');

这会创建一个id为 ``App\Util\Rot13Transformer`` 的服务“别名”。
得益于此，自动装配会看到这一点，并在 ``Rot13Transformer`` 类被类型约束时使用它。

.. tip::

    核心bundle使用别名来允许服务被自动装配。例如MonologBu​​ndle创建了一个id为 ``logger`` 的服务。
    但它也增加了一个指向 ``logger`` 服务的 ``Psr\Log\LoggerInterface`` 别名。
    这就是为什么使用 ``Psr\Log\LoggerInterface`` 类型约束的参数可以自动装配的原因。

.. _autowiring-interface-alias:

接口的使用
-----------------------

你可能还会发现自己的类型约束是抽象的（例如接口）而不是具体类，这样的话它可以将依赖替换为其他对象。

为了遵循此最佳做法，假设你决定创建一个 ``TransformerInterface``::

    namespace App\Util;

    interface TransformerInterface
    {
        public function transform($value);
    }

然后，你更新 ``Rot13Transformer`` 以实现它::

    // ...
    class Rot13Transformer implements TransformerInterface
    {
        // ...
    }

既然你有了一个接口，你应该使用它作为你的类型约束::

    class TwitterClient
    {
        public function __construct(TransformerInterface $transformer)
        {
            // ...
        }

        // ...
    }

但是现在，该类型约束（``App\Util\TransformerInterface``）不再匹配该服务（
``App\Util\Rot13Transformer``）的id 。这意味着该参数不能再自动装配了。

要解决此问题，请添加一个 :ref:`别名 <service-autowiring-alias>`：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Util\Rot13Transformer: ~

            # 当检测到一个 ``App\Util\TransformerInterface`` 类型约束时，
            # 将注入 ``App\Util\Rot13Transformer`` 服务
            App\Util\TransformerInterface: '@App\Util\Rot13Transformer'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->
                <service id="App\Util\Rot13Transformer"/>

                <service id="App\Util\TransformerInterface" alias="App\Util\Rot13Transformer"/>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Util\Rot13Transformer;
        use App\Util\TransformerInterface;

        // ...
        $container->autowire(Rot13Transformer::class);
        $container->setAlias(TransformerInterface::class, Rot13Transformer::class);

得益于 ``App\Util\TransformerInterface`` 别名，自动装配子系统知道在处理
``TransformerInterface`` 时应该注入 ``App\Util\Rot13Transformer`` 服务。

.. tip::

    使用 `服务定义原型`_ 时，如果只发现一个实现一个接口的服务，
    并且同时也发现该接口，则配置别名不是必需的，Symfony将自动创建一个。

处理相同类型的多个实现
------------------------------------------------------

假设你创建了第二个类 - 实现了 ``TransformerInterface`` 的 ``UppercaseTransformer`` 类::

    namespace App\Util;

    class UppercaseTransformer implements TransformerInterface
    {
        public function transform($value)
        {
            return strtoupper($value);
        }
    }

如果将此类注册为服务，则现在有 *两个* 实现了 ``App\Util\TransformerInterface`` 类型的服务。
自动装配子系统将无法决定使用哪一个服务。请记住，自动装配不是魔术，它只是查找id与类型约束匹配的服务。
因此，你需要通过创建一个对应正确的服务ID的别名来选择一个默认服务（请参阅 :ref:`autowiring-interface-alias`）。
此外，如果要在某些情况下使用一个实现，并在某些其他情况下使用另一个实现，则可以定义多个命名化的自动装配别名。

例如，默认情况下，你可能希望在 ``TransformerInterface`` 接口被类型约束时默认使用
``Rot13Transformer`` 实现，但在某些特定情况下使用 ``UppercaseTransformer`` 实现。
为此，你可以从 ``TransformerInterface`` 接口创建一个普通别名到
``Rot13Transformer``，然后从包含该接口的特殊字符串创建一个
*命名化的自动装配别名*，后跟一个与你在执行注入时使用的变量名称相匹配的变量名称::

    namespace App\Service;

    use App\Util\TransformerInterface;

    class MastodonClient
    {
        private $transformer;

        public function __construct(TransformerInterface $shoutyTransformer)
        {
            $this->transformer = $shoutyTransformer;
        }

        public function toot($user, $key, $status)
        {
            $transformedStatus = $this->transformer->transform($status);

            // ... 连接到Mastodon并发送转换后的状态
        }
    }

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Util\Rot13Transformer: ~
            App\Util\UppercaseTransformer: ~

            # 当检测到 ``App\Util\TransformerInterface``
            # 被类型约束到 ``$shoutyTransformer`` 参数时，
            # 将注入 ``App\Util\UppercaseTransformer`` 服务。
            App\Util\TransformerInterface $shoutyTransformer: '@App\Util\UppercaseTransformer'

            # 如果用于注入的参数不匹配，但类型约束仍然匹配，
            # 则将注入 ``App\Util\Rot13Transformer`` 服务。
            App\Util\TransformerInterface: '@App\Util\Rot13Transformer'

            App\Service\TwitterClient:
                # Rot13Transformer将作为 $transformer 参数传递
                autowire: true

                # 如果要选择非默认服务，并且不想使用命名化的自动装配别名，请手动装配：
                #     $transformer: '@App\Util\UppercaseTransformer'
                # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... -->
                <service id="App\Util\Rot13Transformer"/>
                <service id="App\Util\UppercaseTransformer"/>

                <service id="App\Util\TransformerInterface" alias="App\Util\Rot13Transformer"/>
                <service
                    id="App\Util\TransformerInterface $shoutyTransformer"
                    alias="App\Util\UppercaseTransformer"/>

                <service id="App\Service\TwitterClient" autowire="true">
                    <!-- <argument key="$transformer" type="service" id="App\Util\UppercaseTransformer"/> -->
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Service\MastodonClient;
        use App\Service\TwitterClient;
        use App\Util\Rot13Transformer;
        use App\Util\TransformerInterface;
        use App\Util\UppercaseTransformer;

        // ...
        $container->autowire(Rot13Transformer::class);
        $container->autowire(UppercaseTransformer::class);
        $container->setAlias(TransformerInterface::class, Rot13Transformer::class);
        $container->setAlias(
            TransformerInterface::class.' $shoutyTransformer',
            UppercaseTransformer::class
        );
        $container->autowire(TwitterClient::class)
            //->setArgument('$transformer', new Reference(UppercaseTransformer::class))
        ;
        $container->autowire(MastodonClient::class);

得益于 ``App\Util\TransformerInterface`` 别名，任何使用类型约束此接口的参数都将被传递
``App\Util\Rot13Transformer`` 服务。如果参数已命名为
``$shoutyTransformer``，则将使用 ``App\Util\UppercaseTransformer`` 来替代。
但是，你也可以通过在 ``arguments`` 键下指定参数来手动装配任何 *其他* 服务。

.. versionadded:: 4.2

    Symfony 4.2中引入了命名化自动装配别名。

修复不能自动装配的参数
---------------------------------

自动装配仅在你的参数是一个 *对象* 时有效。
但是如果你有一个标量参数（例如一个字符串），则无法自动装配：Symfony将抛出一个明确的异常。

要解决此问题，你可以 :ref:`手动装配有问题的参数 <services-manually-wire-args>`。
你装配好比较困难的参数部分，Symfony会负责其余的事情。

.. _autowiring-calls:

自动装配的其他方法（例如Setter）
---------------------------------------

为一个服务启用自动装配后，你 *还* 可以将容器配置为在该类实例化时调用它上面的方法。
例如，假设你要注入 ``logger`` 服务，并决定使用setter注入::

    namespace App\Util;

    class Rot13Transformer
    {
        private $logger;

        /**
         * @required
         */
        public function setLogger(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }

        public function transform($value)
        {
            $this->logger->info('Transforming '.$value);
            // ...
        }
    }

自动装配将自动调用 *任何* 在其上方有 ``@required`` 注释的方法，并自动装配每个参数。
如果需要手动将某些参数装配到一个方法，则始终可以显式的 :doc:`配置方法调用 </service_container/calls>`。

控制器动作方法的自动装配
------------------------------------

如果你正在使用Symfony Framework，你还可以为控制器的动作方法自动装配参数。
这是自动装配的特殊情况，是为方便起见而存在。
有关详细信息，请参阅 :ref:`controller-accessing-services`。

性能问题
------------------------

感谢Symfony的编译容器，使用自动装配 *不会* 有性能损失。
但是，在 ``dev`` 环境中可能会有些许的性能损失，因为你修改类时可能会导致容器更频繁地重建。
如果容器重建很慢（可能在非常大的项目中），则可能无法使用自动装配。

公共和可复用Bundle
---------------------------

公共bundle应明确配置其服务，而不是依赖自动装配。

.. _Rapid Application Development: https://en.wikipedia.org/wiki/Rapid_application_development
.. _ROT13: https://en.wikipedia.org/wiki/ROT13
.. _服务定义原型: https://symfony.com/blog/new-in-symfony-3-3-psr-4-based-service-discovery
