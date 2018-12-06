.. index::
   single: Dependency Injection; Lazy Services

延迟服务
=============

.. seealso::

    另一种延迟注入服务的方法是通过
    :doc:`服务订阅器 </service_container/service_subscribers_locators>`。

为何选择延迟服务？
------------------

在某些情况下，你可能希望注入一个实例化有些繁杂的服务，但又并不总是在对象中使用它。
例如，假设你有一个 ``NewsletterManager``，并且将一个 ``mailer`` 服务注入其中。
实际上，在 ``NewsletterManager`` 中只有几个方法使用了 ``mailer``，
即使你不需要使用 ``mailer``，系统也总是会实例化该服务来构建你的 ``NewsletterManager``。

配置延迟服务就是一个解决方案。
使用延迟服务，实际上会注入 ``mailer`` 服务的一个“代理”。
它外观和行为很像 ``mailer`` ，但在你以某种方式与该代理交互之前，真实的 ``mailer`` 并没有被实例化。

安装
------------

要使用延迟服务实例化，你需要安装 ``symfony/proxy-manager-bridge`` 包：

.. code-block:: terminal

    $ composer require symfony/proxy-manager-bridge

配置
-------------

现在，你可以通过操作其定义来标记该服务为 ``lazy``：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Twig\AppExtension:
                lazy:  true

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\Twig\AppExtension" lazy="true" />
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Twig\AppExtension;

        $container->register(AppExtension::class)
            ->setLazy(true);

将该服务注入另一个服务后，会注入一个具有相同类签名的虚拟 `代理`_ 来代表该服务。
直接调用 ``Container::get()`` 也是如此。

一旦你尝试与该服务交互（例如，调用其中一个方法），实际的类将被实例化。

要检查代理是否有效，你可以检查收到的对象的接口::

    dump(class_implements($service));
    // 输出将包含 "ProxyManager\Proxy\LazyLoadingInterface"

.. note::

    如果未安装 `ProxyManager bridge`_ 和 `ocramius/proxy-manager`_，
    容器将跳过 ``lazy`` 标签并像往常那样直接实例化该服务。

其他资源
--------------------

你可以在 `ProxyManager文档`_ 中阅读有关如何实例化、生成和初始化代理的更多信息。

.. _`ProxyManager bridge`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bridge/ProxyManager
.. _`代理`: https://en.wikipedia.org/wiki/Proxy_pattern
.. _`ProxyManager文档`: https://github.com/Ocramius/ProxyManager/blob/master/docs/lazy-loading-value-holder.md
.. _`ocramius/proxy-manager`: https://github.com/Ocramius/ProxyManager
