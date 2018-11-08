.. index::
    single: Templating; app Variable

如何通过 ``app`` 变量访问Twig中的用户、请求、会话等
================================================================================

在每个请求期间，Symfony将在Twig和PHP模板引擎中设置默认的 ``app`` 全局模板变量。
该 ``app`` 变量是一个 :class:`Symfony\\Bridge\\Twig\\AppVariable` 实例，
它允许你自动访问某些特定于应用的变量：

``app.user``
    表示当前的用户，如果没有用户则为 ``null``。
    存储在此变量中的值可以是
    :class:`Symfony\\Component\\Security\\Core\\User\\UserInterface` 对象，
    实现 ``__toString()`` 方法的任何其他对象，甚至是一个常规字符串。
``app.request``
    表示当前请求的 :class:`Symfony\\Component\\HttpFoundation\\Request` 对象
    （取决于你的应用，它可以是子请求或常规请求，稍后会再述）。
``app.session``
    表示当前用户会话的 :class:`Symfony\\Component\\HttpFoundation\\Session\\Session` 对象，
    如果对象不存在，则为 ``null``。
``app.environment``
    当前环境的名称（``dev``、``prod`` 等等）。
``app.debug``
    如果处于调试模式则为 ``True``。否则就为 ``False``。

.. code-block:: html+twig

    <p>Username: {{ app.user.username }}</p>
    {% if app.debug %}
        <p>Request method: {{ app.request.method }}</p>
        <p>Application Environment: {{ app.environment }}</p>
    {% endif %}

.. tip::

    你可以添加自己的全局模板变量，请参阅 :doc:`/templating/global_variables`。
