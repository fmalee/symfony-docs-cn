表单
=====

由于其广泛的范围和无穷无尽的功能列表，表单是最被滥用的Symfony组件之一。
在本章中，我们将向你展示一些最佳实践，以便你可以快速而有效的使用表单。

创建表单
--------------

.. best-practice::

    将表单定义为PHP类。

Form组件允许你在控制器代码中构建表单。当你不需要在别处复用表单时这是很好用的。
但是为了组织代码和可复用，我们推荐你把每一个表单定义在它们自己的PHP类中::

    namespace App\Form;

    use App\Entity\Post;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\DateTimeType;
    use Symfony\Component\Form\Extension\Core\Type\EmailType;
    use Symfony\Component\Form\Extension\Core\Type\TextareaType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class PostType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('title')
                ->add('summary', TextareaType::class)
                ->add('content', TextareaType::class)
                ->add('authorEmail', EmailType::class)
                ->add('publishedAt', DateTimeType::class)
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'data_class' => Post::class,
            ]);
        }
    }

.. best-practice::

    把表单类型（form type）类放到 ``App\Form`` 命名空间下，
    除非你使用了其他的表单定制类，比如数据转换器。

要使用该类，使用 ``createForm()`` 并传递进完全限定(fully qualified)的类名::

    // ...
    use App\Form\PostType;

    // ...
    public function new(Request $request)
    {
        $post = new Post();
        $form = $this->createForm(PostType::class, $post);

        // ...
    }

表单按钮配置
-------------------------

表单类应该尝试与它们的使用位置无关。这使它们以后更容易重复使用。

.. best-practice::

    在模板中添加表单按钮，而不是在表单类或控制器中。

Symfony表单组件允许你在表单类中把按钮作为字段来添加。这是一种简化渲染表单模板的好方法。
但是，如果直接在表单类中添加按钮，这将会实质上影响该表单的使用范围::

    class PostType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('save', SubmitType::class, ['label' => 'Create Post'])
            ;
        }

        // ...
    }

这个表单 *也许* 被设计为创建贴子用，但如果你希望复用它来编辑贴子，那么按钮标签就会出错。
取而代之，一些开发者在控制器中配置按钮::

    namespace App\Controller\Admin;

    use App\Entity\Post;
    use App\Form\PostType;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Form\Extension\Core\Type\SubmitType;
    use Symfony\Component\HttpFoundation\Request;

    class PostController extends AbstractController
    {
        // ...

        public function new(Request $request)
        {
            $post = new Post();
            $form = $this->createForm(PostType::class, $post);
            $form->add('submit', SubmitType::class, [
                'label' => 'Create',
                'attr' => ['class' => 'btn btn-default pull-right'],
            ]);

            // ...
        }
    }

这也是一个重要的错误，因为你将表示标记（标签，CSS类等）与纯PHP代码混合在一起。
关注点分离(seperation of concern)永远是值得遵循的最佳实践，
所以应该把这些与视图相关的东西，移动到视图层：

.. code-block:: html+twig

    {{ form_start(form) }}
        {{ form_widget(form) }}

        <input type="submit" class="btn" value="Create"/>
    {{ form_end(form) }}

验证
----------

:ref:`约束 <reference-form-option-constraints>` 选项
允许你将 :doc:`验证约束 </reference/constraints>` 附加到任何表单字段上。
但是，这样做会限制表单验证在其他表单或其映射的对象上的重用。

.. best-practice::

    不要在表单中定义验证约束，而是在表单映射到的对象上定义验证约束。

例如，要验证使用表单编辑的帖子的标题不为空，请在 ``Post`` 对象中添加以下内容::

    // src/Entity/Post.php

    // ...
    use Symfony\Component\Validator\Constraints as Assert;

    class Post
    {
        /**
         * @Assert\NotBlank
         */
        public $title;
    }

渲染表单
------------------

有很多方式可以渲染你的表单，比如用一行代码渲出整个表单，或是独立渲染每一个表单字段。
哪种更合适取决于你所需要的表单自定义程度。

最简单的方法之一——在开发过程中特别有用——就是渲染表单标签并使用 ``form_widget()`` 函数函数来渲染出所有字段：

.. code-block:: twig

    {{ form_start(form, {attr: {class: 'my-form-class'} }) }}
        {{ form_widget(form) }}
    {{ form_end(form) }}

如果你需要精细控制字段的渲染，那么你应去除 ``form_widget(form)`` 函数并手动逐个渲染字段。
参考 :doc:`/form/form_customization` 来了解具体办法，以及 *如何* 能够使用全局主题来控制表单渲染。

处理表单提交
---------------------

在处理表单的提交时，通常遵循着相似的模式::

    public function new(Request $request)
    {
        // 创建表单 ...

        $form->handleRequest($request);

        if ($form->isSubmitted() && $form->isValid()) {
            $entityManager = $this->getDoctrine()->getManager();
            $entityManager->persist($post);
            $entityManager->flush();

            return $this->redirectToRoute('admin_post_show', [
                'id' => $post->getId()
            ]);
        }

        // 渲染模板
    }

我们建议你使用单个动作来完成渲染表单和处理表单提交两件事。
例如，你 *可以* 使用 *仅* 生成表单的 ``new()`` 方法和 *仅* 处理表单提交的 ``create()`` 方法。
这两个方法几乎是完全相同的。所以更省力的办法是让 ``new()`` 处理所有事。

下一章: :doc:`/best_practices/i18n`
