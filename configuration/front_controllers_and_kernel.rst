.. index::
    single: How the front controller, ``Kernel`` and environments
    work together

了解前端控制器、内核和环境如何协同工作
=============================================================================

:ref:`配置环境 <configuration-environments>`
一文介绍了Symfony如何使用环境来运行具有不同配置设置的应用的基础知识。
本节将更深入地解释当你的应用启动(bootstrapped)时会发生什么。
要深入了解这个过程，你需要了解三个可以协同工作的部分：

* `前端控制器`_
* `Kernel类`_
* `环境`_

.. note::

    通常，你不需要定义自己的前端控制器或 ``Kernel`` 类，因为Symfony提供了合理的默认实现。
    本文旨在解释幕后发生的事情。

前端控制器
--------------------

前端控制器是一个设计 `模式`_; 它是一个应用中供所有请求通过的代码的一部分。

在Symfony Skeleton中，此角色由 ``public/`` 目录中的 ``index.php`` 文件执行。
这是处理请求时执行的第一个PHP脚本。

前端控制器的主要目的是创建一个 ``Kernel`` 实例（更多是这一点），使其处理请求并将结果响应返回给浏览器。

因为每个请求都通过它进行路由，所以前端控制器可用于在设置内核之前执行全局初始化或使用其他功能来 `装饰`_ 内核。
例子包括：

* 配置自动加载器或添加其他自动加载机制;
* 通过使用 :ref:`HttpCache <symfony-gateway-cache>` 封装内核来添加HTTP级别的缓存;
* 启用 :doc:`Debug组件 </components/debug>`。

你可以通过在URL中添加前端控制器来选择使用的前端控制器，例如：

.. code-block:: text

     http://localhost/index.php/some/path/...

如你所见，此URL包含要用作前端控制器的PHP脚本。
你可以使用它切换到位于 ``public/`` 目录中的自定义前端控制器。

.. seealso::

    你似乎永远不想在URL中显示前端控制器。
    这可以通过配置web服务器来实现，如 :doc:`/setup/web_server_configuration` 的实例。

从技术上讲，在命令行上运行Symfony时使用的 ``bin/console`` 脚本也是前端控制器。
但该控制器仅用于命令行请求，而不用于Web。

Kernel类
----------------

:class:`Symfony\\Component\\HttpKernel\\Kernel` 是Symfony的核心。
它负责设置应用使用的所有软件包，并为它们提供应用的配置。
然后，它在 :method:`Symfony\\Component\\HttpKernel\\HttpKernelInterface::handle`
方法服务请求之前创建服务容器。

Symfony应用中使用的内核继承自 :class:`Symfony\\Component\\HttpKernel\\Kernel`
并使用 :class:`Symfony\\Bundle\\FrameworkBundle\\Kernel\\MicroKernelTrait`。
``Kernel`` 类留下了 :class:`Symfony\\Component\\HttpKernel\\KernelInterface` 的一些方法未实现，
并且 ``MicroKernelTrait`` 定义了几个抽象方法，所以你必须实现所有这些：

:method:`Symfony\\Component\\HttpKernel\\KernelInterface::registerBundles`
    它必须返回运行应用所需的所有包的数组。

:method:`Symfony\\Bundle\\FrameworkBundle\\Kernel\\MicroKernelTrait::configureRoutes`
    它向应用添加单独的路由或路由集合（例如，加载某些配置文件中定义的路由）。

:method:`Symfony\\Bundle\\FrameworkBundle\\Kernel\\MicroKernelTrait::configureContainer`
    它从配置文件或使用``loadFromExtension()`` 方法加载应用配置，还可以注册新的容器参数和服务。

要填充这些（小）空白，你的应用需要继承Kernel类并使用MicroKernelTrait来实现这些方法。
Symfony默认提供的内核位于 ``src/Kernel.php`` 文件。

此类使用环境的名称 - 该名称将传递给内核的
:method:`constructor <Symfony\\Component\\HttpKernel\\Kernel::__construct>`
方法并可通过 :method:`Symfony\\Component\\HttpKernel\\Kernel::getEnvironment`
获得 - 来决定启用哪些包。

你可以自由创建自己的、替代的或其他 ``Kernel`` 变体。
你所需要的只是调整你的（或添加一个新的）前端控制器以使用新内核。

.. note::

    ``Kernel`` 的名称和位置不固定。当将 :doc:`多个内核继承到一个单一的应用 </configuration/multiple_kernels>`，
    它可能因此意义而添加额外的子目录，例如 ``src/admin/AdminKernel.php`` 和 ``src/api/ApiKernel.php``。
    重要的是你的前端控制器能够创建适当的内核实例。

.. note::

    ``Kernel`` 可以提供更多内容，例如 :doc:`重写默认目录结构 </configuration/override_dir_structure>`。
    但是，通过实施多个 ``Kernel`` 实现，你无需动态更改此类内容的可能性很高。

.. index::
   single: Configuration; Debug mode

调试模式
~~~~~~~~~~

``Kernel`` 构造函数的第二个参数指定应用是否应以“调试模式”运行。无论
:ref:`配置环境 <configuration-environments>` 如何，Symfony应用都可以在调试模式设置为
``true`` 或 ``false`` 的情况下运行。

这会影响应用中的许多内容，例如在错误页面上显示堆栈跟踪，或者是否在每个请求上动态的重建缓存文件。
虽然不是必需的，但调试模式通常在 ``dev`` 和 ``test`` 环境下被设置为
``true``，而 ``prod`` 环境下则为 ``false``。

与 :ref:`配置环境 <selecting-the-active-environment>` 类似，你也可以使用
:ref:`.env文件 <config-dot-env>` 来启用/禁用调试模式：

.. code-block:: bash

    # .env
    # 将其设置为1以启用调试模式
    APP_DEBUG=0

此值可以在运行命令之前通过传递 ``APP_DEBUG`` 值来进行重写：

.. code-block:: terminal

    # 使用 .env 文件中定义的调试模式
    $ php bin/console command_name

    # 忽略 .env 文件并为此命令启用调试模式
    $ APP_DEBUG=1 php bin/console command_name

在内部，调试模式的值将成为 :doc:`服务容器 </service_container>` 内部使用的 ``kernel.debug``
参数。如果你查看应用的配置文件，你将看到所使用的参数，例如，打开Twig的调试模式：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            debug: '%kernel.debug%'

    .. code-block:: xml

        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config debug="%kernel.debug%"/>

        </container>

    .. code-block:: php

        $container->loadFromExtension('twig', [
            'debug' => '%kernel.debug%',
            // ...
        ]);

环境
----------------

如上所述，``Kernel`` 必须实现另一种方法 -  :method:`Symfony\\Bundle\\FrameworkBundle\\Kernel\\MicroKernelTrait::configureContainer`。
此方法负责从正确的 *环境* 加载应用的配置。

:ref:`配置环境 <configuration-environments>` 允许使用不同的配置来执行相同的代码。
Symfony默认提供三个环境，分别叫做 ``dev``、``prod`` 和 ``test``。

从技术上讲，这些名称只不过是从前端控制器传递给 ``Kernel`` 的构造函数的字符串。
然后可以在 ``configureContainer()`` 方法中使用此名称来确定要加载的配置文件。

Symfony的默认 ``Kernel`` 类通过首先加载在 ``config/packages/*`` 中找到的配置文件来实现该方法，
然后该文件在 ``config/packages/ENVIRONMENT_NAME/`` 中被找到。
如果你需要更复杂的加载配置的方式，那么你可以自由地以不同方式实现此方法。

.. index::
   single: Environments; Cache directory

环境和缓存目录
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Symfony以多种方式利用缓存：应用配置、路由配置、Twig模板等都缓存到存储在文件系统的文件中的PHP对象中。

默认情况下，这些缓存的文件主要存储在 ``var/cache/`` 目录中。但是，每个环境都会缓存自己的文件集：

.. code-block:: text

    your-project/
    ├─ var/
    │  ├─ cache/
    │  │  ├─ dev/   # *dev* 环境的缓存目录
    │  │  └─ prod/  # *prod* 环境的缓存目录
    │  ├─ ...

有时，检查缓存文件以了解某些内容是如何工作的可能会对调试很有帮助。
这样做时，请记住查看你正在使用的环境的对应目录（最常见的是在开发和调试时的
``dev/``）。虽然它可以变化，但 ``var/cache/dev/`` 目录会包含以下内容：

``appDevDebugProjectContainer.php``
    “服务容器”的缓存，表示缓存的应用配置。

``appDevUrlGenerator.php``
    从路由配置生成的PHP类，并在生成URL时使用。

``appDevUrlMatcher.php``
    用于路由匹配的PHP类 - 在这里查看用于匹配传入URL到不同路由的已编译的正则表达式逻辑。

``twig/``
    该目录包含所有Twig模板的缓存。

.. note::

    你可以更改缓存目录位置和名称。有关更多信息，请阅读
    :doc:`/configuration/override_dir_structure` 一文。

.. _`模式`: https://en.wikipedia.org/wiki/Front_Controller_pattern
.. _`装饰`: https://en.wikipedia.org/wiki/Decorator_pattern
