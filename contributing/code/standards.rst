代码标准
================

Symfony代码由全球数千名开发人员提供。
为了让每一段代码看起来都很熟悉，Symfony定义了一些所有贡献必须遵循的编码标准。

这些Symfony编码标准基于 `PSR-1`_、`PSR-2`_ 和 `PSR-4`_ 标准，因此你可能已经了解其中的大多数标准。

让你的代码遵循以下代码标准
--------------------------------------------

Symfony不是手动审查代码，而是确保你提供的代码与预期的代码语法相匹配。
首先，安装 `PHP CS Fixer`_ 工具，然后运行此命令来解决任何问题：

.. code-block:: terminal

    $ cd your-project/
    $ php php-cs-fixer.phar fix -v

如果你忘记运行此命令并发起有任何语法问题的拉取请求时，我们的自动化工具将向你发出警告，并提供解决方案。

Symfony代码标准的细节
----------------------------------

如果你想详细了解Symfony编码标准，这里有一个包含下述大多数特性的简短示例：

.. code-block:: html+php

    <?php

    /*
     * This file is part of the Symfony package.
     *
     * (c) Fabien Potencier <fabien@symfony.com>
     *
     * For the full copyright and license information, please view the LICENSE
     * file that was distributed with this source code.
     */

    namespace Acme;

    /**
     * Coding standards demonstration.
     */
    class FooBar
    {
        const SOME_CONST = 42;

        /**
         * @var string
         */
        private $fooBar;

        /**
         * @param string $dummy Some argument description
         */
        public function __construct($dummy)
        {
            $this->fooBar = $this->transformText($dummy);
        }

        /**
         * @return string
         *
         * @deprecated
         */
        public function someDeprecatedMethod()
        {
            @trigger_error(sprintf('The %s() method is deprecated since version 2.8 and will be removed in 3.0. Use Acme\Baz::someMethod() instead.', __METHOD__), E_USER_DEPRECATED);

            return Baz::someMethod();
        }

        /**
         * Transforms the input given as first argument.
         *
         * @param bool|string $dummy   Some argument description
         * @param array       $options An options collection to be used within the transformation
         *
         * @return string|null The transformed input
         *
         * @throws \RuntimeException When an invalid option is provided
         */
        private function transformText($dummy, array $options = array())
        {
            $defaultOptions = array(
                'some_default' => 'values',
                'another_default' => 'more values',
            );

            foreach ($options as $option) {
                if (!in_array($option, $defaultOptions)) {
                    throw new \RuntimeException(sprintf('Unrecognized option "%s"', $option));
                }
            }

            $mergedOptions = array_merge(
                $defaultOptions,
                $options
            );

            if (true === $dummy) {
                return null;
            }

            if ('string' === $dummy) {
                if ('values' === $mergedOptions['some_default']) {
                    return substr($dummy, 0, 5);
                }

                return ucwords($dummy);
            }
        }

        /**
         * Performs some basic check for a given value.
         *
         * @param mixed $value     Some value to check against
         * @param bool  $theSwitch Some switch to control the method's flow
         *
         * @return bool|void The resultant check if $theSwitch isn't false, void otherwise
         */
        private function reverseBoolean($value = null, $theSwitch = false)
        {
            if (!$theSwitch) {
                return;
            }

            return !$value;
        }
    }

结构
~~~~~~~~~

* 在每个逗号分隔符后添加一个空格;

* 在二元运算符（``==``、``&&``...）周围添加一个空格，但串联（``.``）运算符除外;

* 将一元运算符（``!``、``--``...）放在受影响变量附近;

* 除非你需要类型转换(juggling)，否则总是使用 `全等比较`_ ;

* 在针对一个表达式检查一个变量时使用 `Yoda条件`_ ，为了避免该条件语句内的意外分配
  （这适用于 ``==``、``!=``、``===`` 以及 ``!==``）;

* 在多行数组中的每个数组项之后添加逗号，即使在最后一个数组之后也是如此;

* 在 ``return`` 语句之前添加一个空行，除非在语句组中只有一个返回（如 ``if`` 语句）;

* 一个函数返回明确的 ``null`` 值时使用 ``return null;``，
  如果函数返回的是 ``void`` 值，则使用 ``return;``;

* 使用大括号来表示控制结构体，而不管它包含的语句数量;

* 为每个文件定义一个类 - 这不适用于不打算从外部实例化的私有辅助类，因此不关心 `PSR-0`_ 和 `PSR-4`_ 自动加载标准;

* 在类名称的同一行声明类继承和所有实现的接口;

* 在方法之前声明类属性;

* 首先声明公有方法，然后声明受保护方法，最后是私有方法。
  此规则的例外是类的构造函数以及PHPUnit测试方法的 ``setUp()``、``tearDown()`` 方法，因为他们必须始终是第一个方法，以便提高可读性;

* 在与方法/函数名称同一行上声明所有参数，无论有多少参数;

* 无论构造函数具有多少个参数，在实例化该类时都使用括号;

* 必须使用 :phpfunction:`sprintf` 串联(concatenated)异常和错误消息的字符串;

* 调用带有 ``E_USER_DEPRECATED`` 类型的 :phpfunction:`trigger_error` 必须使用 ``@`` 操作符切换到选择加入（opt-in）
  阅读 :ref:`contributing-code-conventions-deprecations` 了解详情。

* 不要在有返回或抛出(throw)东西的 ``if`` 和 ``case`` 条件之后使用 ``else``、``elseif``、``break``;

* 不要在 ``[`` 偏移访问器周围和 ``]`` 偏移访问器之前使用空格;

* 为不属于全局命名空间的每个类添加 ``use`` 语句。

* 当PHPDoc标签（如 ``@param`` 或 ``@ return``）包含 ``null`` 和其他类型时，总是将
  ``null`` 放置在类型列表的末尾。

命名约定
~~~~~~~~~~~~~~~~~~

* 使用 `小骆驼拼写法`_ 命名PHP变量、函数和方法的名称、参数（例如 ``$acceptableContentTypes``、``hasSession()``）;

* 使用 `蛇形拼写法`_ 命名配置参数和Twig模板变量（例如 ``framework.csrf_protection``、``http_status_code``）;

* 对所有PHP类使用名称空间，使用 `大骆驼拼写法`_ 命名类名称（例如 ``ConsoleLogger``）;

* 除PHPUnit的 ``*TestCase`` 之外的所有抽象类都需要使用 ``Abstract`` 前缀。
  请注意，一些早期的Symfony类不遵循此约定，并且由于向后兼容性原因而未重命名。
  但是，所有新的抽象类都必须遵循此命名约定;

* 给接口添加 ``Interface`` 后缀;

* 给复用添加 ``Trait`` 后缀;

* 给异常添加 ``Exception`` 后缀;

* 使用大骆峰拼写法命名PHP文件（例如``EnvVarProcessor.php``），
  以蛇形拼写法命名Twig模板和Web资源（``section_layout.html.twig``、``index.scss``）;

* 对于PHPDocs的类型约束和类型转换(casting)，使用 ``bool`` (代替 ``boolean`` 或 ``Boolean``)，
  ``int`` (代替 ``integer``), ``float`` (代替 ``double`` 或 ``real``);

* 不要忘记查阅更详细的 :doc:`conventions` 文档以获得更直观的命名注意事项。

.. _service-naming-conventions:

服务命名约定
~~~~~~~~~~~~~~~~~~~~~~~~~~

* 服务名称必须与其类的完全限定类名（FQCN）相同（例如 ``App\EventSubscriber\UserSubscriber``）;

* 如果同一类有多个服务，则使用FQCN命名主服务，并对其余服务使用小写和下划线名称。
  可以选择使用点号将他们分组（例如 ``something.service_name``、``fos_user.something.service_name``）;

* 对参数名称使用小写字母（除非使用 ``%env(VARIABLE_NAME)%`` 语法引用环境变量）;

* 为共有服务添加类别名（如 ``Symfony\Component\Something\ClassName`` 的别名是 ``something.service_name``）。

文档
~~~~~~~~~~~~~

* 为所有的类、方法和函数添加PHPDoc块（尽管可能会要求你删除不增加值的PHPDoc）;

* 将注释组合在一起，以便相同类型的注释紧跟在一起，并且不同类型的注释由单个空行分隔;

* 如果方法没有返回任何内容，则省略 ``@return`` 标签;

* 不使用 ``@package`` 和 ``@subpackage`` 注解;

* 不要内联PHPDoc块，即使它们只包含一个标签（例如不要将 ``/** {@inheritdoc} */`` 放在一行中）;

* 添加新类或对现有类进行重大更改时，可以添加或扩展包含个人联系信息的 ``@author`` 标记。
  请注意，可以根据 :doc:`core team </contributing/code/core_team>` 的请求更新或删除个人联系信息。

许可
~~~~~~~

* Symfony是在MIT许可下发布的，该许可块必须出现在命名空间之前的每个PHP文件的顶部。

.. _`PHP CS Fixer`: http://cs.sensiolabs.org/
.. _`PSR-0`: https://www.php-fig.org/psr/psr-0/
.. _`PSR-1`: https://www.php-fig.org/psr/psr-1/
.. _`PSR-2`: https://www.php-fig.org/psr/psr-2/
.. _`PSR-4`: https://www.php-fig.org/psr/psr-4/
.. _`全等比较`: https://php.net/manual/en/language.operators.comparison.php
.. _`Yoda条件`: https://en.wikipedia.org/wiki/Yoda_conditions
.. _`小骆驼拼写法`: https://en.wikipedia.org/wiki/Camel_case
.. _`大骆驼拼写法`: https://en.wikipedia.org/wiki/Camel_case
.. _`蛇形拼写法`: https://en.wikipedia.org/wiki/Snake_case
