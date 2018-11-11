.. index::
    single: Profiling; Matchers

如何使用匹配器有条件地启用分析器
========================================================

.. caution::

    在Symfony 4.0中删除了使用匹配器有条件地启用分析器的可能性。

现在无法使用匹配器有条件地启用/禁用Symfony分析器，因为在Symfony 4.0中删除了该功能。
但是，你可以在控制器中使用 :class:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler`
类的 ``enable()`` 和 ``disable()`` 方法以编程方式管理分析器::

    use Symfony\Component\HttpKernel\Profiler\Profiler;
    // ...

    class DefaultController
    {
        // ...

        public function someMethod(?Profiler $profiler)
        {
            // 如果你的环境没有分析器（如默认下的 ``prod``），则不会设置 $profiler
            if (null !== $profiler) {
                // 如果分析器存在，则禁用此特定控制器动作中的分析器
                $profiler->disable();
            }

            // ...
        }
    }

为了将分析器注入到控制器，你需要创建指向现有 ``profiler`` 服务的一个别名：

.. configuration-block::

    .. code-block:: yaml

        # config/services_dev.yaml
        services:
            Symfony\Component\HttpKernel\Profiler\Profiler: '@profiler'

    .. code-block:: xml

        <!-- config/services_dev.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="Symfony\Component\HttpKernel\Profiler\Profiler" alias="profiler" />
            </services>
        </container>

    .. code-block:: php

        // config/services_dev.php
        use Symfony\Component\HttpKernel\Profiler\Profiler;

        $container->setAlias(Profiler::class, 'profiler');
