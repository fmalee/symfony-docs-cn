.. index::
    single: Doctrine; Lifecycle Callbacks

如何使用生命周期回调
====================================

有时，你需要在插入、更新或删除实体之前或之后执行操作。
这些类型的操作称为“生命周期”回调，
因为它们是你需要在实体生命周期的不同阶段执行的回调方法（例如插入、更新、删除实体等）。

如果你正在为元数据使用注释，请首先启用生命周期回调。如果你使用YAML或XML进行映射，则不需要这样做。

.. code-block:: php-annotations

    /**
     * @ORM\Entity()
     * @ORM\HasLifecycleCallbacks()
     */
    class Product
    {
        // ...
    }

现在，你可以在任何可用的生命周期事件上告诉Doctrine执行一个方法。
例如，假设你要仅在实体首次持久化（即插入）时，将 ``createdAt`` 日期列设置为当前日期：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Product.php

        /**
         * @ORM\PrePersist
         */
        public function setCreatedAtValue()
        {
            $this->createdAt = new \DateTime();
        }

    .. code-block:: yaml

        # config/doctrine/Product.orm.yml
        App\Entity\Product:
            type: entity
            # ...
            lifecycleCallbacks:
                prePersist: [setCreatedAtValue]

    .. code-block:: xml

        <!-- config/doctrine/Product.orm.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

            <entity name="App\Entity\Product">
                <!-- ... -->
                <lifecycle-callbacks>
                    <lifecycle-callback type="prePersist" method="setCreatedAtValue" />
                </lifecycle-callbacks>
            </entity>
        </doctrine-mapping>

.. note::

    上面的示例假定你已创建并映射了一个 ``createdAt`` 属性（此处未显示）。

现在，在实体首次持久化之前，Doctrine将自动调用此方法，该 ``createdAt`` 字段将设置为当前日期。

你可以使用其他几个生命周期事件。
有关其他生命周期事件和生命周期回调的更多信息，请参阅 `生命周期事件文档`_。

.. sidebar:: 生命周期回调和事件监听器

    请注意， ``setCreatedAtValue()`` 方法不接收任何参数。
    生命周期回调始终是这种情况并且是有意的：
    生命周期回调应该是与实体中的内部转换(transforming)数据有关的简单方法（例如，设置一个创建/更新的字段、生成一个slug值）。

    如果你需要做一些更繁重的操作 - 比如记录日志或发送电子邮件 -
    你应该注册一个外部类为事件监听器或订阅器，并让它访问你需要的任何资源。
    有关更多信息，请参阅 :doc:`/doctrine/event_listeners_subscribers`。

.. _`生命周期事件文档`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/events.html#lifecycle-events
