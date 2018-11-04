.. index::
   single: Console; Generating URLs

如何从控制台生成URL
=====================================

很不幸运，命令行上下文不知道你的虚拟主机或域名。
这意味着如果你在控制台命令中生成绝对URL，你可能最终会得到像 ``http://localhost/foo/bar`` 的一些不太有用的东西。

要解决此问题，你需要配置“请求上下文”，这是一种奇特的方式，表示你需要配置你的环境，以便它知道生成URL时应使用的URL。

有两种配置请求上下文的方法：在应用级别和每个命令中。

全局配置请求上下文
----------------------------------------

要配置URL生成器使用的请求上下文，
你可以重新定义参数并将其作为默认值来更改默认的host（``localhost``）和scheme（``http``）。
如果Symfony未在根目录中运行，你还可以配置基础路径（同时用于URL生成器和资产）。

请注意，这不会影响通过普通Web请求生成的URL，因为它们会覆盖默认值。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            router.request_context.host: 'example.org'
            router.request_context.scheme: 'https'
            router.request_context.base_url: 'my/path'
            asset.request_context.base_path: '%router.request_context.base_url%'
            asset.request_context.secure: true

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <parameters>
                <parameter key="router.request_context.host">example.org</parameter>
                <parameter key="router.request_context.scheme">https</parameter>
                <parameter key="router.request_context.base_url">my/path</parameter>
                <parameter key="asset.request_context.base_path">%router.request_context.base_url%</parameter>
                <parameter key="asset.request_context.secure">true</parameter>
            </parameters>

        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('router.request_context.host', 'example.org');
        $container->setParameter('router.request_context.scheme', 'https');
        $container->setParameter('router.request_context.base_url', 'my/path');
        $container->setParameter('asset.request_context.base_path', $container->getParameter('router.request_context.base_url'));
        $container->setParameter('asset.request_context.secure', true);

.. versionadded:: 3.4
    ``asset.request_context.*`` 参数在Symfony的3.4中引入。

按命令配置请求上下文
-------------------------------------------

要仅在一个命令中更改它，你可以从路由服务中获取请求上下文并覆盖其设置::

    // src/Command/DemoCommand.php
    use Symfony\Component\Routing\RouterInterface;
    // ...

    class DemoCommand extends Command
    {
        private $router;

        public function __construct(RouterInterface $router)
        {
            parent::__construct();

            $this->router = $router;
        }

        protected function execute(InputInterface $input, OutputInterface $output)
        {
            $context = $this->router->getContext();
            $context->setHost('example.com');
            $context->setScheme('https');
            $context->setBaseUrl('my/path');

            $url = $this->router->generate('route-name', array('param-name' => 'param-value'));
            // ...
        }
    }
