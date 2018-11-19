.. index::
    single: Workflow; Workflows as State Machines

作为状态机的工作流
===========================

工作流组件以一个 *工作流网络* 为模型，而该网络是 `Petri net`_ 的子类。
通过添加进一步的限制，你可以获取一个状态机。最重要的一点是一个状态机不能同时存在于多个位置。
还有一点值得注意，在定义图中，一个工作流通常不具有循环路径，但对于一个状态机来说却很常见。

状态机示例
--------------------------

一个拉取请求以一个初始的“start”状态开始，还有一个在Travis上运行测试的状态。
完成此操作后，拉取请求处于“review”状态，其中贡献者可以要求更改、拒绝或接受该拉取请求。
你可以随时“update”该拉取请求，不过这将导致另一个Travis的运行。

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
                        - travis
                        - review
                        - merged
                        - closed
                    transitions:
                        submit:
                            from: start
                            to: travis
                        update:
                            from: [coding, travis, review]
                            to: travis
                        wait_for_review:
                            from: travis
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
        <?xml version="1.0" encoding="utf-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
        >

            <framework:config>
                <framework:workflow name="pull_request" type="state_machine">
                    <framework:marking-store type="single_state"/>

                    <framework:support>App\Entity\PullRequest</framework:support>

                    <framework:place>start</framework:place>
                    <framework:place>coding</framework:place>
                    <framework:place>travis</framework:place>
                    <framework:place>review</framework:place>
                    <framework:place>merged</framework:place>
                    <framework:place>closed</framework:place>

                    <framework:transition name="submit">
                        <framework:from>start</framework:from>

                        <framework:to>travis</framework:to>
                    </framework:transition>

                    <framework:transition name="update">
                        <framework:from>coding</framework:from>
                        <framework:from>travis</framework:from>
                        <framework:from>review</framework:from>

                        <framework:to>travis</framework:to>
                    </framework:transition>

                    <framework:transition name="wait_for_review">
                        <framework:from>travis</framework:from>

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

        // # config/packages/workflow.php
        $container->loadFromExtension('framework', array(
            // ...
            'workflows' => array(
                'pull_request' => array(
                  'type' => 'state_machine',
                  'supports' => array('App\Entity\PullRequest'),
                  'places' => array(
                    'start',
                    'coding',
                    'travis',
                    'review',
                    'merged',
                    'closed',
                  ),
                  'transitions' => array(
                    'submit'=> array(
                      'from' => 'start',
                      'to' => 'travis',
                    ),
                    'update'=> array(
                      'from' => array('coding','travis','review'),
                      'to' => 'travis',
                    ),
                    'wait_for_review'=> array(
                      'from' => 'travis',
                      'to' => 'review',
                    ),
                    'request_change'=> array(
                      'from' => 'review',
                      'to' => 'coding',
                    ),
                    'accept'=> array(
                      'from' => 'review',
                      'to' => 'merged',
                    ),
                    'reject'=> array(
                      'from' => 'review',
                      'to' => 'closed',
                    ),
                    'reopen'=> array(
                      'from' => 'start',
                      'to' => 'review',
                    ),
                  ),
                ),
            ),
        ));

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
            // ...
        }

        // ...
    }

.. _Petri net: https://en.wikipedia.org/wiki/Petri_net
