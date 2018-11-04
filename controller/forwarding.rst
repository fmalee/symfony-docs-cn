.. index::
    single: Controller; Forwarding

如何将请求转发给另一个控制器
=============================================

虽然不常见，但你可以使用
:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\AbstractController::forward`
方法在内部转发到另一个控制器。
这不是重定向用户的浏览器，而是创建一个“内部”子请求并调用定义的控制器。
``forward()`` 方法返回 *那个* 控制器返回的
:class:`Symfony\\Component\\HttpFoundation\\Response` 对象::

    public function index($name)
    {
        $response = $this->forward('App\Controller\OtherController::fancy', array(
            'name'  => $name,
            'color' => 'green',
        ));

        // ... 进一步修改响应或直接返回

        return $response;
    }

传递给该方法的数组成为目标控制器的参数。
目标控制器方法可能如下所示::

    public function fancy($name, $color)
    {
        // ... 创建并返回一个Response对象
    }

就像为路由创建控制器一样，``fancy()`` 方法的参数顺序无关紧要：匹配是通过名称完成的。
