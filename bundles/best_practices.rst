.. index::
   single: Bundle; Best practices

可重用Bundle的最佳实践
===================================

This article is all about how to structure your **reusable bundles** so that
they're easy to configure and extend. Reusable bundles are those meant to be
shared privately across many company projects or publicly so any Symfony project
can install them.

.. index::
   pair: Bundle; Naming conventions

.. _bundles-naming-conventions:

Bundle Name
-----------

A bundle is also a PHP namespace. The namespace must follow the `PSR-4`_
interoperability standard for PHP namespaces and class names: it starts with a
vendor segment, followed by zero or more category segments, and it ends with the
namespace short name, which must end with ``Bundle``.

A namespace becomes a bundle as soon as you add a bundle class to it. The
bundle class name must follow these rules:

* Use only alphanumeric characters and underscores;
* Use a StudlyCaps name (i.e. camelCase with the first letter uppercased);
* Use a descriptive and short name (no more than two words);
* Prefix the name with the concatenation of the vendor (and optionally the
  category namespaces);
* Suffix the name with ``Bundle``.

Here are some valid bundle namespaces and class names:

==========================  ==================
Namespace                   Bundle Class Name
==========================  ==================
``Acme\Bundle\BlogBundle``  AcmeBlogBundle
``Acme\BlogBundle``         AcmeBlogBundle
==========================  ==================

By convention, the ``getName()`` method of the bundle class should return the
class name.

.. note::

    If you share your bundle publicly, you must use the bundle class name as
    the name of the repository (AcmeBlogBundle and not BlogBundle for instance).

.. note::

    Symfony core Bundles do not prefix the Bundle class with ``Symfony``
    and always add a ``Bundle`` sub-namespace; for example:
    :class:`Symfony\\Bundle\\FrameworkBundle\\FrameworkBundle`.

Each bundle has an alias, which is the lower-cased short version of the bundle
name using underscores (``acme_blog`` for AcmeBlogBundle). This alias
is used to enforce uniqueness within a project and for defining bundle's
configuration options (see below for some usage examples).

Directory Structure
-------------------

The basic directory structure of an AcmeBlogBundle must read as follows:

.. code-block:: text

    <your-bundle>/
    ├─ AcmeBlogBundle.php
    ├─ Controller/
    ├─ README.md
    ├─ LICENSE
    ├─ Resources/
    │   ├─ config/
    │   ├─ doc/
    │   │  └─ index.rst
    │   ├─ translations/
    │   ├─ views/
    │   └─ public/
    └─ Tests/

**The following files are mandatory**, because they ensure a structure convention
that automated tools can rely on:

* ``AcmeBlogBundle.php``: This is the class that transforms a plain directory
  into a Symfony bundle (change this to your bundle's name);
* ``README.md``: This file contains the basic description of the bundle and it
  usually shows some basic examples and links to its full documentation (it
  can use any of the markup formats supported by GitHub, such as ``README.rst``);
* ``LICENSE``: The full contents of the license used by the code. Most third-party
  bundles are published under the MIT license, but you can `choose any license`_;
* ``Resources/doc/index.rst``: The root file for the Bundle documentation.

The depth of subdirectories should be kept to a minimum for the most used
classes and files. Two levels is the maximum.

The bundle directory is read-only. If you need to write temporary files, store
them under the ``cache/`` or ``log/`` directory of the host application. Tools
can generate files in the bundle directory structure, but only if the generated
files are going to be part of the repository.

The following classes and files have specific emplacements (some are mandatory
and others are just conventions followed by most developers):

===================================================  ========================================
Type                                                 Directory
===================================================  ========================================
Commands                                             ``Command/``
Controllers                                          ``Controller/``
Service Container Extensions                         ``DependencyInjection/``
Doctrine ORM entities (when not using annotations)   ``Entity/``
Doctrine ODM documents (when not using annotations)  ``Document/``
Event Listeners                                      ``EventListener/``
Configuration (routes, services, etc.)               ``Resources/config/``
Web Assets (CSS, JS, images)                         ``Resources/public/``
Translation files                                    ``Resources/translations/``
Validation (when not using annotations)              ``Resources/config/validation/``
Serialization (when not using annotations)           ``Resources/config/serialization/``
Templates                                            ``Resources/views/``
Unit and Functional Tests                            ``Tests/``
===================================================  ========================================

Classes
-------

The bundle directory structure is used as the namespace hierarchy. For
instance, a ``ContentController`` controller which is stored in
``Acme/BlogBundle/Controller/ContentController.php`` would have the fully
qualified class name of ``Acme\BlogBundle\Controller\ContentController``.

All classes and files must follow the :doc:`Symfony coding standards </contributing/code/standards>`.

Some classes should be seen as facades and should be as short as possible, like
Commands, Helpers, Listeners and Controllers.

Classes that connect to the event dispatcher should be suffixed with
``Listener``.

Exception classes should be stored in an ``Exception`` sub-namespace.

Vendors
-------

A bundle must not embed third-party PHP libraries. It should rely on the
standard Symfony autoloading instead.

A bundle should also not embed third-party libraries written in JavaScript,
CSS or any other language.

Tests
-----

A bundle should come with a test suite written with PHPUnit and stored under
the ``Tests/`` directory. Tests should follow the following principles:

* The test suite must be executable with a simple ``phpunit`` command run from
  a sample application;
* The functional tests should only be used to test the response output and
  some profiling information if you have some;
* The tests should cover at least 95% of the code base.

.. note::

    A test suite must not contain ``AllTests.php`` scripts, but must rely on the
    existence of a ``phpunit.xml.dist`` file.

Continuous Integration
----------------------

Testing bundle code continuously, including all its commits and pull requests,
is a good practice called Continuous Integration. There are several services
providing this feature for free for open source projects. The most popular
service for Symfony bundles is called `Travis CI`_.

Here is the recommended configuration file (``.travis.yml``) for Symfony bundles,
which test the two latest :doc:`LTS versions </contributing/community/releases>`
of Symfony and the latest beta release:

.. code-block:: yaml

    language: php
    sudo: false
    cache:
        directories:
            - $HOME/.composer/cache/files
            - $HOME/symfony-bridge/.phpunit

    env:
        global:
            - PHPUNIT_FLAGS="-v"
            - SYMFONY_PHPUNIT_DIR="$HOME/symfony-bridge/.phpunit"

    matrix:
        fast_finish: true
        include:
              # Minimum supported dependencies with the latest and oldest PHP version
            - php: 7.2
              env: COMPOSER_FLAGS="--prefer-stable --prefer-lowest" SYMFONY_DEPRECATIONS_HELPER="weak_vendors"
            - php: 7.0
              env: COMPOSER_FLAGS="--prefer-stable --prefer-lowest" SYMFONY_DEPRECATIONS_HELPER="weak_vendors"

              # Test the latest stable release
            - php: 7.0
            - php: 7.1
            - php: 7.2
              env: COVERAGE=true PHPUNIT_FLAGS="-v --coverage-text"

              # Test LTS versions. This makes sure we do not use Symfony packages with version greater
              # than 2 or 3 respectively. Read more at https://github.com/symfony/lts
            - php: 7.2
              env: DEPENDENCIES="symfony/lts:^2"
            - php: 7.2
              env: DEPENDENCIES="symfony/lts:^3"

              # Latest commit to master
            - php: 7.2
              env: STABILITY="dev"

        allow_failures:
              # Dev-master is allowed to fail.
            - env: STABILITY="dev"

    before_install:
        - if [[ $COVERAGE != true ]]; then phpenv config-rm xdebug.ini || true; fi
        - if ! [ -z "$STABILITY" ]; then composer config minimum-stability ${STABILITY}; fi;
        - if ! [ -v "$DEPENDENCIES" ]; then composer require --no-update ${DEPENDENCIES}; fi;

    install:
        # To be removed when this issue will be resolved: https://github.com/composer/composer/issues/5355
        - if [[ "$COMPOSER_FLAGS" == *"--prefer-lowest"* ]]; then composer update --prefer-dist --no-interaction --prefer-stable --quiet; fi
        - composer update ${COMPOSER_FLAGS} --prefer-dist --no-interaction
        - ./vendor/bin/simple-phpunit install

    script:
        - composer validate --strict --no-check-lock
        # simple-phpunit is the PHPUnit wrapper provided by the PHPUnit Bridge component and
        # it helps with testing legacy code and deprecations (composer require symfony/phpunit-bridge)
        - ./vendor/bin/simple-phpunit $PHPUNIT_FLAGS

Consider using the `Travis cron`_ tool to make sure your project is built even if
there are no new pull requests or commits.

Installation
------------

Bundles should set ``"type": "symfony-bundle"`` in their ``composer.json`` file.
With this, :doc:`Symfony Flex </setup/flex>` will be able to automatically
enable your bundle when it's installed.

If your bundle requires any setup (e.g. configuration, new files, changes to
`.gitignore`, etc), then you should create a `Symfony Flex recipe`_.

Documentation
-------------

All classes and functions must come with full PHPDoc.

Extensive documentation should also be provided in the ``Resources/doc/``
directory.
The index file (for example ``Resources/doc/index.rst`` or
``Resources/doc/index.md``) is the only mandatory file and must be the entry
point for the documentation. The
:doc:`reStructuredText (rST) </contributing/documentation/format>` is the format
used to render the documentation on symfony.com.

Installation Instructions
~~~~~~~~~~~~~~~~~~~~~~~~~

In order to ease the installation of third-party bundles, consider using the
following standardized instructions in your ``README.md`` file.

.. configuration-block::

    .. code-block:: markdown

        Installation
        ============

        Applications that use Symfony Flex
        ----------------------------------

        Open a command console, enter your project directory and execute:

        ```console
        $ composer require <package-name>
        ```

        Applications that don't use Symfony Flex
        ----------------------------------------

        ### Step 1: Download the Bundle

        Open a command console, enter your project directory and execute the
        following command to download the latest stable version of this bundle:

        ```console
        $ composer require <package-name>
        ```

        This command requires you to have Composer installed globally, as explained
        in the [installation chapter](https://getcomposer.org/doc/00-intro.md)
        of the Composer documentation.

        ### Step 2: Enable the Bundle

        Then, enable the bundle by adding it to the list of registered bundles
        in the `app/AppKernel.php` file of your project:

        ```php
        <?php
        // app/AppKernel.php

        // ...
        class AppKernel extends Kernel
        {
            public function registerBundles()
            {
                $bundles = array(
                    // ...
                    new <vendor>\<bundle-name>\<bundle-long-name>(),
                );

                // ...
            }

            // ...
        }
        ```

    .. code-block:: rst

        Installation
        ============

        Applications that use Symfony Flex
        ----------------------------------

        Open a command console, enter your project directory and execute:

        .. code-block:: bash

            $ composer require <package-name>

        Applications that don't use Symfony Flex
        ----------------------------------------

        Step 1: Download the Bundle
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~

        Open a command console, enter your project directory and execute the
        following command to download the latest stable version of this bundle:

        .. code-block:: terminal

            $ composer require <package-name>

        This command requires you to have Composer installed globally, as explained
        in the `installation chapter`_ of the Composer documentation.

        Step 2: Enable the Bundle
        ~~~~~~~~~~~~~~~~~~~~~~~~~

        Then, enable the bundle by adding it to the list of registered bundles
        in the ``app/AppKernel.php`` file of your project:

        .. code-block:: php

            <?php
            // app/AppKernel.php

            // ...
            class AppKernel extends Kernel
            {
                public function registerBundles()
                {
                    $bundles = array(
                        // ...

                        new <vendor>\<bundle-name>\<bundle-long-name>(),
                    );

                    // ...
                }

                // ...
            }

        .. _`installation chapter`: https://getcomposer.org/doc/00-intro.md

The example above assumes that you are installing the latest stable version of
the bundle, where you don't have to provide the package version number
(e.g. ``composer require friendsofsymfony/user-bundle``). If the installation
instructions refer to some past bundle version or to some unstable version,
include the version constraint (e.g. ``composer require friendsofsymfony/user-bundle "~2.0@dev"``).

Optionally, you can add more installation steps (*Step 3*, *Step 4*, etc.) to
explain other required installation tasks, such as registering routes or
dumping assets.

Routing
-------

If the bundle provides routes, they must be prefixed with the bundle alias.
For example, if your bundle is called AcmeBlogBundle, all its routes must be
prefixed with ``acme_blog_``.

Templates
---------

If a bundle provides templates, they must use Twig. A bundle must not provide
a main layout, except if it provides a full working application.

Translation Files
-----------------

If a bundle provides message translations, they must be defined in the XLIFF
format; the domain should be named after the bundle name (``acme_blog``).

A bundle must not override existing messages from another bundle.

Configuration
-------------

To provide more flexibility, a bundle can provide configurable settings by
using the Symfony built-in mechanisms.

For simple configuration settings, rely on the default ``parameters`` entry of
the Symfony configuration. Symfony parameters are simple key/value pairs; a
value being any valid PHP value. Each parameter name should start with the
bundle alias, though this is just a best-practice suggestion. The rest of the
parameter name will use a period (``.``) to separate different parts (e.g.
``acme_blog.author.email``).

The end user can provide values in any configuration file:

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            acme_blog.author.email: 'fabien@example.com'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="acme_blog.author.email">fabien@example.com</parameter>
            </parameters>

        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('acme_blog.author.email', 'fabien@example.com');

Retrieve the configuration parameters in your code from the container::

    $container->getParameter('acme_blog.author.email');

While this mechanism requires the least effort, you should consider using the
more advanced :doc:`semantic bundle configuration </bundles/configuration>` to
make your configuration more robust.

Versioning
----------

Bundles must be versioned following the `Semantic Versioning Standard`_.

Services
--------

If the bundle defines services, they must be prefixed with the bundle alias.
For example, AcmeBlogBundle services must be prefixed with ``acme_blog``.

In addition, services not meant to be used by the application directly, should
be :ref:`defined as private <container-private-services>`. For public services,
:ref:`aliases should be created <service-autowiring-alias>` from the interface/class
to the service id. For example, in MonologBundle, an alias is created from
``Psr\Log\LoggerInterface`` to ``logger`` so that the ``LoggerInterface`` type-hint
can be used for autowiring.

Services should not use autowiring or autoconfiguration. Instead, all services should
be defined explicitly.

.. seealso::

    You can learn much more about service loading in bundles reading this article:
    :doc:`How to Load Service Configuration inside a Bundle </bundles/extension>`.

Composer Metadata
-----------------

The ``composer.json`` file should include at least the following metadata:

``name``
    Consists of the vendor and the short bundle name. If you are releasing the
    bundle on your own instead of on behalf of a company, use your personal name
    (e.g. ``johnsmith/blog-bundle``). Exclude the vendor name from the bundle
    short name and separate each word with an hyphen. For example: AcmeBlogBundle
    is transformed into ``blog-bundle`` and AcmeSocialConnectBundle is
    transformed into ``social-connect-bundle``.

``description``
    A brief explanation of the purpose of the bundle.

``type``
    Use the ``symfony-bundle`` value.

``license``
    a string (or array of strings) with a `valid license identifier`_, such as ``MIT``.

``autoload``
    This information is used by Symfony to load the classes of the bundle. It's
    recommended to use the `PSR-4`_ autoload standard.

In order to make it easier for developers to find your bundle, register it on
`Packagist`_, the official repository for Composer packages.

Resources
---------

If the bundle references any resources (config files, translation files, etc.),
don't use physical paths (e.g. ``__DIR__/config/services.xml``) but logical
paths (e.g. ``@FooBundle/Resources/config/services.xml``).

The logical paths are required because of the bundle overriding mechanism that
lets you override any resource/file of any bundle. See :ref:`http-kernel-resource-locator`
for more details about transforming physical paths into logical paths.

Beware that templates use a simplified version of the logical path shown above.
For example, an ``index.html.twig`` template located in the ``Resources/views/Default/``
directory of the FooBundle, is referenced as ``@Foo/Default/index.html.twig``.

Learn more
----------

* :doc:`/bundles/extension`
* :doc:`/bundles/configuration`

.. _`PSR-4`: https://www.php-fig.org/psr/psr-4/
.. _`Symfony Flex recipe`: https://github.com/symfony/recipes
.. _`Semantic Versioning Standard`: https://semver.org/
.. _`Packagist`: https://packagist.org/
.. _`choose any license`: https://choosealicense.com/
.. _`valid license identifier`: https://spdx.org/licenses/
.. _`Travis CI`: https://travis-ci.org/
.. _`Travis Cron`: https://docs.travis-ci.com/user/cron-jobs/
