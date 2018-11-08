如何使用MongoDbSessionHandler在MongoDB数据库中存储会话
========================================================================

默认的Symfony会话存储将会话信息写入文件。
一些中型到大型网站使用名为MongoDB的NoSQL数据库来存储会话值而不是文件，
因为数据库在多个Web服务器环境中更易于使用和扩展。

Symfony有一个内置的NoSQL数据库会话存储解决方案：
:class:`Symfony\\Component\\HttpFoundation\\Session\\Storage\\Handler\\MongoDbSessionHandler`。
要使用它，你需要：

A) 注册一个 ``MongoDbSessionHandler`` 服务；

B) 在 ``framework.session.handler_id`` 配置下配置此项。

若要了解如何配置类似的处理器，请参阅 :doc:`/doctrine/pdo_session_storage`。

设置MongoDB集合
---------------------------------

你无需执行任何操作来初始化会话集合。
但是，你可能希望添加索引以提高垃圾回收性能。在 `MongoDB shell`_ 中：

.. code-block:: javascript

    use session_db
    db.session.ensureIndex( { "expires_at": 1 }, { expireAfterSeconds: 0 } )

.. _MongoDB shell: http://docs.mongodb.org/v2.2/tutorial/getting-started-with-the-mongo-shell/
