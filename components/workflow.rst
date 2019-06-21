.. index::
   single: Workflow
   single: Components; Workflow

Workflow组件
======================

    Workflow组件提供用于管理工作流或有限状态机的工具。

安装
------------

.. code-block:: terminal

    $ composer require symfony/workflow

.. include:: /components/require_autoload.rst.inc

创建工作流
-------------------

工作流组件为你提供了一种面向对象的方式来定义对象所经历的过程或生命周期。
该过程中的每个步骤或阶段被称为一个 *位置*。你还可以定义一个 *过渡*，它描述了从一个位置到另一个位置的动作。

.. image:: /_images/components/workflow/states_transitions.png

一组位置和过渡创建了一个 **定义**。
一个工作流需要一个 ``Definition`` 和一个将状态写入对象的方法（即一个
:class:`Symfony\\Component\\Workflow\\MarkingStore\\MarkingStoreInterface` 实例）。

请考虑以下的博客文章示例。一个帖子可以具有多个预定义状态（`draft`、`review`、`rejected`、`published`）中的一个。
在工作流程中，这些状态称为 **位置**。你可以像这样定义工作流::

    use Symfony\Component\Workflow\DefinitionBuilder;
    use Symfony\Component\Workflow\MarkingStore\SingleStateMarkingStore;
    use Symfony\Component\Workflow\Transition;
    use Symfony\Component\Workflow\Workflow;

    $definitionBuilder = new DefinitionBuilder();
    $definition = $definitionBuilder->addPlaces(['draft', 'review', 'rejected', 'published'])
        // Transitions are defined with a unique name, an origin place and a destination place
        // 过渡是使用唯一名称，原始位置和目标位置来定义的
        ->addTransition(new Transition('to_review', 'draft', 'review'))
        ->addTransition(new Transition('publish', 'review', 'published'))
        ->addTransition(new Transition('reject', 'review', 'rejected'))
        ->build()
    ;

    $marking = new SingleStateMarkingStore('currentState');
    $workflow = new Workflow($definition, $marking);

现在 ``Workflow`` 可以帮助你根据博客文章的 *位置* 来决定允许哪些操作。
这将使你的域逻辑保持在一个位置而不会遍布你的应用。

定义多个工作流时，应考虑使用一个 ``Registry``，这是一个可以存储和访问不同工作流的对象。
一个注册表还将帮助你确定一个工作流是否支持你正尝试使用的对象::

    use Acme\Entity\BlogPost;
    use Acme\Entity\Newsletter;
    use Symfony\Component\Workflow\Registry;
    use Symfony\Component\Workflow\SupportStrategy\InstanceOfSupportStrategy;

    $blogWorkflow = ...
    $newsletterWorkflow = ...

    $registry = new Registry();
    $registry->addWorkflow($blogWorkflow, new InstanceOfSupportStrategy(BlogPost::class));
    $registry->addWorkflow($newsletterWorkflow, new InstanceOfSupportStrategy(Newsletter::class));

用法
-----

在使用工作流配置了一个 ``Registry`` 后，你可以从中检索工作流并按如下方式来使用它::

    // ...
    // 假设 $post 默认情况下处于 "draft" 状态
    $post = new BlogPost();
    $workflow = $registry->get($post);

    $workflow->can($post, 'publish'); // False
    $workflow->can($post, 'to_review'); // True

    $workflow->apply($post, 'to_review'); // $post 现在处于 "review" 状态

    $workflow->can($post, 'publish'); // True
    $workflow->getEnabledTransitions($post); // ['publish', 'reject']

扩展阅读
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /workflow/*

.. _Packagist: https://packagist.org/packages/symfony/workflow
