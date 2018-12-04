@Cache
======

``@Cache`` 注释可以很容易地定义HTTP缓存头的过期和验证。

HTTP到期策略
--------------------------

``@Cache`` 注释可以很容易地定义HTTP缓存::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Cache;

    /**
     * @Cache(expires="tomorrow", public=true)
     */
    public function indexAction()
    {
    }

你还可以使用一个类上的注释来定义控制器的所有动作的缓存::

    /**
     * @Cache(expires="tomorrow", public=true)
     */
    class BlogController extends Controller
    {
    }

当类配置和方法配置之间存在冲突时，后者会覆盖前者::

    /**
     * @Cache(expires="tomorrow")
     */
    class BlogController extends Controller
    {
        /**
         * @Cache(expires="+2 days")
         */
        public function indexAction()
        {
        }
    }

.. note::

   ``expires`` 属性采用PHP的 ``strtotime()`` 函数来理解任何有效日期。

HTTP验证策略
--------------------------

可以使用 ``lastModified`` 和 ``Etag`` 属性来管理HTTP验证缓存头。
``lastModified`` 添加一个 ``Last-Modified``
标头到响应，而 ``Etag`` 则添加一个 ``Etag`` 标头。

当响应未被修改时，两者都自动触发此逻辑以返回一个304响应（在这个例子中，**未** 调用该控制器）::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Cache;

    /**
     * @Cache(lastModified="post.getUpdatedAt()", Etag="'Post' ~ post.getId() ~ post.getUpdatedAt().getTimestamp()")
     */
    public function indexAction(Post $post)
    {
        // 你的代码
        // 如果是304，则不会被调用
    }

它与以下代码大致相同::

    public function myAction(Request $request, Post $post)
    {
        $response = new Response();
        $response->setLastModified($post->getUpdatedAt());
        if ($response->isNotModified($request)) {
            return $response;
        }

        // 你的代码
    }

.. note::

    HTTP标头的Etag值是使用 ``sha256`` 算法进行散列的表达式的结果。

属性
----------

以下是一个可用的属性及其对应的HTTP标头的列表：

======================================================================= ===================================================================
注释                                                                     响应方法
======================================================================= ===================================================================
``@Cache(expires="tomorrow")``                                          ``$response->setExpires()``
``@Cache(smaxage="15")``                                                ``$response->setSharedMaxAge()``
``@Cache(maxage="15")``                                                 ``$response->setMaxAge()``
``@Cache(maxstale="15")``                                               ``$response->headers->addCacheControlDirective('max-stale', 15)``
``@Cache(vary={"Cookie"})``                                             ``$response->setVary()``
``@Cache(public=true)``                                                 ``$response->setPublic()``
``@Cache(lastModified="post.getUpdatedAt()")``                          ``$response->setLastModified()``
``@Cache(Etag="post.getId() ~ post.getUpdatedAt().getTimestamp()")``    ``$response->setEtag()``
``@Cache(mustRevalidate=true)``                                         ``$response->headers->addCacheControlDirective('must-revalidate')``
======================================================================= ===================================================================

.. note::

    smaxage、maxage和maxstale属性也可以获得具有相对时间格式的字符串（1天，2周，......）。
