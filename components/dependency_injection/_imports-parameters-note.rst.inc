.. note::

    由于参数的解析方式，你无法在动态地导入中使用它们来构建路径。
    这意味着与以下内容类似的将不起作用：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            imports:
                - { resource: '%kernel.project_dir%/somefile.yaml' }

        .. code-block:: xml

            <!-- config/services.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    https://symfony.com/schema/dic/services/services-1.0.xsd">

                <imports>
                    <import resource="%kernel.project_dir%/somefile.yaml"/>
                </imports>
            </container>

        .. code-block:: php

            // config/services.php
            $loader->import('%kernel.project_dir%/somefile.yaml');
