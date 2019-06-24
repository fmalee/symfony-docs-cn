.. index::
    single: Workflow; Dumping Workflows

如何转储工作流
=====================

为了帮助你调试工作流，你可以生成SVG或PNG图像的可视化表示。首先，安装生成图像所需的任何免费、开源应用：

* `Graphviz`_，提供 ``dot`` 命令；
* `PlantUML`_，提供 ``plantuml.jar`` 文件（需要Java）。

如果要在Symfony应用中定义工作流，请运行此命令将其转储为映像：

.. code-block:: terminal

    # 使用Graphviz的'dot'和SVG图像
    $ php bin/console workflow:dump workflow-name | dot -Tsvg -o graph.svg

    # 使用Graphviz的'dot'和PNG图像
    $ php bin/console workflow:dump workflow-name | dot -Tpng -o graph.png

    # 使用Graphviz的'plantuml.jar'
    $ php bin/console workflow:dump workflow_name --dump-format=puml | java -jar plantuml.jar -p  > graph.png

    # 在转储的工作流中高亮显示'place1'和'place2'
    $ php bin/console workflow:dump workflow-name place1 place2 | dot -Tsvg -o graph.svg

DOT图像将如下所示：

.. image:: /_images/components/workflow/blogpost.png

PlantUML图像将如下所示：

.. image:: /_images/components/workflow/blogpost_puml.png

如果要在Symfony应用之外创建工作流，请使用 ``GraphvizDumper`` 或 ``StateMachineGraphvizDumper``
类来创建DOT文件，并使用 ``PlantUmlDumper`` 来创建PlantUML文件::

    // 将此代码添加到PHP脚本中，例如：dump-graph.php
    $dumper = new GraphvizDumper();
    echo $dumper->dump($definition);

    # 如果更喜欢PlantUML，则使用此段代码：
    # $dumper = new PlantUmlDumper();
    # echo $dumper->dump($definition);

.. code-block:: terminal

    # 用你的PHP脚本的名称替换'dump-graph.php'
    $ php dump-graph.php | dot -Tsvg -o graph.svg
    $ php dump-graph.php | java -jar plantuml.jar -p  > graph.png

.. _`Graphviz`: http://www.graphviz.org
.. _`PlantUML`: http://plantuml.com/
