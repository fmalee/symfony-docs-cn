.. index::
   single: Doctrine; Custom DQL functions

如何注册自定义DQL函数
====================================

Doctrine允许你指定自定义DQL函数。
有关此主题的更多信息，请阅读Doctrine的教程："`DQL用户定义的函数`_"。

在Symfony中，你可以按如下方式注册自定义DQL函数：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            orm:
                # ...
                dql:
                    string_functions:
                        test_string: App\DQL\StringFunction
                        second_string: App\DQL\SecondStringFunction
                    numeric_functions:
                        test_numeric: App\DQL\NumericFunction
                    datetime_functions:
                        test_datetime: App\DQL\DatetimeFunction

    .. code-block:: xml

        <!-- config/packages/doctrine.xml -->
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:orm>
                    <!-- ... -->
                    <doctrine:dql>
                        <doctrine:string-function name="test_string">App\DQL\StringFunction</doctrine:string-function>
                        <doctrine:string-function name="second_string">App\DQL\SecondStringFunction</doctrine:string-function>
                        <doctrine:numeric-function name="test_numeric">App\DQL\NumericFunction</doctrine:numeric-function>
                        <doctrine:datetime-function name="test_datetime">App\DQL\DatetimeFunction</doctrine:datetime-function>
                    </doctrine:dql>
                </doctrine:orm>
            </doctrine:config>
        </container>

    .. code-block:: php

        // config/packages/doctrine.php
        use App\DQL\StringFunction;
        use App\DQL\SecondStringFunction;
        use App\DQL\NumericFunction;
        use App\DQL\DatetimeFunction;

        $container->loadFromExtension('doctrine', array(
            'orm' => array(
                // ...
                'dql' => array(
                    'string_functions' => array(
                        'test_string'   => StringFunction::class,
                        'second_string' => SecondStringFunction::class,
                    ),
                    'numeric_functions' => array(
                        'test_numeric' => NumericFunction::class,
                    ),
                    'datetime_functions' => array(
                        'test_datetime' => DatetimeFunction::class,
                    ),
                ),
            ),
        ));

.. note::

    如果 ``entity_managers`` 明确命名，
    直接使用orm配置该函数将会触发 `Unrecognized option "dql" under "doctrine.orm"` 异常。
    ``dql`` 配置块必须在命名实体管理器下定义。

    .. configuration-block::

        .. code-block:: yaml

            # config/packages/doctrine.yaml
            doctrine:
                orm:
                    # ...
                    entity_managers:
                        example_manager:
                            # 将功能放在这里
                            dql:
                                datetime_functions:
                                    test_datetime: App\DQL\DatetimeFunction

        .. code-block:: xml

            <!-- config/packages/doctrine.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd
                    http://symfony.com/schema/dic/doctrine
                    http://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

                <doctrine:config>
                    <doctrine:orm>
                        <!-- ... -->

                        <doctrine:entity-manager name="example_manager">
                            <!-- place your functions here -->
                            <doctrine:dql>
                                <doctrine:datetime-function name="test_datetime">
                                    App\DQL\DatetimeFunction
                                </doctrine:datetime-function>
                            </doctrine:dql>
                        </doctrine:entity-manager>
                    </doctrine:orm>
                </doctrine:config>
            </container>

        .. code-block:: php

            // config/packages/doctrine.php
            use App\DQL\DatetimeFunction;

            $container->loadFromExtension('doctrine', array(
                'doctrine' => array(
                    'orm' => array(
                        // ...
                        'entity_managers' => array(
                            'example_manager' => array(
                                // place your functions here
                                'dql' => array(
                                    'datetime_functions' => array(
                                        'test_datetime' => DatetimeFunction::class,
                                    ),
                                ),
                            ),
                        ),
                    ),
                ),
            ));

.. _`DQL用户定义的函数`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/cookbook/dql-user-defined-functions.html
