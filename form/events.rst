.. index::
    single: Forms; Form Events

表单事件
===========

Form组件提供了一个结构化的过程，可以通过使用
:doc:`EventDispatcher组件 </components/event_dispatcher>` 来让你自定义表单。
使用表单事件，你可以在工作流的不同步骤中修改信息或字段：从表单填充到请求中的数据提交。

例如，如果需要根据请求的值来添加一个字段，则可以如下所示向
``FormEvents::PRE_SUBMIT`` 事件注册一个事件监听器::

    // ...

    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;

    $listener = function (FormEvent $event) {
        // ...
    };

    $form = $formFactory->createBuilder()
        // ... 添加表单字段
        ->addEventListener(FormEvents::PRE_SUBMIT, $listener);

    // ...

表单工作流
-----------------

表单提交工作流
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: /_images/components/form/general_flow.png
    :align: center

1) 预填充表单 (``FormEvents::PRE_SET_DATA`` 和 ``FormEvents::POST_SET_DATA``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: /_images/components/form/set_data_flow.png
    :align: center

在一个表单的预填充期间（
:method:`Form::setData() <Symfony\\Component\\Form\\Form::setData>`
被调用后）会调度两个事件：``FormEvents::PRE_SET_DATA`` 和 ``FormEvents::POST_SET_DATA``。

A) ``FormEvents::PRE_SET_DATA`` 事件
.........................................

``Form::setData()`` 方法开始时会调度 ``FormEvents::PRE_SET_DATA`` 事件。它可以用于：

* 修改预填充期间给出的数据;
* 根据预填充的数据修改表单（动态的添加或删除字段）。

===============  ========
数据类型           值
===============  ========
Model data       ``null``
Normalized data  ``null``
View data        ``null``
===============  ========

.. seealso::

    在 :ref:`表单事件信息表 <component-form-event-table>` 中一目了然地查看所有表单事件。

.. caution::

    在 ``FormEvents::PRE_SET_DATA`` 期间，
    :method:`Form::setData() <Symfony\\Component\\Form\\Form::setData>`
    被锁定并且如果被使用则将抛出一个异常。如果你希望修改数据，则应使用
    :method:`FormEvent::setData() <Symfony\\Component\\Form\\FormEvent::setData>`。

.. sidebar:: 表单组件中的 ``FormEvents::PRE_SET_DATA``

    :class:`Symfony\\Component\\Form\\Extension\\Core\\Type\\CollectionType` 表单类型依赖于
    :class:`Symfony\\Component\\Form\\Extension\\Core\\EventListener\\ResizeFormListener`
    订阅器，该订阅器监听了 ``FormEvents::PRE_SET_DATA``
    事件，以便根据预填充对象中的数据，通过删除和添加所有的表单行(row)来对表单的字段重新排序。

B) ``FormEvents::POST_SET_DATA`` 事件
..........................................

:method:`Form::setData() <Symfony\\Component\\Form\\Form::setData>`
方法结束时会调度 ``FormEvents::POST_SET_DATA`` 事件。该事件主要用于在预填充表单后读取数据。

===============  ====================================================
数据类型           值
===============  ====================================================
Model data       ``setData()`` 注入的Model data
Normalized data  使用一个模型转换器转换的Model data
View data        使用一个视图转换器转换的Normalized data
===============  ====================================================

.. seealso::

    在 :ref:`表单事件信息表 <component-form-event-table>` 中一目了然地查看所有表单事件。

.. sidebar:: 表单组件中的 ``FormEvents::POST_SET_DATA``

    :class:`Symfony\\Component\\Form\\Extension\\DataCollector\\EventListener\\DataCollectorListener`
    类订阅了 ``FormEvents::POST_SET_DATA``
    事件，以便从非规范化(denormalized)的Model和View数据中收集有关表单的信息。

2) 提交表单 (``FormEvents::PRE_SUBMIT``, ``FormEvents::SUBMIT`` 和 ``FormEvents::POST_SUBMIT``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: /_images/components/form/submission_flow.png
    :align: center

:method:`Form::handleRequest() <Symfony\\Component\\Form\\Form::handleRequest>`
或 :method:`Form::submit() <Symfony\\Component\\Form\\Form::submit>`
方法被调用时，会调度三个事件：``FormEvents::PRE_SUBMIT``、``FormEvents::SUBMIT``、
``FormEvents::POST_SUBMIT``。

A) ``FormEvents::PRE_SUBMIT`` 事件
.......................................

:method:`Form::submit() <Symfony\\Component\\Form\\Form::submit>`
方法开始时会调度 ``FormEvents::PRE_SUBMIT`` 事件。

它可以用于：

* 在将数据提交到表单之前更改请求中的数据;
* 在将数据提交到表单之前添加或删除表单字段。

===============  ========================================
数据类型           值
===============  ========================================
Model data       与在 ``FormEvents::POST_SET_DATA`` 时相同
Normalized data  与在 ``FormEvents::POST_SET_DATA`` 时相同
View data        与在 ``FormEvents::POST_SET_DATA`` 时相同
===============  ========================================

.. seealso::

    在 :ref:`表单事件信息表 <component-form-event-table>` 中一目了然地查看所有表单事件。

.. sidebar:: 表单组件中 ``FormEvents::PRE_SUBMIT``

    :class:`Symfony\\Component\\Form\\Extension\\Core\\EventListener\\TrimListener`
    订阅器订阅了 ``FormEvents::PRE_SUBMIT`` 事件，以便修剪(trim)请求中的数据（针对字符串值）。
    :class:`Symfony\\Component\\Form\\Extension\\Csrf\\EventListener\\CsrfValidationListener`
    订阅器订阅了 ``FormEvents::PRE_SUBMIT`` 事件，以便验证CSRF令牌。

B) ``FormEvents::SUBMIT`` 事件
...................................

在 :method:`Form::submit() <Symfony\\Component\\Form\\Form::submit>`
方法将Normalized数据转换回Model和View数据之前会调度 ``FormEvents::SUBMIT`` 事件。

它可以被用于在正常化(normalized)的数据内容中修改数据。

===============  ===================================================================================
数据类型           值
===============  ===================================================================================
Model data       与在 ``FormEvents::POST_SET_DATA`` 时相同
Normalized data  对使用一个视图转换器的请求进行反向转换而得来的请求数据
View data        与在 ``FormEvents::POST_SET_DATA`` 时相同
===============  ===================================================================================

.. seealso::

    在 :ref:`表单事件信息表 <component-form-event-table>` 中一目了然地查看所有表单事件。

.. caution::

    在这个点，你无法在表单中添加或删除字段。

.. sidebar:: 表单组件中的 ``FormEvents::SUBMIT``

    :class:`Symfony\\Component\\Form\\Extension\\Core\\EventListener\\FixUrlProtocolListener`
    订阅了 ``FormEvents::SUBMIT`` 事件，以便在已提交的数据没有协议时附加一个默认协议到URL字段。

C) ``FormEvents::POST_SUBMIT`` 事件
........................................

一旦模型和视图数据被非规范化(denormalized)，``FormEvents::POST_SUBMIT``
方法就会调度 ``FormEvents::POST_SUBMIT`` 事件。

它可用于在非规范化后获取数据。

===============  =============================================================
数据类型           值
===============  =============================================================
Model data       使用一个模型转换器反向转换的Normalized data
Normalized data  与在 ``FormEvents::SUBMIT`` 时相同
View data        使用一个视图转换器转换的Normalized data
===============  =============================================================

.. seealso::

    在 :ref:`表单事件信息表 <component-form-event-table>` 中一目了然地查看所有表单事件。

.. caution::

    在这个点，你无法在当前表单及其子表单中添加或删除字段。

.. sidebar:: 表单组件中的 ``FormEvents::POST_SUBMIT``

    :class:`Symfony\\Component\\Form\\Extension\\DataCollector\\EventListener\\DataCollectorListener`
    订阅了 ``FormEvents::POST_SUBMIT`` 事件以便收集有关表单的信息。
    :class:`Symfony\\Component\\Form\\Extension\\Validator\\EventListener\\ValidationListener`
    订阅了 ``FormEvents::POST_SUBMIT`` 事件以便自动验证非规范化(denormalized)的对象。

注册事件监听器或事件订阅器
------------------------------------------------

为了能够使用Form事件，你需要创建事件监听器或事件订阅器并将其注册到一个事件。

每个“表单”事件的名称在 :class:`Symfony\\Component\\Form\\FormEvents` 类中被定义为一个常量。
此外，每个事件回调（监听器或订阅器方法）都被传递一个参数，该参数是一个
:class:`Symfony\\Component\\Form\\FormEvent` 实例。
事件对象包含对表单当前状态和正在处理的当前数据的引用。

.. _component-form-event-table:

======================  =============================  ===============
名称                     ``FormEvents`` 常量             事件的数据
======================  =============================  ===============
``form.pre_set_data``   ``FormEvents::PRE_SET_DATA``   Model data
``form.post_set_data``  ``FormEvents::POST_SET_DATA``  Model data
``form.pre_submit``     ``FormEvents::PRE_SUBMIT``     Request data
``form.submit``         ``FormEvents::SUBMIT``         Normalized data
``form.post_submit``    ``FormEvents::POST_SUBMIT``    View data
======================  =============================  ===============

事件监听器
~~~~~~~~~~~~~~~

一个事件监听器可以是任何类型的有效可调用对象。
例如，你可以在 ``FormFactory`` 的 ``addEventListener`` 方法中内联(inline)定义一个事件监听器函数::

    // ...

    use Symfony\Component\Form\Extension\Core\Type\CheckboxType;
    use Symfony\Component\Form\Extension\Core\Type\EmailType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;

    $form = $formFactory->createBuilder()
        ->add('username', TextType::class)
        ->add('showEmail', CheckboxType::class)
        ->addEventListener(FormEvents::PRE_SUBMIT, function (FormEvent $event) {
            $user = $event->getData();
            $form = $event->getForm();

            if (!$user) {
                return;
            }

            // 检查用户是否选择显示他们的电子邮件地址。
            // 如果之前就提交了数据，则需要删除请求变量中包含的额外的值。
            if (true === $user['showEmail']) {
                $form->add('email', EmailType::class);
            } else {
                unset($user['email']);
                $event->setData($user);
            }
        })
        ->getForm();

    // ...

创建一个表单类型类后，可以使用其中一个方法作为一个回调以提高可读性::

    // src/Form/SubscriptionType.php
    namespace App\Form;

    use Symfony\Component\Form\Extension\Core\Type\CheckboxType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;

    // ...
    class SubscriptionType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('username', TextType::class)
                ->add('showEmail', CheckboxType::class)
                ->addEventListener(
                    FormEvents::PRE_SET_DATA,
                    [$this, 'onPreSetData']
                )
            ;
        }

        public function onPreSetData(FormEvent $event)
        {
            // ...
        }
    }

事件订阅器
~~~~~~~~~~~~~~~~~

事件订阅器有不同的用途：

* 提高可读性;
* 监听多个事件;
* 在单个类中重组多个监听器。

请思考以下表单事件订阅器的示例::

    // src/Form/EventListener/AddEmailFieldListener.php
    namespace App\Form\EventListener;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Form\Extension\Core\Type\EmailType;
    use Symfony\Component\Form\FormEvent;
    use Symfony\Component\Form\FormEvents;

    class AddEmailFieldListener implements EventSubscriberInterface
    {
        public static function getSubscribedEvents()
        {
            return [
                FormEvents::PRE_SET_DATA => 'onPreSetData',
                FormEvents::PRE_SUBMIT   => 'onPreSubmit',
            ];
        }

        public function onPreSetData(FormEvent $event)
        {
            $user = $event->getData();
            $form = $event->getForm();

            // 检查初始数据中的用户是否选择显示他们的电子邮件。
            if (true === $user->isShowEmail()) {
                $form->add('email', EmailType::class);
            }
        }

        public function onPreSubmit(FormEvent $event)
        {
            $user = $event->getData();
            $form = $event->getForm();

            if (!$user) {
                return;
            }

            // 检查用户是否选择显示他们的电子邮件地址。
            // 如果之前就提交了数据，则需要删除请求变量中包含的额外的值。
            if (true === $user['showEmail']) {
                $form->add('email', EmailType::class);
            } else {
                unset($user['email']);
                $event->setData($user);
            }
        }
    }

要注册事件订阅器，请使用 ``addEventSubscriber()`` 方法::

    use App\Form\EventListener\AddEmailFieldListener;
    use Symfony\Component\Form\Extension\Core\Type\CheckboxType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    // ...

    $form = $formFactory->createBuilder()
        ->add('username', TextType::class)
        ->add('showEmail', CheckboxType::class)
        ->addEventSubscriber(new AddEmailFieldListener())
        ->getForm();

    // ...
