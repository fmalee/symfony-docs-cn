@Template
=========

从4.0版本开始，``@Template`` 注释仅支持Twig（并且 **未**
使用Symfony的Templating组件，即 ``framework`` 配置中没有设置 ``templating`` 项）。

用法
-----

``@Template`` 注释使用一个模板名称来关联一个控制器::

    use Sensio\Bundle\FrameworkExtraBundle\Configuration\Template;

    /**
     * @Template("@SensioBlog/post/show.html.twig")
     */
    public function showAction($id)
    {
        // 获取Post
        $post = ...;

        return array('post' => $post);
    }

使用 ``@Template`` 注释时，控制器应返回一个参数数组以传递给视图，而不是一个 ``Response`` 对象。

.. note::

    如果要以流的形式传输模板，可以使用以下配置进行设置::

        /**
         * @Template(isStreamable=true)
         */
        public function showAction($id)
        {
            // ...
        }


.. tip::
   如果该动作返回一个 ``Response`` 对象，则直接忽略对应的 ``@Template`` 注释。

如果类似上述示例的情况，即模板以控制器和动作名称命名，你甚至可以省略注释值::

    /**
     * @Template
     */
    public function showAction($id)
    {
        // 获取Post
        $post = ...;

        return array('post' => $post);
    }

.. tip::
   子命名空间会使用下划线来转换。例如
   ``Sensio\BlogBundle\Controller\UserProfileController::showDetailsAction()``
   动作将解析为 ``@SensioBlog/user_profile/show_details.html.twig``。

如果传递给模板的唯一参数是方法参数，则可以使用 ``vars`` 属性，而不用返回一个数组。
这在与 ``@ParamConverter`` :doc:`注释
<converters>` 联合使用时非常有用::

    /**
     * @ParamConverter("post", class="SensioBlogBundle:Post")
     * @Template("@SensioBlog/post/show.html.twig", vars={"post"})
     */
    public function showAction(Post $post)
    {
    }

得益于约定，上面的例子相当于::

    /**
     * @Template(vars={"post"})
     */
    public function showAction(Post $post)
    {
    }

如果一个方法返回 ``null`` 并且未定义 ``vars``
属性，则该方法的所有参数都会自动传递给模板，所以该注释可以更简洁::

    /**
     * @Template
     */
    public function showAction(Post $post)
    {
    }
