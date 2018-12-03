.. index::
   single: Tests; HTTP authentication

如何在功能测试中模拟HTTP身份认证
========================================================

在功能测试中认证请求可能会降低整个测试套件的速度。
这可能会成为一个问题，尤其是当测试重现用户进行认证时的步骤时，例如提交登录表单或使用OAuth认证服务。

本文介绍了两种最常用的技术，可以避免这些问题并在使用认证时创建快速的测试。

仅为测试使用更快的认证机制
------------------------------------------------------

当你的应用使用 ``form_login`` 认证时，你可以通过允许它们使用HTTP认证来加快测试速度。
这样处理的话，你的测试使用简单快速的HTTP Basic方法进行认证，而你的真实用户仍然可以通过正常的登录表单来登录。

诀窍是在应用防火墙中使用 ``http_basic`` 认证，但仅在用于测试环境的配置文件中配置：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/test/security.yaml
        security:
            firewalls:
                # 用自己的防火墙名称来替换 'main'
                main:
                    http_basic: ~

    .. code-block:: xml

        <!-- config/packages/test/security.xml -->
        <security:config>
            <!-- replace 'main' by the name of your own firewall -->
            <security:firewall name="main">
              <security:http-basic />
           </security:firewall>
        </security:config>

    .. code-block:: php

        // config/packages/test/security.php
        $container->loadFromExtension('security', array(
            'firewalls' => array(
                // replace 'main' by the name of your own firewall
                'main' => array(
                    'http_basic' => array(),
                ),
            ),
        ));

现在，该测试可以使用 ``createClient()`` 的第二个参数将用户名和密码作为服务器变量来通过HTTP进行认证::

    $client = static::createClient(array(), array(
        'PHP_AUTH_USER' => 'username',
        'PHP_AUTH_PW'   => 'pa$$word',
    ));

也可以基于每个请求来传递用户名和密码::

    $client->request('DELETE', '/post/12', array(), array(), array(
        'PHP_AUTH_USER' => 'username',
        'PHP_AUTH_PW'   => 'pa$$word',
    ));

创建认证令牌
---------------------------------

如果你的应用使用更高级的认证机制，则无法使用之前的技巧，但仍可以更快地进行测试。
现在的诀窍是绕过常规认证过程，自己创建 *认证令牌* 并将其存储在会话中。

此技术需要一些安全组件内部的知识，但下面显示了一个完整的示例，你可以根据自己的需求进行调整::

    // tests/Controller/DefaultControllerTest.php
    namespace App\Tests\Controller;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;
    use Symfony\Component\BrowserKit\Cookie;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Security\Core\Authentication\Token\UsernamePasswordToken;

    class DefaultControllerTest extends WebTestCase
    {
        private $client = null;

        public function setUp()
        {
            $this->client = static::createClient();
        }

        public function testSecuredHello()
        {
            $this->logIn();
            $crawler = $this->client->request('GET', '/admin');

            $this->assertSame(Response::HTTP_OK, $this->client->getResponse()->getStatusCode());
            $this->assertSame('Admin Dashboard', $crawler->filter('h1')->text());
        }

        private function logIn()
        {
            $session = $this->client->getContainer()->get('session');

            $firewallName = 'secure_area';
            // 如果未定义多个已连接防火墙，则上下文默认为该防火墙名称
            // 请参阅 https://symfony.com/doc/current/reference/configuration/security.html#firewall-context
            $firewallContext = 'secured_area';

            // 你可能需要使用一个不同的令牌类，具体取决于你的应用。
            // 例如，使用安保认证时，你必须实例化 PostAuthenticationGuardToken
            $token = new UsernamePasswordToken('admin', null, $firewallName, array('ROLE_ADMIN'));
            $session->set('_security_'.$firewallContext, serialize($token));
            $session->save();

            $cookie = new Cookie($session->getName(), $session->getId());
            $this->client->getCookieJar()->set($cookie);
        }
    }
