.. index::
    single: Workflow; Dumping Workflows

如何Dump工作流
=====================

为了帮助你调试工作流，你可以使用一个 ``DumperInterface``
来转储你的工作流或状态机的示意图(representation)。
Symfony提供2个基于Dot的不同dumper。

使用 ``GraphvizDumper`` 或 ``StateMachineGraphvizDumper`` 来创建DOT文件，或使用
``PlantUmlDumper`` 处理PlantUML文件。两种类型都可以转换为PNG或SVG图像。

上面定义的工作流图像：

.. code-block:: php

    // dump-graph-dot.php
    $dumper = new GraphvizDumper();
    echo $dumper->dump($definition);

.. code-block:: php

    // dump-graph-puml.php
    $dumper = new PlantUmlDumper();
    echo $dumper->dump($definition);

.. code-block:: terminal

    $ php dump-graph-dot.php | dot -Tpng -o dot_graph.png
    $ php dump-graph-puml.php | java -jar plantuml.jar -p  > puml_graph.png

    # run this command if you prefer SVG images:
    # 如果您喜欢SVG图像，请运行此命令：
    # $ php dump-graph-dot.php | dot -Tsvg -o dot_graph.svg

DOT结果如下所示：

.. image:: /_images/components/workflow/blogpost.png

PUML结果：

.. image:: /_images/components/workflow/blogpost_puml.png

在Symfony应用中，你可以使用 ``workflow:dump`` 来利用这些命令转储文件：

.. code-block:: terminal

    $ php bin/console workflow:dump name | dot -Tsvg -o graph.svg
    $ php bin/console workflow:dump name --dump-format=puml | java -jar plantuml.jar -p  > workflow.png

.. note::

    ``dot`` 命令是Graphviz的一部分。你可以在 `Graphviz.org`_ 上下载并阅读更多相关信息。

    ``plantuml.jar`` 命令是PlantUML的一部分。你可以在 `PlantUML.com`_ 上下载并阅读更多相关信息。


.. _Graphviz.org: http://www.graphviz.org
.. _PlantUML.com: http://plantuml.com/
