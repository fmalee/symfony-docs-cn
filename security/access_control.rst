.. _security-access-control-explanation:

安全性access_control如何工作？
==========================================

对于每个传入的请求，Symfony检查会每个 ``access_control`` 以找到 *一个* 与当前请求相匹配的那个项。
一旦找到匹配的 ``access_control`` 项，它就会停止 - 只有 *第一个* 匹配的 ``access_control`` 会被用于限制访问。

每个 ``access_control`` 都有几个选项来配置两件事情：

#. :ref:`传入请求是否与此访问控制项匹配 <security-access-control-matching-options>`
#. :ref:`一旦匹配，是否应该执行某种访问限制 <security-access-control-enforcement-options>`:

.. _security-access-control-matching-options:

1. 匹配选项
-------------------

Symfony 为每个 ``access_control`` 项创建一个
:class:`Symfony\\Component\\HttpFoundation\\RequestMatcher`
实例，用于确定是否应在此请求上使用给定的访问控制。以下选项会被用于匹配 ``access_control`` 项：

* ``path``
* ``ip`` 或 ``ips`` (也支持网络掩码)
* ``port``
* ``host``
* ``methods``

以下面的 ``access_control`` 项为例：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            access_control:
                - { path: ^/admin, roles: ROLE_USER_IP, ip: 127.0.0.1 }
                - { path: ^/admin, roles: ROLE_USER_IP, ip: 127.0.0.1, port: 8080 }
                - { path: ^/admin, roles: ROLE_USER_HOST, host: symfony\.com$ }
                - { path: ^/admin, roles: ROLE_USER_METHOD, methods: [POST, PUT] }
                - { path: ^/admin, roles: ROLE_USER }

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->
                <rule path="^/admin" role="ROLE_USER_IP" ip="127.0.0.1" />
                <rule path="^/admin" role="ROLE_USER_IP" ip="127.0.0.1" port="8080" />
                <rule path="^/admin" role="ROLE_USER_HOST" host="symfony\.com$" />
                <rule path="^/admin" role="ROLE_USER_METHOD" methods="POST, PUT" />
                <rule path="^/admin" role="ROLE_USER" />
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...
            'access_control' => array(
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER_IP',
                    'ip' => '127.0.0.1',
                ),
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER_IP',
                    'ip' => '127.0.0.1',
                    'port' => '8080',
                ),
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER_HOST',
                    'host' => 'symfony\.com$',
                ),
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER_METHOD',
                    'methods' => 'POST, PUT',
                ),
                array(
                    'path' => '^/admin',
                    'role' => 'ROLE_USER',
                ),
            ),
        ));

对于每个传入请求，Symfony将根据URI、客户端的IP地址、传入主机名和请求方法来决定使用哪个 ``access_control``。
请记住，使用第一个匹配的规则时，如果一个 ``access_control`` 没有指定
``ip``、``port``、``host`` 或 ``method``，那么该项将匹配任何
``ip``、``port``、``host`` 或 ``method``：

+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| URI             | IP          | PORT        | HOST        | METHOD     | ``access_control``             | Why?                                                        |
+=================+=============+=============+=============+============+================================+=============================================================+
| ``/admin/user`` | 127.0.0.1   | 80          | example.com | GET        | 角色 #1 (``ROLE_USER_IP``)     | The URI matches ``path`` and the IP matches ``ip``.         |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| ``/admin/user`` | 127.0.0.1   | 80          | symfony.com | GET        | 角色 #1 (``ROLE_USER_IP``)     | The ``path`` and ``ip`` still match. This would also match  |
|                 |             |             |             |            |                                | the ``ROLE_USER_HOST`` entry, but *only* the **first**      |
|                 |             |             |             |            |                                | ``access_control`` match is used.                           |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| ``/admin/user`` | 127.0.0.1   | 8080        | symfony.com | GET        | 角色 #2 (``ROLE_USER_PORT``)   | The ``path``, ``ip`` and ``port`` match.                    |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1   | 80          | symfony.com | GET        | 角色 #3 (``ROLE_USER_HOST``)   | The ``ip`` doesn't match the first rule, so the second      |
|                 |             |             |             |            |                                | rule (which matches) is used.                               |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1   | 80          | symfony.com | POST       | 角色 #3 (``ROLE_USER_HOST``)   | The second rule still matches. This would also match the    |
|                 |             |             |             |            |                                | third rule (``ROLE_USER_METHOD``), but only the **first**   |
|                 |             |             |             |            |                                | matched ``access_control`` is used.                         |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1   | 80          | example.com | POST       | 角色 #4 (``ROLE_USER_METHOD``) | The ``ip`` and ``host`` don't match the first two entries,  |
|                 |             |             |             |            |                                | but the third - ``ROLE_USER_METHOD`` - matches and is used. |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| ``/admin/user`` | 168.0.0.1   | 80          | example.com | GET        | 角色 #5 (``ROLE_USER``)        | The ``ip``, ``host`` and ``method`` prevent the first       |
|                 |             |             |             |            |                                | three entries from matching. But since the URI matches the  |
|                 |             |             |             |            |                                | ``path`` pattern of the ``ROLE_USER`` entry, it is used.    |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+
| ``/foo``        | 127.0.0.1   | 80          | symfony.com | POST       | matches no entries             | This doesn't match any ``access_control`` rules, since its  |
|                 |             |             |             |            |                                | URI doesn't match any of the ``path`` values.               |
+-----------------+-------------+-------------+-------------+------------+--------------------------------+-------------------------------------------------------------+

.. _security-access-control-enforcement-options:

2. 访问限制
---------------------

一旦Symfony确定哪个 ``access_control`` 项匹配（如果有的话），然后它 *强制* 基于 ``roles``、``allow_if`` 和 ``requires_channel`` 选项进行访问限制：

* ``roles`` 如果用户没有给定角色，则拒绝访问（在内部，抛出一个
  :class:`Symfony\\Component\\Security\\Core\\Exception\\AccessDeniedException`）。
  如果此值是多个角色的数组，则用户必须至少拥有其中一个（在使用 :ref:`访问决策管理器 <components-security-access-decision-manager>` 的默认 ``affirmative``
  策略时 ），如果使用 ``unanimous`` 策略，则要求必须拥有所有角色；

* ``allow_if`` 如果表达式返回false，则拒绝访问;

* ``requires_channel`` 如果传入请求的通道（例如 ``http``）与该值（例如
  ``https``）不匹配，则用户将被重定向（例如，从 ``http`` 重定向到 ``https``，或反之亦然）。

.. tip::

    如果拒绝访问，系统将尝试认证用户（如果尚未认证的话）（例如，将用户重定向到登录页面）。
    如果用户已登录，将显示403“拒绝访问”错误页面。
    有关更多信息，请参见 :doc:`/controller/error_pages`。

通过IP匹配 access_control
-----------------------------

可能会出现某些案例，就是你需要有一个 *仅* 匹配来自某个IP地址或范围的请求的 ``access_control`` 项。
例如，这可能被用来拒绝来自一个URL模式的所有请求，除了那些来自一个可信的内部服务器的访问。

.. caution::

    正如你将在下面的示例中解释的那样，``ips`` 选项不限于一个特定的IP地址。
    相反，使用 ``ips`` 键意味着该 ``access_control`` 项仅匹配此IP地址，从不同IP地址访问它的用户将继续沿着 ``access_control`` 列表继续。

下面是一个示例，说明如何配置一些示例 ``/internal*`` URL模式，以便只能通过本地服务器自身的请求访问它：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            access_control:
                #
                # 'ips' 选项支持IP地址和子网掩码
                - { path: ^/internal, roles: IS_AUTHENTICATED_ANONYMOUSLY, ips: [127.0.0.1, ::1, 192.168.0.1/24] }
                - { path: ^/internal, roles: ROLE_NO_ACCESS }

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <!-- the 'ips' option supports IP addresses and subnet masks -->
                <rule path="^/internal" role="IS_AUTHENTICATED_ANONYMOUSLY">
                    <ip>127.0.0.1</ip>
                    <ip>::1</ip>
                </rule>

                <rule path="^/internal" role="ROLE_NO_ACCESS" />
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            // ...
            'access_control' => array(
                array(
                    'path' => '^/internal',
                    'role' => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    // the 'ips' option supports IP addresses and subnet masks
                    'ips' => array('127.0.0.1', '::1'),
                ),
                array(
                    'path' => '^/internal',
                    'role' => 'ROLE_NO_ACCESS',
                ),
            ),
        ));

当路径是 ``/internal/something``，IP地址是来自外部的 ``10.0.0.1`` 时，我们看看它是如何工作的：

* 第一个访问控制规则被忽略，因为 ``path`` 匹配但该IP地址与访问控制列出的任何一个IP都不匹配;

* 第二个访问控制规则被启用，因为其唯一的限制是 ``path``，所以它匹配成功。
  如果你确认没有用户拥有 ``ROLE_NO_ACCESS``，那么该访问会被拒绝（``ROLE_NO_ACCESS``
  可以是任何与现有角色不匹配的东西，它只是一个始终拒绝访问的技巧）。

但是，如果有相同的请求却来自 ``127.0.0.1`` 或 ``::1`` （IPv6环回地址）：

* 现在，第一个访问控制规则被启用，因为 ``path`` 和 ``ip``
  都匹配，然后该访问被允许，因为用户总是拥有 ``IS_AUTHENTICATED_ANONYMOUSLY`` 角色。

* 第一个规则匹配后，就不会检查第二个访问规则。


.. _security-allow-if:

使用表达式进行限制
~~~~~~~~~~~~~~~~~~~~~~~~~

一旦一个 ``access_control`` 项匹配，就可以通过 ``roles`` 键拒绝访问，或在
``allow_if`` 键的表达式中使用更复杂的逻辑：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            access_control:
                -
                    path: ^/_internal/secure
                    allow_if: "'127.0.0.1' == request.getClientIp() or is_granted('ROLE_ADMIN')"

    .. code-block:: xml

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <rule path="^/_internal/secure"
                    allow-if="'127.0.0.1' == request.getClientIp() or is_granted('ROLE_ADMIN')" />
            </config>
        </srv:container>

    .. code-block:: php

            'access_control' => array(
                array(
                    'path' => '^/_internal/secure',
                    'allow_if' => '"127.0.0.1" == request.getClientIp() or is_granted("ROLE_ADMIN")',
                ),
            ),

在这种情况下，当用户尝试访问任何以 ``/_internal/secure`` 开头的URL时，只有在IP地址是 ``127.0.0.1`` 或用户具有 ``ROLE_ADMIN``  角色的情况下，才会授予他们访问权限。

在表达式中，你可以访问许多不同的变量和函数，包括 ``request``，它是Symfony的
:class:`Symfony\\Component\\HttpFoundation\\Request`
对象（请参阅 :ref:`component-http-foundation-request`）。

有关其他函数和变量的列表，请参阅 :ref:`函数和变量s <security-expression-variables>`。

.. tip::

    ``allow_if`` 表达式也可以包含用
    :ref:`表达式提供器 <components-expression-language-provider>` 注册的自定义函数。

    .. versionadded:: 4.1
        在Symfony 4.1中引入了在 ``allow_if`` 表达式中使用自定义函数的功能。

限制端口
------------------

添加 ``port`` 选项到任何 ``access_control`` 项以要求用户通过一个特定端口访问这些URL。
例如，对于 ``localhost:8080`` 这就比较有用。

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            access_control:
                - { path: ^/cart/checkout, roles: IS_AUTHENTICATED_ANONYMOUSLY, port: 8080 }

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <rule path="^/cart/checkout"
                role="IS_AUTHENTICATED_ANONYMOUSLY"
                port="8080"
            />
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            'access_control' => array(
                array(
                    'path' => '^/cart/checkout',
                    'role' => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    'port' => '8080',
                ),
            ),
        ));

强制使用特定通道（http，https）
-------------------------------

你还可以要求用户通过SSL访问一个URL。
可以在任何 ``access_control`` 项中使用 ``requires_channel`` 参数。
如果该 ``access_control`` 项被匹配并且请求正在使用 ``http`` 频道，则该用户将被重定向到 ``https``：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            # ...
            access_control:
                - { path: ^/cart/checkout, roles: IS_AUTHENTICATED_ANONYMOUSLY, requires_channel: https }

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <rule path="^/cart/checkout"
                role="IS_AUTHENTICATED_ANONYMOUSLY"
                requires-channel="https"
            />
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        $container->loadFromExtension('security', array(
            'access_control' => array(
                array(
                    'path' => '^/cart/checkout',
                    'role' => 'IS_AUTHENTICATED_ANONYMOUSLY',
                    'requires_channel' => 'https',
                ),
            ),
        ));
