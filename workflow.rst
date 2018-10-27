工作流
========

一个工作流，是你应用中一个流程(process)的一个模型。
它可能是博客文章从草稿、审核到发布的过程。
另一个例子是，当一位用户提交一系列不同的表单以完成一个任务时。
类似的进程最好从你的模型中脱离，而且应该在配置信息中进行定义。

一个工作流的 **定义** 包括位置和动作，以从一个位置来到另一个位置。这些动作被称为 **过渡**。
工作流还需要知道每个对象在工作流中的位置。
那个 **marking store(标记存储区)** 写入了对象的一个属性来记住当前位置。

.. note::

    上面的专有名词一般被用于讨论工作流和 `Petri nets`_

工作流组件支持状态机（state machines）。
状态机是一个工作流的子集（subset），其目的是持有你的模型的一个状态。
在 :doc:`/workflow/state-machines` 一文可以读到更多的关于状态机的差异和特定的功能。

示例
--------

最简单的工作流是下面这种。它包括两个位置和一个过渡。

.. image:: /_images/components/workflow/simple.png

当用来描述一个真实业务时，工作流可以是更复杂的。下面的工作流描述了在一个应聘应用中进行应聘的流程。

.. image:: /_images/components/workflow/job_application.png

在此示例中填写应聘应用时，根据你申请的工作，有4到7个步骤。
一些工作需要用户回答性格测试，逻辑测试和/或形式要求。有些则工作没有。
``GuardEvent`` 用于确定一个特定应用被许可的后续步骤。

通过像这样定义一个工作流，流程如何被展现就能知其大概。
流程的逻辑并不与控制器、模型或视图混为一谈。只需更改配置即可更改步骤的顺序。

扩展阅读
----------

.. toctree::
   :maxdepth: 1

   workflow/usage
   workflow/state-machines
   workflow/dumping-workflows

.. _Petri nets: https://en.wikipedia.org/wiki/Petri_net
