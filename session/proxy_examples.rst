.. index::
   single: Sessions, Session Proxy, Proxy

会话代理示例
======================

会话代理机制有多种用途，本文演示了两种常见用法。
你可以通过定义继承 :class:`Symfony\\Component\\HttpFoundation\\Session\\Storage\\Proxy\\SessionHandlerProxy`
类的类来创建自定义保存处理器，而不是使用常规的会话处理器。

然后，将该类定义为 :ref:`服务 <service-container-creating-service>`。
如果你使用的是 :ref:`默认的services.yaml配置 <service-container-services-load-example>`，则会自动执行此操作。

最后，使用 ``framework.session.handler_id`` 配置选项告诉Symfony使用你的会话处理器而不是默认的会话处理器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            session:
                # ...
                handler_id: App\Session\CustomSessionHandler

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <framework:config>
                <framework:session handler-id="App\Session\CustomSessionHandler"/>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        use App\Session\CustomSessionHandler;
        $container->loadFromExtension('framework', [
            // ...
            'session' => [
                // ...
                'handler_id' => CustomSessionHandler::class,
            ],
        ]);

继续阅读下一节，了解如何在实践中使用会话处理器来解决两个常见用例：加密会话信息和定义只读访客会话。

会话数据的加密
--------------------------

如果要加密会话数据，可以根据需要使用代理加密和解密会话。
以下示例使用 `php-encryption`_ 库，但你可以将其调整为你可能正在使用的任何其他库::

    // src/Session/EncryptedSessionProxy.php
    namespace App\Session;

    use Defuse\Crypto\Crypto;
    use Defuse\Crypto\Key;
    use Symfony\Component\HttpFoundation\Session\Storage\Proxy\SessionHandlerProxy;

    class EncryptedSessionProxy extends SessionHandlerProxy
    {
        private $key;

        public function __construct(\SessionHandlerInterface $handler, Key $key)
        {
            $this->key = $key;

            parent::__construct($handler);
        }

        public function read($id)
        {
            $data = parent::read($id);

            return Crypto::decrypt($data, $this->key);
        }

        public function write($id, $data)
        {
            $data = Crypto::encrypt($data, $this->key);

            return parent::write($id, $data);
        }
    }

只读访客会话
-----------------------

有些应用需要对访客用户进行会话，但是没有持久保存会话的特别需求。
在这种情况下，你可以在写入会话之前拦截会话::

    // src/Session/ReadOnlySessionProxy.php
    namespace App\Session;

    use App\Entity\User;
    use Symfony\Component\HttpFoundation\Session\Storage\Proxy\SessionHandlerProxy;
    use Symfony\Component\Security\Core\Security;

    class ReadOnlySessionProxy extends SessionHandlerProxy
    {
        private $security;

        public function __construct(\SessionHandlerInterface $handler, Security $security)
        {
            $this->security = $security;

            parent::__construct($handler);
        }

        public function write($id, $data)
        {
            if ($this->getUser() && $this->getUser()->isGuest()) {
                return;
            }

            return parent::write($id, $data);
        }

        private function getUser()
        {
            $user = $this->security->getUser();
            if (is_object($user)) {
                return $user;
            }
        }
    }

.. _`php-encryption`: https://github.com/defuse/php-encryption
