工作流
========

使用Symfony应用中的Workflow组件需要首先了解有关工作流和状态机的一些基本理论和概念。
阅读 :doc:`本文 </workflow/introduction>` 以获得快速概述。

安装
------------

在使用 :doc:`Symfony Flex </setup/flex>`
的应用中，运行此命令以在使用之前安装工作流功能：

.. code-block:: terminal

    $ composer require symfony/workflow

配置
-------------

要查看所有的配置选项，并且你正在使用Symfony项目中的组件，请运行以下命令：

.. code-block:: terminal

    $ php bin/console config:dump-reference framework workflows

创建工作流
-------------------

一个工作流是你的对象经历的流程或生命周期。该流程中的每个步骤或阶段称为一个
*位置*。你还可以定义 *过渡*，以描述从一个位置到另一个位置的动作。

.. image:: /_images/components/workflow/states_transitions.png

一组位置和过渡创建了一个 **定义**。工作流需要一个 ``Definition`` 方法将状态写入对象（即一个
:class:`Symfony\\Component\\Workflow\\MarkingStore\\MarkingStoreInterface` 实例）。

请考虑以下博客文章示例。一个帖子可以有这些位置：``draft``、``reviewed``、``rejected``、``published``。
你可以像这样定义工作流：

.. configuration-block::

    .. code-block:: yaml

        # config/framework.yaml
        framework:
            workflows:
                blog_publishing:
                    type: 'workflow' # 或 'state_machine'
                    audit_trail:
                        enabled: true
                    marking_store:
                        type: 'multiple_state' # 或 'single_state'
                        arguments:
                            - 'currentPlace'
                    supports:
                        - App\Entity\BlogPost
                    initial_place: draft
                    places:
                        - draft
                        - reviewed
                        - rejected
                        - published
                    transitions:
                        to_review:
                            from: draft
                            to:   reviewed
                        publish:
                            from: reviewed
                            to:   published
                        reject:
                            from: reviewed
                            to:   rejected

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony https://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >

            <framework:config>
                <framework:workflow name="blog_publishing" type="workflow">
                    <framework:audit-trail enabled="true"/>

                    <framework:marking-store type="single_state">
                        <framework:argument>currentPlace</framework:argument>
                    </framework:marking-store>

                    <framework:support>App\Entity\BlogPost</framework:support>

                    <framework:place>draft</framework:place>
                    <framework:place>reviewed</framework:place>
                    <framework:place>rejected</framework:place>
                    <framework:place>published</framework:place>

                    <framework:transition name="to_review">
                        <framework:from>draft</framework:from>
                        <framework:to>reviewed</framework:to>
                    </framework:transition>

                    <framework:transition name="publish">
                        <framework:from>reviewed</framework:from>
                        <framework:to>published</framework:to>
                    </framework:transition>

                    <framework:transition name="reject">
                        <framework:from>reviewed</framework:from>
                        <framework:to>rejected</framework:to>
                    </framework:transition>

                </framework:workflow>

            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', [
            // ...
            'workflows' => [
                'blog_publishing' => [
                    'type' => 'workflow', // or 'state_machine'
                    'audit_trail' => [
                        'enabled' => true
                    ],
                    'marking_store' => [
                        'type' => 'multiple_state', // or 'single_state'
                        'arguments' => ['currentPlace']
                    ],
                    'supports' => ['App\Entity\BlogPost'],
                    'places' => [
                        'draft',
                        'reviewed',
                        'rejected',
                        'published',
                    ],
                    'transitions' => [
                        'to_review' => [
                            'from' => 'draft',
                            'to' => 'reviewed',
                        ],
                        'publish' => [
                            'from' => 'reviewed',
                            'to' => 'published',
                        ],
                        'reject' => [
                            'from' => 'reviewed',
                            'to' => 'rejected',
                        ],
                    ],
                ],
            ],
        ]);

.. tip::

    如果你要创建第一个工作流，请考虑使用 ``workflow:dump`` 命令来
    :doc:`调试工作流内容 </workflow/dumping-workflows>`。

按照配置，标记存储区使用以下属性::

    class BlogPost
    {
        // 此属性由标记存储区使用
        public $currentPlace;
        public $title;
        public $content;
    }

.. note::

    标记存储区类型可以是 "multiple_state" 或
    "single_state"。单个状态的标记存储区不支持同时在多个位置上的模型。

.. tip::

    ``marking_store`` 选项的 ``type``（默认值为 ``single_state``）和
    ``arguments``（默认值为 ``marking``）属性是可选的。如果省略，将使用它们的默认值。

.. tip::

    将 ``audit_trail.enabled`` 选项设置为 ``true``
    会使应用为工作流的活动生成详细的日志消息。

通过名为 ``blog_publishing`` 的工作流，你可以获得帮助来决定允许在一个博客文章上执行哪些操作::

    use App\Entity\BlogPost;
    use Symfony\Component\Workflow\Exception\LogicException;

    $post = BlogPost();

    $workflow = $this->container->get('workflow.blog_publishing');
    $workflow->can($post, 'publish'); // False
    $workflow->can($post, 'to_review'); // True

    // 更新帖子上的当前状态
    try {
        $workflow->apply($post, 'to_review');
    } catch (LogicException $exception) {
        // ...
    }

    // 查看处于当前状态的帖子的所有可用过渡
    $transitions = $workflow->getEnabledTransitions($post);

使用事件
------------

为了使你的工作流更灵活，你可以使用 ``EventDispatcher` 来构造 ``Workflow`` 对象。
你现在可以创建事件监听器来阻止过渡（即取决于博客文章中的数据），并在工作流操作发生时执行其他动作（例如发送通知）。

每个步骤都有三个按顺序触发的事件：

* 每个工作流的事件；
* 相关工作流的事件；
* 与特定过程或位置有关的工作流事件。

启动一个状态过渡时，将按以下顺序调度事件：

``workflow.guard``
    验证过渡是否被阻止（请参阅 :ref:`安保事件 <workflow-usage-guard-events>`
    和 :ref:`阻止过渡 <workflow-blocking-transitions>`）。

    调度的三个事件是：

    * ``workflow.guard``
    * ``workflow.[workflow name].guard``
    * ``workflow.[workflow name].guard.[transition name]``

``workflow.leave``
    主题即将离开一个位置。

    调度的三个事件是：

    * ``workflow.leave``
    * ``workflow.[workflow name].leave``
    * ``workflow.[workflow name].leave.[place name]``

``workflow.transition``
    主题正在经历该过渡。

    调度的三个事件是：

    * ``workflow.transition``
    * ``workflow.[workflow name].transition``
    * ``workflow.[workflow name].transition.[transition name]``

``workflow.enter``
    主题即将进入一个新的位置。此事件在主题位置更新之前触发，这意味着该主题的标记尚未使用新位置进行更新。

    调度的三个事件是：

    * ``workflow.enter``
    * ``workflow.[workflow name].enter``
    * ``workflow.[workflow name].enter.[place name]``

``workflow.entered``
    主题已进入位置并且标记已更新（使其成为在Doctrine中刷新数据的好位置）。

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
    触发现在可供主题访问的每个过渡。

    调度的三个事件是：

    * ``workflow.announce``
    * ``workflow.[workflow name].announce``
    * ``workflow.[workflow name].announce.[transition name]``

.. note::

    即使对于停留在同一位置的过渡，也会触发离开和进入事件。

以下是每次 "blog_publishing" 工作流离开某个位置时如何启用日志记录的示例::

    use Psr\Log\LoggerInterface;
    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Workflow\Event\Event;

    class WorkflowLogger implements EventSubscriberInterface
    {
        private $logger;
        
        public function __construct(LoggerInterface $logger)
        {
            $this->logger = $logger;
        }

        public function onLeave(Event $event)
        {
            $this->logger->alert(sprintf(
                'Blog post (id: "%s") performed transition "%s" from "%s" to "%s"',
                $event->getSubject()->getId(),
                $event->getTransition()->getName(),
                implode(', ', array_keys($event->getMarking()->getPlaces())),
                implode(', ', $event->getTransition()->getTos())
            ));
        }

        public static function getSubscribedEvents()
        {
            return [
                'workflow.blog_publishing.leave' => 'onLeave',
            ];
        }
    }

.. _workflow-usage-guard-events:

安保事件
~~~~~~~~~~~~

有一种特殊的事件称为“安保事件”。每次调用 ``Workflow::can``、``Workflow::apply``
或 ``Workflow::getEnabledTransitions`` 时，它们的事件监听器都会被执行。
使用安保事件，你可以添加自定义逻辑来决定应阻止哪些过渡。这是一个安保事件名称列表。

* ``workflow.guard``
* ``workflow.[workflow name].guard``
* ``workflow.[workflow name].guard.[transition name]``

如果任何博客帖子缺少一个标题，此示例将阻止将其过渡为 “reviewed”::

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Workflow\Event\GuardEvent;

    class BlogPostReviewListener implements EventSubscriberInterface
    {
        public function guardReview(GuardEvent $event)
        {
            /** @var App\Entity\BlogPost $post */
            $post = $event->getSubject();
            $title = $post->title;

            if (empty($title)) {
                // 如果帖子没有标题，则阻止 "to_review" 过渡
                $event->setBlocked(true);
            }
        }

        public static function getSubscribedEvents()
        {
            return [
                'workflow.blog_publishing.guard.to_review' => ['guardReview'],
            ];
        }
    }

事件的方法
~~~~~~~~~~~~~

每个工作流事件都是一个 :class:`Symfony\\Component\\Workflow\\Event\\Event`
实例。这意味着每个事件都可以访问以下信息：

:method:`Symfony\\Component\\Workflow\\Event\\Event::getMarking`
    返回工作流的 :class:`Symfony\\Component\\Workflow\\Marking`。

:method:`Symfony\\Component\\Workflow\\Event\\Event::getSubject`
    返回调度事件的对象。

:method:`Symfony\\Component\\Workflow\\Event\\Event::getTransition`
    返回调度事件的 :class:`Symfony\\Component\\Workflow\\Transition`。

:method:`Symfony\\Component\\Workflow\\Event\\Event::getWorkflowName`
    返回一个包含触发事件的工作流的名称的字符串。

:method:`Symfony\\Component\\Workflow\\Event\\Event::getMetadata`
    返回元一个数据。

对于安保事件，有一个 :class:`Symfony\\Component\\Workflow\\Event\\GuardEvent`
扩展类。这个类还有两个以上的方法：

:method:`Symfony\\Component\\Workflow\\Event\\GuardEvent::isBlocked`
    如果过渡被阻止则返回。

:method:`Symfony\\Component\\Workflow\\Event\\GuardEvent::setBlocked`
    设置阻止值。

:method:`Symfony\\Component\\Workflow\\Event\\GuardEvent::getTransitionBlockerList`
    返回 :class:`Symfony\\Component\\Workflow\\TransitionBlockerList` 事件。请参阅
    :ref:`阻止过渡 <workflow-blocking-transitions>`。

:method:`Symfony\\Component\\Workflow\\Event\\GuardEvent::addTransitionBlocker`
    添加一个 :class:`Symfony\\Component\\Workflow\\TransitionBlocker` 实例。

.. _workflow-blocking-transitions:

阻止过渡
--------------------

可以通过执行自定义逻辑来控制工作流的执行，以在应用它之前确定当前过渡是被阻止还是被允许。
此功能由“安保”提供，可以通过两种方式使用。

首先，你可以监听 :ref:`安保事件 <workflow-usage-guard-events>`。
或者，你可以为过渡定义一个 ``guard`` 配置选项。此选项的值是使用
:doc:`ExpressionLanguage组件 </components/expression_language>` 创建的任何有效表达式：

.. code-block:: yaml

    # config/packages/workflow.yaml
    framework:
        workflows:
            blog_publishing:
                # 之前的配置
                transitions:
                    to_review:
                        # 仅当当前用户具有 ROLE_REVIEWER 角色时才允许过渡。
                        guard: "is_granted('ROLE_REVIEWER')"
                        from: draft
                        to:   reviewed
                    publish:
                        # 或 "is_anonymous"、"is_remember_me"、"is_fully_authenticated"、"is_granted"、"is_valid"
                        guard: "is_authenticated"
                        from: reviewed
                        to:   published
                    reject:
                        # 或任何有效的表达语言，其中“subject”指的是受支持的对象
                        guard: "has_role('ROLE_ADMIN') and subject.isRejectable()"
                        from: reviewed
                        to:   rejected

当你停止发生一个过渡时，你还可以使用过渡阻止器来阻止并返回友好的错误消息。
在示例中，我们从 :class:`Symfony\\Component\\Workflow\\Event\\Event`
的元数据中获取此消息，为你提供管理文本的中心位置。

这个例子已经简化了；在生产中，你可能更喜欢使用
:doc:`Translation </components/translation>` 组件在一个位置管理消息::

    namespace App\Listener\Workflow\Task;

    use Symfony\Component\EventDispatcher\EventSubscriberInterface;
    use Symfony\Component\Workflow\Event\GuardEvent;
    use Symfony\Component\Workflow\TransitionBlocker;

    class BlogPostPublishListener implements EventSubscriberInterface
    {
        public function guardPublish(GuardEvent $event)
        {
            $eventTransition = $event->getTransition();
            $hourLimit = $event->getMetadata('hour_limit', $eventTransition);

            if (date('H') <= $hourLimit) {
                return;
            }

            // 如果超过晚上8点，则阻止 "publish" 过渡，并为最终用户发送消息
            $explanation = $event->getMetadata('explanation', $eventTransition);
            $event->addTransitionBlocker(new TransitionBlocker($explanation , 0));
        }

        public static function getSubscribedEvents()
        {
            return [
                'workflow.blog_publishing.guard.publish' => ['guardPublish'],
            ];
        }
    }

.. versionadded:: 4.1

    在Symfony 4.1中引入了过渡阻止器。

在Twig中使用
---------------

Symfony定义了几个Twig函数来管理工作流并减少模板中域逻辑的需求：

``workflow_can()``
    如果给定对象可以进行给定过渡，则返回 ``true``。

``workflow_transitions()``
    返回一个包含为给定对象启用的所有过渡的数组。

``workflow_marked_places()``
    返回一个包含给定标记的位置的数组。

``workflow_has_marked_place()``
    如果给定对象的标记具有给定的状态，则返回 ``true``。

以下示例展示了这些函数：

.. code-block:: html+twig

    <h3>Actions on Blog Post</h3>
    {% if workflow_can(post, 'publish') %}
        <a href="...">Publish</a>
    {% endif %}
    {% if workflow_can(post, 'to_review') %}
        <a href="...">Submit to review</a>
    {% endif %}
    {% if workflow_can(post, 'reject') %}
        <a href="...">Reject</a>
    {% endif %}

    {# 或者循环已启用的过渡 #}
    {% for transition in workflow_transitions(post) %}
        <a href="...">{{ transition.name }}</a>
    {% else %}
        No actions available.
    {% endfor %}

    {# 检查对象是否在某个特定位置 #}
    {% if workflow_has_marked_place(post, 'reviewed') %}
        <p>This post is ready for review.</p>
    {% endif %}

    {# 检查对象上是否标记了某个位置 #}
    {% if 'reviewed' in workflow_marked_places(post) %}
        <span class="label">Reviewed</span>
    {% endif %}

存储元数据
----------------

.. versionadded:: 4.1

    Symfony 4.1中引入了在工作流中存储元数据的特性。

如果你需要它，你可以使用 ``metadata`` 选项在工作流、它们的位置和它们的过渡中存储任意元数据。 此元数据可以像工作流的标题一样简单，也可以像你自己的应用所需的那样复杂：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/workflow.yaml
        framework:
            workflows:
                blog_publishing:
                    metadata:
                        title: 'Blog Publishing Workflow'
                    # ...
                    places:
                        draft:
                            metadata:
                                max_num_of_words: 500
                        # ...
                    transitions:
                        to_review:
                            from: draft
                            to:   review
                            metadata:
                                priority: 0.5
                        publish:
                            from: reviewed
                            to:   published
                            metadata:
                                hour_limit: 20
                                explanation: 'You can not publish after 8 PM.'

    .. code-block:: xml

        <!-- config/packages/workflow.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony https://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >
            <framework:config>
                <framework:workflow name="blog_publishing">
                    <framework:metadata>
                        <framework:title>Blog Publishing Workflow</framework:title>
                    </framework:metadata>
                    <!-- ... -->
                    <framework:place name="draft">
                        <framework:metadata>
                            <framework:max-num-of-words>500</framework:max-num-of-words>
                        </framework:metadata>
                    </framework:place>
                    <!-- ... -->
                    <framework:transition name="to_review">
                        <framework:from>draft</framework:from>
                        <framework:to>review</framework:to>
                        <framework:metadata>
                            <framework:priority>0.5</framework:priority>
                        </framework:metadata>
                    </framework:transition>
                    <framework:transition name="publish">
                        <framework:from>reviewed</framework:from>
                        <framework:to>published</framework:to>
                        <framework:metadata>
                            <framework:hour_limit>20</framework:priority>
                            <framework:explanation>You can not publish after 8 PM.</framework:priority>
                        </framework:metadata>
                    </framework:transition>
                </framework:workflow>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/workflow.php
        $container->loadFromExtension('framework', [
            // ...
            'workflows' => [
                'blog_publishing' => [
                    'metadata' => [
                        'title' => 'Blog Publishing Workflow',
                    ],
                    // ...
                    'places' => [
                        'draft' => [
                            'metadata' => [
                                'max_num_of_words' => 500,
                            ],
                        ],
                        // ...
                    ],
                    'transitions' => [
                        'to_review' => [
                            'from' => 'draft',
                            'to' => 'review',
                            'metadata' => [
                                'priority' => 0.5,
                            ],
                        ],
                        'publish' => [
                            'from' => 'reviewed',
                            'to' => 'published',
                            'metadata' => [
                                'hour_limit' => 20,
                                'explanation' => 'You can not publish after 8 PM.',
                            ],
                        ],
                    ],
                ],
            ],
        ]);

然后，你可以在控制器中访问此元数据，如下所示::

    use App\Entity\BlogPost;
    use Symfony\Component\Workflow\Registry;

    public function myController(Registry $registry, BlogPost $post)
    {
        $workflow = $registry->get($post);

        $title = $workflow
            ->getMetadataStore()
            ->getWorkflowMetadata()['title'] ?? 'Default title'
        ;

        // 或者
        $aTransition = $workflow->getDefinition()->getTransitions()[0];
        $transitionTitle = $workflow
            ->getMetadataStore()
            ->getTransitionMetadata($aTransition)['priority'] ?? 0
        ;
    }

有一个适用于每个元数据级别的快捷方式::

    $title = $workflow->getMetadataStore()->getMetadata('title');
    $priority = $workflow->getMetadataStore()->getMetadata('priority');

在控制器的 :ref:`闪存消息 <flash-messages>` 中::

    // $transition = ...; (Transition的一个实例)

    // $workflow是一个从 Registry 中检索的 Workflow 实例（参见上文）
    $title = $workflow->getMetadataStore()->getMetadata('title', $transition);
    $this->addFlash('info', "You have successfully applied the transition with title: '$title'");

也可以在监听器器中从
:class:`Symfony\\Component\\Workflow\\Event\\Event` 对象访问元数据。

在Twig模板中，元数据可通过 ``workflow_metadata()`` 函数获得：

.. code-block:: html+twig

    <h2>Metadata of Blog Post</h2>
    <p>
        <strong>Workflow</strong>:<br>
        <code>{{ workflow_metadata(blog_post, 'title') }}</code>
    </p>
    <p>
        <strong>Current place(s)</strong>
        <ul>
            {% for place in workflow_marked_places(blog_post) %}
                <li>
                    {{ place }}:
                    <code>{{ workflow_metadata(blog_post, 'max_num_of_words', place) ?: 'Unlimited'}}</code>
                </li>
            {% endfor %}
        </ul>
    </p>
    <p>
        <strong>Enabled transition(s)</strong>
        <ul>
            {% for transition in workflow_transitions(blog_post) %}
                <li>
                    {{ transition.name }}:
                    <code>{{ workflow_metadata(blog_post, 'priority', transition) ?: '0' }}</code>
                </li>
            {% endfor %}
        </ul>
    </p>

扩展阅读
----------

.. toctree::
   :maxdepth: 1

   workflow/introduction
   workflow/dumping-workflows
