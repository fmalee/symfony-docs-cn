.. index::
   single: Routing; Extra Information

查找数据库中的路由：Symfony CMF DynamicRouter
============================================================

核心Symfony路由系统非常适合处理复杂的路由集。部署期间会转储为高度优化的路由缓存。

但是，当处理大量数据，而每个数据又都需要一个可读的URL（例如，用于搜索引擎优化）时，路由可能会变慢。
此外，如果希望用户能编辑路由，则需要经常重建路由缓存。

对于这些情况，``DynamicRouter`` 提供了另一种方法：

* 路由存储在数据库中;
* 路径字段上有一个数据库索引，查找可以扩展到大量不同的路由;
* 写入操作只会影响数据库的索引，这非常有效。

如果在部署期间知道所有路由并且数量不是太多，则使用 :doc:`自定义路由加载器 <custom_route_loader>`
是添加更多路由的首选方法。
当只使用一种类型的对象时，对象上的slug参数和 ``@ParamConverter`` 注释正常运作（请参阅 FrameworkExtraBundle_）。

当你需要 ``Route`` 对象具有Symfony的完整功能集时，``DynamicRouter`` 非常有用。

DynamicRouter内置支持Doctrine ORM和Doctrine PHPCR-ODM，
同时提供了 ``ContentRepositoryInterface`` 来编写自定义加载器，
例如用于其他数据库类型或REST API或其他任何东西。

可以在 `Symfony CMF文档`_ 中了解DynamicRouter。

.. _FrameworkExtraBundle: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/converters.html
.. _`Symfony CMF文档`: https://symfony.com/doc/current/cmf/bundles/routing/dynamic.html
