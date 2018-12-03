.. index::
   single: Tests; Insulating clients

如何测试几个客户端的交互
==============================================

如果你需要模拟不同客户端之间的交互（例如，聊天室），请创建多个客户端::

    // ...

    $harry = static::createClient();
    $sally = static::createClient();

    $harry->request('POST', '/say/sally/Hello');
    $sally->request('GET', '/messages');

    $this->assertEquals(Response::HTTP_CREATED, $harry->getResponse()->getStatusCode());
    $this->assertRegExp('/Hello/', $sally->getResponse()->getContent());

该代码可以正常运行，除非你的代码维持一个全局状态或者它依赖于具有某种全局状态的第三方库。
在这种情况下，你可以隔离（insulate）你的客户端::

    // ...

    $harry = static::createClient();
    $sally = static::createClient();

    $harry->insulate();
    $sally->insulate();

    $harry->request('POST', '/say/sally/Hello');
    $sally->request('GET', '/messages');

    $this->assertEquals(Response::HTTP_CREATED, $harry->getResponse()->getStatusCode());
    $this->assertRegExp('/Hello/', $sally->getResponse()->getContent());

已隔离客户端在专用且干净的PHP进程中透明地执行其请求，从而避免任何副作用。

.. tip::

    由于一个已隔离客户端比较慢，你可以将一个客户端保留在主进程中，然后隔离其他客户端。

.. caution::

    绝缘测试需要一些序列化和反序列化操作。
    如果你的测试包含无法序列化的数据（例如使用 ``UploadedFile``
    类时的文件流），你将看到一个关于 *"serialization is not allowed"* 的异常。
    这是PHP的技术限制，因此唯一的解决方案是不隔离那些测试。
