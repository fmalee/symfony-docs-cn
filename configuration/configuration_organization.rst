.. index::
    single: Configuration

如何组织配置文件
===================================

Symfony骨架定义了三个 :doc:`执行环境 </configuration/environments>`
叫 ``dev``、``prod`` 和 ``test``。环境表示使用不同配置执行相同代码库的方法。

为了选择配置文件来加载每个环境，
Symfony执行 ``Kernel`` 类的 ``configureContainer()`` 方法::

    // src/Kernel.php
    use Symfony\Component\Config\Loader\LoaderInterface;
    use Symfony\Component\DependencyInjection\ContainerBuilder;
    use Symfony\Component\HttpKernel\Kernel as BaseKernel;

    class Kernel extends BaseKernel
    {
        const CONFIG_EXTS = '.{php,xml,yaml,yml}';

        // ...

        public function configureContainer(ContainerBuilder $container, LoaderInterface $loader)
        {
            $confDir = $this->getProjectDir().'/config';
            $loader->load($confDir.'/packages/*'.self::CONFIG_EXTS, 'glob');
            if (is_dir($confDir.'/packages/'.$this->environment)) {
                $loader->load($confDir.'/packages/'.$this->environment.'/**/*'.self::CONFIG_EXTS, 'glob');
            }
            $loader->load($confDir.'/services'.self::CONFIG_EXTS, 'glob');
            $loader->load($confDir.'/services_'.$this->environment.self::CONFIG_EXTS, 'glob');
        }
    }

对于 ``dev`` 环境，Symfony按以下顺序加载以下配置文件和目录：

#. ``config/packages/*``
#. ``config/packages/dev/*``
#.  ``config/services.yaml``
#. ``config/services_dev.yaml``

因此，默认Symfony应用的配置文件遵循以下结构：

.. code-block:: text

    your-project/
    ├─ config/
    │  └─ packages/
    │     ├─ dev/
    |     │  ├─ framework.yaml
    │     │  └─ ...
    │     ├─ prod/
    │     │  └─ ...
    │     ├─ test/
    │     │  └─ ...
    |     ├─ framework.yaml
    │     └─ ...
    │     ├─ services.yaml
    │     └─ services_dev.yaml
    ├─ ...

选择此默认结构是为了简化 - 每个软件包和环境一个文件。
但与任何其他Symfony功能一样，你可以自定义它以更好地满足你的需求。

高级技巧
-------------------

Symfony使用 :doc:`Config组件 </components/config>` 来加载配置文件，该组件提供了一些高级功能。

混合和匹配配置格式
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

配置文件可以导入任何其他内置配置格式定义的文件
（``.yaml`` 或 ``.yml``、``.xml``、``.php``、``.ini``）：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        imports:
            - { resource: 'my_config_file.xml' }
            - { resource: 'legacy.php' }

        # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="my_config_file.yaml" />
                <import resource="legacy.php" />
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // config/services.php
        $loader->import('my_config_file.yaml');
        $loader->import('legacy.xml');

        // ...

如果使用任何其他配置格式，则必须定义自己的继承自
:class:`Symfony\\Component\\DependencyInjection\\Loader\\FileLoader` 的加载器类。
当配置值是动态时，你可以使用PHP配置文件来执行你自己的逻辑。
此外，你可以定义自己的服务以从数据库或Web服务加载配置。

全局配置文件
~~~~~~~~~~~~~~~~~~~~~~~~~~

某些系统管理员可能更喜欢将敏感参数存储在项目目录之外的文件中。
想象一下，你网站的数据库凭据存储在 ``/etc/sites/mysite.com/parameters.yaml`` 文件中。
通过在从任何其他配置文件导入时指示完整的文件路径，可以从项目文件夹外部加载文件：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        imports:
            - { resource: '/etc/sites/mysite.com/parameters.yaml', ignore_errors: true }

        # ...

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <imports>
                <import resource="/etc/sites/mysite.com/parameters.yaml" ignore-errors="true" />
            </imports>

            <!-- ... -->
        </container>

    .. code-block:: php

        // config/services.php
        $loader->import('/etc/sites/mysite.com/parameters.yaml', null, true);

        // ...

.. tip::

    ``ignore_errors`` 选项（这是加载器的 ``import()`` 方法的第三个可选的参数）
    将忽略加载的文件不存在的错误。
    在这个例子中需要这样做，因为大多数情况下，本地开发人员不会拥有与生产服务器上保存的文件相同的文件。

如你所见，有很多方法可以组织配置文件。
你可以选择其中一种，甚至可以创建自己的自定义方式来组织文件。
有关更多自定义，请参阅 ":doc:`/configuration/override_dir_structure`"。
