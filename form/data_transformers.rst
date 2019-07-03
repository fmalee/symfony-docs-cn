.. index::
   single: Form; Data transformers

如何使用数据转换器
============================

数据转换器用于将字段的数据转换为可以在表单中显示的格式（并在提交时转换回来）。它们已经在内部用于许多字段类型。
例如，:doc:`DateType </reference/forms/types/date>` 字段可以渲染为一个格式化的 ``yyyy-MM-dd`` 输入文本框。
在内部，数据转换器将 ``DateTime`` 字段的起始值转换为 ``yyyy-MM-dd``
字符串以渲染表单，然后在提交时再转换回为一个 ``DateTime`` 对象。

.. caution::

    当一个表单字段的 ``inherit_data`` 选项设置为 ``true`` 时，数据转换器将不会应用于该字段。

.. seealso::

    如果你需要将一个值映射到表单字段并返回，而不是转换该值的表示，则应使用一个数据映射器。
    请参阅 :doc:`/form/data_mappers`。

.. _simple-example-sanitizing-html-on-user-input:

简单示例：将用户输入的字符串标签转换为数组
--------------------------------------------------------------------

假设你有一个带有 ``text`` 类型的标签的任务表单::

    // src/Form/Type/TaskType.php
    namespace App\Form\Type;

    use App\Entity\Task;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    // ...
    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('tags', TextType::class);
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'data_class' => Task::class,
            ]);
        }

        // ...
    }

``tags`` 在内部存储为数组形式，展示给用户时则是一个方便编辑的以逗号分隔的字符串。

这是将一个自定义数据转换器附加到 ``tags`` 字段的 *最佳* 时机。最简单的方法是使用
:class:`Symfony\\Component\\Form\\CallbackTransformer` 类::

    // src/Form/Type/TaskType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\CallbackTransformer;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormBuilderInterface;
    // ...

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('tags', TextType::class);

            $builder->get('tags')
                ->addModelTransformer(new CallbackTransformer(
                    function ($tagsAsArray) {
                        // 将数组转换为一个字符串
                        return implode(', ', $tagsAsArray);
                    },
                    function ($tagsAsString) {
                        // 将字符串转换回一个数组
                        return explode(', ', $tagsAsString);
                    }
                ))
            ;
        }

        // ...
    }

``CallbackTransformer`` 使用两个回调函数作为参数。第一个回调将原始值转换为将用于渲染字段的格式。
第二个回调是则相反：它将提交的值转换回你将在代码中使用的格式。

.. tip::

    ``addModelTransformer()`` 方法接受 *任何* 实现了
    :class:`Symfony\\Component\\Form\\DataTransformerInterface` 的对象
    - 因此你可以创建自己的类，而不是将所有逻辑放在表单中（请参阅下一节）。

你也可以稍微更改格式，以在添加字段时添加转换器::

    use Symfony\Component\Form\Extension\Core\Type\TextType;

    $builder->add(
        $builder
            ->create('tags', TextType::class)
            ->addModelTransformer(...)
    );

更难的例子：将问题编号转换为问题实体
-----------------------------------------------------------------

假设你具有从Task实体到Issue实体的多对一关系（即每个Task都有一个与其相关Issue的可选外键）。
添加一个包含所有问题的列表框最终可能会 *很* 长并且需要很长时间才能加载。
相反，你决定要添加一个文本框，用户可以在其中输入问题编号。

首先像往常一样设置文本字段::

    // src/Form/Type/TaskType.php
    namespace App\Form\Type;

    use App\Entity\Task;
    use Symfony\Component\Form\Extension\Core\Type\TextareaType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    // ...
    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('description', TextareaType::class)
                ->add('issue', TextType::class)
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'data_class' => Task::class,
            ]);
        }

        // ...
    }

不错的开始！但是如果你停在这里并提交了表单，那么Task的 ``issue`` 属性就是一个字符串（例如“55”）。
那么如何在提交时将其转换为一个 ``Issue`` 实体？

创建转换器
~~~~~~~~~~~~~~~~~~~~~~~~

你可以像之前一样使用 ``CallbackTransformer``。
但由于这个用例有点复杂，因此可以创建一个新的转换器类以使 ``TaskType`` 表单类保持简洁。

创建一个 ``IssueToNumberTransformer`` 类：它将负责问题编号和 ``Issue`` 对象之间的转换::

    // src/Form/DataTransformer/IssueToNumberTransformer.php
    namespace App\Form\DataTransformer;

    use App\Entity\Issue;
    use Doctrine\ORM\EntityManagerInterface;
    use Symfony\Component\Form\DataTransformerInterface;
    use Symfony\Component\Form\Exception\TransformationFailedException;

    class IssueToNumberTransformer implements DataTransformerInterface
    {
        private $entityManager;

        public function __construct(EntityManagerInterface $entityManager)
        {
            $this->entityManager = $entityManager;
        }

        /**
         * 将对象（issue）转换为字符串（number）。
         *
         * @param  Issue|null $issue
         * @return string
         */
        public function transform($issue)
        {
            if (null === $issue) {
                return '';
            }

            return $issue->getId();
        }

        /**
         * 将字符串（number）转换为对象（issue）。
         *
         * @param  string $issueNumber
         * @return Issue|null
         * @throws TransformationFailedException 如果对象 (issue) 未找到.
         */
        public function reverseTransform($issueNumber)
        {
            // 没有问题编号? 它是可选的，所以没问题。
            if (!$issueNumber) {
                return;
            }

            $issue = $this->entityManager
                ->getRepository(Issue::class)
                // 使用其ID查询问题
                ->find($issueNumber)
            ;

            if (null === $issue) {
                // 触发一个验证错误
                // 此消息不会显示给用户
                // 请参阅 invalid_message 选项
                throw new TransformationFailedException(sprintf(
                    'An issue with number "%s" does not exist!',
                    $issueNumber
                ));
            }

            return $issue;
        }
    }

就像在第一个例子中一样，一个转换器有两个方向。
``transform()`` 方法负责将在代码中使用的数据转换为可以在表单中渲染的格式（例如，将 ``Issue`` 对象为转换为其 ``id`` 字符串）。
``reverseTransform()`` 方法则相反：它将提交的值转换回你想要的格式（例如，将 ``id`` 转换回 ``Issue`` 对象）。

要触发一个验证错误，请抛出一个
:class:`Symfony\\Component\\Form\\Exception\\TransformationFailedException`。
但是，传递给此异常的消息不会显示给用户。你需要使用 ``invalid_message`` 选项来设置该消息（请参阅下文）。

.. note::

    当 ``null`` 传递给 ``transform()`` 方法时，转换器应该返回它正在转换的类型的等效值（例如，一个空字符串，整数的0，浮点数的0.0）。

使用转换器
~~~~~~~~~~~~~~~~~~~~~

接下来，你需要在 ``TaskType`` 中使用 ``IssueToNumberTransformer`` 对象并将其添加到 ``issue`` 字段中。
这不是问题！添加一个 ``__construct()`` 方法并类型约束该新类::

    // src/Form/Type/TaskType.php
    namespace App\Form\Type;

    use App\Form\DataTransformer\IssueToNumberTransformer;
    use Symfony\Component\Form\Extension\Core\Type\TextareaType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    // ...
    class TaskType extends AbstractType
    {
        private $transformer;

        public function __construct(IssueToNumberTransformer $transformer)
        {
            $this->transformer = $transformer;
        }

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('description', TextareaType::class)
                ->add('issue', TextType::class, [
                    // 数据转换器失败的一个验证消息
                    'invalid_message' => 'That is not a valid issue number',
                ]);

            // ...

            $builder->get('issue')
                ->addModelTransformer($this->transformer);
        }

        // ...
    }

每当转换器抛出异常，就会向用户显示 ``invalid_message``。
你可以使用 ``setInvalidMessage()``
方法在数据转换器中设置最终用户的错误消息，而不是每次都显示相同的消息。它还允许你包含用户值::

    // src/Form/DataTransformer/IssueToNumberTransformer.php
    namespace App\Form\DataTransformer;

    use Symfony\Component\Form\DataTransformerInterface;
    use Symfony\Component\Form\Exception\TransformationFailedException;

    class IssueToNumberTransformer implements DataTransformerInterface
    {
        // ...

        public function reverseTransform($issueNumber)
        {
            // ...

            if (null === $issue) {
                $privateErrorMessage = sprintf('An issue with number "%s" does not exist!', $issueNumber);
                $publicErrorMessage = 'The given "{{ value }}" value is not a valid issue number.';

                $failure = new TransformationFailedException($privateErrorMessage);
                $failure->setInvalidMessage($publicErrorMessage, [
                    '{{ value }}' => $issueNumber,
                ]);

                throw $failure;
            }

            return $issue;
        }
    }

.. versionadded:: 4.3

    Symfony 4.3中引入了 ``setInvalidMessage()`` 方法。

仅此而已！如果你正在使用
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，Symfony将通过
:ref:`自动装配 <services-autowire>` 和 :ref:`自动配置 <services-autoconfigure>`
自动知道将一个``IssueToNumberTransformer`` 实例传递到 ``TaskType``。否则，
:ref:`将表单类注册为服务 <service-container-creating-service>`，并使用 ``form.type``
标签对其进行 :doc:`标记 </service_container/tags>`。

现在，你可以使用你的 ``TaskType``::

    // 例如在一个控制器的某个地方
    $form = $this->createForm(TaskType::class, $task);

    // ...

很酷，你完工了！你的用户将能够在文本字段中输入问题编号，该编号将转换回一个 ``Issue`` 对象。
这意味着，在成功提交后，Form组件将传递一个实际的 ``Issue`` 对象到 ``Task::setIssue()``，而不是一个问题编号。

如果找不到该问题，将为该字段创建一个表单错误，并且可以使用 ``invalid_message`` 字段选项定制它的错误消息。

.. caution::

    添加转换器时要小心。例如，下例是 **错误** 的，因为转换器将应用于整个表单，而不仅仅是这个字段::

        // **错误示范** - 转换器将应用于整个表单
        // 请查看上个例子中的正确代码
        $builder->add('issue', TextType::class)
            ->addModelTransformer($transformer);

.. _using-transformers-in-a-custom-field-type:

创建一个可复用的issue_selector字段
----------------------------------------

在上面的示例中，你将转换器应用于一个普通的 ``text`` 字段。
但是如果你经常进行这种转换，那么最好是
:doc:`创建一个自定义字段类型 </form/create_custom_field_type>`。此操作会自动完成。

首先，创建自定义字段类型类::

    // src/Form/IssueSelectorType.php
    namespace App\Form;

    use App\Form\DataTransformer\IssueToNumberTransformer;
    use Doctrine\Common\Persistence\ObjectManager;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class IssueSelectorType extends AbstractType
    {
        private $transformer;

        public function __construct(IssueToNumberTransformer $transformer)
        {
            $this->transformer = $transformer;
        }

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->addModelTransformer($this->transformer);
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'invalid_message' => 'The selected issue does not exist',
            ]);
        }

        public function getParent()
        {
            return TextType::class;
        }
    }

很好！它将像一个文本字段（``getParent()``）一样操作和渲染，但会自动拥有数据转换器
*以及* 附带一个默认值的 ``invalid_message`` 选项。

只要你使用 :ref:`自动装配 <services-autowire>` 和
:ref:`自动配置 <services-autoconfigure>`，就可以立即开始使用该表单::

    // src/Form/Type/TaskType.php
    namespace App\Form\Type;

    use App\Form\DataTransformer\IssueToNumberTransformer;
    use Symfony\Component\Form\Extension\Core\Type\TextareaType;
    // ...

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('description', TextareaType::class)
                ->add('issue', IssueSelectorType::class)
            ;
        }

        // ...
    }

.. tip::

    如果你没有使用 ``自动装配`` 和 ``自动配置``，请参阅
    :doc:`/form/create_custom_field_type` 以知道如何配置你的 ``IssueSelectorType``。

.. _model-and-view-transformers:

关于模型和视图转换器
---------------------------------

在上面的例子中，该转换器被用作一个“model”变压器。
实际上，有两种不同类型的转换器和三种不同类型的底层数据。

.. image:: /_images/form/data-transformer-types.png
   :align: center

在任何表单中，三种不同类型的数据分别是：

#. **Model data** - 这是你的应用使用的格式的数据（例如一个 ``Issue`` 对象）。
   如果你调用了 ``Form::getData()`` 或 ``Form::setData()``，那么你正在处理“model”数据。

#. **Norm Data** - 这是你的数据的规范化版本，
   通常与“model”数据相同（尽管不在我们的示例中）。它并不常用。

#. **View Data** - 这是用于填充表单字段自身的格式。它也是用户提交的数据的格式。
   当你调用 ``Form::submit($data)`` 时，该 ``$data`` 处于“view”数据格式。

两种不同类型的转换器有助于转换为以下每种类型的数据：

**模型转换器**:
    - ``transform()``: "model"数据 => "norm"数据
    - ``reverseTransform()``: "norm"数据 => "model"数据

**视图转换器**:
    - ``transform()``: "norm"数据 => "view"数据
    - ``reverseTransform()``: "view"数据 => "norm"数据

你需要哪种转换器取决于你的具体情况。

要使用视图转换器，请调用 ``addViewTransformer()``。

那么为什么要使用模型转换器呢？
---------------------------------

在此示例中，该字段是一个 ``text`` 字段，而一个文本字段在“norm”和“view”格式中始终是一个简单、标量的格式。
出于这个原因，最合适的转换器是“模型”转换器，即 *norm* 格式(问题编号字符串)和 *模型* 格式(Issue对象)之间的转换。

转换器之间的区别是微妙的，你应该总是考虑一个字段的“norm”数据应该是什么。
例如，一个 ``text`` 字段的“norm”数据是一个字符串，但是 ``date`` 字段的却是 ``DateTime`` 对象。

.. tip::

    作为一个通用规则，规范化的数据应包含尽可能多的信息。
