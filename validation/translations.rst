.. index::
    single: Validation; Translation

如何翻译验证约束的消息
===============================================

如果你对Form组件使用验证约束，则可以通过为 ``validators``
:ref:`域 <using-message-domains>` 创建一个翻译资源来翻译错误消息。

首先，假设你已经创建了一个普通的PHP对象，你需要在应用中的某个位置使用它::

    // src/Entity/Author.php
    namespace App\Entity;

    class Author
    {
        public $name;
    }

通过任何支持的方法来添加约束。将消息选项设置为翻译源文本。
例如，要保证 ``$name`` 属性不为空，请添加以下内容：

.. configuration-block::

    .. code-block:: php-annotations

        // src/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank(message="author.name.not_blank")
             */
            public $name;
        }

    .. code-block:: yaml

        # config/validator/validation.yaml
        App\Entity\Author:
            properties:
                name:
                    - NotBlank: { message: 'author.name.not_blank' }

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                https://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="App\Entity\Author">
                <property name="name">
                    <constraint name="NotBlank">
                        <option name="message">author.name.not_blank</option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Mapping\ClassMetadata;

        class Author
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank([
                    'message' => 'author.name.not_blank',
                ]));
            }
        }

现在，在 ``translations/`` 目录中创建一个 ``validators`` 目录(catalog)文件：

.. configuration-block::

    .. code-block:: xml

        <!-- translations/validators.en.xlf -->
        <?xml version="1.0"?>
        <xliff version="1.2" xmlns="urn:oasis:names:tc:xliff:document:1.2">
            <file source-language="en" datatype="plaintext" original="file.ext">
                <body>
                    <trans-unit id="author.name.not_blank">
                        <source>author.name.not_blank</source>
                        <target>Please enter an author name.</target>
                    </trans-unit>
                </body>
            </file>
        </xliff>

    .. code-block:: yaml

        # translations/validators.en.yaml
        author.name.not_blank: Please enter an author name.

    .. code-block:: php

        // translations/validators.en.php
        return [
            'author.name.not_blank' => 'Please enter an author name.',
        ];

如果是第一次创建此文件，你可能需要清除缓存（即使在开发环境中）。
