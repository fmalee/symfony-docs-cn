.. index::
   single: Sessions, sessions directory

配置保存会话文件的目录
=======================================================

默认情况下，Symfony将会话元数据存储在文件系统上。
如果要控制该路径，请更新 ``framework.session.save_path`` 配置键：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            session:
                handler_id: session.handler.native_file
                save_path: '%kernel.project_dir%/var/sessions/%kernel.environment%'

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:session handler-id="session.handler.native_file" save-path="%kernel.project_dir%/var/sessions/%kernel.environment%" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            'session' => array(
                'handler_id' => 'session.handler.native_file',
                'save_path' => '%kernel.project_dir%/var/sessions/%kernel.environment%'
            ),
        ));

在其他地方存储会话（例如数据库）
------------------------------------------

你以使用该 ``handler_id`` 选项将会话数据存储在任何位置。
有关会话保存处理器的讨论，请参阅 :doc:`/components/http_foundation/session_configuration`。
还有关于在 :doc:`关系型数据库 </doctrine/pdo_session_storage>` 或 :doc:`NoSQL数据库 </doctrine/mongodb_session_storage>` 中存储会话的文章。
