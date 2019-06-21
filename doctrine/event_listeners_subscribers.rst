.. index::
   single: Doctrine; Event listeners and subscribers

.. _doctrine-event-config:
.. _how-to-register-event-listeners-and-subscribers:

Doctrine事件监听器和订阅器
========================================

Doctrine包具有丰富的事件系统，系统内发生的几乎任何事情都会触发事件。
对你而言，这意味着你可以创建任意 :doc:`服务 </service_container>`，
并让Doctrine在它内部发生某个特定操作（例如 ``prePersist()``）时通知这些对象。
例如，每当保存数据库中的对象时创建独立的搜索索引，该功能就派上用场了。

Doctrine定义了两种可以监听Doctrine事件的对象：监听器和订阅器。
两者都非常相似，但监听器更直接。有关更多信息，请参阅Doctrine网站上的 `事件系统`_。

该Doctrine网站还罗列了可以监听的所有现有事件。

配置监听器/订阅器
-----------------------------------

要注册服务以充当事件监听器或订阅器，你必须使用适当的名称对其进行 :doc:`标记 </service_container/tags>`。
根据你的用例，你可以将监听器挂钩(hook)到每个DBAL连接和ORM实体管理器，
或者只挂钩到一个特定的DBAL连接以及使用此连接的所有实体管理器。

.. configuration-block::

    .. code-block:: yaml

        services:
            # ...

            App\EventListener\SearchIndexer:
                tags:
                    - { name: doctrine.event_listener, event: postPersist }
            App\EventListener\SearchIndexer2:
                tags:
                    - { name: doctrine.event_listener, event: postPersist, connection: default }
            App\EventListener\SearchIndexerSubscriber:
                tags:
                    - { name: doctrine.event_subscriber, connection: default }

    .. code-block:: xml

        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine">
            <services>
                <!-- ... -->

                <service id="App\EventListener\SearchIndexer">
                    <tag name="doctrine.event_listener" event="postPersist"/>
                </service>
                <service id="App\EventListener\SearchIndexer2">
                    <tag name="doctrine.event_listener" event="postPersist" connection="default"/>
                </service>
                <service id="App\EventListener\SearchIndexerSubscriber">
                    <tag name="doctrine.event_subscriber" connection="default"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        use App\EventListener\SearchIndexer;
        use App\EventListener\SearchIndexer2;
        use App\EventListener\SearchIndexerSubscriber;

        $container->autowire(SearchIndexer::class)
            ->addTag('doctrine.event_listener', ['event' => 'postPersist'])
        ;
        $container->autowire(SearchIndexer2::class)
            ->addTag('doctrine.event_listener', [
                'event' => 'postPersist',
                'connection' => 'default',
            ])
        ;
        $container->autowire(SearchIndexerSubscriber::class)
            ->addTag('doctrine.event_subscriber', ['connection' => 'default'])
        ;

创建监听器类
---------------------------

在前面的示例中，``SearchIndexer`` 服务已配置为 ``postPersist`` 事件上的Doctrine监听器。
该服务背后的类必须有一个 ``postPersist()`` 方法，该方法将在调度事件时调用::

    // src/EventListener/SearchIndexer.php
    namespace App\EventListener;

    use App\Entity\Product;
    // 对于Doctrine < 2.4: use Doctrine\ORM\Event\LifecycleEventArgs;
    use Doctrine\Common\Persistence\Event\LifecycleEventArgs;

    class SearchIndexer
    {
        public function postPersist(LifecycleEventArgs $args)
        {
            $entity = $args->getObject();

            // 只针对 "Product" 实体上的一些动作
            if (!$entity instanceof Product) {
                return;
            }

            $entityManager = $args->getObjectManager();
            // ... 使用 Product 做一些事情
        }
    }

在每个事件中，你都可以访问一个 ``LifecycleEventArgs`` 对象，
该对象使你可以访问事件的实体对象和实体管理器本身。

需要注意的一件重要事情是，监听器将监听应用中的 *所有* 实体。
因此，如果你只对处理特定类型的实体（例如 ``Product`` 实体而非 ``BlogPost`` 实体）感兴趣，
则应在方法中检查实体的类类型（如上所示）。

.. tip::

    在Doctrine 2.4中，引入了一个名为实体监听器的功能。
    它是用于实体的生命周期监听器类。你可以在 `DoctrineBundle文档`_ 中阅读相关内容。

创建订阅器类
-----------------------------

Doctrine事件订阅器必须实现该 ``Doctrine\Common\EventSubscriber`` 接口，
并为其订阅的每个事件都有一个对应事件方法::

    // src/EventListener/SearchIndexerSubscriber.php
    namespace App\EventListener;

    use App\Entity\Product;
    use Doctrine\Common\EventSubscriber;
    // 对于 Doctrine < 2.4: use Doctrine\ORM\Event\LifecycleEventArgs;
    use Doctrine\Common\Persistence\Event\LifecycleEventArgs;
    use Doctrine\ORM\Events;

    class SearchIndexerSubscriber implements EventSubscriber
    {
        public function getSubscribedEvents()
        {
            return [
                Events::postPersist,
                Events::postUpdate,
            ];
        }

        public function postUpdate(LifecycleEventArgs $args)
        {
            $this->index($args);
        }

        public function postPersist(LifecycleEventArgs $args)
        {
            $this->index($args);
        }

        public function index(LifecycleEventArgs $args)
        {
            $entity = $args->getObject();

            // 也许你只想对一些“Product”实体采取动作
            if ($entity instanceof Product) {
                $entityManager = $args->getObjectManager();
                // ... 使用 Product 做一些事情
            }
        }
    }

.. tip::

    Doctrine事件订阅器无法返回灵活的方法数组来调用
    :ref:`Symfony事件订阅器 <event_dispatcher-using-event-subscribers>` 可以调用的事件。
    Doctrine事件订阅者必须返回他们订阅的事件名称的简单数组。
    然后，Doctrine将期望订阅器上的方法与每个订阅事件具有相同的名称，就像使用事件监听器时一样。

有关完整参考，请参阅Doctrine文档中的 `事件系统`_ 一章。

性能的注意事项
--------------------------

监听器和订阅器之间的一个重要区别是Symfony惰性加载实体监听器。
这意味着只有在实际触发相关事件时才从服务容器中获取（并实例化）监听器类。

这就是为什么最好尽可能使用实体监听器而不是订阅器。

事件监听器的优先级
------------------------------

如果你有同一事件的多个监听器，则可以使用标签上的 ``priority`` 属性来控制调用它们的顺序。
优先级使用正整数或负整数来定义（默认为 ``0``）。数字越大意味着更早地调用该监听器。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\EventListener\MyHighPriorityListener:
                tags:
                    - { name: doctrine.event_listener, event: postPersist, priority: 10 }

            App\EventListener\MyLowPriorityListener:
                tags:
                    - { name: doctrine.event_listener, event: postPersist, priority: 1 }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine">

            <services>
                <service id="App\EventListener\MyHighPriorityListener" autowire="true">
                    <tag name="doctrine.event_listener" event="postPersist" priority="10"/>
                </service>
                <service id="App\EventListener\MyLowPriorityListener" autowire="true">
                    <tag name="doctrine.event_listener" event="postPersist" priority="1"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\EventListener\MyHighPriorityListener;
        use App\EventListener\MyLowPriorityListener;

        $container
            ->autowire(MyHighPriorityListener::class)
            ->addTag('doctrine.event_listener', ['event' => 'postPersist', 'priority' => 10])
        ;

        $container
            ->autowire(MyLowPriorityListener::class)
            ->addTag('doctrine.event_listener', ['event' => 'postPersist', 'priority' => 1])
        ;

.. _`事件系统`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/events.html
.. _`DoctrineBundle文档`: https://symfony.com/doc/current/bundles/DoctrineBundle/entity-listeners.html
