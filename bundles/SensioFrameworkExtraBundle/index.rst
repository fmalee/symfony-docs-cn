SensioFrameworkExtraBundle
==========================

默认的Symfony ``FrameworkBundle`` 实现了一个基础但强大而灵活的MVC框架。
`SensioFrameworkExtraBundle`_ 扩展了它，以添加甜蜜的转换和注释。
它可以使控制器更加简洁。

安装
------------

此Bundle可以使用官方的Symfony指令。要自动安装和配置它，请运行：

.. code-block:: bash

    $ composer require annotations

这就完成了！
You're done!

如果你不使用Symfony Flex，那么请将其添加到你的 ``composer.json`` 文件中：

.. code-block:: bash

    $ composer require sensio/framework-extra-bundle

然后，像任何其他bundle一样，将它包含在你的Kernel类中::

    public function registerBundles()
    {
        $bundles = array(
            // ...

            new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle(),
        );

        // ...
    }

配置
-------------

当此Bundle在Kernel类中注册后，默认情况下会启用Bundle提供的所有功能。

默认配置如下：

.. configuration-block::

    .. code-block:: yaml

        sensio_framework_extra:
            router:      { annotations: true } #不推荐使用; 请改用使用Symfony核心的路由注释
            request:     { converters: true, auto_convert: true }
            view:        { annotations: true }
            cache:       { annotations: true }
            security:    { annotations: true }
            psr_message: { enabled: false } # 如果安装了PSR-7桥接，则默认为true


    .. code-block:: xml

        <!-- xmlns:sensio-framework-extra="http://symfony.com/schema/dic/symfony_extra" -->
        <sensio-framework-extra:config>
            <router annotations="true" />
            <request converters="true" auto_convert="true" />
            <view annotations="true" />
            <cache annotations="true" />
            <security annotations="true" />
            <psr-message enabled="false" /> <!-- Defaults to true if the PSR-7 bridge is installed -->
        </sensio-framework-extra:config>

    .. code-block:: php

        // load the profiler
        $container->loadFromExtension('sensio_framework_extra', array(
            'router'      => array('annotations' => true),
            'request'     => array('converters' => true, 'auto_convert' => true),
            'view'        => array('annotations' => true),
            'cache'       => array('annotations' => true),
            'security'    => array('annotations' => true),
            'psr_message' => array('enabled' => false), // Defaults to true if the PSR-7 bridge is installed
        ));

你可以通过将一个或多个设置定义为 ``false`` 来禁用某些注释和转换。

控制器的注释
---------------------------

注释是轻松配置控制器的好方法，从路由到缓存配置都支持使用注释。

即使注释不是一个PHP的原生功能，但比传统的Symfony配置方法，它有几个优点：

* 代码和配置在同一个地方（控制器类）;
* 易学易用;
* 书写简洁;
* 使你的控制器变薄（因为它的唯一责任是从模型中获取数据）。

.. tip::

   如果你使用视图类，那么注释是避免为简单和常见用例创建视图类的好方法。

该Bundle定义了以下注释：

.. toctree::
   :maxdepth: 1

   annotations/routing
   annotations/converters
   annotations/view
   annotations/cache
   annotations/security

此示例显示了所有可用的注释::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Cache;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\ParamConverter;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Method;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\IsGranted;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;

    /**
     * @Route("/blog")
     * @Cache(expires="tomorrow")
     */
    class AnnotController
    {
        /**
         * @Route("/")
         * @Template
         */
        public function indexAction()
        {
            $posts = ...;

            return array('posts' => $posts);
        }

        /**
         * @Route("/{id}")
         * @Method("GET")
         * @ParamConverter("post", class="SensioBlogBundle:Post")
         * @Template("@SensioBlog/annot/show.html.twig", vars={"post"})
         * @Cache(smaxage="15", lastmodified="post.getUpdatedAt()", etag="'Post' ~ post.getId() ~ post.getUpdatedAt()")
         * @IsGranted("ROLE_SPECIAL_USER")
         * @Security("has_role('ROLE_ADMIN') and is_granted('POST_SHOW', post)")
         */
        public function showAction(Post $post)
        {
        }
    }

由于 ``showAction`` 方法遵循一些约定，因此你可以省略一些注释::

    /**
     * @Route("/{id}")
     * @Cache(smaxage="15", lastModified="post.getUpdatedAt()", Etag="'Post' ~ post.getId() ~ post.getUpdatedAt()")
     * @IsGranted("ROLE_SPECIAL_USER")
     * @Security("has_role('ROLE_ADMIN') and is_granted('POST_SHOW', post)")
     */
    public function showAction(Post $post)
    {
    }

需要将该路由导入为任何其他路由资源，例如：

.. code-block:: yaml

    # app/config/routing.yml

    # 从一个控制器目录导入路由
    annot:
        resource: "@AnnotRoutingBundle/Controller"
        type:     annotation

PSR-7支持
-------------

SensioFrameworkExtraBundle为 `PSR-7`_ 中定义的HTTP消息接口提供支持。
它在控制器中允许注入 ``Psr\Http\Message\ServerRequestInterface``
实例和返回 ``Psr\Http\Message\ResponseInterface`` 实例。

要启用此功能，必须安装 `HttpFoundation的PSR-7桥接`_ 和 `Zend Diactoros`_：

.. code-block:: bash

    $ composer require symfony/psr-http-message-bridge zendframework/zend-diactoros

然后，PSR-7消息就可以直接在控制器中使用，如下面的代码片段所示::

    namespace AppBundle\Controller;

    use Psr\Http\Message\ServerRequestInterface;
    use Zend\Diactoros\Response;

    class DefaultController
    {
        public function indexAction(ServerRequestInterface $request)
        {
            // 与PSR-7请求互动

            $response = new Response();
            // 与PSR-7响应互动

            return $response;
        }
    }

请注意，在内部，Symfony始终使用 :class:`Symfony\\Component\\HttpFoundation\\Request`
和 :class:`Symfony\\Component\\HttpFoundation\\Response` 实例。

.. _`SensioFrameworkExtraBundle`: https://github.com/sensiolabs/SensioFrameworkExtraBundle
.. _`PSR-7`: http://www.php-fig.org/psr/psr-7/
.. _`HttpFoundation的PSR-7桥接`: https://github.com/symfony/psr-http-message-bridge
.. _`Zend Diactoros`: https://github.com/zendframework/zend-diactoros
