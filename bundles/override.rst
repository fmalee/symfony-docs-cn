.. index::
   single: Bundle; Inheritance

如何重写Bundle的任何部分
====================================

使用第三方软件包时，你可能希望自定义或重写它的某些功能。本文档介绍了重写一个bundle最常见功能的方法。

.. tip::

    bundle重写机制意味着你不能使用物理路径来引用bundle的资源（例如 ``__DIR__/config/services.xml``）。
    始终在bundle中使用逻辑路径（例如 ``@FooBundle/Resources/config/services.xml``），并在需要时调用
    :ref:`locateResource() 方法 <http-kernel-resource-locator>` 将它们转换为物理路径。

.. _override-templates:

模板
---------

可以在 ``<your-project>/templates/bundles/<bundle-name>/`` 目录中重写第三方bundle的模板。
新模板必须使用与原始模板相同的名称和路径（相对于 ``<bundle>/Resources/views/``）。

例如，要重写FOSUserBundle中的
``Resources/views/Registration/confirmed.html.twig`` 模板，请创建此模板： ``<your-project>/templates/bundles/FOSUserBundle/Registration/confirmed.html.twig``

.. caution::

    如果在一个新位置添加模板，则 *可能* 需要清除缓存（``php bin/console cache:clear``），即使你处于调试模式也是如此。

你可能只想重写模板的一个或多个区块，而不是重写整个模板。
但是，由于你正重写将要扩展的模板，因此最终会出现无限循环错误。
解决方案是使用模板名称中的特殊的``!`` 前缀来告诉Symfony你要从原始模板扩展，而不是从正重写的模板扩展：

.. code-block:: twig

    {# templates/bundles/FOSUserBundle/Registration/confirmed.html.twig #}
    {# 从一个被重写的模板扩展时，特殊的 '!' 前缀可以避免错误 #}
    {% extends "@!FOSUserBundle/Registration/confirmed.html.twig" %}

    {% block some_block %}
        ...
    {% endblock %}

.. _templating-overriding-core-templates:

.. tip::

    Symfony内部也使用一些bundle，因此你可以应用相同的技术来重写核心的Symfony模板。
    例如，你可以重写TwigBundle模板的 :doc:`自定义错误页面 </controller/error_pages>`。

路由
-------

在Symfony中永远不会自动导入路由。
如果要引入任何bundle中的路由，则必须在应用中的某个位置手动导入它们（例如 ``config/routes.yaml``）。

“重写”一个bundle的路由的最简单方法是永远不导入它。
不导入第三方bundle的路由，而是将该路由文件复制到你的应用中，修改它，然后导入它。

控制器
-----------

如果控制器是服务，请参阅下一节中有关如何重写服务的部分。
否则，使用与要重写的控制器关联的相同路径定义一个新的路由和控制器，并确保在该bundle之前加载新路由。

服务 & 配置
------------------------

如果要修改bundle创建的服务，可以使用 :doc:`服务修饰 </service_container/service_decoration>`。

如果你想要做更高级的操作，如删除其他bundles创建的服务，你必须在一个
:doc:`compiler pass </service_container/compiler_passes>` 中使用
:doc:`服务定义 </service_container/definitions>`。

实体 & 实体映射
-------------------------

如果bundle不是使用注释而是在配置文件中定义实体映射，则可以将它们视为任何其他常规的bundle配置文件进行重写。
唯一需要注意的是，你必须重写所有的这些映射配置文件，而不仅仅是你实际想要重写的那些。

如果bundle提供一个已映射的父类(superclass)（例如FOSUserBundle中的 ``User`` 实体），则可以重写它的属性和关联关系。
在 `Doctrine文档`_ 中了解有关此功能及其局限性的更多信息。

表单
-----

可以通过 :doc:`单类型扩展 </form/create_form_type_extension>` 修改现有表单类型。

.. _override-validation:

验证元数据
-------------------

Symfony从每个bundle中加载所有的验证配置文件，并将它们组合到一个验证元数据树中。
这意味着你可以添加新约束到一个属性，但不能重写它们。

要解决此问题，第三方bundle需要具有 :doc:`验证组 </validation/groups>` 的配置。
例如，FOSUserBundle具有此配置。
要创建自己的验证，请将该约束添加到一个新验证组：

.. configuration-block::

    .. code-block:: yaml

        # config/validator/validation.yaml
        FOS\UserBundle\Model\User:
            properties:
                plainPassword:
                    - NotBlank:
                        groups: [AcmeValidation]
                    - Length:
                        min: 6
                        minMessage: fos_user.password.short
                        groups: [AcmeValidation]

    .. code-block:: xml

        <!-- config/validator/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="FOS\UserBundle\Model\User">
                <property name="plainPassword">
                    <constraint name="NotBlank">
                        <option name="groups">
                            <value>AcmeValidation</value>
                        </option>
                    </constraint>

                    <constraint name="Length">
                        <option name="min">6</option>
                        <option name="minMessage">fos_user.password.short</option>
                        <option name="groups">
                            <value>AcmeValidation</value>
                        </option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

现在，更新FOSUserBundle配置，然后它将使用你的验证组而不是原始验证组。

.. _override-translations:

翻译
------------

翻译与bundle无关，而与 :ref:`翻译域 <using-message-domains>` 有关。
因此，只要新文件使用相同的域，你就可以重写 ``translations/`` 主目录中的任何bundle翻译文件。

例如，要重写 ``Resources/translations/FOSUserBundle.es.yml`` 文件中定义的翻译，请创建一个
``<your-project>/translations/FOSUserBundle.es.yml`` 文件。

.. _`Doctrine文档`: http://docs.doctrine-project.org/projects/doctrine-orm/en/latest/reference/inheritance-mapping.html#overrides
