.. index::
    single: Forms; Changing the action and method

如何更改表单的动作和方法
=============================================

默认情况下，表单将通过HTTP POST请求提交到渲染表单的同一URL。
有时你想要更改这些参数。你可以通过几种不同的方式完成此操作。

如果你使用 :class:`Symfony\\Component\\Form\\FormBuilder`
构建表单，则可以使用 ``setAction()`` 和 ``setMethod()`` 方法：

.. configuration-block::

    .. code-block:: php-symfony

        // src/Controller/DefaultController.php
        namespace App\Controller;

        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
        use Symfony\Component\Form\Extension\Core\Type\DateType;
        use Symfony\Component\Form\Extension\Core\Type\SubmitType;
        use Symfony\Component\Form\Extension\Core\Type\TextType;

        class DefaultController extends AbstractController
        {
            public function new()
            {
                // ...

                $form = $this->createFormBuilder($task)
                    ->setAction($this->generateUrl('target_route'))
                    ->setMethod('GET')
                    ->add('task', TextType::class)
                    ->add('dueDate', DateType::class)
                    ->add('save', SubmitType::class)
                    ->getForm();

                // ...
            }
        }

    .. code-block:: php-standalone

        use Symfony\Component\Form\Forms;
        use Symfony\Component\Form\Extension\Core\Type\DateType;
        use Symfony\Component\Form\Extension\Core\Type\FormType;
        use Symfony\Component\Form\Extension\Core\Type\SubmitType;
        use Symfony\Component\Form\Extension\Core\Type\TextType;

        // ...

        $formFactoryBuilder = Forms::createFormFactoryBuilder();

        // Form factory builder configuration ...

        $formFactory = $formFactoryBuilder->getFormFactory();

        $form = $formFactory->createBuilder(FormType::class, $task)
            ->setAction('...')
            ->setMethod('GET')
            ->add('task', TextType::class)
            ->add('dueDate', DateType::class)
            ->add('save', SubmitType::class)
            ->getForm();

.. note::

    此示例假定你已创建一个指向处理该表单的控制器的 ``target_route`` 路由。

使用一个表单类型类时，可以将动作和方法作为表单选项传递：

.. configuration-block::

    .. code-block:: php-symfony

        // src/Controller/DefaultController.php
        namespace App\Controller;

        use App\Form\TaskType;
        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

        class DefaultController extends AbstractController
        {
            public function new()
            {
                // ...

                $form = $this->createForm(TaskType::class, $task, [
                    'action' => $this->generateUrl('target_route'),
                    'method' => 'GET',
                ]);

                // ...
            }
        }

    .. code-block:: php-standalone

        use App\Form\TaskType;
        use Symfony\Component\Form\Forms;

        $formFactoryBuilder = Forms::createFormFactoryBuilder();

        // Form factory builder configuration ...

        $formFactory = $formFactoryBuilder->getFormFactory();

        $form = $formFactory->create(TaskType::class, $task, [
            'action' => '...',
            'method' => 'GET',
        ]);

最后，你可以通过它们传递给 ``form()`` 或 ``form_start()`` 辅助方法来重写模板中的动作和方法：

.. code-block:: twig

    {# templates/default/new.html.twig #}
    {{ form_start(form, {'action': path('target_route'), 'method': 'GET'}) }}

.. note::

    如果表单的方法不是GET或POST，而是PUT、PATCH或DELETE，Symfony将插入一个名为 ``_method``
    的隐藏字段来存储该方法。
    表单将在正常的POST请求中提交，但Symfony的路由器能够检测 ``_method`` 参数并将其解释为PUT、PATCH或DELETE请求。
    请参阅 :ref:`configuration-framework-http_method_override` 选项。
