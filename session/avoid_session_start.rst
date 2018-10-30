.. index::
    single: Sessions, cookies

避免为匿名用户启动会话
===========================================

无论何时读取、写入甚至检查会话中的数据，都会自动启动会话。
这意味着如果你需要避免为某些用户创建一个会话cookie，则可能很困难：你必须\ *完全*\避免访问会话。

例如，在这种情况下的一个常见操作需要检查存储在会话中的闪存消息。
以下代码将保证(guarantee)\ *始终*\启动一个会话：

.. code-block:: html+twig

    {% for message in app.flashes('notice') %}
        <div class="flash-notice">
            {{ message }}
        </div>
    {% endfor %}

即使用户没有登录，即使你没有创建任何闪存消息，只要调用 ``flashBag`` 的 ``get()`` （甚至 ``has()``）方法即会启动一个会话。
这可能会损害你的应用性能，因为所有用户都将收到一个会话cookie。
要避免此行为，请在尝试访问闪存消息之前进行检查：

.. code-block:: html+twig

    {% if app.request.method == 'POST' %}
        {% for message in app.flashes('notice') %}
            <div class="flash-notice">
                {{ message }}
            </div>
        {% endfor %}
    {% endif %}
