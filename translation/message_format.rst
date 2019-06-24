.. index::
    single: Translation; Message Format

如何使用ICU MessageFormat翻译消息
=====================================================

.. versionadded:: 4.2

   在Symfony 4.2中引入了对ICU MessageFormat的支持。

应用中的消息（即字符串）几乎从不是完全静态的。它们包含变量或其他复杂逻辑，如复数。
为了处理这个问题，Translator组件启用对 `ICU MessageFormat`_ 语法的支持。

.. tip::

    你可以在此 `在线编辑器`_ 中测试ICU MessageFormatter的示例。

使用ICU消息格式
----------------------------

为了使用ICU消息格式，:ref:`消息域 <using-message-domains>`
必须带有 ``+intl-icu`` 后缀：

======================  ===============================
普通文件名               ICU消息格式文件名
======================  ===============================
``messages.en.yaml``    ``messages+intl-icu.en.yaml``
``messages.fr_FR.xlf``  ``messages+intl-icu.fr_FR.xlf``
``admin.en.yaml``       ``admin+intl-icu.en.yaml``
======================  ===============================

现在，:phpclass:`MessageFormatter` 将在翻译期间处理此文件中的所有消息。

.. _component-translation-placeholders:

消息占位符
--------------------

MessageFormat的基础用法允许你在消息中使用占位符（在ICU MessageFormat中被称为 *参数*）：

.. configuration-block::

    .. code-block:: yaml

        # translations/messages+intl-icu.en.yaml
        say_hello: 'Hello {name}!'

    .. code-block:: xml

        <!-- translations/messages+intl-icu.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="say_hello">
                        <source>say_hello</source>
                        <target>Hello {name}!</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // translations/messages+intl-icu.en.php
        return [
            'say_hello' => "Hello {name}!",
        ];

大括号（``{...}``）内的所有内容都由格式化器处理，并替换为它的占位符::

    // 打印 "Hello Fabien!"
    echo $translator->trans('say_hello', ['name' => 'Fabien']);

    // 打印 "Hello Symfony!"
    echo $translator->trans('say_hello', ['name' => 'Symfony']);

根据条件选择不同的消息
-------------------------------------------------

大括号语法允许“修改”变量的输出。其中一个就是 ``select`` 函数。它的作用类似于PHP的
`switch语句`_，便是允许根据变量的值来使用不同的字符串。它的典型用法是性别：

.. configuration-block::

    .. code-block:: yaml

        # translations/messages+intl-icu.en.yaml
        invitation_title: >
            {organizer_gender, select,
                female {{organizer_name} has invited you for her party!}
                male   {{organizer_name} has invited you for his party!}
                other  {{organizer_name} have invited you for their party!}
            }

    .. code-block:: xml

        <!-- translations/messages+intl-icu.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="invitation_title">
                        <source>invitation_title</source>
                        <target>{organizer_gender, select, female {{organizer_name} has invited you for her party!} male {{organizer_name} has invited you for his party!} other {{organizer_name} have invited you for their party!}}</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // translations/messages+intl-icu.en.php
        return [
            'invitation_title' => '{organizer_gender, select,
                female {{organizer_name} has invited you for her party!}
                male   {{organizer_name} has invited you for his party!}
                other  {{organizer_name} have invited you for their party!}
            }',
        ];

这可能看起来非常复杂。所有函数的基本语法是
``{variable_name, function_name, function_statement}``（如后所述，某些函数的
``function_statement`` 是可选的）。在这个例子中，函数的名称是
``select``，并且其语句包含此选择的“cases”。此函数被应用于 ``organizer_gender`` 变量::

    // 打印 "Ryan has invited you for his party!"
    echo $translator->trans('invitation_title', [
        'organizer_name' => 'Ryan',
        'organizer_gender' => 'male',
    ]);

    // 打印 "John & Jane have invited you for their party!"
    echo $translator->trans('invitation_title', [
        'organizer_name' => 'John & Jane',
        'organizer_gender' => 'not_applicable',
    ]);

``{...}`` 语法在 "文字" 和 "代码" 模式之间交替使用。这将允许你在 select 语句中使用文字文本：

#. 第一个 ``{organizer_gender, select, ...}`` 区块启动“代码”模式，这意味着
   ``organizer_gender`` 被作为变量处理。
#. 内部的 ``{... has invited you for her party!}``
   区块将你带回“文字”模式，这意味着该文本不会被处理。
#. 在 ``{organizer_name}`` 区块内，再次启动“代码”模式，将允许
   ``organizer_name`` 作为变量处理。

.. tip::

    虽然在switch语句中只放 ``her``、``his`` 或 ``their``
    似乎更合乎逻辑，但最好是在消息的最外层结构中用“复杂参数”。
    这样的字符串对于翻译人员来说可读性更好，正如你在 ``other``
    case中所看到的，句子的其他部分可能会受到变量的影响。

复数
-------------

另一个有趣的函数是 ``plural``。它允许你处理消息中的复数（例如 ``There are 3 apples`` vs
``There is one apple``）。该函数看起来非常类似于 ``select`` 函数：

.. configuration-block::

    .. code-block:: yaml

        # translations/messages+intl-icu.en.yaml
        num_of_apples: >
            {apples, plural,
                =0    {There are no apples}
                one   {There is one apple...}
                other {There are # apples!}
            }

    .. code-block:: xml

        <!-- translations/messages+intl-icu.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="num_of_apples">
                        <source>num_of_apples</source>
                        <target>{apples, plural, =0 {There are no apples} one {There is one apple...} other {There are # apples!}}</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // translations/messages+intl-icu.en.php
        return [
            'num_of_apples' => '{apples, plural,
                =0    {There are no apples}
                one   {There is one apple...}
                other {There are # apples!}
            }',
        ];

复数规则实际上非常复杂，并且每种语言都有所不同。
例如，俄语对以1结尾的数字使用不同的复数形式；以2,3或4结尾的数字；以5,6,7,8或9结尾的数字; 甚至一些例外！

为了正确地翻译复数， ``plural`` 函数中的可能情况对于每种语言也是不同的。例如，俄罗斯有
``one``、``few``、``many`` 以及 ``other``，而英国只有 ``one`` 和
``other``。可以在Unicode的 `语言复数规则`_
文档中找到完整的可能案例列表。通过使用 ``=`` 前缀，你还可以匹配精确值（如上例所示的 ``0``）。

此字符串的用法与变量和 select 相同::

    // 打印 "There is one apple..."
    echo $translator->trans('num_of_apples', ['apples' => 1]);

    // 打印 "There are 23 apples!"
    echo $translator->trans('num_of_apples', ['apples' => 23]);

.. note::

    你还可以设置 ``offset`` 变量以确定复数是否应该偏移（例如，在
    ``You and # other people`` / ``You and # other person`` 这样的句子中）。

.. tip::

    组合 ``select`` 和 ``plural`` 函数时，尽量保证 ``select`` 是最外层的函数：

    .. code-block:: text

        {gender_of_host, select,
            female {
                {num_guests, plural, offset:1
                =0    {{host} does not give a party.}
                =1    {{host} invites {guest} to her party.}
                =2    {{host} invites {guest} and one other person to her party.}
                other {{host} invites {guest} and # other people to her party.}}
            }
            male {
                {num_guests, plural, offset:1
                =0    {{host} does not give a party.}
                =1    {{host} invites {guest} to his party.}
                =2    {{host} invites {guest} and one other person to his party.}
                other {{host} invites {guest} and # other people to his party.}}
            }
            other {
                {num_guests, plural, offset:1
                =0    {{host} does not give a party.}
                =1    {{host} invites {guest} to their party.}
                =2    {{host} invites {guest} and one other person to their party.}
                other {{host} invites {guest} and # other people to their party.}}
            }
        }

其他占位符函数
--------------------------------

除此之外，ICU MessageFormat还带有其他一些有趣的函数。

序数
~~~~~~~

类似于 ``plural``，``selectordinal`` 允许你使用数字作为序数比例（ordinal scale）：

.. configuration-block::

    .. code-block:: yaml

        # translations/messages+intl-icu.en.yaml
        finish_place: >
            You finished {place, selectordinal,
                one   {#st}
                two   {#nd}
                few   {#rd}
                other {#th}
            }!

        # 当只将数字格式化为序数时（如上所述），你还可以使用 `ordinal` 函数：
        finish_place: You finished {place, ordinal}!

    .. code-block:: xml

        <!-- translations/messages+intl-icu.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="finish_place">
                        <source>finish_place</source>
                        <target>You finished {place, selectordinal, one {#st} two {#nd} few {#rd} other {#th}}!</target>
                    </trans-unit>

                    <!-- when only formatting the number as ordinal (like
                         above), you can also use the `ordinal` function: -->
                    <trans-unit id="finish_place">
                        <source>finish_place</source>
                        <target>You finished {place, ordinal}!</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // translations/messages+intl-icu.en.php
        return [
            'finish_place' => 'You finished {place, selectordinal,
                one {#st}
                two {#nd}
                few {#rd}
                other {#th}
            }!',

            // when only formatting the number as ordinal (like above), you can
            // also use the `ordinal` function:
            'finish_place' => 'You finished {place, ordinal}!',
        ];

.. code-block:: php

    // 打印 "You finished 1st!"
    echo $translator->trans('finish_place', ['place' => 1]);

    // 打印 "You finished 9th!"
    echo $translator->trans('finish_place', ['place' => 9]);

    // 打印 "You finished 23rd!"
    echo $translator->trans('finish_place', ['place' => 23]);

Unicode的 `语言复数规则`_ 文档中也展示了可能的例子。

日期和时间
~~~~~~~~~~~~~

日期和时间函数允许你使用 :phpclass:`IntlDateFormatter`
来格式化目标语言环境中的日期：

.. configuration-block::

    .. code-block:: yaml

        # translations/messages+intl-icu.en.yaml
        published_at: 'Published at {publication_date, date} - {publication_date, time, short}'

    .. code-block:: xml

        <!-- translations/messages+intl-icu.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="published_at">
                        <source>published_at</source>
                        <target>Published at {publication_date, date} - {publication_date, time, short}</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // translations/messages+intl-icu.en.php
        return [
            'published_at' => 'Published at {publication_date, date} - {publication_date, time, short}',
        ];

``time`` 和 ``date`` 函数的“函数语句”可以是对应于 `IntlDateFormatter类定义的常量`_
的 ``short``、``medium``、``long`` 或 ``full``::

    // 打印 "Published at Jan 25, 2019 - 11:30 AM"
    echo $translator->trans('published_at', ['publication_date' => new \DateTime('2019-01-25 11:30:00')]);

数字
~~~~~~~

``number`` 格式化器允许你使用 Intl 的 :phpclass:`NumberFormatter` 来格式化数字：

.. configuration-block::

    .. code-block:: yaml

        # translations/messages+intl-icu.en.yaml
        progress: '{progress, number, percent} of the work is done'
        value_of_object: 'This artifact is worth {value, number, currency}'

    .. code-block:: xml

        <!-- translations/messages+intl-icu.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="progress">
                        <source>progress</source>
                        <target>{progress, number, percent} of the work is done</target>
                    </trans-unit>

                    <trans-unit id="value_of_object">
                        <source>value_of_object</source>
                        <target>This artifact is worth {value, number, currency}</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: php

        // translations/messages+intl-icu.en.php
        return [
            'progress' => '{progress, number, percent} of the work is done',
            'value_of_object' => 'This artifact is worth {value, number, currency}',
        ];

.. code-block:: php

    // 打印 "82% of the work is done"
    echo $translator->trans('progress', ['progress' => 0.82]);
    // 打印 "100% of the work is done"
    echo $translator->trans('progress', ['progress' => 1]);

    // 打印 "This artifact is worth $9,988,776.65"
    // 如果我们将其翻译成法语，其值将显示为 "9 988 776,65 €"。
    echo $translator->trans('value_of_object', ['value' => 9988776.65]);

.. _`在线编辑器`: http://format-message.github.io/icu-message-format-for-translators/
.. _`ICU MessageFormat`: http://userguide.icu-project.org/formatparse/messages
.. _`switch语句`: https://php.net/control-structures.switch
.. _`语言复数规则`: http://www.unicode.org/cldr/charts/latest/supplemental/language_plural_rules.html
.. _`IntlDateFormatter类定义的常量`: https://php.net/manual/en/class.intldateformatter.php
