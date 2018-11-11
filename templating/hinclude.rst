.. index::
    single: Templating; hinclude.js

如何使用hinclude.js嵌入异步内容
==================================================

可以使用 hinclude.js_ 脚本库异步嵌入控制器。
由于嵌入式内容来自另一个页面（或控制器），
Symfony使用标准的 ``render()`` 函数的一个版本来配置 ``hinclude`` 标签：

.. code-block:: twig

    {{ render_hinclude(controller('...')) }}
    {{ render_hinclude(url('...')) }}

.. note::

    hinclude.js_ 需要包含在你的页面中才能工作。

.. note::

    使用控制器而不是URL时，必须启用Symfony的 ``fragments`` 配置：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/framework.yaml
            framework:
                # ...
                fragments: { path: /_fragment }

        .. code-block:: xml

            <!-- config/packages/framework.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

                <!-- ... -->
                <framework:config>
                    <framework:fragment path="/_fragment" />
                </framework:config>
            </container>

        .. code-block:: php

            // config/packages/framework.php
            $container->loadFromExtension('framework', array(
                // ...
                'fragments' => array('path' => '/_fragment'),
            ));

默认内容（在加载时或者禁用脚本时使用）可以在应用配置中全局设置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            # ...
            templating:
                hinclude_default_template: hinclude.html.twig

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <!-- ... -->
            <framework:config>
                <framework:templating hinclude-default-template="hinclude.html.twig" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            // ...
            'templating' => array(
                'hinclude_default_template' => array(
                    'hinclude.html.twig',
                ),
            ),
        ));

你可以为每个 ``render()`` 函数定义默认模板（它将覆盖定义的任何全局默认模板）：

.. code-block:: twig

    {{ render_hinclude(controller('...'),  {
        'default': 'default/content.html.twig'
    }) }}

或者你也可以指定一个字符串为默认显示的内容：

.. code-block:: twig

    {{ render_hinclude(controller('...'), {'default': 'Loading...'}) }}

.. _`hinclude.js`: http://mnot.github.io/hinclude/
