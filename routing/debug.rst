.. index::
    single: Routing; Debugging

如何可视化和调试路由
=================================

在添加和自定义路由时，能够可视化并获取有关路由的详细信息会很有帮助。
查看应用中每条路由的好方法是通过 ``debug:router`` 控制台命令，该命令列出应用中 *所有* 已配置的路由：

.. code-block:: terminal

    $ php bin/console debug:router

    ------------------ -------- -------- ------ ----------------------------------------------
     Name               Method   Scheme   Host   Path
    ------------------ -------- -------- ------ ----------------------------------------------
     homepage           ANY      ANY      ANY    /
     contact            GET      ANY      ANY    /contact
     contact_process    POST     ANY      ANY    /contact
     article_show       ANY      ANY      ANY    /articles/{_locale}/{year}/{title}.{_format}
     blog               ANY      ANY      ANY    /blog/{page}
     blog_show          ANY      ANY      ANY    /blog/{slug}
    ------------------ -------- -------- ------ ----------------------------------------------

你还可以通过使用路由名称作为命令的参数来获取有关单个路由的非常具体的信息：

.. code-block:: terminal

    $ php bin/console debug:router article_show

    # 或使用名称的一部分来搜索路由
    $ php bin/console debug:router blo

      Select one of the matching routes:
      [0] blog
      [1] blog_show

.. versionadded:: 4.1
    Symfony 4.1中引入了查找部分路由名称的功能。

同样，如果要测试一个URL是否与给定路由匹配，请使用 ``router:match`` 命令。
这对于调试路由问题并找出与给定URL关联的路由非常有用：

.. code-block:: terminal

    $ php bin/console router:match /blog/my-latest-post

    Route "blog_show" matches
