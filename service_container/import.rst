.. index::
    single: DependencyInjection; Importing Resources
    single: Service Container; Importing Resources

如何导入配置文件/资源
===========================================

.. tip::

    在本节中，服务配置文件被称为 *资源*。
    虽然大多数配置资源都是文件（例如YAML，XML，PHP），
    但Symfony能够从任何地方（例如数据库甚至通过外部Web服务）加载配置。

服务容器使用单个配置资源（默认为 ``config/services.yaml``）来进行构建。
这为你的应用中的服务提供了绝对的灵活性。

可以通过两种不同的方式导入外部服务配置。
第一种方法，使用 ``imports`` 指令，通常用于导入其他资源。
第二种方法，使用依赖注入扩展，用于从第三方bundel加载配置。
继续阅读以了解有关这两种方法的更多信息。

.. index::
    single: Service Container; Imports

.. _service-container-imports-directive:

使用 ``imports`` 导入配置
----------------------------------------

默认情况下，服务配置存在于 ``config/services.yaml``。
但是如果该文件越变越大，你可以自由组织成多个文件。假设你决定将某些配置移动到一个新文件：

.. configuration-block::

    .. code-block:: yaml

        # config/services/mailer.yaml
        parameters:
            # ... 一些参数

        services:
            # ... 一些服务

    .. code-block:: xml

        <!-- config/services/mailer.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <!-- ... some parameters -->
            </parameters>

            <services>
                <!-- ... some services -->
            </services>
        </container>

    .. code-block:: php

        // config/services/mailer.php

        // ... some parameters
        // ... some services

要导入此文件，可以通过使用 ``imports`` 键来加载它：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        imports:
            - { resource: services/mailer.yaml }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <imports>
                <import resource="services/mailer.xml"/>
            </imports>
        </container>

    .. code-block:: php

        // config/services.php
        $loader->import('services/mailer.php');

``resource`` 用于定位文件位置，可以使相对于当前文件的相对路径，也可以是一个绝对路径。

.. include:: /components/dependency_injection/_imports-parameters-note.rst.inc

.. index::
    single: Service Container; Extension configuration

.. _service-container-extension-configuration:

通过容器扩展导入配置
------------------------------------------------

第三方bundle的容器配置（包括Symfony核心服务）通常使用另一种方法来加载：
一个 :doc:`容器扩展 </bundles/extension>`。

在内部，每个bundle都在你目前所见的文件中定义其服务。
但是，不使用 ``import`` 指令导入这些文件。相反，bundle使用一个 *依赖注入扩展* 来自动加载该文件。
一旦启用了一个bundle，就会调用它的扩展，而该扩展可以加载服​​务配置文件。

实际上，在 ``config/packages/`` 中的每个配置文件都会被传递到其相关bundle的扩展中 -
例如 ``FrameworkBundle`` 或 ``TwigBundle`` - 并用于进一步配置这些服务。
