.. index::
   single: Form; Events

如何使用表单事件动态修改表单
=================================================

通常情况下，无法静态的创建一个表单。在本文中，你将学习如何根据三种常见用例来自定义表单：

1) :ref:`form-events-underlying-data`

   示例：你有一个“产品”表单，并需要根据正在编辑的底层产品上的数据来修改/添加/删除一个字段。

2) :ref:`form-events-user-data`

   示例：你创建了一个“好友消息”表单，并且需要构建一个下拉菜单，其中仅包含与 *当前* 已认证的用户成为朋友的用户。

3) :ref:`form-events-submitted-data`

   示例：在注册表单上，你有一个“国家”字段和一个应根据“国家”字段中的值动态填充的“州”字段。

如果你希望了解有关表单事件背后的基础知识，可以查看 :doc:`表单事件 </form/events>` 文档。

.. _form-events-underlying-data:

根据底层数据来自定义表单
--------------------------------------------------

在开始动态生成表单之前，请记住一个裸(bare)表单类的外观::

    // src/Form/Type/ProductType.php
    namespace App\Form\Type;

    use App\Entity\Product;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('name');
            $builder->add('price');
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'data_class' => Product::class,
            ]);
        }
    }

.. note::

    如果你还不熟悉此代码的特定部分，则可能需要退一步并在继续之前先参阅 :doc:`表单文档 </forms>`。

暂时假设这个表单使用了一个假想的“Product”类，它只有两个属性（“name”和“price”）。
无论是创建新产品还是编辑现有产品（例如，从数据库中获取的产品），从该类生成的表单将看起来完全相同。

现在假设你不希望用户在创建对象后能够更改 ``name`` 值。
为此，你可以依赖Symfony的 :doc:`EventDispatcher组件 </components/event_dispatcher>`
系统来分析对象中的数据，并根据Product对象的数据来修改表单。
在本文中，你将学习到如何为表单来增加这种级别的灵活性。

向表单类添加事件监听器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

因此，不是直接添加 ``name`` 部件，而是将创建该特定字段的职责委托给一个事件监听器::

    // src/Form/Type/ProductType.php
    namespace App\Form\Type;

    // ...
    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('price');

            $builder->addEventListener(FormEvents::PRE_SET_DATA, function (FormEvent $event) {
                // ... 如果需要，则添加name字段
            });
        }

        // ...
    }

目标是 *仅* 在底层 ``Product`` 对象是新对象时创建一个 ``name`` 字段（例如，尚未持久化到数据库）。
基于此，事件监听器可能如下所示::

    // ...
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...
        $builder->addEventListener(FormEvents::PRE_SET_DATA, function (FormEvent $event) {
            $product = $event->getData();
            $form = $event->getForm();

            // 检查Product对象是不是"新"的
            // 如果没有数据传递给表单，则数据为“null”。
            // 这就应该算是一个新的“产品”
            if (!$product || null === $product->getId()) {
                $form->add('name', TextType::class);
            }
        });
    }

.. note::

    ``FormEvents::PRE_SET_DATA`` 行实际上会解析为 ``form.pre_set_data`` 字符串。
    :class:`Symfony\\Component\\Form\\FormEvents` 服务于组织目的(organizational purpose)。
    这是一个集中化的位置，你可以在其中找到所有可用的各种表单事件。
    你可以通过 :class:`Symfony\\Component\\Form\\FormEvents` 类查看表单事件的完整列表。

添加事件订阅器到表单类
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

为了更好的可复用性，或者如果事件监听器中存在一些重要的逻辑，你还可以将用于创建 ``name``
字段的逻辑移动到一个 :ref:`事件订阅器 <event_dispatcher-using-event-subscribers>` 中::

    // src/Form/EventListener/AddNameFieldSubscriber.php
    namespace App\Form\EventListener;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;

    class AddNameFieldSubscriber implements EventSubscriberInterface
    {
        public static function getSubscribedEvents()
        {
            // 告诉调度器你要监听 form.pre_set_data 事件并且应该调用 preSetData 方法。
            return [FormEvents::PRE_SET_DATA => 'preSetData'];
        }

        public function preSetData(FormEvent $event)
        {
            $product = $event->getData();
            $form = $event->getForm();

            if (!$product || null === $product->getId()) {
                $form->add('name', TextType::class);
            }
        }
    }

很好！现在在表单类中使用它::

    // src/Form/Type/ProductType.php
    namespace App\Form\Type;

    // ...
    use App\Form\EventListener\AddNameFieldSubscriber;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('price');

            $builder->addEventSubscriber(new AddNameFieldSubscriber());
        }

        // ...
    }

.. _form-events-user-data:

如何基于用户数据动态的生成表单
----------------------------------------------------

有时，你希望动态生成表单不仅基于表单中的数据而且还基于其他内容 - 例如来自当前用户的一些数据。
假设你有一个社交网站，网站的用户只能给标记为好友的人发消息。
在这种情况下，消息的“选择列表”应只包含当前用户的好友用户。

创建表单类型
~~~~~~~~~~~~~~~~~~~~~~

使用一个事件监听器，你的表单可能如下所示::

    // src/Form/Type/FriendMessageFormType.php
    namespace App\Form\Type;

    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\TextareaType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;

    class FriendMessageFormType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('subject', TextType::class)
                ->add('body', TextareaType::class)
            ;
            $builder->addEventListener(FormEvents::PRE_SET_DATA, function (FormEvent $event) {
                // ... 添加一个当前用户的好友的选择列表
            });
        }
    }

现在的问题是获取当前用户并创建一个仅包含该用户的好友的选择字段。
这可以通过将 ``Security`` 服务注入该表单类型来解决，它可以让你获取到当前的用户对象::

    use Symfony\Component\Security\Core\Security;
    // ...

    class FriendMessageFormType extends AbstractType
    {
        private $security;

        public function __construct(Security $security)
        {
            $this->security = $security;
        }

        // ....
    }

自定义表单类型
~~~~~~~~~~~~~~~~~~~~~~~~~

现在你已掌握了所有的基础知识，你可以使用安全助手的功能来填充监听器的逻辑::

    // src/Form/Type/FriendMessageFormType.php
    use App\Entity\User;
    use Doctrine\ORM\EntityRepository;
    use Symfony\Bridge\Doctrine\Form\Type\EntityType;
    use Symfony\Component\Form\Extension\Core\Type\TextareaType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Security\Core\Security;
    // ...

    class FriendMessageFormType extends AbstractType
    {
        private $security;

        public function __construct(Security $security)
        {
            $this->security = $security;
        }

        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('subject', TextType::class)
                ->add('body', TextareaType::class)
            ;

            // 获取用户，并快速检查其是否存在
            $user = $this->security->getUser();
            if (!$user) {
                throw new \LogicException(
                    'The FriendMessageFormType cannot be used without an authenticated user!'
                );
            }

            $builder->addEventListener(FormEvents::PRE_SET_DATA, function (FormEvent $event) use ($user) {
                if (null !== $event->getData()->getFriend()) {
                    // 我们不需要添加该"好友"字段，因为该消息将发送给一个既定的好友
                    return;
                }

                $form = $event->getForm();

                $formOptions = [
                    'class' => User::class,
                    'choice_label' => 'fullName',
                    'query_builder' => function (UserRepository $userRepository) use ($user) {
                        // 在你的仓库上调用一个返回查询构建器的方法
                        // return $userRepository->createFriendsQueryBuilder($user);
                    },
                ];

                // 创建该字段，这类似于 $builder->add()
                // 字段名称, 字段类型, 字段选项
                $form->add('friend', EntityType::class, $formOptions);
            });
        }

        // ...
    }

.. note::

    你可能想知道，既然你可以访问 ``User`` 对象，为什么不直接在 ``buildForm()``
    方法中使用它并省略该事件监听器？
    这是因为在 ``buildForm()`` 方法中这样做会导致 **整个** 表单类型被修改，而不仅仅是当前的表单实例。
    这通常不是问题，但从技术上讲，在单个请求上是可以使用单个表单类型来创建多个表单或字段的。

使用该表单
~~~~~~~~~~~~~~

如果你正在使用 :ref:`自动装配 <services-autowire>` 和
:ref:`自动配置 <services-autoconfigure>`，那么你的表单已经可以使用了！
否则，请参阅 :doc:`/form/form_dependencies` 以了解如何将表单类型注册为服务。

在控制器中，像往常一样创建表单::

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class FriendMessageController extends AbstractController
    {
        public function new(Request $request)
        {
            $form = $this->createForm(FriendMessageFormType::class);

            // ...
        }
    }

你还可以将该表单类型嵌入另一个表单::

    // 在其他一些“表单类型”类中
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder->add('message', FriendMessageFormType::class);
    }

.. _form-events-submitted-data:

已提交表单的动态生成
--------------------------------------

可能出现的另一种情况，你希望根据用户提交的特定数据来自定义表单。
例如，假设你有一个运动会的注册表单。某些运动将允许你在字段上指定首发位置。
不妨假定这是一个 ``choice`` 字段。只是它的可用选项将取决于每项运动。
足球将有前锋、后卫、守门员等...棒球将有一个投手但不会有守门员。
为了通过验证，你需要选择正确的选项。

运动会作为一个实体字段(EntityType)传递给表单。所以我们可以像这样访问每项运动::

    // src/Form/Type/SportMeetupType.php
    namespace App\Form\Type;

    use Symfony\Bridge\Doctrine\Form\Type\EntityType;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;
    // ...

    class SportMeetupType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('sport', EntityType::class, [
                    'class'       => 'App\Entity\Sport',
                    'placeholder' => '',
                ])
            ;

            $builder->addEventListener(
                FormEvents::PRE_SET_DATA,
                function (FormEvent $event) {
                    $form = $event->getForm();

                    // 这就是你的实体，即SportMeetup
                    $data = $event->getData();

                    $sport = $data->getSport();
                    $positions = null === $sport ? [] : $sport->getAvailablePositions();

                    $form->add('position', EntityType::class, [
                        'class' => 'App\Entity\Position',
                        'placeholder' => '',
                        'choices' => $positions,
                    ]);
                }
            );
        }

        // ...
    }

当你首次构建此表单并展示给用户时，此示例应该可以完美地运行。

但是，当你处理表单提交时，事情就会变得复杂了。
这是因为 ``PRE_SET_DATA`` 事件只是告诉我们你开始时使用的数据（例如一个空的
``SportMeetup`` 对象），而 *不是* 已提交的数据。

在一个表单中，​​我们通常可以监听以下事件：

* ``PRE_SET_DATA``
* ``POST_SET_DATA``
* ``PRE_SUBMIT``
* ``SUBMIT``
* ``POST_SUBMIT``

此处的关键，是要为新字段所依赖的字段添加一个 ``POST_SUBMIT`` 监听器。
如果向一个子表单（例如 ``sport``）添加了一个 ``POST_SUBMIT`` 监听器，并将新表单添加到父表单，则表单组件将自动检测新字段并将其映射到客户端提交的数据中。

该类型现在看起来像这样::

    // src/Form/Type/SportMeetupType.php
    namespace App\Form\Type;

    use App\Entity\Sport;
    use Symfony\Bridge\Doctrine\Form\Type\EntityType;
    use Symfony\Component\Form\FormInterface;
    // ...

    class SportMeetupType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('sport', EntityType::class, [
                    'class'       => 'App\Entity\Sport',
                    'placeholder' => '',
                ]);
            ;

            $formModifier = function (FormInterface $form, Sport $sport = null) {
                $positions = null === $sport ? [] : $sport->getAvailablePositions();

                $form->add('position', EntityType::class, [
                    'class' => 'App\Entity\Position',
                    'placeholder' => '',
                    'choices' => $positions,
                ]);
            };

            $builder->addEventListener(
                FormEvents::PRE_SET_DATA,
                function (FormEvent $event) use ($formModifier) {
                    // 这就是你的实体，即SportMeetup
                    $data = $event->getData();

                    $formModifier($event->getForm(), $data->getSport());
                }
            );

            $builder->get('sport')->addEventListener(
                FormEvents::POST_SUBMIT,
                function (FormEvent $event) use ($formModifier) {
                    // 这里获取 $event->getForm()->getData() 非常重要，
                    // 因为 $event->getData() 只会给你客户端提交的数据（即，运动的ID）
                    $sport = $event->getForm()->getData();

                    // 由于我们已经将监听器添加到了子表单，因此必须将父表单传递给回调函数！
                    $formModifier($event->getForm()->getParent(), $sport);
                }
            );
        }

        // ...
    }

你可以看到，你需要监听这两个事件，同时需要不同的回调，因为在这两个不同的场景中，你可以使用的数据也会因为不同的事件而不同。
除此之外，在一个给定表单中的监听器总是执行相同的事情。

.. tip::

    ``FormEvents::POST_SUBMIT`` 事件不允许修改该监听器绑定的表单，但允许修改其父表单。

还没有做的一件事情就是在选择运动后对客户端表单进行更新。
这应该通过向你的应用创建一个AJAX回调来处理。假设你有一个用于创建一个运动聚会的控制器::

    // src/Controller/MeetupController.php
    namespace App\Controller;

    use App\Entity\SportMeetup;
    use App\Form\Type\SportMeetupType;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\Request;
    // ...

    class MeetupController extends AbstractController
    {
        public function create(Request $request)
        {
            $meetup = new SportMeetup();
            $form = $this->createForm(SportMeetupType::class, $meetup);
            $form->handleRequest($request);
            if ($form->isSubmitted() && $form->isValid()) {
                // ... 保存聚会，重定向等
            }

            return $this->render(
                'meetup/create.html.twig',
                ['form' => $form->createView()]
            );
        }

        // ...
    }

相关的模板使用一些JavaScript来根据 ``sport`` 字段中的当前选择来更新 ``position`` 表单字段：

.. code-block:: html+twig

    {# templates/meetup/create.html.twig #}
    {{ form_start(form) }}
        {{ form_row(form.sport) }}    {# <select id="meetup_sport" ... #}
        {{ form_row(form.position) }} {# <select id="meetup_position" ... #}
        {# ... #}
    {{ form_end(form) }}

    <script>
    var $sport = $('#meetup_sport');
    // 当 sport 获取到选中项时 ...
    $sport.change(function() {
      // ... 检索相应的表单
      var $form = $(this).closest('form');
      // 模拟表单数据，但只包括选定的运动的值。
      var data = {};
      data[$sport.attr('name')] = $sport.val();
      // 通过Ajax提交数据到表单的动作路径
      $.ajax({
        url : $form.attr('action'),
        type: $form.attr('method'),
        data : data,
        success: function(html) {
          // 替换当前的 position 字段 ...
          $('#meetup_position').replaceWith(
            // 使用一个Ajax的相应返回的值
            $(html).find('#meetup_position')
          );
          // 位置字段现在显示适当的位置。
        }
      });
    });
    </script>

提交整个表但只提取已更新的 ``position`` 字段的主要好处是不需要额外的服务器端代码;
上面所有的生成已提交表单的代码都可以复用。
