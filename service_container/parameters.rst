.. index::
   single: DependencyInjection; Parameters

参数介绍
==========================

你可以在服务容器中定义参数，然后可以直接使用这些参数或将其作为服务定义的一部分。
这有助于分离出你希望更频繁修改的值。

配置文件中的参数
---------------------------------

使用配置文件的 ``parameters`` 区块来设置参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            mailer.transport: sendmail

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="mailer.transport">sendmail</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('mailer.transport', 'sendmail');

你可以通过使用百分比（``%``）符号包裹参数来引用任何配置文件中的参数，例如 ``%mailer.transport%``。
参数的其中一个用途是将该值注入到你的服务。
这样能够实现在【不同应用之间】或【单一应用中基于相同类的多个服务之间】去配置“不同版本的服务”。
你可以选择直接将邮件传输(transport)注入到 ``Mailer`` 类中。
但是将其声明为参数使得修改它更容易，而不是将其依附、隐藏在服务定义中：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            mailer.transport: sendmail

        services:
            App\Service\Mailer:
                arguments: ['%mailer.transport%']

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="mailer.transport">sendmail</parameter>
            </parameters>

            <services>
                <service id="App\Service\Mailer">
                    <argument>%mailer.transport%</argument>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Mailer;

        $container->setParameter('mailer.transport', 'sendmail');

        $container->register(Mailer::class)
            ->addArgument('%mailer.transport%');

.. caution::

    在XML配置文件中的 ``parameter`` 标签之间的值不会被修剪(trimmed)。

    这意味着以下配置示例将具有 ``\n    sendmail\n`` 值：

    .. code-block:: xml

        <parameter key="mailer.transport">
            sendmail
        </parameter>

    在某些情况下（对于常量或类名），这可能会抛出错误。
    为了防止这种情况，你必须始终内联(inline)你的参数，如下所示：

    .. code-block:: xml

        <parameter key="mailer.transport">sendmail</parameter>

.. note::

    如果你使用以 ``@`` 开头或在任何位置有 ``%`` 的字符串，
    则需要通过添加另一个 ``@`` 或 ``%`` 来转义它：

    .. configuration-block::

        .. code-block:: yaml

            # config/services.yaml
            parameters:
                # 将会被解析为字符串 '@securepass'
                mailer_password: '@@securepass'

                # 传递为 http://symfony.com/?foo=%s&amp;bar=%d
                url_pattern: 'http://symfony.com/?foo=%%s&amp;bar=%%d'

        .. code-block:: xml

            <!-- config/services.xml -->
            <parameters>
                <!-- the @ symbol does NOT need to be escaped in XML -->
                <parameter key="mailer_password">@securepass</parameter>

                <!-- But % does need to be escaped -->
                <parameter key="url_pattern">http://symfony.com/?foo=%%s&amp;bar=%%d</parameter>
            </parameters>

        .. code-block:: php

            // config/services.php
            // the @ symbol does NOT need to be escaped in XML
            $container->setParameter('mailer_password', '@securepass');

            // But % does need to be escaped
            $container->setParameter('url_pattern', 'http://symfony.com/?foo=%%s&amp;bar=%%d');

在PHP中获取和设置容器参数
-----------------------------------------------

使用容器的访问器方法可以让你直接简单的来使用容器参数::

    // 检查是否定义了一个参数（参数名称区分大小写）
    $container->hasParameter('mailer.transport');

    // 获取一个参数的值
    $container->getParameter('mailer.transport');

    // 添加一个新参数
    $container->setParameter('mailer.transport', 'sendmail');

.. caution::

    使用 ``.`` 符号是 :ref:`Symfony的约定 <service-naming-conventions>`，这样使参数更易于阅读。
    参数是平行(flat)的键/值元素，因此它们不能被组织成一个嵌套数组

.. note::

    你只能在容器被编译之前设置参数：即不处于运行时(run-time)。
    要了解有关编译容器的更多信息，请参阅 :doc:`/components/dependency_injection/compilation`。

.. _component-di-parameters-array:

数组参数
----------------

参数并不总是扁平(flat)的字符串，它们也可以是包含数组的值。
对于XML格式，你需要对所有数组参数使用 ``type="collection"`` 属性。

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            my_mailer.gateways: [mail1, mail2, mail3]

            my_multilang.language_fallback:
                en:
                    - en
                    - fr
                fr:
                    - fr
                    - en

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="my_mailer.gateways" type="collection">
                    <parameter>mail1</parameter>
                    <parameter>mail2</parameter>
                    <parameter>mail3</parameter>
                </parameter>

                <parameter key="my_multilang.language_fallback" type="collection">
                    <parameter key="en" type="collection">
                        <parameter>en</parameter>
                        <parameter>fr</parameter>
                    </parameter>

                    <parameter key="fr" type="collection">
                        <parameter>fr</parameter>
                        <parameter>en</parameter>
                    </parameter>
                </parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('my_mailer.gateways', ['mail1', 'mail2', 'mail3']);
        $container->setParameter('my_multilang.language_fallback', [
            'en' => ['en', 'fr'],
            'fr' => ['fr', 'en'],
        ]);

环境变量和动态值
----------------------------------------

请参阅 :ref:`config-env-vars`。

.. _component-di-parameters-constants:

常量作为参数
-----------------------

同时还支持将PHP常量设置为参数：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            global.constant.value: !php/const GLOBAL_CONSTANT
            my_class.constant.value: !php/const My_Class::CONSTANT_NAME

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="global.constant.value" type="constant">GLOBAL_CONSTANT</parameter>
                <parameter key="my_class.constant.value" type="constant">My_Class::CONSTANT_NAME</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('global.constant.value', GLOBAL_CONSTANT);
        $container->setParameter('my_class.constant.value', My_Class::CONSTANT_NAME);

二进制值作为参数
---------------------------

如果容器参数的值是二进制值，则需要在YAML和XML配置中将其设置为base64编码值，
在PHP配置中则使用转义序列(escape sequences)：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        parameters:
            some_parameter: !!binary VGhpcyBpcyBhIEJlbGwgY2hhciAH

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">

            <parameters>
                <parameter key="some_parameter" type="binary">VGhpcyBpcyBhIEJlbGwgY2hhciAH</parameter>
            </parameters>
        </container>

    .. code-block:: php

        // config/services.php
        $container->setParameter('some_parameter', 'This is a Bell char \x07');

XML中的PHP关键字
-------------------

默认情况下，在XML中的 ``true``、``false`` 和 ``null`` 会被转换成PHP关键字
（分别为 ``true``、``false`` 和 ``null``）：

.. code-block:: xml

    <parameters>
        <parameter key="mailer.send_all_in_once">false</parameter>
    </parameters>

    <!-- after parsing
    $container->getParameter('mailer.send_all_in_once'); // returns false
    -->

要禁用此行为，请使用 ``string`` 类型：

.. code-block:: xml

    <parameters>
        <parameter key="mailer.some_parameter" type="string">true</parameter>
    </parameters>

    <!-- after parsing
    $container->getParameter('mailer.some_parameter'); // returns "true"
    -->

.. note::

    这不适用于YAML和PHP，因为它们已经内置了对PHP关键字的支持。
