.. index::
    single: Workflow; Usage

如何创建和使用工作流
===============================

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>` 的应用中，运行此命令以在使用之前安装工作流功能：

.. code-block:: terminal

    $ composer require symfony/workflow

创建工作流
-------------------

工作流是你的对象经历的一个过程或一个生命周期。
该过程中的每个步骤或阶段称为一个 *位置*。你还可以定义一个 *过渡*，它描述了从一个位置到另一个位置的动作。

.. image:: /_images/components/workflow/states_transitions.png

一组位置和过渡创建了一个 **定义**。
一个工作流需要一个 ``Definition`` 和一个将状态写入对象的方法（即一个
:class:`Symfony\\Component\\Workflow\\MarkingStore\\MarkingStoreInterface` 实例）。

考虑以下示例，一个博客帖子可以有这些位置：``draft``、``review``、``rejected``、``published``。
你可以像这样定义工作流程：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/workflow.yaml
        framework:
            workflows:
                blog_publishing:
                    type: 'workflow' # or 'state_machine'
                    audit_trail:
                        enabled: true
                    marking_store:
                        type: 'multiple_state' # or 'single_state'
                        arguments:
                            - 'currentPlace'
                    supports:
                        - App\Entity\BlogPost
                    initial_place: draft
                    places:
                        - draft
                        - review
                        - rejected
                        - published
                    transitions:
                        to_review:
                            from: draft
                            to:   review
                        publish:
                            from: review
                            to:   published
                        reject:
                            from: review
                            to:   rejected

    .. code-block:: xml

        <!-- config/packages/workflow.xml -->
        <?xml version="1.0" encoding="utf-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >

            <framework:config>
                <framework:workflow name="blog_publishing" type="workflow">
                    <framework:audit-trail enabled="true" />

                    <framework:marking-store type="single_state">
                      <framework:argument>currentPlace</framework:argument>
                    </framework:marking-store>

                    <framework:support>App\Entity\BlogPost</framework:support>

                    <framework:place>draft</framework:place>
                    <framework:place>review</framework:place>
                    <framework:place>rejected</framework:place>
                    <framework:place>published</framework:place>

                    <framework:transition name="to_review">
                        <framework:from>draft</framework:from>

                        <framework:to>review</framework:to>
                    </framework:transition>

                    <framework:transition name="publish">
                        <framework:from>review</framework:from>

                        <framework:to>published</framework:to>
                    </framework:transition>

                    <framework:transition name="reject">
                        <framework:from>review</framework:from>

                        <framework:to>rejected</framework:to>
                    </framework:transition>

                </framework:workflow>

            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/workflow.php

        $container->loadFromExtension('framework', array(
            // ...
            'workflows' => array(
                'blog_publishing' => array(
                    'type' => 'workflow', // or 'state_machine'
                    'audit_trail' => array(
                        'enabled' => true
                    ),
                    'marking_store' => array(
                        'type' => 'multiple_state', // or 'single_state'
                        'arguments' => array('currentPlace')
                    ),
                    'supports' => array('App\Entity\BlogPost'),
                    'places' => array(
                        'draft',
                        'review',
                        'rejected',
                        'published',
                    ),
                    'transitions' => array(
                        'to_review' => array(
                            'from' => 'draft',
                            'to' => 'review',
                         ),
                         'publish' => array(
                             'from' => 'review',
                             'to' => 'published',
                         ),
                         'reject' => array(
                             'from' => 'review',
                             'to' => 'rejected',
                         ),
                     ),
                 ),
             ),
         ));

.. code-block:: php

    class BlogPost
    {
        // 供 marking store 使用的属性
        public $currentPlace;
        public $title;
        public $content;
    }

.. note::

    标记存储(marking store)的类型可以是“multiple_state”或“single_state”。
    其中单个状态的标记存储不支持一个同时在多个位置上的模型。

.. tip::

    ``marking_store`` 选项的 ``type``（默认值 ``single_state``）和
    ``arguments``（默认值 ``marking``）属性是可选的。如果省略，将使用它们的默认值。

.. tip::

    设置 ``audit_trail.enabled`` 选项为 ``true``，可以让应用为工作流活动生成详细的日志消息。

使用工作流
----------------

一旦创建好 ``blog_publishing`` 工作流程，现在你可以用它来决定一个博客帖子的什么行为是允许的。
例如，在使用 :ref:`默认services.yaml配置 <service-container-services-load-example>`
的应用的控制器内部，你可以通过注入Workflow注册表服务来获取该工作流::

    // ...
    use Symfony\Component\Workflow\Registry;
    use App\Entity\BlogPost;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\Workflow\Exception\TransitionException;

    class BlogController extends AbstractController
    {
        public function edit(Registry $workflows)
        {
            $post = new BlogPost();
            $workflow = $workflows->get($post);

            // 如果同一个类有多个工作流，则将工作流名称作为第二个参数传递
            // $workflow = $workflows->get($post, 'blog_publishing');

            // 你还可以获得与一个对象关联的所有工作流，这很有用
            // 例如，显示后端中所有这些工作流的状态
            $postWorkflows = $workflows->all($post);

            $workflow->can($post, 'publish'); // False
            $workflow->can($post, 'to_review'); // True

            // 更新帖子上的 currentState
            try {
                $workflow->apply($post, 'to_review');
            } catch (TransitionException $exception) {
                // ... 如果不允许该过渡
            }

            // 查看当前状态下帖子的所有可用过渡
            $transitions = $workflow->getEnabledTransitions($post);
        }
    }

.. versionadded:: 4.1
    :class:`Symfony\\Component\\Workflow\\Exception\\TransitionException`
    类是在Symfony的4.1中引入的。

.. versionadded:: 4.1
    :method:`Symfony\\Component\\Workflow\\Registry::all` 方法是在Symfony 4.1中引入的。

使用事件
------------

为了使你的工作流更加灵活，你可以使用一个 ``EventDispatcher`` 来构造 ``Workflow`` 对象。
你现在可以创建事件监听器来阻拦过渡（即取决于博客文章中的数据），并在一个工作流操作发生时执行其他动作（例如发送通知）。

每个步骤都有三个按顺序触发的事件：

* 每个工作流的事件;
* 有关工作流的事件;
* 涉及特定过渡或位置的工作流事件。

启动一个状态过渡时，将按以下顺序调度事件：

``workflow.guard``
    验证是否允许过渡（:ref:`见下文 <workflow-usage-guard-events>`）。

    调度的三个事件是：

    * ``workflow.guard``
    * ``workflow.[workflow name].guard``
    * ``workflow.[workflow name].guard.[transition name]``

``workflow.leave``
    即将离开一个位置。

    调度的三个事件是：

    * ``workflow.leave``
    * ``workflow.[workflow name].leave``
    * ``workflow.[workflow name].leave.[place name]``

``workflow.transition``
    该主题正在经历此过渡。

    调度的三个事件是：

    * ``workflow.transition``
    * ``workflow.[workflow name].transition``
    * ``workflow.[workflow name].transition.[transition name]``

``workflow.enter``
    该主题即将进入一个新的位置。此事件在该主题位置更新之前触发，这意味着主题的标记尚未使用新位置进行更新。

    调度的三个事件是：

    * ``workflow.enter``
    * ``workflow.[workflow name].enter``
    * ``workflow.[workflow name].enter.[place name]``

``workflow.entered``
    该主题已进入位置并且标记已更新（使其成为在Doctrine中刷新数据的好地方）。

    调度的三个事件是：

    * ``workflow.entered``
    * ``workflow.[workflow name].entered``
    * ``workflow.[workflow name].entered.[place name]``

``workflow.completed``
    该对象已完成此过渡。

    调度的三个事件是：

    * ``workflow.completed``
    * ``workflow.[workflow name].completed``
    * ``workflow.[workflow name].completed.[transition name]``


``workflow.announce``
    触发现在可供该主题访问的每个过渡。

    调度的三个事件是：

    * ``workflow.announce``
    * ``workflow.[workflow name].announce``
    * ``workflow.[workflow name].announce.[transition name]``

.. note::

    即使对于保持在相同位置的过渡，也会触发离开和进入事件。

以下是每次 ``blog_publishing`` 工作流离开一个位置时如何启用日志记录的示例::

    use Psr\Log\LoggerInterface;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Workflow\Event\Event;

    class WorkflowLogger implements EventSubscriberInterface
    {
        public function __construct(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }

        public function onLeave(Event $event)
        {
            $this->logger->alert(sprintf(
                'Blog post (id: "%s") performed transaction "%s" from "%s" to "%s"',
                $event->getSubject()->getId(),
                $event->getTransition()->getName(),
                implode(', ', array_keys($event->getMarking()->getPlaces())),
                implode(', ', $event->getTransition()->getTos())
            ));
        }

        public static function getSubscribedEvents()
        {
            return array(
                'workflow.blog_publishing.leave' => 'onLeave',
            );
        }
    }

.. _workflow-usage-guard-events:

安保事件
~~~~~~~~~~~~

有一种被称为“安保事件”的特殊事件。
每次执行一个 ``Workflow::can``、``Workflow::apply`` 或
``Workflow::getEnabledTransitions`` 调用，都会触发这些事件监听器。
他们的事件监听器被调用每次通话Workflow::can，Workflow::apply或者  Workflow::getEnabledTransitions被执行。
使用安保事件，你可以添加自定义逻辑来确定哪些过渡是有效的。这是一个安保事件名称列表：

* ``workflow.guard``
* ``workflow.[workflow name].guard``
* ``workflow.[workflow name].guard.[transition name]``

请参阅以下确保没有标题的博客文章不会移至“review”的示例::

    use Symfony\Component\Workflow\Event\GuardEvent;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;

    class BlogPostReviewListener implements EventSubscriberInterface
    {
        public function guardReview(GuardEvent $event)
        {
            /** @var \App\Entity\BlogPost $post */
            $post = $event->getSubject();
            $title = $post->title;

            if (empty($title)) {
                // 不应允许没有标题的帖子
                $event->setBlocked(true);
            }
        }

        public static function getSubscribedEvents()
        {
            return array(
                'workflow.blogpost.guard.to_review' => array('guardReview'),
            );
        }
    }

事件方法
~~~~~~~~~~~~~

每个工作流事件都是一个 :class:`Symfony\\Component\\Workflow\\Event\\Event` 实例。
这意味着每个事件都可以访问以下信息：

:method:`Symfony\\Component\\Workflow\\Event\\Event::getMarking`
    返回工作流的 :class:`Symfony\\Component\\Workflow\\Marking`。

:method:`Symfony\\Component\\Workflow\\Event\\Event::getSubject`
    返回调度该事件的对象。

:method:`Symfony\\Component\\Workflow\\Event\\Event::getTransition`
    返回调度该事件的 :class:`Symfony\\Component\\Workflow\\Transition`。

:method:`Symfony\\Component\\Workflow\\Event\\Event::getWorkflowName`
    返回一个字符串，其中包含触发该事件的工作流的名称。

对于安保事件，有一个扩展类 :class:`Symfony\\Component\\Workflow\\Event\\GuardEvent`。
这个类还有两个方法：

:method:`Symfony\\Component\\Workflow\\Event\\GuardEvent::isBlocked`
    如果过渡被阻止则返回。

:method:`Symfony\\Component\\Workflow\\Event\\GuardEvent::setBlocked`
    设置阻止值。

在Twig中使用
-------------

Symfony定义了几个Twig函数来管理工作流并减少模板中域逻辑的需求：

``workflow_can()``
    如果给定对象可以进行给定过渡，则返回 ``true``。

``workflow_transitions()``
    返回一个数组，其中包含为给定对象启用的所有过渡。

``workflow_marked_places()``
    返回一个包含给定标记的位置名称的数组。

``workflow_has_marked_place()``
    如果给定对象的标记具有给定的状态，则返回 ``true``。

以下示例展示了这些函数：

.. code-block:: twig

    <h3>Actions</h3>
    {% if workflow_can(post, 'publish') %}
        <a href="...">Publish article</a>
    {% endif %}
    {% if workflow_can(post, 'to_review') %}
        <a href="...">Submit to review</a>
    {% endif %}
    {% if workflow_can(post, 'reject') %}
        <a href="...">Reject article</a>
    {% endif %}

    {# 或者循环已启用的过渡 #}
    {% for transition in workflow_transitions(post) %}
        <a href="...">{{ transition.name }}</a>
    {% else %}
        No actions available.
    {% endfor %}

    {# 检查对象是否在某个特定位置 #}
    {% if workflow_has_marked_place(post, 'to_review') %}
        <p>This post is ready for review.</p>
    {% endif %}

    {# 检查对象上是否被标记了某个位置 #}
    {% if 'waiting_some_approval' in workflow_marked_places(post) %}
        <span class="label">PENDING</span>
    {% endif %}
