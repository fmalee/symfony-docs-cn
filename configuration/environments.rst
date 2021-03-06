.. index::
    single: Environments

如何掌握和创建新环境
=========================================

每个应用都是代码和一组配置的组合，这些配置决定了代码应该如何运行。
配置可以定义正在使用的数据库，是否应该缓存某些内容或者应该如何详细记录日志。

在Symfony中，“环境”的概念是可以使用多种不同配置运行相同的代码库。
例如，``dev`` 环境应该使用使开发变得简单和友好的配置，而 ``prod`` 环境应该使用一组针对速度优化过的配置。

.. index::
   single: Environments; Configuration files

不同的环境，不同的配置文件
-----------------------------------------------------

一个典型的Symfony应用始于三种环境：``dev``、``prod`` 和 ``test``。
如上所述，每个环境都代表了一种使用不同配置来执行同一代码库的方法。
因此，每个环境都加载它自己的配置文件应该不足为奇。这些不同的文件一般按环境来组织：

* 对于 ``dev`` 环境: ``config/packages/dev/``
* 对于 ``prod`` 环境: ``config/packages/prod/``
* 对于 ``test`` 环境: ``config/packages/test/``

实际上，每个环境都只是与其他环境略有不同。这意味着所有环境都共享大量常见配置。
此配置直接放在 ``config/packages/`` 目录中的文件中。

这些文件的位置由应用的内核定义::

    // src/Kernel.php

    // ...
    class Kernel extends BaseKernel
    {
        // ...

        protected function configureContainer(ContainerBuilder $container, LoaderInterface $loader)
        {
            // ...
            $confDir = $this->getProjectDir().'/config';

            // 始终加载在 /config/packages/ 中的所有文件
            $loader->load($confDir.'/packages/*'.self::CONFIG_EXTS, 'glob');

            // 然后，加载在对应环境目录用的文件，如果环境可用的话。
            if (is_dir($confDir.'/packages/'.$this->environment)) {
                $loader->load($confDir.'/packages/'.$this->environment.'/**/*'.self::CONFIG_EXTS, 'glob');
            }

            // 加载一个特定的 services(yaml/xml/php) 文件，
            // 如果可用的话，同时加载 services_ENVIRONMENT.(yaml/xml/php) 文件
            $loader->load($confDir.'/services'.self::CONFIG_EXTS, 'glob');
            $loader->load($confDir.'/services_'.$this->environment.self::CONFIG_EXTS, 'glob');
        }
    }

以默认安装的框架包为例：

* Loaded in all environments, ``config/packages/framework.yaml`` configures the
  framework with some ``secret`` setting;
* In the **prod** environment, nothing extra will be set as there is no
  ``config/packages/prod/`` directory;
* The same applies to **dev**, as there is no
  ``config/packages/dev/framework.yaml``. There are however other packages (e.g.
  ``routing.yaml``) with special dev settings;
* At last, during the **test** environment, the framework's test features are
  enabled in ``config/packages/test/framework.yaml``.

.. index::
   single: Environments; Executing different environments

Executing an Application in different Environments
--------------------------------------------------

To execute the application in each environment, change the ``APP_ENV``
environment variable. During development, this is done in ``.env`` or in ``.env.local``:

.. code-block:: bash

    # .env or .env.local
    APP_ENV=dev

    # or for test:
    #APP_ENV=test

Visit the ``http://localhost:8000/index.php`` page in your web browser to see
your application in the configured environment.

.. tip::

    In production, you can use real environment variables via
    your :ref:`web server configuration <configuration-env-var-in-prod>`.

.. note::

    The given URLs assume that your web server is configured to use the ``public/``
    directory of the application as its root. Read more in :doc:`Installing Symfony </setup>`.

If you open the file you just visited (``public/index.php``), you'll see that
the environment variable is passed to the kernel::

    // public/index.php

    // ...
    $kernel = new Kernel($_SERVER['APP_ENV'], $_SERVER['APP_DEBUG']);

    // ...

.. note::

    The ``test`` environment is used when writing functional tests and is
    usually not accessed in the browser directly via a front controller.

.. index::
   single: Configuration; Debug mode

.. sidebar:: *Debug* Mode

    Important, but unrelated to the topic of *environments* is the second
    argument to the ``Kernel`` constructor. This specifies if the application
    should run in "debug mode". Regardless of the environment, a Symfony
    application can be run with debug mode set to ``true`` or ``false``
    (respectively ``1`` or ``0`` for the ``APP_DEBUG`` variable defined in
    ``.env``). This affects many things in the application, such as displaying
    stacktraces on error pages or if cache files are dynamically rebuilt on
    each request.  Though not a requirement, debug mode is generally set to
    ``true`` for the ``dev`` and ``test`` environments and ``false`` for the
    ``prod`` environment.

    Internally, the value of the debug mode becomes the ``kernel.debug``
    parameter used inside the :doc:`service container </service_container>`.
    If you look inside the application configuration file, you'll see the
    parameter used, for example, to turn Twig's debug mode on:

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/twig.yaml
            twig:
                debug: '%kernel.debug%'

        .. code-block:: xml

            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/twig
                    http://symfony.com/schema/dic/twig/twig-1.0.xsd">

                <twig:config debug="%kernel.debug%" />

            </container>

        .. code-block:: php

            $container->loadFromExtension('twig', array(
                'debug' => '%kernel.debug%',
                // ...
            ));

Selecting the Environment for Console Commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, Symfony commands are executed in whatever environment is defined by
the ``APP_ENV`` environment variable (usually configured in your ``.env`` file).
In previous Symfony versions you could use the ``--env`` (and ``--no-debug``)
command line options to override this value. However, those options were
deprecated in Symfony 4.2.

Use the ``APP_ENV`` (and ``APP_DEBUG``) environment variables to change the
environment and the debug behavior of the commands:

.. code-block:: terminal

    # Symfony's default: 'dev' environment and debug enabled
    $ php bin/console command_name

    # 'prod' environment (debug is always disabled for 'prod')
    $ APP_ENV=prod php bin/console command_name

    # 'test' environment and debug disabled
    $ APP_ENV=test APP_DEBUG=0 php bin/console command_name

.. index::
   single: Environments; Creating a new environment

Creating a new Environment
--------------------------

Since an environment is nothing more than a string that corresponds to a set of
configuration, you can also create your own environments for specific purposes.

Suppose, for example, that before deployment, you need to benchmark your
application. One way to benchmark the application is to use near-production
settings, but with Symfony's ``web_profiler`` enabled. This allows Symfony
to record information about your application while benchmarking.

The best way to accomplish this is via a new environment called, for example,
``benchmark``. Start by creating a new configuration directory and a
configuration file:

.. configuration-block::

    .. code-block:: yaml

        # config/packages/benchmark/web_profiler.yaml
        framework:
            profiler: { only_exceptions: false }

    .. code-block:: xml

        <!-- config/packages/benchmark/web_profiler.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:profiler only-exceptions="false" />
            </framework:config>

        </container>

    .. code-block:: php

        // config/packages/benchmark/web_profiler.php
        $container->loadFromExtension('framework', array(
            'profiler' => array('only_exceptions' => false),
        ));

And... you're finished! The application now supports a new environment called
``benchmark``.

Change the ``APP_ENV`` variable to ``benchmark`` to be able to access the new
environment through your browser:

.. code-block:: bash

    # .env or .env.local
    APP_ENV=benchmark

.. sidebar:: Importing configuration

    Besides loading files in the Kernel, you can also import files in the
    configuration directly. For instance, to make sure the benchmark
    environment is identical to the prod environment, you might want to load
    all its configuration as well.

    You can achieve this by using a special ``imports`` key:

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/benchmark/other.yaml
            imports:
                - { resource: '../prod/' }

                # other resources are possible as well, like importing other
                # files or using globs:
                #- { resource: '/etc/myapp/some_special_config.xml' }
                #- { resource: '/etc/myapp/*.yaml' }

        .. code-block:: xml

            <!-- config/packages/benchmark/other.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/symfony
                    http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <imports>
                    <import resource="../prod/"/>

                    <!-- other resources are possible as well, like importing other
                         files or using globs:
                    <import resource="/etc/myapp/some_special_config.yaml"/>
                    <import resource="/etc/myapp/*.xml"/>
                    -->
                </imports>

            </container>

        .. code-block:: php

            // config/packages/benchmark/other.php
            $loader->import('../prod/');

            // other resources are possible as well, like importing other
            // files or using globs:
            //$loader->import('/etc/myapp/some_special_config.yaml');
            //$loader->import('/etc/myapp/*.php');

.. index::
   single: Environments; Cache directory

Environments and the Cache Directory
------------------------------------

Symfony takes advantage of caching in many ways: the application configuration,
routing configuration, Twig templates and more are cached to PHP objects
stored in files on the filesystem.

By default, these cached files are largely stored in the ``var/cache/`` directory.
However, each environment caches its own set of files:

.. code-block:: text

    your-project/
    ├─ var/
    │  ├─ cache/
    │  │  ├─ dev/   # cache directory for the *dev* environment
    │  │  └─ prod/  # cache directory for the *prod* environment
    │  ├─ ...

Sometimes, when debugging, it may be helpful to inspect a cached file to
understand how something is working. When doing so, remember to look in
the directory of the environment you're using (most commonly ``dev/`` while
developing and debugging). While it can vary, the ``var/cache/dev/`` directory
includes the following:

``appDevDebugProjectContainer.php``
    The cached "service container" that represents the cached application
    configuration.

``appDevUrlGenerator.php``
    The PHP class generated from the routing configuration and used when
    generating URLs.

``appDevUrlMatcher.php``
    The PHP class used for route matching - look here to see the compiled regular
    expression logic used to match incoming URLs to different routes.

``twig/``
    This directory contains all the cached Twig templates.

.. note::

    You can change the directory location and name. For more information
    read the article :doc:`/configuration/override_dir_structure`.

Going further
-------------

Read the article on :doc:`/configuration/external_parameters`.
