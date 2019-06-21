.. index::
    single: Override Symfony

如何重写Symfony的默认目录结构
=====================================================

Symfony自动附带默认目录结构。你可以重写此目录结构以创建自己的目录结构。
默认的目录结构是：

.. code-block:: text

    your-project/
    ├─ assets/
    ├─ bin/
    │  └─ console
    ├─ config/
    ├─ public/
    │  └─ index.php
    ├─ src/
    │  └─ ...
    ├─ templates/
    ├─ tests/
    ├─ translations/
    ├─ var/
    │  ├─ cache/
    │  ├─ log/
    │  └─ ...
    └─ vendor/

.. _override-cache-dir:

重写 ``cache`` 目录
--------------------------------

你可以通过重写应用中 ``Kernel`` 类中的 ``getCacheDir()`` 方法来更改默认缓存目录::

    // src/Kernel.php

    // ...
    class Kernel extends BaseKernel
    {
        // ...

        public function getCacheDir()
        {
            return dirname(__DIR__).'/var/'.$this->environment.'/cache';
        }
    }

在此代码中，``$this->environment`` 是当前环境（如 ``dev``）。
在这种情况下，你已将缓存目录的位置更改为 ``var/{environment}/cache``。

.. caution::

    你应该给每个环境使用不同的 ``cache`` 目录，否则可能会发生一些意外行为。
    每个环境都会生成自己的缓存配置文件，因此每个环境都需要自己的目录来存储这些缓存文件。

.. _override-logs-dir:

重写 ``logs`` 目录
-------------------------------

重写 ``logs`` 目录与重写 ``cache`` 目录相同。
唯一的区别是你需要重写 ``getLogDir()`` 方法::

    // src/Kernel.php

    // ...
    class Kernel extends Kernel
    {
        // ...

        public function getLogDir()
        {
            return dirname(__DIR__).'/var/'.$this->environment.'/log';
        }
    }

在这里，你已将目录的位置更改为 ``var/{environment}/log``。

.. _override-templates-dir:

重写模板目录
--------------------------------

如果模板未存储在默认 ``templates/`` 目录中，请使用 :ref:`twig.paths <config-twig-paths>`
配置选项定义你自己的模板目录（或多个目录）：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/twig.yaml
        twig:
            # ...
            paths: ["%kernel.project_dir%/resources/views"]

    .. code-block:: xml

        <!-- config/packages/twig.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <twig:config>
                <twig:path>%kernel.project_dir%/resources/views</twig:path>
            </twig:config>

        </container>

    .. code-block:: php

        // config/packages/twig.php
        $container->loadFromExtension('twig', [
            'paths' => [
                '%kernel.project_dir%/resources/views',
            ],
        ]);

重写翻译目录
-----------------------------------

如果你的翻译文件未存储在默认 ``translations/`` 目录中，请使用
:ref:`framework.translator.paths <reference-translator-paths>`
配置选项来定义你自己的翻译目录（或多个目录）：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/translation.yaml
        framework:
            translator:
                # ...
                paths: ["%kernel.project_dir%/i18n"]

    .. code-block:: xml

        <!-- config/packages/translation.xml -->
        <?xml version="1.0" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:twig="http://symfony.com/schema/dic/twig"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/twig
                https://symfony.com/schema/dic/twig/twig-1.0.xsd">

            <framework:config>
                <framework:translator>
                    <framework:path>%kernel.project_dir%/i18n</framework:path>
                </framework:translator>
            </framework:config>

        </container>

    .. code-block:: php

        // config/packages/translation.php
        $container->loadFromExtension('framework', [
            'translator' => [
                'paths' => [
                    '%kernel.project_dir%/i18n',
                ],
            ],
        ]);

.. _override-web-dir:
.. _override-the-web-directory:

重写 ``public`` 目录
---------------------------------

如果需要重命名或移动 ``public`` 目录，则唯一需要保证的是 ``index.php`` 前端控制器中 ``var`` 目录的路径仍然正确。
如果你只是重命名该目录，那就没事了。但是如果你以某种方式移动它，你可能需要在这些文件中修改这些路径::

    require_once __DIR__.'/../path/to/vendor/autoload.php';

你还需要更改 ``composer.json`` 文件中的 ``extra.public-dir`` 选项：

.. code-block:: json

    {
        "...": "...",
        "extra": {
            "...": "...",
            "public-dir": "my_new_public_dir"
        }
    }

.. tip::

    某些共享主机具有一个 ``public_html`` Web根目录。
    将你的web目录 ``public`` 重命名为 ``public_html``
    就是让你的Symfony项目在共享主机上运行的一种方式。
    另一种方法是将应用部署到Web根目录之外的目录，删除 ``public_html`` 目录，
    然后将其替换为项目中 ``public`` 目录的符号链接。

重写 ``vendor`` 目录
---------------------------------

要重写 ``vendor`` 目录，你需要在 ``composer.json`` 文件中定义 ``vendor-dir`` 选项，如下所示：

.. code-block:: json

    {
        "config": {
            "bin-dir": "bin",
            "vendor-dir": "/some/dir/vendor"
        },
    }

.. tip::

    如果你在虚拟环境中工作并且无法使用NFS，则可能会对此修改感兴趣 -
    例如，如果你在客户机操作系统中使用Vagrant/VirtualBox来运行Symfony应用。
