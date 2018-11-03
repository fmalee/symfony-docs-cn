测试
=====

在所有可用的不同类型的测试中，这些最佳实践仅关注单元和功能测试。
单元测试允许你测试特定功能的输入和输出。
功能测试允许你模拟“浏览器”浏览网站上的页面，单击链接，填写表单并断言你在页面上看到某些内容。

单元测试
----------

单元测试用于测试你的“业务逻辑”，它应该存在于独立的Symfony类中。
出于这个原因，Symfony对用于单元测试的工具没有真正的意见。但是，最流行的工具是 `PHPUnit`_ 和 `PHPSpec`_。

功能测试
----------------

创建真正好用的功能测试可能很难，所以一些开发人员完全跳过这些。
不要跳过功能测试！通过定义一些简单的功能测试，你可以在部署之前快速发现一些重大错误：:

.. best-practice::

    定义一个功能测试，至少检查你的应用页面是否成功加载。

:ref:`PHPUnit数据提供程序 <testing-data-providers>` 帮助你实现功能测试::

    // tests/ApplicationAvailabilityFunctionalTest.php
    namespace App\Tests;

    use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;

    class ApplicationAvailabilityFunctionalTest extends WebTestCase
    {
        /**
         * @dataProvider urlProvider
         */
        public function testPageIsSuccessful($url)
        {
            $client = self::createClient();
            $client->request('GET', $url);

            $this->assertTrue($client->getResponse()->isSuccessful());
        }

        public function urlProvider()
        {
            yield ['/'];
            yield ['/posts'];
            yield ['/post/fixture-post-1'];
            yield ['/blog/category/fixture-category'];
            yield ['/archives'];
            // ...
        }
    }

此代码检查了所有给定的URL是否成功加载，这意味着它们的HTTP响应状态代码介于200和299之间。
这可能看起来不那么有用，但考虑到它花费的精力很少，所以在你的应用中使用它是值得的。

在计算机软件领域，这种测试称为 `烟雾测试`_，包括 *“初步测试，以揭示严重到足以拒绝预期软件发布的简单故障”*。

在功能测试中硬编码URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

有些人可能会问，为什么以前的功能测试不使用URL生成器的服务：

.. best-practice::

    在功能测试中使用硬编码的URL，而不是使用URL生成器来生成URL。

思考下面的功能测试代码，使用了 ``router`` 服务来生成测试页面的URL::

    // ...
    private $router; // 假定它包含了 Symfony 路由服务

    public function testBlogArchives()
    {
        $client = self::createClient();
        $url = $this->router->generate('blog_archives');
        $client->request('GET', $url);

        // ...
    }

代码执行正确，但却有一个 *致命* 缺陷。
如果开发人员错误地更改了 ``blog_archives`` 路由的路径，虽然测试仍会通过，但原始（旧）的URL将无效！
这意味着任何收藏该URL的书签都将被破坏，你也将失去任何搜索引擎的页面排名(page ranking)。

测试JavaScript功能
--------------------------------

内置的功能测试客户端很棒，但它不能用于测试页面上的任何JavaScript行为。
如果需要对此进行测试，请考虑在PHPUnit中使用 `Mink`_ 库。

当然，如果你有一个繁杂的JavaScript前端，你应该考虑使用纯基于JavaScript的测试工具。

深入学习功能测试
---------------------------------

可以考虑在 `HautelookAliceBundle`_ 中使用 `Faker`_ 和 `Alice`_ 为你的测试装置生成真实(real-looking)的数据。

.. _`PHPUnit`: https://phpunit.de/
.. _`PHPSpec`: https://www.phpspec.net/
.. _`烟雾测试`: https://en.wikipedia.org/wiki/Smoke_testing_(software)
.. _`Mink`: http://mink.behat.org
.. _`HautelookAliceBundle`: https://github.com/hautelook/AliceBundle
.. _`Faker`: https://github.com/fzaninotto/Faker
.. _`Alice`: https://github.com/nelmio/alice
