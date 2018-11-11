.. index::
    single: DependencyInjection; Request
    single: Service Container; Request

如何从服务容器中检索请求
======================================================

每当你需要在服务中访问当前请求时，你可以将其作为参数添加到需要请求的方法中或注入 ``request_stack`` 服务并通过调用
:method:`Symfony\\Component\\HttpFoundation\\RequestStack::getCurrentRequest`
方法来访问 ``Request`` ::

    namespace App\Newsletter;

    use Symfony\Component\HttpFoundation\RequestStack;

    class NewsletterManager
    {
        protected $requestStack;

        public function __construct(RequestStack $requestStack)
        {
            $this->requestStack = $requestStack;
        }

        public function anyMethod()
        {
            $request = $this->requestStack->getCurrentRequest();
            // ... 使用该请求做些事情
        }

        // ...
    }

现在，和任何正常服务一样注入 ``request_stack``。
如果你使用的是
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，则会通过自动装配自动进行。

.. tip::

    在控制器中，你可以通过将 ``Request`` 对象作为参数传递给动作方法来获取该对象。
    请参阅 :ref:`controller-request-argument` 以获取更详细的信息。
