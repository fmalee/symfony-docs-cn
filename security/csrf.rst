.. index::
    single: CSRF; CSRF protection

如何实施CSRF保护
================================

CSRF - 或 `跨站点请求伪造`_ - 是一种恶意用户试图让你的合法用户无意中提交他们不打算提交的数据的方法。

CSRF保护的工作原理是在表单中添加一个隐藏字段，其中包含只有你和你的用户才知道的值。
这可确保是用户自身（而非其他实体）提交给定数据。

在使用CSRF保护之前，请将其安装到你的项目中：

.. code-block:: terminal

    $ composer require symfony/security-csrf

然后，使用 ``csrf_protection`` 选项启用/禁用CSRF保护
（有关更多信息，请参阅 :ref:`CSRF配置参考 <reference-framework-csrf-protection>`）：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/framework.yaml
        framework:
            # ...
            csrf_protection: ~

    .. code-block:: xml

        <!-- config/packages/framework.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:csrf-protection enabled="true" />
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', array(
            'csrf_protection' => null,
        ));

Symfony表单中的CSRF保护
--------------------------------

使用Symfony Form组件创建的表单默认包含CSRF令牌，Symfony会自动检查它们，因此你无需做任何事情来防止CSRF攻击。

.. _form-csrf-customization:

默认情况下，Symfony会在名为 ``_token`` 的隐藏字段中添加CSRF令牌，但这可以在每个表单的基础上进行自定义::

    // ...
    use App\Entity\Task;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class TaskType extends AbstractType
    {
        // ...

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class'      => Task::class,
                // 在表单中启用/禁用CSRF保护
                'csrf_protection' => true,
                // 保存令牌的HTML隐藏字段的名称
                'csrf_field_name' => '_token',
                // 用于生成令牌值的一个任意字符串
                // 为每个表单使用不同的字符串可以提高其安全性
                'csrf_token_id'   => 'task_item',
            ));
        }

        // ...
    }

.. caution::

    由于令牌存储在会话中，因此只要渲染具有CSRF保护的表单，系统就会自动启动一个会话。

.. caution::

    CSRF令牌对每个用户来说都是不同的。在缓存包含CSRF令牌的表单的页面时要小心。
    有关更多信息，请参阅 :doc:`/http_cache/form_csrf_caching`。

登录表单中的CSRF保护
------------------------------

请参阅 :doc:`/security/form_login_setup` 来了解如何在登录表单启用CSRF保护。

HTML表单中的CSRF保护
-----------------------------

.. versionadded:: 4.1
    在4.1之前的Symfony版本中，CSRF支持需要安装Symfony Form组件，即使你并不使用该组件。

也可以将CSRF保护添加到不受Symfony Form组件管理的常规HTML表单中，例如用于删除项目的简单表单。
首先，使用Twig模板中的 ``csrf_token()`` 函数生成CSRF令牌并将其存储为表单的隐藏字段：

.. code-block:: twig

    <form action="{{ url('admin_post_delete', { id: post.id }) }}" method="post">
        {# csrf_token() 的参数是用于生成令牌的任意值 #}
        <input type="hidden" name="token" value="{{ csrf_token('delete-item') }}" />

        <button type="submit">Delete item</button>
    </form>

然后，在控制器动作中获取CSRF令牌的值，并使用
:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\AbstractController::isCsrfTokenValid`
来检查其有效性::

    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function delete(Request $request)
    {
        $submittedToken = $request->request->get('token');

        // 'delete-item'与模板中用于生成令牌的值相同
        if ($this->isCsrfTokenValid('delete-item', $submittedToken)) {
            // ... 做些事情，例如删除一个对象
        }
    }

.. _`跨站点请求伪造`: http://en.wikipedia.org/wiki/Cross-site_request_forgery
