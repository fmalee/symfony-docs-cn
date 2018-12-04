.. index::
   single: Tests; Profiling

如何在功能测试中使用分析器
============================================

强烈建议在功能测试时仅测试响应。
但是，如果编写用于监视生产服务器的功能测试，那么你可能希望对分析数据编写测试，因为它为你提供了检查各种内容并强制执行某些指标的好方法。

.. _speeding-up-tests-by-not-collecting-profiler-data:

在测试中启用分析器
------------------------------

使用 :doc:`Symfony分析器 </profiler>` 来收集数据可能会显著降低测试速度。
这就是为什么Symfony会默认禁用它：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/test/web_profiler.yaml

        # ...
        framework:
            profiler: { enabled: true, collect: false }

    .. code-block:: xml

        <!-- config/packages/test/web_profiler.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                        http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <!-- ... -->

            <framework:config>
                <framework:profiler enabled="true" collect="false" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/test/web_profiler.php

        // ...
        $container->loadFromExtension('framework', array(
            // ...
            'profiler' => array(
                'enabled' => true,
                'collect' => false,
            ),
        ));

可以通过设置 ``collect`` 为 ``true`` 来为所有测试启用分析器。
但是，如果你只需要在几个测试中使用分析器，则可以将其全局禁用，并通过调用
``$client->enableProfiler()`` 以在每个测试中单独启用分析器。

测试分析器信息
--------------------------------

Symfony分析器收集的数据可用于检查数据库调用的数量，框架中耗费的时间等。
所有这些信息都是通过调用 ``$client->getProfile()`` 获得的收集器来提供的::

    class LuckyControllerTest extends WebTestCase
    {
        public function testRandomNumber()
        {
            $client = static::createClient();

            // 仅为下一个请求启用分析器（如果创建了新请求，则必须再次调用此方法）
            // （如果分析器不可用，它什么也不做）
            $client->enableProfiler();

            $crawler = $client->request('GET', '/lucky/number');

            // ... 写一些关于响应的断言

            // 检查是否启用分析器
            if ($profile = $client->getProfile()) {
                // check the number of requests
                $this->assertLessThan(
                    10,
                    $profile->getCollector('db')->getQueryCount()
                );

                // 检查框架中花费的时间
                $this->assertLessThan(
                    500,
                    $profile->getCollector('time')->getDuration()
                );
            }
        }
    }

如果由于分析数据导致测试失败（例如，太多数据库查询），你可能希望在测试完成后使用Web分析器来分析请求。
只要你在错误消息中嵌入令牌，那就很容易实现该目的::

    $this->assertLessThan(
        30,
        $profile->getCollector('db')->getQueryCount(),
        sprintf(
            'Checks that query count is less than 30 (token %s)',
            $profile->getToken()
        )
    );

.. note::

    即使你 :doc:`隔离客户端 </testing/insulating_clients>`，或者使用一个HTTP层进行测试，也可以使用分析器信息。

.. tip::

    可以阅读内置 :doc:`数据收集器 </profiler/data_collector>` 的API，以了解有关其接口的更多信息。
