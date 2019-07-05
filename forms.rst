.. index::
   single: Forms

表单
=====

.. admonition:: Screencast
    :class: screencast

    更喜欢视频教程? 可以观看 `Symfony Forms screencast series`_ 系列录像.

对一个Web开发者来说，处理HTML表单是一个最为普通又极具挑战的任务。
Symfony整合了一个Form组件，帮助你处理表单。
在本章，你将从零开始创建一个复杂的表单，学习表单类库中的重要功能。

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令以在使用表单功能之前安装它：

.. code-block:: terminal

    $ composer require symfony/form

.. note::

    Symfony的Form组件是一个独立的类库，你可以在Symfony项目之外使用它。
    参考 :doc:`Form组件文档 </components/form>` 以了解更多。

.. index::
   single: Forms; Create a simple form

创建一个简单的表单
----------------------

假设你正在构建一个简单的待办事项列表，来显示一些“任务”。
你需要创建一个表单来让你的用户编辑和创建任务。
在这之前，先来看看 ``Task`` 类，它可呈现和存储一个单一任务的数据::

    // src/Entity/Task.php
    namespace App\Entity;

    class Task
    {
        protected $task;
        protected $dueDate;

        public function getTask()
        {
            return $this->task;
        }

        public function setTask($task)
        {
            $this->task = $task;
        }

        public function getDueDate()
        {
            return $this->dueDate;
        }

        public function setDueDate(\DateTime $dueDate = null)
        {
            $this->dueDate = $dueDate;
        }
    }

这个类是一个“普通的PHP对象”，因为到目前为止，它没有和Symfony互动也没有引用其它类库。
它只是一个普通的PHP对象，直接解决了 *你* 的应用内部的问题（即需要在应用中代表(represent)任务）。
到本文结束时，你将能够将数据提交到一个 ``Task`` 实例（通过HTML表单），验证其数据并将其持久花到数据库中。

.. index::
   single: Forms; Create a form in a controller

构建表单
~~~~~~~~~~~~~~~~~

现在你已经创建了一个 ``Task`` 类，下一步就是创建和渲染一个真正的HTML表单了。
在Symfony中，这是通过构建一个表单对象并将其渲染到模版来完成的。
现在，在控制器里即可完成所有这些::

    // src/Controller/TaskController.php
    namespace App\Controller;

    use App\Entity\Task;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Form\Extension\Core\Type\DateType;
    use Symfony\Component\Form\Extension\Core\Type\SubmitType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\HttpFoundation\Request;

    class TaskController extends AbstractController
    {
        public function new(Request $request)
        {
            // 创建一个task对象，为了方便演示，同时赋予它一些虚拟数据
            $task = new Task();
            $task->setTask('Write a blog post');
            $task->setDueDate(new \DateTime('tomorrow'));

            $form = $this->createFormBuilder($task)
                ->add('task', TextType::class)
                ->add('dueDate', DateType::class)
                ->add('save', SubmitType::class, ['label' => 'Create Task'])
                ->getForm();

            return $this->render('task/new.html.twig', [
                'form' => $form->createView(),
            ]);
        }
    }

.. tip::

    这个例子展示了如何直接在控制器中构建你的表单。
    后面的  ":ref:`form-creating-form-classes`" 中，
    你将使用一个独立的类来构建表单，建议在表单可重用时使用该方法。

创建表单需要相对较少的代码，因为Symfony表单对象是使用“表单构建器”构建的。
表单构建器的目的是允许你编写简单的表单“指令(recipes)”，并让它完成实际构建表单的所有繁重工作。

在此示例中，你已向表单添加了两个字段 -- ``task`` 和 ``dueDate``
-- 对应于 ``Task`` 类中的 ``task`` 和 ``dueDate`` 属性。
你还为每个字段分配了一个“类型”（例如 ``TextType`` 和 ``DateType``），用其完全限定的类名表示。
除此之外，它还决定为该字段渲染哪个HTML表单标签。

最后，你添加了一个带有自定义标签的提交按钮以向服务器提交表单。

Symfony​​附带了许多内置类型，它们将被简短地介绍（见下面的内置表单类型）。
Symfony开箱附带许多内置类型，它们将在稍后讨论（参见 :ref:`forms-type-reference`）。

.. index::
  single: Forms; Basic template rendering

渲染表单
~~~~~~~~~~~~~~~~~~

表单创建之后，下一步就是渲染它。
这是通过传递一个特殊的表单“视图”对象（注意上例控制器中的
``$form->createView()`` 方法）到你的模板，并通过一系列的
:ref:`表单辅助函数 <reference-form-twig-functions>` 来实现的：

.. code-block:: twig

    {# templates/task/new.html.twig #}
    {{ form(form) }}

.. image:: /_images/form/simple-form.png
    :align: center

仅此而已！:ref:`form() 函数 <reference-forms-twig-form>`
会渲染所有的字段，*还有* ``<form>`` 的开始和结束标签。默认情况下，表单方法是
``POST``，目标URL与显示表单的URL相同。

尽管如此，它并不是很灵活。通常，你需要更多地控制整个表单或其某些字段的外观。
Symfony提供了几种方法：

* 如果你的应用使用CSS框架（如Bootstrap或Foundation），请使用任意的
  :ref:`内置表单主题 <symfony-builtin-forms>`，以使所有表单与应用其余部分的样式相匹配;
* 如果你只想自定义应用的几个字段或几种表单，请阅读
  :doc:`如何自定义表单渲染 </form/form_customization>` 一文;
* 如果要以相同的方式自定义所有表单，请创建
  :doc:`Symfony表单主题 </form/form_themes>` （基于任何内置主题或从头开始）。

在继续下去之前，请注意，为什么渲染出来的 ``task`` 输入框中有一个来自 ``$task`` 对象的 ``task`` 属性值（即“Write a blog post”）。
这是表单的第一个任务：从一个对象中获取数据并把它转换成一种适当的格式，以便在HTML表单中被渲染。

.. tip::

    表单系统足够智能，它们通过 ``getTask()`` 和 ``setTask()`` 方法来访问 ``Task`` 类中受保护的 ``task`` 属性。
    除非是公共属性，否则 *必须* 有一个 "getter" 和 "setter" 方法被定义，以便表单组件能从这些属性中获取和写入数据。
    对于布尔型的属性，你可以使用一个 "isser" 和 "hasser" 方法（如 ``isPublished()`` 和 ``hasReminder()`` 来替代getter方法（``getPublished()`` 和 ``getReminder()``）。

.. index::
  single: Forms; Handling form submissions

.. _form-handling-form-submissions:

处理表单提交
~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，表单会将POST请求提交回渲染它的同一个控制器。

此处，表单的第二个任务就是把用户提交的数据传回到一个对象的属性之中。
要做到这一点，用户提交的数据必须写入表单对象才行。向控制器中添加以下功能::

    // ...
    use Symfony\Component\HttpFoundation\Request;

    public function new(Request $request)
    {
        // 直接设置一个全新的$task对象（删除了虚拟数据）
        $task = new Task();

        $form = $this->createFormBuilder($task)
            ->add('task', TextType::class)
            ->add('dueDate', DateType::class)
            ->add('save', SubmitType::class, ['label' => 'Create Task'])
            ->getForm();

        $form->handleRequest($request);

        if ($form->isSubmitted() && $form->isValid()) {
            // $form->getData() 持有提交过来的值
            // 但是，原始的 `$task` 变量也已被更新了
            $task = $form->getData();

            // ... 一些操作，比如把任务存到数据库中
            // 例如，如果Tast对象是一个Doctrine实体，保存它！
            // $entityManager = $this->getDoctrine()->getManager();
            // $entityManager->persist($task);
            // $entityManager->flush();

            return $this->redirectToRoute('task_success');
        }

        return $this->render('task/new.html.twig', [
            'form' => $form->createView(),
        ]);
    }

.. caution::

    注意 ``createView()`` 方法应该在 ``handleRequest()`` 被调用 *之后* 再调用。
    否则，针对 ``*_SUBMIT`` 表单事件的修改，将不会应用到视图层(比如验证的错误信息)。

控制器在处理表单时遵循的是一个通用模式（common pattern），它有三个可能的途径：

#. 当浏览器初始加载一个页面时，表单被创建和渲染。
   :method:`Symfony\\Component\\Form\\FormInterface::handleRequest` 意识到表单没有被提交进而什么都不做。
   如果表单未被提交，:method:`Symfony\\Component\\Form\\FormInterface::isSubmitted` 返回 ``false``;

#. 当用户提交表单时，:method:`Symfony\\Component\\Form\\FormInterface::handleRequest`
   会识别这个动作并立即将提交的数据写入到 ``$task`` 对象的 ``task`` 和 ``dueDate`` 属性。
   然后验证该对象。如果它是无效的（验证在下一章），
   :method:`Symfony\\Component\\Form\\FormInterface::isValid` 会返回 ``false``，
   进而表单被再次渲染，只是这次附加了验证错误;

#. 当用户以合法数据提交表单的时，提交的数据会被再次写入到表单，但这一次
   :method:`Symfony\\Component\\Form\\FormInterface::isValid` 返回 ``true``。
   在把用户重定向到其他一些页面之前（如一个“谢谢”或“成功”的页面），
   你有机会用 ``$task`` 对象来进行某些操作（比如把它持久化到数据库）。

   .. note::

      表单成功提交之后的将用户重定向，是为了防止用户通过浏览器的“刷新”按钮重复提交数据。

.. seealso::

    如果你需要精确地控制何时表单被提交，或哪些数据被传给表单，你可以使用 :method:`Symfony\\Component\\Form\\FormInterface::submit`。更多信息请参考
    :ref:`form-call-submit-directly`。

.. index::
   single: Forms; Validation

.. _forms-form-validation:

表单验证
---------------

在上一节中，你了解了附带了有效或无效数据的表单是如何被提交的。
在Symfony中，验证环节是在底层对象中进行的（例如 ``Task``）。
换句话说，问题不在于“表单”是否有效，而是在表单将提交的数据应用于 ``$task`` 对象后，该对象是否有效。
调用 ``$form->isValid()`` 是一个快捷方式，它询问 ``$task`` 对象是否获得了合法数据。

在使用验证之前，请在应用中添加对它的支持：

.. code-block:: terminal

    $ composer require symfony/validator

验证是通过把一组规则（称之为“约束(constraints)”）添加到一个类中来完成的。
我们给 ``Task`` 类添加约束，使 ``task`` 属性不能为空，  ``dueDate`` 字段不为空且必须是一个有效的 \DateTime 对象。

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Task.php
        namespace App\Entity;

        use Symfony\Component\Validator\Constraints as Assert;

        class Task
        {
            /**
             * @Assert\NotBlank
             */
            public $task;

            /**
             * @Assert\NotBlank
             * @Assert\Type("\DateTime")
             */
            protected $dueDate;
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\Task:
            properties:
                task:
                    - NotBlank: ~
                dueDate:
                    - NotBlank: ~
                    - Type: \DateTime

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                https://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\Task">
                <property name="task">
                    <constraint name="NotBlank"/>
                </property>
                <property name="dueDate">
                    <constraint name="NotBlank"/>
                    <constraint name="Type">\DateTime</constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/Task.php
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Constraints\Type;
        use Symfony\Component\Validator\Mapping\ClassMetadata;

        class Task
        {
            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('task', new NotBlank());

                $metadata->addPropertyConstraint('dueDate', new NotBlank());
                $metadata->addPropertyConstraint(
                    'dueDate',
                    new Type(\DateTime::class)
                );
            }
        }

就是这样！如果你现在重新以非法数据提交表单，你将会看到相应的错误被输出到表单。

验证是Symfony一个非常强大的功能，它拥有自己的 :doc:`专属文档 </validation>`。

.. _forms-html5-validation-disable:

.. sidebar:: HTML5 验证

    自HTML5诞生后，许多浏览器都原生支持了客户端的验证约束。
    最常用的验证之激活方式，是在一个必填字段上渲染一个 ``required`` 属性。
    对于支持HTML5的浏览器来说，如果用户尝试提交一个空字段到表单时，会有一条浏览器原生信息显示出来。

    生成出来的表单充分利用了这个新功能，通过添加一些有意义的HTML属性来触发验证。
    客户端验证，也可通过把 ``novalidate`` 属性添加到 ``form`` 标签，或是把 ``formnovalidate`` 添加到提交标签来关闭之。
    这在你想要测试服务器端的验证规则却被浏览器端阻止，例如，在提交空白字段时，就非常有用。

    .. code-block:: twig

        {# templates/task/new.html.twig #}
        {{ form_start(form, {'attr': {'novalidate': 'novalidate'}}) }}
        {{ form_widget(form) }}
        {{ form_end(form) }}

.. index::
   single: Forms; Built-in field types

.. _forms-type-reference:

内置的字段类型
--------------------

Symfony标配了大量的字段类型，涵盖了你所能遇到的全部常规表单字段和数据类型：

.. include:: /reference/forms/types/map.rst.inc

你也可以定义自己的字段类型。参考 :doc:`/form/create_custom_field_type`。

.. index::
   single: Forms; Field type options

字段类型选项
~~~~~~~~~~~~~~~~~~

每一种字段类型都有一定数量的选项用于配置。
比如， ``dueDate`` 字段当前被渲染成3个选择框。
而 :doc:`DateType </reference/forms/types/date>` 字段可以被配置渲染成一个单一的文本框
（用户可以输入字符串作为日期）::

    ->add('dueDate', DateType::class, ['widget' => 'single_text'])

.. image:: /_images/form/simple-form-2.png
    :align: center

每一种字段类型都有一系列不同的选项用于个性配置。
关于字段类型的细节都可以在每种类型的文档中找到。

.. sidebar:: ``required`` 选项

    最常用的是 ``required`` 选项，它可以应用于任何字段。默认情况下它被设置为 ``true``。
    这就意味着支持HTML5的浏览器会使用客户端验证来判断字段是否为空。
    如果你不想需要这种行为，要么 :ref:`关闭 HTML5 验证 <forms-html5-validation-disable>`，
    要么把字段的 ``required`` 选项设置为 ``false``::

        ->add('dueDate', DateType::class, [
            'widget' => 'single_text',
            'required' => false
        ])

    要注意设置 ``required`` 为 ``true`` 并 *不* 意味着服务器端验证会被使用。
    换句话说，如果用户提交一个空值（blank）到该字段（比如在老旧浏览器中，或是使用web service时），
    这个空值当被作为有效值予以采纳，除非你使用了Symfony的 ``NotBlank`` 或者 ``NotNull`` 验证约束。

    也就是说，``required`` 选项是很 "nice"，但是服务端验证却应该 *始终* 使用。

.. sidebar:: ``label`` 选项

    表单字段可以使用 ``label`` 选项来设置表单字段的标签，它适用于任何字段::

        ->add('dueDate', DateType::class, [
            'widget' => 'single_text',
            'label'  => 'Due Date',
        ])

    字段的标签也可以在模版渲染表单时进行设置，详情见下文。
    如果你不需要把标签关联到你的输入框，你可以设置该选项值为 ``false``。

    .. tip::

        默认情况下，必需字段的 ``<label>`` 标签使用 ``required``
        CSS类渲染，因此你可以应用这些CSS样式来为必填字段显示一个星号：

        .. code-block:: css

            label.required:before {
                content: "*";
            }

.. index::
   single: Forms; Field type guessing

.. _forms-field-guessing:

字段类型猜测
-------------------

现在你已经添加了验证元数据到 ``Task`` 类，Symfony对于你的字段已有所了解。
如果你允许，Symfony可以“猜到”你的字段类型并帮你设置好。
在下面的例子中，Symfony可以根据验证规则猜测到 ``task`` 字段是一个标准的 ``TextType`` 字段，
``dueDate`` 是 ``DateType`` 字段::

    public function new()
    {
        $task = new Task();

        $form = $this->createFormBuilder($task)
            ->add('task')
            ->add('dueDate', null, ['widget' => 'single_text'])
            ->add('save', SubmitType::class)
            ->getForm();
    }

当你省略了 ``add()`` 方法的第二个参数（或者你输入 ``null``)时，“猜测”机制会被激活。
如果你输入一个选项数组作为第三个参数（比如上面的 ``dueDate``），这些选项将应用于被猜测的字段。

.. caution::

    如果你的表单使用了一个特定的验证组，则字段类型“guesser”在猜测字段类型时仍将考虑 *所有* 验证约束
    （包括不属于正在使用的验证组的约束）。

.. index::
   single: Forms; Field type guessing

字段类型选项的猜测
~~~~~~~~~~~~~~~~~~~~~~~~~~~

除了猜测字段类型，Symfony还可尝试猜出字段选项的正确值。

.. tip::

    当这些选项被设置时，字段将以特殊的HTML属性进行渲染，以用于HTML5的客户端验证。
    但是，它们不会在服务端生成等效的验证规则（如 ``Assert\Length``）。
    所以你需要手动地添加这些服务器端的规则，然后这些字段类型的选项接下来可以根据这些规则被猜出来。

``required``
    ``required`` 选项可以基于验证规则 (如，该字段是否为 ``NotBlank`` 或 ``NotNull``)
    或者是Doctrine的元数据 (如，该字段是否为 ``nullable``)而被猜出来。
    这非常有用，因为你的客户端验证将自动匹配到你的验证规则。

``maxlength``
    如果字段是某些文本型字段，那么 ``maxlength`` 选项可以基于验证约束 (字段是否应用了 ``Length`` 或 ``Range``) 或者是Doctrine元数据 (通过该字段的长度) 而被猜出来。

.. caution::

    这些字段选项 *仅* 在你使用Symfony进行类型猜测时
    （即，传入 ``null`` 作为 ``add()`` 方法的第二个参数或忽略该参数）才会进行猜测。

如果你希望改变某个被猜出来的值，可以在字段类型的选项数组中传入此项进行重写::

    ->add('task', null, ['attr' => ['maxlength' => 4]])

.. index::
   single: Forms; Creating form classes

.. _form-creating-form-classes:

创建表单类
---------------------

正如你看到的，表单可以直接在控制器中被创建和使用。
然而，一个更好的做法，是在一个单独的PHP类中创建表单。
它能在你程序中的任何地方复用。
创建一个新类，它将包含构建任务表单的逻辑::

    // src/Form/TaskType.php
    namespace App\Form;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\SubmitType;
    use Symfony\Component\Form\FormBuilderInterface;

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('task')
                ->add('dueDate', null, ['widget' => 'single_text'])
                ->add('save', SubmitType::class)
            ;
        }
    }

这个新类包含了创建任务表单所需要的方方面面。它可用于在控制器中创建表单::

    // src/Controller/TaskController.php
    use App\Form\TaskType;

    public function new()
    {
        $task = ...;
        $form = $this->createForm(TaskType::class, $task);

        // ...
    }

把表单逻辑置于它自己的类中，可以让表单在你的项目任何地方复用。
这是创建表单最好的方式，但是决定权在你。

.. _form-data-class:

.. sidebar:: 设置 ``data_class``

    每个表单都需要知道“持有底层数据的类”的名称（如 ``App\Entity\Task`` )。
    通常情况下，这是根据传入 ``createForm()`` 方法的第二个参数来猜测的（例如 ``$task``）。
    以后，当你开始嵌入表单时，这便不再够用。
    因此，虽然并非总是必要，但通过添加下面代码到你的表单类型类中，以显式地指定 ``data_class`` 选项是一个好办法::

        // src/Form/TaskType.php
        use App\Entity\Task;
        use Symfony\Component\OptionsResolver\OptionsResolver;

        // ...
        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'data_class' => Task::class,
            ]);
        }

.. tip::

    当把表单映射成对象时，所有的字段都将被映射。表单中的任何字段如果在映射对象上“不存在”，都会抛出异常。

    当你需要在表单中使用附加字段（如，一个 “你是否同意这些声明？”的复选框）
    而这个字段不需要被映射到底层对象时，你需要设置 ``mapped`` 选项为 ``false``::

        use Symfony\Component\Form\FormBuilderInterface;

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('task')
                ->add('dueDate')
                ->add('agreeTerms', CheckboxType::class, ['mapped' => false])
                ->add('save', SubmitType::class)
            ;
        }

    另外，若表单的任何字段未包含在提交过来的数据中，那么这些字段将被显式设置为 ``null``。

    我们可以在控制器中访问字段数据::

        $form->get('agreeTerms')->getData();

    此外，还可以直接修改未映射的字段的数据::

        $form->get('agreeTerms')->setData(true);


.. note::

    表单名称是从类型类名称自动生成的。如果要修改它，请使用
    :method:`Symfony\\Component\\Form\\FormFactoryInterface::createNamed` 方法::

        // src/Controller/DefaultController.php
        use App\Form\TaskType;
        use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

        class DefaultController extends AbstractController
        {
            public function newAction()
            {
                $task = ...;
                $form = $this->get('form.factory')->createNamed('name', TaskType::class, $task);

                // ...
            }
        }

    你甚至可以通过将名称设置为空字符串来完全取消命名。

总结
-------

构建表单时，请记住表单的第一个目标是将数据从对象（``Task``）转换为HTML表单，以便用户可以修改该数据。
表单的第二个目标是获取用户提交的数据并将其重新应用于该对象。

在表单系统中还有很多东西需要学习，它还有很多 *强大* 的技巧。

扩展阅读
------------------------

.. toctree::
    :maxdepth: 1
    :glob:

    /form/*
    /controller/upload_file
    /reference/forms/types
    /security/csrf

.. _`Symfony Form component`: https://github.com/symfony/form
.. _`DateTime`: https://php.net/manual/en/class.datetime.php
.. _`Symfony Forms screencast series`: https://symfonycasts.com/screencast/symfony-forms
