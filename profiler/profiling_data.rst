.. index::
   single: Profiling; Profiling data

如何以编程方式访问分析数据
=============================================

大多数情况下，使用基于Web的可视化工具访问和分析分析器的信息。
但是，你还可以通过 ``profiler`` 服务提供的方法以编程方式检索分析信息。

当响应对象可用时，使用
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::loadProfileFromResponse`
方法访问其关联的配置文件::

    // ... $profiler 就是 'profiler' 服务
    $profile = $profiler->loadProfileFromResponse($response);

当分析器存储有关一个请求的数据时，它还会将一个令牌与其关联;
此令牌在响应的 ``X-Debug-Token`` HTTP标头中可用。
使用此令牌，你可以通过
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::loadProfile`
方法访问任何过去响应的配置文件::

    $token = $response->headers->get('X-Debug-Token');
    $profile = $profiler->loadProfile($token);

.. tip::

    启用分析器但未启用Web调试工具栏时，请使用浏览器的”开发人员工具”检查页面以获取 ``X-Debug-Token`` HTTP标头的值。

``profiler`` 服务还提供了基于某些标准查找令牌的
:method:`Symfony\\Component\\HttpKernel\\Profiler\\Profiler::find` 方法::

    // 获取最新的10个令牌
    $tokens = $profiler->find('', '', 10, '', '', '');

    // 获取包含 /admin/ 的所有URL的最新的10个令牌
    $tokens = $profiler->find('', '/admin/', 10, '', '', '');

    // 获取本地POST请求的最新10个令牌
    $tokens = $profiler->find('127.0.0.1', '', 10, 'POST', '', '');

    // 获取2到4天前发生的请求的最近的10个令牌
    $tokens = $profiler->find('', '', 10, '', '4 days ago', '2 days ago');
