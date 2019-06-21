在模板中使用翻译
==============================

Twig模板
--------------

.. _translation-tags:

使用Twig标签
~~~~~~~~~~~~~~~

Symfony提供专门的Twig标签（``trans`` 和 ``transchoice``）来帮助 *静态文本块* 的消息翻译：

.. code-block:: twig

    {% trans %}Hello %name%{% endtrans %}

    {% transchoice count %}
        {0} There are no apples|{1} There is one apple|]1,Inf[ There are %count% apples
    {% endtranschoice %}

``transchoice`` 标签自动从当前上下文获取 ``%count%`` 变量并将其传递给翻译器。
只有在 ``%var%`` 模式后使用一个占位符时，此机制才有效。

.. deprecated:: 4.2

    ``transchoice`` 标签自Symfony 4.2起被弃用，并且将在5.0中删除。可以使用带有 ``trans``
    标签的 :doc:`ICU MessageFormat </translation/message_format>` 来代替。

.. caution::

    使用标签在Twig模板中进行翻译时，需要占位符的 ``%var%`` 表示法。

.. tip::

    如果你需要在字符串中使用百分比字符（``%``），请将其翻倍：``{% trans %}Percent: %percent%%%{% endtrans %}``

你还可以指定消息域并传递一些额外的变量：

.. code-block:: twig

    {% trans with {'%name%': 'Fabien'} from 'app' %}Hello %name%{% endtrans %}

    {% trans with {'%name%': 'Fabien'} from 'app' into 'fr' %}Hello %name%{% endtrans %}

    {% transchoice count with {'%name%': 'Fabien'} from 'app' %}
        {0} %name%, there are no apples|{1} %name%, there is one apple|]1,Inf[ %name%, there are %count% apples
    {% endtranschoice %}

.. _translation-filters:

使用Twig过滤器
~~~~~~~~~~~~~~~~~~

``trans`` 和 ``transchoice`` 过滤器可用于翻译 *变量文本* 和复杂表达式：

.. code-block:: twig

    {{ message|trans }}

    {{ message|transchoice(5) }}

    {{ message|trans({'%name%': 'Fabien'}, 'app') }}

    {{ message|transchoice(5, {'%name%': 'Fabien'}, 'app') }}

.. deprecated:: 4.2

    ``transchoice`` 标签自Symfony 4.2起被弃用，并且将在5.0中删除。可以使用带有 ``trans``
    标签的 :doc:`ICU MessageFormat </translation/message_format>` 来代替。

.. tip::

    使用翻译标签或过滤器具有相同的效果，但有一个细微差别：自动输出转义仅适用于使用过滤器的翻译。
    换句话说，如果你需要确保翻译消息 *未* 输出转义，则必须在翻译过滤器后应用 ``raw`` 过滤器：

    .. code-block:: html+twig

        {# 标签之间翻译的文本永远不会被转义 #}
        {% trans %}
            <h3>foo</h3>
        {% endtrans %}

        {% set message = '<h3>foo</h3>' %}

        {# 默认情况下，通过过滤器翻译的字符串和变量将被转义 #}
        {{ message|trans|raw }}
        {{ '<h3>bar</h3>'|trans|raw }}

.. tip::

    你可以使用单个标签为整个Twig模板设置翻译域：

    .. code-block:: twig

       {% trans_default_domain 'app' %}

    请注意，这仅影响当前模板，而不影响任何“引用”模板（为了避免副作用）。

PHP模板
-------------

可以通过 ``translator`` 助手在PHP模板中访问翻译服务::

    <?= $view['translator']->trans('Symfony is great') ?>

    <?= $view['translator']->transChoice(
        '{0} There are no apples|{1} There is one apple|]1,Inf[ There are %count% apples',
        10,
        ['%count%' => 10]
    ) ?>
