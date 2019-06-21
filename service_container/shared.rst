.. index::
    single: Service Container; Shared Services

如何定义非共享服务
=================================

在服务容器中，默认情况下共享所有服务。这意味着每次检索该服务时，你都将获得 *同一个* 实例。
这通常是你想要的行为，但在某些情况下，你可能希望始终获得一个 *新* 实例。

要始终获得新实例，请在服务定义中将 ``shared`` 设置为 ``false``：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\SomeNonSharedService:
                shared: false
                # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <services>
            <service id="App\SomeNonSharedService" shared="false"/>
        </services>

    .. code-block:: php

        // config/services.php
        use App\SomeNonSharedService;

        $container->register(SomeNonSharedService::class)
            ->setShared(false);

现在，无论何时从容器请求 ``App\SomeNonSharedService`` ，你都将被传递一个新实例。
