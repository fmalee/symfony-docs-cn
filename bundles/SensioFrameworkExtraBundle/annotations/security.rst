@Security & @IsGranted
======================

用法
-----

``@Security`` 和 ``@IsGranted`` 用于限制控制器的访问::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Security;
    use Sensio\Bundle\FrameworkExtraBundle\Configuration\IsGranted;

    class PostController extends Controller
    {
        /**
         * @IsGranted("ROLE_ADMIN")
         *
         * 或使用 @Security 以获得更过灵活性:
         *
         * @Security("is_granted('ROLE_ADMIN') and is_granted('ROLE_FRIENDLY_USER')")
         */
        public function indexAction()
        {
            // ...
        }
    }

@IsGranted
----------

``@IsGranted()`` 注释是限制访问的最简单方法。
它使用角色，或基于传递给控制器​​的变量来使用自定义投票器来限制访问::

    /**
     * @Route("/posts/{id}")
     *
     * @IsGranted("ROLE_ADMIN")
     * @IsGranted("POST_SHOW", subject="post")
     */
    public function showAction(Post $post)
    {
    }

每个 ``IsGranted()`` 都必须授予用户访问该控制器的访问权限。

.. tip::

    ``@IsGranted("POST_SHOW", subject="post")`` 是一个使用自定义安全投票器的示例。
    有关更多详细信息，请参阅 `安全投票器文档`_。

你还可以控制消息和状态代码::

    /**
     * 将会抛出一个普通的 AccessDeniedException:
     *
     * @IsGranted("ROLE_ADMIN", message="No access! Get out!")
     *
     * 将会抛出一个附带404状态码的HttpException:
     *
     * @IsGranted("ROLE_ADMIN", statusCode=404, message="Post not found")
     */
    public function showAction(Post $post)
    {
    }

@Security
---------

``@Security`` 注释比 ``@IsGranted`` 更加灵活：它允许你传递一个包含自定义逻辑的 *表达式*::

    /**
     * @Security("is_granted('ROLE_ADMIN') and is_granted('POST_SHOW', post)")
     */
    public function showAction(Post $post)
    {
        // ...
    }

表达式可以使用安全配置中 ``access_control`` 部分的所有函数，并附加了 ``is_granted()`` 函数。

该表达式可以访问以下变量：

* ``token``: 当前的安全令牌;
* ``user``: 当前用户对象;
* ``request``: 请求实例;
* ``roles``: 用户角色;
* 以及请求的所有属性。

你可以使用 ``statusCode`` 抛出一个
``Symfony\Component\HttpKernel\Exception\HttpException`` 异常来代替
``Symfony\Component\Security\Core\Exception\AccessDeniedException``::

    /**
     * @Security("is_granted('POST_SHOW', post)", statusCode=404)
     */
    public function showAction(Post $post)
    {
    }

``message`` 选项允许你自定义该异常的消息::

    /**
     * @Security("is_granted('POST_SHOW', post)", statusCode=404, message="Resource not found.")
     */
    public function showAction(Post $post)
    {
    }

.. tip::

    你还可以在控制器类上添加 ``@IsGranted`` 或  ``@Security``
    注释，以控制该类中的 *所有* 动作的访问。

.. _`安全投票器文档`: http://symfony.com/doc/current/cookbook/security/voters_data_permission.html
