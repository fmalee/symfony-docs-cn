工作流和状态机
============================

工作流
---------

一个工作流，是你应用中一个流程(process)的一个模型。
它可能是博客文章从草稿、审核到发布的过程。
另一个例子是，当一位用户提交一系列不同的表单以完成一个任务时。
类似的进程最好从你的模型中脱离，而且应该在配置信息中进行定义。

一个工作流的一个 **定义** 包括位置和动作，以从一个位置转移到另一个位置。这些动作被称为 **过渡**。
工作流还需要知道每个对象在工作流中的位置。
**标记存储区** 通过写入对象的一个属性来记住当前位置。

.. note::

    上面的专有名词一般被用于讨论工作流和 `Petri nets`_

示例
~~~~~~~~

最简单的工作流是下面这种。它包括两个位置和一个过渡。

.. image:: /_images/components/workflow/simple.png

当用来描述一个真实业务时，工作流可以是更复杂的。下面的工作流描述了在一个应聘应用中进行应聘的流程。

.. image:: /_images/components/workflow/job_application.png

在此示例中填写应聘应用时，根据你申请的工作，有4到7个步骤。
一些工作需要用户回答性格测试、逻辑测试和/或形式要求。有些则工作没有。
用 ``GuardEvent`` 来确定一个特定申请所许可的后续步骤。

通过像这样定义一个工作流，流程如何被展现就能知其大概。
流程的逻辑并不与控制器、模型或视图混为一谈。只需更改配置即可更改步骤的顺序。

状态机
--------------

状态机是工作流的子集，其目的是保持模型的状态。它们之间最重要的区别是：

* 工作流可以同时存在于多个位置，而状态机不能;
* 工作流通常在定义图中没有循环路径，但它通常用于状态机;
* 为了应用一个过渡，工作流要求对象位于过渡的所有先前的位置，而状态机仅要求对象至少位于其中一个位置。

示例
~~~~~~~

一个拉动请求以初始的“start”状态开始，然后是“test”状态，
例如在持续集成(continuous integration)堆栈上运行测试。
完成此操作后，拉取请求处于“review”状态，其中贡献者可以要求更改，拒绝或接受拉取请求。
你可以随时“update”该拉取请求，这将导致另一次持续集成运行。

.. image:: /_images/components/workflow/pull_request.png

以下是该拉取请求状态机的配置。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/workflow.yaml
        framework:
            workflows:
                pull_request:
                    type: 'state_machine'
                    supports:
                        - App\Entity\PullRequest
                    initial_place: start
                    places:
                        - start
                        - coding
                        - test
                        - review
                        - merged
                        - closed
                    transitions:
                        submit:
                            from: start
                            to: test
                        update:
                            from: [coding, test, review]
                            to: test
                        wait_for_review:
                            from: test
                            to: review
                        request_change:
                            from: review
                            to: coding
                        accept:
                            from: review
                            to: merged
                        reject:
                            from: review
                            to: closed
                        reopen:
                            from: closed
                            to: review

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
                <framework:workflow name="pull_request" type="state_machine">
                    <framework:marking-store type="single_state"/>

                    <framework:support>App\Entity\PullRequest</framework:support>

                    <framework:place>start</framework:place>
                    <framework:place>coding</framework:place>
                    <framework:place>test</framework:place>
                    <framework:place>review</framework:place>
                    <framework:place>merged</framework:place>
                    <framework:place>closed</framework:place>

                    <framework:transition name="submit">
                        <framework:from>start</framework:from>

                        <framework:to>test</framework:to>
                    </framework:transition>

                    <framework:transition name="update">
                        <framework:from>coding</framework:from>
                        <framework:from>test</framework:from>
                        <framework:from>review</framework:from>

                        <framework:to>test</framework:to>
                    </framework:transition>

                    <framework:transition name="wait_for_review">
                        <framework:from>test</framework:from>

                        <framework:to>review</framework:to>
                    </framework:transition>

                    <framework:transition name="request_change">
                        <framework:from>review</framework:from>

                        <framework:to>coding</framework:to>
                    </framework:transition>

                    <framework:transition name="accept">
                        <framework:from>review</framework:from>

                        <framework:to>merged</framework:to>
                    </framework:transition>

                    <framework:transition name="reject">
                        <framework:from>review</framework:from>

                        <framework:to>closed</framework:to>
                    </framework:transition>

                    <framework:transition name="reopen">
                        <framework:from>closed</framework:from>

                        <framework:to>review</framework:to>
                    </framework:transition>

                </framework:workflow>

            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/workflow.php
        $container->loadFromExtension('framework', [
            // ...
            'workflows' => [
                'pull_request' => [
                  'type' => 'state_machine',
                  'supports' => ['App\Entity\PullRequest'],
                  'places' => [
                    'start',
                    'coding',
                    'test',
                    'review',
                    'merged',
                    'closed',
                  ],
                  'transitions' => [
                    'submit'=> [
                      'from' => 'start',
                      'to' => 'test',
                    ],
                    'update'=> [
                      'from' => ['coding', 'test', 'review'],
                      'to' => 'test',
                    ],
                    'wait_for_review'=> [
                      'from' => 'test',
                      'to' => 'review',
                    ],
                    'request_change'=> [
                      'from' => 'review',
                      'to' => 'coding',
                    ],
                    'accept'=> [
                      'from' => 'review',
                      'to' => 'merged',
                    ],
                    'reject'=> [
                      'from' => 'review',
                      'to' => 'closed',
                    ],
                    'reopen'=> [
                      'from' => 'start',
                      'to' => 'review',
                    ],
                  ],
                ],
            ],
        ]);

在使用 :ref:`默认services.yaml配置 <service-container-services-load-example>`
的Symfony应用中，你可以通过注入工作流注册表服务来获取此状态机::

    // ...
    use Symfony\Component\Workflow\Registry;

    class SomeService
    {
        private $workflows;

        public function __construct(Registry $workflows)
        {
            $this->workflows = $workflows;
        }

        public function someMethod($subject)
        {
            $stateMachine = $this->workflows->get($subject, 'pull_request');
            $stateMachine->apply($subject, 'wait_for_review');
            // ...
        }

        // ...
    }

Symfony自动为你在配置中定义的每个工作流（:class:`Symfony\\Component\\Workflow\\Workflow`）或
状态机（:class:`Symfony\\Component\\Workflow\\StateMachine`）创建一个服务。
这意味着你可以在服务定义中分别使用 ``workflow.pull_request`` 或
``state_machine.pull_request`` 来访问对应的服务::

    // ...
    use Symfony\Component\Workflow\StateMachine;

    class SomeService
    {
        private $stateMachine;

        public function __construct(StateMachine $stateMachine)
        {
            $this->stateMachine = $stateMachine;
        }

        public function someMethod($subject)
        {
            $this->stateMachine->apply($subject, 'wait_for_review', [
                'log_comment' => 'My logging comment for the wait for review transition.',
            ]);
            // ...
        }

        // ...
    }

.. _`Petri nets`: https://en.wikipedia.org/wiki/Petri_net
