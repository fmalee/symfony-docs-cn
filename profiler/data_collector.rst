.. index::
   single: Profiling; Data collector

如何创建自定义数据收集器
=====================================

:doc:`Symfony Profiler </profiler>` 使用一些特殊的被称为数据收集器的类来获取它的分析和调试信息。
Symfony捆绑了其中的一些，但你也可以创建自己的收集器。

创建自定义的数据收集器
--------------------------------

数据收集器是一个实现了
:class:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollectorInterface`
的PHP类。为方便起见，你的数据收集器也可以从
:class:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollector`
类继承，该类实现对应接口并提供一些工具和 ``$this->data`` 属性来存储收集的信息。

以下示例显示了存储有关请求的信息的一个自定义收集器::

    // src/DataCollector/RequestCollector.php
    namespace App\DataCollector;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\HttpKernel\DataCollector\DataCollector;

    class RequestCollector extends DataCollector
    {
        public function collect(Request $request, Response $response, \Exception $exception = null)
        {
            $this->data = [
                'method' => $request->getMethod(),
                'acceptable_content_types' => $request->getAcceptableContentTypes(),
            ];
        }

        public function reset()
        {
            $this->data = [];
        }

        public function getName()
        {
            return 'app.request_collector';
        }

        // ...
    }

:method:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollectorInterface::collect` 方法:
    将收集的数据存储在本地属性（如果从
    :class:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollector`
    继承，则是 ``$this->data``）中。
    如果无法通过请求或响应获取要收集的数据，请在数据收集器中注入所需的服务。

    .. caution::

        ``collect()`` 方法仅被调用一次。
        它不用于“采集”（gather）数据，而是用于“拾取”你的服务中存储的数据。

    .. caution::

        由于分析器序会列化数据收集器的实例，因此不应存储无法序列化的对象（如PDO对象），或者你需要提供自己的
        ``serialize()`` 方法。

:method:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollectorInterface::reset` 方法:
    它在请求期间被调用，以便重置分析器的状态。
    在重置分析器状态的请求到重置之间调用它。
    可以使用它移除利用 ``collect()`` 方法收集的所有信息。

:method:`Symfony\\Component\\HttpKernel\\DataCollector\\DataCollectorInterface::getName` 方法:
    返回该收集器标识符，该标识符在应用中必须是唯一的。
    此值将在稍后被用来访问该收集器的信息（请参阅
    :doc:`/testing/profiling`），因此建议返回一个简短、小写且没有空格的字符串。

.. _data_collector_tag:

启用自定义数据收集器
-------------------------------

如果你使用的是带 ``autoconfigure`` 的
:ref:`默认的services.yaml配置 <service-container-services-load-example>`，
那么Symfony的将自动看到新的数据采集器！下次刷新时会调用你的``collect()`` 方法。

如果你不使用 ``autoconfigure``，你也可以
:ref:`手动装配你的服务 <services-explicitly-configure-wire-services>`，并将它
:doc:`标记 </service_container/tags>` 为 ``data_collector``。

添加Web分析器模板
-----------------------------

你的数据收集器收集的信息可以显示在Web调试工具栏和Web分析器中。
为此，你需要创建一个包含一些特定区块的Twig模板。

但是，首先必须在数据收集器类中添加一些getter，以便模板访问收集的信息::

    // src/DataCollector/RequestCollector.php
    namespace App\DataCollector;

    use Symfony\Component\HttpKernel\DataCollector\DataCollector;

    class RequestCollector extends DataCollector
    {
        // ...

        public function getMethod()
        {
            return $this->data['method'];
        }

        public function getAcceptableContentTypes()
        {
            return $this->data['acceptable_content_types'];
        }
    }

在这个最简单的例子中，你只想在工具栏中显示信息而不提供分析器面板。
这需要定义 ``toolbar`` 区块并设置两个名为 ``icon`` 和 ``text`` 的变量的值：

.. code-block:: html+twig

    {% extends '@WebProfiler/Profiler/layout.html.twig' %}

    {% block toolbar %}
        {% set icon %}
            {# 这是在工具栏中显示为一个面板的内容 #}
            <svg xmlns="http://www.w3.org/2000/svg"> ... </svg>
            <span class="sf-toolbar-value">Request</span>
        {% endset %}

        {% set text %}
            {# 这是在鼠标悬停在工具栏面板上时显示的内容 #}
            <div class="sf-toolbar-info-piece">
                <b>Method</b>
                <span>{{ collector.method }}</span>
            </div>

            <div class="sf-toolbar-info-piece">
                <b>Accepted content type</b>
                <span>{{ collector.acceptableContentTypes|join(', ') }}</span>
            </div>
        {% endset %}

        {# 'link' 值设置为 'false' 表示此面板不显示为Web分析器的一个切片 #}
        {{ include('@WebProfiler/Profiler/toolbar_item.html.twig', { link: false }) }}
    {% endblock %}

.. tip::

    内置收集器模板将所有图像定义为嵌入式SVG文件。
    这使得它们可以在任何地方工作，而无需弄乱网络资产链接：

    .. code-block:: twig

        {% set icon %}
            {{ include('data_collector/icon.svg') }}
            {# ... #}
        {% endset %}

如果工具栏面板包含扩展的Web分析器信息，则该Twig模板还必须定义其他区块：

.. code-block:: html+twig

    {% extends '@WebProfiler/Profiler/layout.html.twig' %}

    {% block toolbar %}
        {% set icon %}
            {# ... #}
        {% endset %}

        {% set text %}
            <div class="sf-toolbar-info-piece">
                {# ... #}
            </div>
        {% endset %}

        {{ include('@WebProfiler/Profiler/toolbar_item.html.twig', { 'link': true }) }}
    {% endblock %}

    {% block head %}
        {# 可选项。你可以在这里链接或定义自己的CSS和JS内容。 #}
        {# 使用{{ parent() }} 来扩展默认样式而不是重写它们。 #}
    {% endblock %}

    {% block menu %}
        {# 使用全屏分析器时会出现此左侧菜单。 #}
        <span class="label">
            <span class="icon"><img src="..." alt=""/></span>
            <strong>Request</strong>
        </span>
    {% endblock %}

    {% block panel %}
        {# 可选项，用于显示最多细节。 #}
        <h2>Acceptable Content Types</h2>
        <table>
            <tr>
                <th>Content Type</th>
            </tr>

            {% for type in collector.acceptableContentTypes %}
            <tr>
                <td>{{ type }}</td>
            </tr>
            {% endfor %}
        </table>
    {% endblock %}

``menu`` 和 ``panel`` 区块是唯一要求的区块，以定义与该数据收集器相关联的网络分析器面板中要显示的内容。
所有的区块都可以访问该 ``collector`` 对象。

最后，要启用数据收集器模板，请重写你的服务配置以指定一个包含该模板的标签：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            App\DataCollector\RequestCollector:
                tags:
                    -
                        name:     data_collector
                        template: 'data_collector/template.html.twig'
                        # 必须匹配 getName() 方法返回的值
                        id:       'app.request_collector'
                        # 可选的优先级
                        # priority: 300
                public: false

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="App\DataCollector\RequestCollector" public="false">
                    <!-- priority="300" -->
                    <tag name="data_collector"
                        template="data_collector/template.html.twig"
                        id="app.request_collector"
                    />
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\DataCollector\RequestCollector;

        $container
            ->autowire(RequestCollector::class)
            ->setPublic(false)
            ->addTag('data_collector', [
                'template' => 'data_collector/template.html.twig',
                'id'       => 'app.request_collector',
                // 'priority' => 300,
            ])
        ;

工具栏中每个面板的位置由收集器的优先级确定。优先级应定义为正整数或负整数，默认为 ``0``。
大多数内置收集器都使用 ``255`` 作为其优先级。
如果你希望在它们之前显示你的收集器，请使用更高的值（如300）。
