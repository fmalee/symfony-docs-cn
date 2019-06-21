.. index::
   single: Doctrine; Resolving target entities
   single: Doctrine; Define relationships with abstract classes and interfaces

如何定义与抽象类和接口的关系
================================================================

bundle的目标之一是创建谨慎的功能包，这些功能包没有很多（如果有的话）依赖关系，
允许你在其他应用中使用该功能而不包含不必要的项。

Doctrine 2.2包含一个名为 ``ResolveTargetEntityListener`` 的新实用工具，
它通过拦截Doctrine中的某些调用并在运行时重写元数据映射中的 ``targetEntity`` 参数来起作用。
这意味着在你的bundle中，你可以在映射中使用接口或抽象类，并期望在运行时正确映射到具体实体。

此功能允许你定义不同实体之间的关联关系，而不会使它们成为硬依赖。

背景
----------

假设你有一个提供发票功能的InvoiceBundle和一个包含客户管理工具的CustomerBundle。
你希望将这些分开，因为它们可以在其他系统中相互使用，但是对于你应用，你希望将它们一起使用。

在这种情况下，你有一个与不存在的对象有关联关系的 ``Invoice`` 实体，一个 ``InvoiceSubjectInterface``。
目标是使用 ``ResolveTargetEntityListener`` 实现该接口的真实对象来替换对接口的任何提及。

设置
------

本文使用以下两个基础实体（为简洁起见不完整）来解释如何设置和使用 ``ResolveTargetEntityListener``。

客户实体::

    // src/Entity/Customer.php
    namespace App\Entity;

    use Acme\CustomerBundle\Entity\Customer as BaseCustomer;
    use Acme\InvoiceBundle\Model\InvoiceSubjectInterface;
    use Doctrine\ORM\Mapping as ORM;

    /**
     * @ORM\Entity
     * @ORM\Table(name="customer")
     */
    class Customer extends BaseCustomer implements InvoiceSubjectInterface
    {
        // 在此示例中，InvoiceSubjectInterface中定义的任何方法都已在BaseCustomer中实现
    }

发票实体::

    // src/Acme/InvoiceBundle/Entity/Invoice.php
    namespace Acme\InvoiceBundle\Entity;

    use Acme\InvoiceBundle\Model\InvoiceSubjectInterface;
    use Doctrine\ORM\Mapping as ORM;

    /**
     * Represents an Invoice.
     *
     * @ORM\Entity
     * @ORM\Table(name="invoice")
     */
    class Invoice
    {
        /**
         * @ORM\ManyToOne(targetEntity="Acme\InvoiceBundle\Model\InvoiceSubjectInterface")
         * @var InvoiceSubjectInterface
         */
        protected $subject;
    }

InvoiceSubjectInterface::

    // src/Acme/InvoiceBundle/Model/InvoiceSubjectInterface.php
    namespace Acme\InvoiceBundle\Model;

    /**
     * 发票Subject对象应实现的接口。
     * 在大多数情况下，只有一个对象应该实现此接口，
     * 因为ResolveTargetEntityListener只能将目标更改为单个对象。
     */
    interface InvoiceSubjectInterface
    {
        // 列出InvoiceBundle需要访问该subject的任何其他方法,
        // 以便确保你可以访问这些方法。

        /**
         * @return string
         */
        public function getName();
    }

接下来，你需要配置监听器，它告诉DoctrineBundle有关于替换的信息：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            # ...
            orm:
                # ...
                resolve_target_entities:
                    Acme\InvoiceBundle\Model\InvoiceSubjectInterface: App\Entity\Customer

    .. code-block:: yaml

        # config/packages/doctrine.yaml
        doctrine:
            # ...
            orm:
                # ...
                resolve_target_entities:
                    Acme\InvoiceBundle\Model\InvoiceSubjectInterface: App\Entity\Customer

    .. code-block:: xml

        <!-- config/packages/doctrine.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:doctrine="http://symfony.com/schema/dic/doctrine"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/doctrine
                https://symfony.com/schema/dic/doctrine/doctrine-1.0.xsd">

            <doctrine:config>
                <doctrine:orm>
                    <!-- ... -->
                    <doctrine:resolve-target-entity interface="Acme\InvoiceBundle\Model\InvoiceSubjectInterface">App\Entity\Customer</doctrine:resolve-target-entity>
                </doctrine:orm>
            </doctrine:config>
        </container>

    .. code-block:: php

        // config/packages/doctrine.php
        use Acme\InvoiceBundle\Model\InvoiceSubjectInterface;
        use App\Entity\Customer;

        $container->loadFromExtension('doctrine', [
            'orm' => [
                // ...
                'resolve_target_entities' => [
                    InvoiceSubjectInterface::class => Customer::class,
                ],
            ],
        ]);

结束语
--------------

通过 ``ResolveTargetEntityListener``，你可以分离你的bundle，
使它们自己可用，但仍然能够定义不同对象之间的关联关系。
通过使用此方法，你的bunlde最终将更容易独立维护。
