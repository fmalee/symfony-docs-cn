.. index::
   single: Asset; Custom Version Strategy

如何自定义资产的版本策略
===============================================

资产版本控制是一种通过向静态资产（CSS，JavaScript，图像等）的URL添加版本标识符来提高Web应用性能的技术。
当资产内容发生更改时，其标识符也会被修改以强制浏览器再次下载该资产而不是重用已缓存的资产。

如果你的应用需要高级版本控制，例如根据某些外部信息动态生成版本，则可以创建自己的版本策略。

.. note::

    Symfony通过
    :ref:`version <reference-framework-assets-version>`,
    :ref:`version_format <reference-assets-version-format>`, 和
    :ref:`json_manifest_path <reference-assets-json-manifest-path>`
    配置选项提供各种缓存清除实现。

创建自己的资产版本策略
----------------------------------------

以下示例显示如何创建与 `gulp-buster`_ 兼容的版本策略。
此工具定义了一个叫 ``busters.json`` 的配置文件，该文件将每个资产文件映射到其内容哈希中：

.. code-block:: json

    {
        "js/script.js": "f9c7afd05729f10f55b689f36bb20172",
        "css/style.css": "91cd067f79a5839536b46c494c4272d8"
    }

实现VersionStrategyInterface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

资产版本策略是实现 :class:`Symfony\\Component\\Asset\\VersionStrategy\\VersionStrategyInterface` 的PHP类。
在此示例中，该类的构造函数使用 `gulp-buster`_ 生成的清单(manifest)文件的路径和生成的版本字符串的格式作为参数::

    // src/Asset/VersionStrategy/GulpBusterVersionStrategy.php
    namespace App\Asset\VersionStrategy;

    use Symfony\Component\Asset\VersionStrategy\VersionStrategyInterface;

    class GulpBusterVersionStrategy implements VersionStrategyInterface
    {
        /**
         * @var string
         */
        private $manifestPath;

        /**
         * @var string
         */
        private $format;

        /**
         * @var string[]
         */
        private $hashes;

        /**
         * @param string      $manifestPath
         * @param string|null $format
         */
        public function __construct($manifestPath, $format = null)
        {
            $this->manifestPath = $manifestPath;
            $this->format = $format ?: '%s?%s';
        }

        public function getVersion($path)
        {
            if (!is_array($this->hashes)) {
                $this->hashes = $this->loadManifest();
            }

            return isset($this->hashes[$path]) ? $this->hashes[$path] : '';
        }

        public function applyVersion($path)
        {
            $version = $this->getVersion($path);

            if ('' === $version) {
                return $path;
            }

            $versionized = sprintf($this->format, ltrim($path, '/'), $version);

            if ($path && '/' === $path[0]) {
                return '/'.$versionized;
            }

            return $versionized;
        }

        private function loadManifest()
        {
            return json_decode(file_get_contents($this->manifestPath), true);
        }
    }

注册策略为服务
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

创建策略PHP类后，将其注册为Symfony服务。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\Asset\VersionStrategy\GulpBusterVersionStrategy:
                arguments:
                    - "%kernel.project_dir%/busters.json"
                    - "%%s?version=%%s"
                public: false

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd"
        >
            <services>
                <service id="App\Asset\VersionStrategy\GulpBusterVersionStrategy" public="false">
                    <argument>%kernel.project_dir%/busters.json</argument>
                    <argument>%%s?version=%%s</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use App\Asset\VersionStrategy\GulpBusterVersionStrategy;

        $container->autowire(GulpBusterVersionStrategy::class)
            ->setArguments(
                array(
                    '%kernel.project_dir%/busters.json',
                    '%%s?version=%%s',
                )
        )->setPublic(false);

最后，通过 :ref:`version_strategy <reference-assets-version-strategy>` 选项，
为所有应用资产启用新的资产版本控制，
或仅在某些 :ref:`资产包 <reference-framework-assets-packages>` 中启用：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            # ...
            assets:
                version_strategy: 'App\Asset\VersionStrategy\GulpBusterVersionStrategy'

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:assets version-strategy="App\Asset\VersionStrategy\GulpBusterVersionStrategy" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        use App\Asset\VersionStrategy\GulpBusterVersionStrategy;

        $container->loadFromExtension('framework', array(
            // ...
            'assets' => array(
                'version_strategy' => GulpBusterVersionStrategy::class,
            ),
        ));

.. _`gulp-buster`: https://www.npmjs.com/package/gulp-buster
