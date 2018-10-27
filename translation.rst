.. index::
   single: Translations

翻译
============

术语“国际化”（通常缩写为i18n）是指将应用中的字符串和其他特定于语言环境的部分抽象到一个层中的过程，在该层中可以根据用户的语言环境（即语言和国家/地区）对它们进行翻译和转换。
对于文本，这意味着使用能够将文本（或“消息”）翻译成用户语言的函数来包装每个文本::

    // 文本始终以英语输出
    dump('Hello World');
    die();

    // 文本将以用户指定语言或默认英语输出
    dump($translator->trans('Hello World'));
    die();

.. note::

    术语 *locale* 译为语言环境，大致指用户的语言和国家/地区。
    在程序中，它可以是用于管理翻译和其他格式差异（例如货币格式）的任何字符串。
    建议使用 `ISO 639-1`_ *语言* 代码，下划线（``_``），然后是 `ISO 3166-1 alpha-2`_ *国家* 代码
    （例如表示法语/法国的 ``fr_FR``）。

在本章，你将学习如何使用Symfony框架中的翻译组件。
你可以阅读 :doc:`Translation组件文档 </components/translation/usage>` 来了解更多。
整体上，翻译的过程有如下几步：

#. :ref:`开启和配置 <translation-configuration>` Symfony的翻译服务；

#. 将字符串（即“消息”）抽象出来，封装在对 ``Translator`` 的调用中（":ref:`translation-basic`"）；

#. 为每个支持的语言环境 :ref:`创建翻译的资源/文件 <translation-resources>`，以翻译应用中的每条消息；

#. 做个决定，为请求 :doc:`设置和管理用户的语言环境 </translation/locale>`，
   也可以选择 :doc:`在用户的整个会话中 </session/locale_sticky_session>`。

.. _translation-configuration:

安装
------------

首先，运行此命令以在使用之前安装翻译器：

.. code-block:: terminal

    $ composer require symfony/translation

配置
-------------

上一个命令创建了一个初始配置文件，
你可以在其中定义应用的默认语言环境以及Symfony无法找到某些翻译时将使用的 :ref:`后备语言环境 <translation-fallback>`：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/translation.yaml
        framework:
            default_locale: 'en'
            translator:
                fallbacks: ['en']
                # ...

    .. code-block:: xml

        <!-- config/packages/translation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config default-locale="en">
                <framework:translator>
                    <framework:fallback>en</framework:fallback>
                    <!-- ... -->
                </framework:translator>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/translation.php
        $container->loadFromExtension('framework', array(
            'default_locale' => 'en',
            'translator' => array('fallbacks' => array('en')),
            // ...
        ));

翻译中使用的语言环境是存储在请求中的。这通常通过路由上的 ``_locale`` 属性来设置
（请参 :ref:`translation-locale-url`）。

.. _translation-basic:

基本翻译
-----------------

文本的翻译是通过 ``translator`` 服务（:class:`Symfony\\Component\\Translation\\Translator`）完成的。
要翻译文本块（称为 *消息*），请使用 :method:`Symfony\\Component\\Translation\\Translator::trans` 方法。
例如，假设你正在从控制器内部翻译一个简单消息::

    // ...
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Translation\TranslatorInterface;

    public function index(TranslatorInterface $translator)
    {
        $translated = $translator->trans('Symfony is great');

        // ...
    }

.. _translation-resources:

执行此代码时，Symfony将尝试根据用户的 ``locale`` 翻译 “Symfony is great” 消息。
为此，你需要告诉Symfony如何通过“翻译资源”来翻译消息，“翻译资源”通常是包含给定语言环境的翻译集合的文件。
翻译的“词典”可以用几种不同的格式创建，XLIFF 是推荐的格式：

.. configuration-block::

    .. code-block:: xml

        <!-- translations/messages.fr.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="symfony_is_great">
                        <source>Symfony is great</source>
                        <target>J'aime Symfony</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: yaml

        # translations/messages.fr.yaml
        Symfony is great: J'aime Symfony

    .. code-block:: php

        // translations/messages.fr.php
        return array(
            'Symfony is great' => 'J\'aime Symfony',
        );

有关这些文件的位置信息，请参阅 :ref:`translation-resource-locations`。

现在，如果用户语言环境的语言是法语（例如 ``fr_FR`` 或 ``fr_BE``），则该消息将被翻译成 ``J'aime Symfony``。
你还可以在 :ref:`模板 <translation-tags>` 中翻译消息。

翻译流程
~~~~~~~~~~~~~~~~~~~~~~~

为了能翻译一条信息，Symfony执行以下简明流程：

* 确定存储在请求中的当前用户的 ``locale``;

* 从为 ``locale`` 定义的翻译资源（例如 ``fr_FR``）加载一个翻译消息的目录（例如，大集合）。
  来自 :ref:`后备语言环境 <translation-fallback>` 的消息也会加载并添加到该目录（如果它们尚不存在）。
  最终结果是生成了一个“翻译大词典”。

* 如果该消息位于目录中，则返回翻译消息。如果不存在，则翻译器返回原始消息。

使用 ``trans()`` 方法时，Symfony会在相应的消息目录中查找确切的字符串并返回它（如果存在）。

消息占位符
--------------------

有时，需要翻译包含一个变量的消息::

    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Translation\TranslatorInterface;

    public function index(TranslatorInterface $translator, $name)
    {
        $translated = $translator->trans('Hello '.$name);

        // ...
    }

但是，为此字符串创建翻译是不可能的，因为翻译器将尝试查找确切的消息，包括变量部分（例如 *"Hello Ryan"* 或 *"Hello Fabien"*）。

有关如何处理此情况的详细信息，请参阅组件文档中的 :ref:`component-translation-placeholders`。
有关如何在模板中执行此操作，请参阅 :ref:`translation-tags`。

复数
-------------

另一个复杂因素是，可能你的翻译基于某些变量会在单复数之间变化：

.. code-block:: text

    There is one apple.
    There are 5 apples.

要处理这个问题，使用 :method:`Symfony\\Component\\Translation\\Translator::transChoice`
方法或在 :ref:`模板 <translation-tags>` 中使用``transchoice`` 标签/过滤器。

有关更多信息，请参阅翻译组件文档中的 :ref:`component-translation-pluralization`。

模板中的翻译
-------------------------

大多数情况下，翻译发生在模板中。Symfony为Twig和PHP模板提供原生支持。

.. _translation-tags:

Twig模板
~~~~~~~~~~~~~~

Symfony提供专门的Twig标签（``trans`` 和 ``transchoice``）来帮助 *静态文本块* 的消息翻译：

.. code-block:: twig

    {% trans %}Hello %name%{% endtrans %}

    {% transchoice count %}
        {0} There are no apples|{1} There is one apple|]1,Inf[ There are %count% apples
    {% endtranschoice %}

``transchoice`` 标签自动从当前上下文获取 ``%count%`` 变量并将其传递给翻译器。
只有在 ``%var%`` 模式后使用一个占位符时，此机制才有效。

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

``trans`` 和 ``transchoice`` 过滤器可用于翻译 *变量文本* 和复杂表达式：

.. code-block:: twig

    {{ message|trans }}

    {{ message|transchoice(5) }}

    {{ message|trans({'%name%': 'Fabien'}, 'app') }}

    {{ message|transchoice(5, {'%name%': 'Fabien'}, 'app') }}

.. tip::

    使用翻译标签或过滤器具有相同的效果，但有一个细微差别：自动输出转义仅适用于使用过滤器的翻译。
    换句话说，如果你需要确保翻译消息 *未* 输出转义，则必须在翻译过滤器后应用 ``raw`` 过滤器：

    .. code-block:: twig

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
~~~~~~~~~~~~~

可以通过 ``translator`` 助手在PHP模板中访问翻译服务：

.. code-block:: html+php

    <?= $view['translator']->trans('Symfony is great') ?>

    <?= $view['translator']->transChoice(
        '{0} There are no apples|{1} There is one apple|]1,Inf[ There are %count% apples',
        10,
        array('%count%' => 10)
    ) ?>

自动提取翻译内容和更新目录
-------------------------------------------------------------------

翻译应用时最耗时的任务是提取要翻译的所有模板内容并保持所有翻译文件的同步。
Symfony包含一个名为 ``translation:update`` 的命令，可以帮助你完成这些任务：

.. code-block:: terminal

    # 使用在 app/Resources/ 模板中找到的缺失字符串来更新法语翻译文件
    $ ./bin/console translation:update --dump-messages --force fr

    # 使用AppBundle中找到的缺失字符串更新英文翻译文件
    $ ./bin/console translation:update --dump-messages --force en AppBundle

.. note::

    如果要查看缺少的翻译字符串而不实际更新翻译文件，请从上面的命令中删除 ``--force`` 选项。

.. tip::

    如果你需要从其他来源（例如控制器，表单和闪存消息）中提取翻译字符串，请考虑使用更高级的第三方 `TranslationBundle`_。

.. _translation-resource-locations:

翻译资源/文件的名称和位置
---------------------------------------------

Symfony在以下默认位置查找消息文件（即翻译）：

* ``translations/`` 目录 (在项目的根目录);

* ``src/Resources/<bundle name>/translations/`` 目录;

* ``Resources/translations/`` 目录（任何bundle中）.

此处列出的位置按照优先级排序。也就是说，你可以在前两个目录中的任何一个中覆盖bundle的翻译消息。

覆盖机制工作在键级别(key level)：只有被覆盖的键才需要列在更高优先级的消息文件中。
如果在一个消息文件中找不到键，则翻译器将自动回退到优先级较低的消息文件中。

翻译文件的文件名也很重要：每个消息文件必须根据以下路径命名：``domain.locale.loader``:

* **domain**: 将消息组织到群组中的可选方法（例如， ``admin``、``navigation`` 或默认 ``messages``）
  - 请参阅 :ref:`using-message-domains`;

* **locale**: 翻译所针对的语言环境（例如 ``en_GB``, ``en`` 等）;

* **loader**: Symfony应如何加载和解析文件（例如 ``xlf``, ``php``, ``yaml`` 等）。

加载器可以是任何已注册的加载器的名称。默认情况下，Symfony提供了许多加载器，包括：

* ``xlf``: XLIFF文件;
* ``php``: PHP文件;
* ``yaml``: YAML文件.

选择使用哪种加载器完全取决于你，这是一个品味问题。推荐的选项是使用 ``xlf`` 进行翻译。
有关更多选项，请参阅 :ref:`component-translator-message-catalogs`。

.. note::

    你可以在配置中使用 :ref:`paths <reference-translator-paths>` 选项添加其他翻译目录：

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/translation.yaml
            framework:
                translator:
                    paths:
                        - '%kernel.project_dir%/custom/path/to/translations'

        .. code-block:: xml

            <!-- config/packages/translation.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:framework="http://symfony.com/schema/dic/symfony"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-Instance"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/symfony
                    http://symfony.com/schema/dic/symfony/symfony-1.0.xsd"
            >

                <framework:config>
                    <framework:translator>
                        <framework:path>%kernel.project_dir%/custom/path/to/translations</framework:path>
                    </framework:translator>
                </framework:config>
            </container>

        .. code-block:: php

            // config/packages/translation.php
            $container->loadFromExtension('framework', array(
                'translator' => array(
                    'paths' => array(
                        '%kernel.project_dir%/custom/path/to/translations',
                    ),
                ),
            ));

.. note::

    你还可以通过实现 :class:`Symfony\\Component\\Translation\\Loader\\LoaderInterface`
    接口的自定义类来将翻译存储在数据库或任何其他存储中。
    有关更多信息，请参阅 :ref:`dic-tags-translation-loader` 标签。

.. caution::

    每次创建 *新* 的翻译资源（或安装包含翻译资源的bundle）时，请务必清除缓存，以便Symfony可以发现新的翻译资源：

    .. code-block:: terminal

        $ php bin/console cache:clear

.. _translation-fallback:

后备翻译语言环境
----------------------------

想象一下，用户的语言环境是 ``fr_FR`` ，并且你正在翻译 ``Symfony is great`` 键。
要查找法语翻译，Symfony实际上会检查多个语言环境的翻译资源：

#. 首先，Symfony在 ``fr_FR`` 翻译资源中查找翻译（例如 ``messages.fr_FR.xlf``）;

#. 如果找不到，Symfony会在 ``fr`` 翻译资源中查找翻译（例如 ``messages.fr.xlf``）;

#. 如果仍未找到翻译，Symfony将使用默认为 ``en`` 的 ``fallbacks`` 配置参数（请参阅 `配置`_）。

.. note::

    有关详细信息，请参阅 :ref:`reference-framework-translator-logging`.

处理用户的语言环境
--------------------------

翻译根据用户的语言环境进行。阅读 :doc:`/translation/locale` 以了解有关如何处理它的更多信息。

翻译数据库内容
----------------------------

数据库内容的翻译应由Doctrine通过可 `Translatable Extension`_ 或可 `Translatable Behavior`_ （PHP 5.4+）来处理。
有关更多信息，请参阅这些库的文档。

调试翻译
----------------------

当你使用不同语言处理许多翻译消息时，可能很难跟踪哪些翻译缺失以及哪些翻译不再使用。
阅读 :doc:`/translation/debug`，以了解如何识别这些消息。

总结
-------

使用Symfony翻译组件，创建国际化应用不再是一个痛苦的过程，并归结为几个基本步骤：

* 通过在 :method:`Symfony\\Component\\Translation\\Translator::trans`
  或 :method:`Symfony\\Component\\Translation\\Translator::transChoice`
  方法中封装每个消息，在应用中抽象消息
  （在 :doc:`/components/translation/usage` 中了解这一点）;

* 通过创建翻译消息文件将每个消息翻译成多个语言环境。Symfony发现并处理每个文件，因为它的名称遵循特定的约定;

* 管理用户的语言环境，该语言环境存储在请求中，但也可以在用户的​​会话中设置。

扩展阅读
----------

.. toctree::
    :maxdepth: 1

    translation/locale
    translation/debug
    translation/lint

.. _`i18n`: https://en.wikipedia.org/wiki/Internationalization_and_localization
.. _`ISO 3166-1 alpha-2`: https://en.wikipedia.org/wiki/ISO_3166-1#Current_codes
.. _`ISO 639-1`: https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
.. _`Translatable Extension`: http://atlantic18.github.io/DoctrineExtensions/doc/translatable.html
.. _`Translatable Behavior`: https://github.com/KnpLabs/DoctrineBehaviors
.. _`TranslationBundle`: https://github.com/php-translation/symfony-bundle
