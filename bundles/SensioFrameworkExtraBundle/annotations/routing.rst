@Route 和 @Method
==================

从5.2版本开始，**不推荐使用SensioFrameworkExtraBundle的路由注释**，因为它们现在是Symfony的核心功能。

如何更新你的应用
-------------------------------

``@Route`` 注释
~~~~~~~~~~~~~~~~~~~~~

Symfony的 ``@Route`` 注释类似于SensioFrameworkExtraBundle的注释，因此你只需更新注释类的命名空间：

.. code-block:: diff

    -use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    +use Symfony\Component\Routing\Annotation\Route;

    class DefaultController extends Controller
    {
        /**
         * @Route("/")
         */
        public function index()
        {
            // ...
        }
    }

主要区别在于Symfony的注释不再定义 ``service`` 选项，该选项用于通过从容器中获取给定服务来实例化控制器。
在现代Symfony应用中，所有 `控制器默认都是服务`_ ，其服务ID是其完全限定的类名，因此不再需要此选项。

``@Method`` 注释
~~~~~~~~~~~~~~~~~~~~~~

SensioFrameworkExtraBundle的 ``@Method`` 注释已被删除。
替代性的，Symfony的 ``@Route`` 注释定义了一个限制路由的HTTP方法的 ``methods`` 选项：

.. code-block:: diff

    -use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;
    -use Sensio\Bundle\FrameworkExtraBundle\Configuration\Method;
    +use Symfony\Component\Routing\Annotation\Route;

    class DefaultController extends Controller
    {
        /**
    -      * @Route("/show/{id}")
    -      * @Method({"GET", "HEAD"})
    +      * @Route("/show/{id}", methods={"GET","HEAD"})
         */
        public function show($id)
        {
            // ...
        }
    }

阅读Symfony文档中 `有关路由的章节`_，了解有关这些和其他可用注释的所有内容。

.. _`控制器默认都是服务`: https://symfony.com/doc/current/controller/service.html
.. _`有关路由的章节`: https://symfony.com/doc/current/routing.html
