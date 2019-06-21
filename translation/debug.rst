.. index::
    single: Translation; Debug
    single: Translation; Missing Messages
    single: Translation; Unused Messages

如何查找丢失或未使用的翻译消息
==================================================

在维护应用或bundle时，你可以添加或删除翻译消息，也可能会忘记更新翻译消息目录。
``debug:translation`` 命令可以帮助你找到这些丢失或未使用的翻译消息模板：

.. code-block:: twig

    {# 当使用 trans 过滤器和标签时，该消息可以被发现 #}
    {% trans %}Symfony is great{% endtrans %}

    {{ 'Symfony is great'|trans }}

.. caution::

    提取器无法找到在模板外部翻译的消息，如表单标签或控制器。
    模板中的使用变量或表达式的动态翻译也无法被检测到：

    .. code-block:: twig

        {# 该翻译使用一个Twig变量，所以它也无法被检测到 #}
        {% set message = 'Symfony is great' %}
        {{ message|trans }}

假设你的应用的默认语言环境已经配置为 ``fr``，而且 ``en``
是后备语言环境（有关如何配置这些的文档，请参阅 :ref:`translation-configuration`
和 :ref:`translation-fallback`）。假设你已经为 ``fr`` 语言环境设置了一些翻译：

.. configuration-block::

    .. code-block:: xml

        <!-- translations/messages.fr.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="1">
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
        return [
            'Symfony is great' => 'J\'aime Symfony',
        ];

以及 ``en`` 语言环境的翻译：

.. configuration-block::

    .. code-block:: xml

        <!-- translations/messages.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="1">
                        <source>Symfony is great</source>
                        <target>Symfony is great</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: yaml

        # translations/messages.en.yaml
        Symfony is great: Symfony is great

    .. code-block:: php

        // translations/messages.en.php
        return [
            'Symfony is great' => 'Symfony is great',
        ];

要检查应用的 ``fr`` 语言环境中的所有消息，请运行：

.. code-block:: terminal

    $ php bin/console debug:translation fr

    ---------  ------------------  ----------------------  -------------------------------
     State      Id                  Message Preview (fr)    Fallback Message Preview (en)
    ---------  ------------------  ----------------------  -------------------------------
     unused     Symfony is great    J'aime Symfony          Symfony is great
    ---------  ------------------  ----------------------  -------------------------------

它会显示一个表格，该表格包含一个消息在 ``fr`` 语言环境中的翻译效果，以及使用
``en`` 后备语言环境时该消息的翻译效果。
最重要的是，它还会显示何时该翻译与后备翻译相同（这可能意味着该消息未正确翻译）。
此外，它还示意 ``Symfony is great`` 消息未被使用，因为它已被翻译，但你尚未在任何地方使用它。

现在，如果你在某个模板中翻译该消息，你将获得以下输出：

.. code-block:: terminal

    $ php bin/console debug:translation fr

    ---------  ------------------  ----------------------  -------------------------------
     State      Id                  Message Preview (fr)    Fallback Message Preview (en)
    ---------  ------------------  ----------------------  -------------------------------
                Symfony is great    J'aime Symfony          Symfony is great
    ---------  ------------------  ----------------------  -------------------------------

``state`` 为空，表示该消息在 ``fr`` 语言环境中已翻译并已在一个或多个模板中使用。

如果从 ``fr`` 语言环境的翻译文件中删除 ``Symfony is great`` 消息，然后再次运行该命令，你将获得：

.. code-block:: terminal

    $ php bin/console debug:translation fr

    ---------  ------------------  ----------------------  -------------------------------
     State      Id                  Message Preview (fr)    Fallback Message Preview (en)
    ---------  ------------------  ----------------------  -------------------------------
     missing    Symfony is great    Symfony is great        Symfony is great
    ---------  ------------------  ----------------------  -------------------------------

``state`` 示意该消息已丢失，因为它未在 ``fr`` 语言环境中翻译，但仍在模板中使用。
此外，``fr`` 语言环境中的消息和 ``en`` 语言环境中的消息相同。
这是一种特殊情况，因为未翻译的消息的id的值就是其在 ``en`` 语言环境中的翻译。

如果将 ``en`` 语言环境中的翻译文件的内容复制到 ``fr`` 语言环境中的翻译文件，然后再运行该命令，你将获得：

.. code-block:: terminal

    $ php bin/console debug:translation fr

    ----------  ------------------  ----------------------  -------------------------------
     State      Id                  Message Preview (fr)    Fallback Message Preview (en)
    ----------  ------------------  ----------------------  -------------------------------
     fallback    Symfony is great    Symfony is great        Symfony is great
    ----------  ------------------  ----------------------  -------------------------------

你可以看到该消息的翻译在 ``fr`` 和 ``en`` 语言环境中是一样的，这意味着此消息可能已从法语复制到英语，但你可能忘记去翻译它。

默认情况下会检查所有的域，但你可以指定单个域：

.. code-block:: terminal

    $ php bin/console debug:translation en --domain=messages

当应用有大量消息时，可以通过使用 ``--only-unused`` 或 ``--only-missing``
选项来显示仅使用的或仅缺失的消息：

.. code-block:: terminal

    $ php bin/console debug:translation en --only-unused
    $ php bin/console debug:translation en --only-missing
