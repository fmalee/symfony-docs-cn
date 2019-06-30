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
                https://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony
                https://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:csrf-protection enabled="true"/>
            </framework:config>
        </container>

    .. code-block:: php

        // config/packages/framework.php
        $container->loadFromExtension('framework', [
            'csrf_protection' => null,
        ]);

用于CSRF保护的令牌对于每个用户而言都是不同的，并且它们存储在会话中。
这就是为什么一旦渲染具有CSRF保护的表单就会自动启动会话。

.. _caching-pages-that-contain-csrf-protected-forms:

此外，这意味着你无法完全缓存包含CSRF保护的表单页面。作为替代方案，你可以：

* 将表单嵌入到未缓存的 :doc:`ESI片段 </http_cache/esi>` 中并缓存其余页面内容;
* 缓存整个页面并通过未缓存的AJAX请求来加载表单;
* 缓存整个页面并配合 :doc:`hinclude.js </templating/hinclude>`
  来使用未缓存的AJAX请求来加载CSRF令牌，并用它替换表单字段值。

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
            $resolver->setDefaults([
                'data_class'      => Task::class,
                // 在表单中启用/禁用CSRF保护
                'csrf_protection' => true,
                // 保存令牌的HTML隐藏字段的名称
                'csrf_field_name' => '_token',
                // 用于生成令牌值的一个任意字符串
                // 为每个表单使用不同的字符串可以提高其安全性
                'csrf_token_id'   => 'task_item',
            ]);
        }

        // ...
    }

你还可以自定义CSRF表单字段的渲染，创建一个自定义
:doc:`表单主题 </form/form_themes>`，并使用 ``csrf_token`` 作为字段的前缀（例如，定义
``{% block csrf_token_widget %} ... {% endblock %}`` 以自定义整个表单字段的内容）。

.. versionadded:: 4.3

    Symfony 4.3中引入了 ``csrf_token`` 表单字段前缀。

登录表单中的CSRF保护
------------------------------

请参阅 :doc:`/security/form_login_setup` 来了解如何在登录表单启用CSRF保护。你还可以
:ref:`为注销动作配置CSRF保护 <reference-security-logout-csrf>`。

.. _csrf-protection-in-html-forms:

手动生成和检查CSRF令牌
--------------------------------------------

虽然Symfony Forms默认提供自动CSRF保护，但你可能需要手动生成和检查CSRF令牌，
例如使用非Symfony Form组件管理的常规HTML表单时。

考虑创建一个简单的HTML表单以允许删除项目。首先，使用
:ref:`csrf_token() Twig函数 <reference-twig-function-csrf-token>`
在模板中生成CSRF令牌并将其存储为隐藏的表单字段：

.. code-block:: html+twig

    <form action="{{ url('admin_post_delete', { id: post.id }) }}" method="post">
        {# csrf_token() 的参数是用于生成令牌的任意字符串 #}
        <input type="hidden" name="token" value="{{ csrf_token('delete-item') }}"/>

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
