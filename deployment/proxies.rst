如何配置Symfony以在负载均衡器或反向代理后面工作
==========================================================================

部署应用时，你可能会在负载均衡器（例如AWS Elastic Load Balancing）或反向代理
（例如用于 :doc:`缓存</http_cache>` 的Varnish）之后。

在大多数情况下，这不会导致Symfony出现任何问题。
但是，当请求通过代理时，会使用标准的 ``Forwarded`` 标头或 ``X-Forwarded-*`` 标头发送某些请求信息。
例如，用户的真实IP将存储在标准的 ``Forwarded: for="..."`` 标头或 ``X-Forwarded-For`` 标头中，
而不是读取 ``REMOTE_ADDR`` （现在将是你的反向代理的IP地址）。

如果你不配置Symfony以查找这些标头，
你将获得有关于客户端IP地址、客户端是否通过HTTPS连接、客户端端口以及请求的主机名等的错误信息。

.. _request-set-trusted-proxies:

解决方案：setTrustedProxies()
-----------------------------

要解决此问题，你需要告诉Symfony要信任的反向代理的IP地址以及反向代理用于发送信息的标头：

.. code-block:: php

    // public/index.php

    // ...
    $request = Request::createFromGlobals();

    // 告诉Symfony关于反向代理的信息
    Request::setTrustedProxies(
        // 你的代理的IP地址 (或范围)
        ['192.0.0.1', '10.0.0.0/8'],

        // 信任 *所有* "X-Forwarded-*" 标头
        Request::HEADER_X_FORWARDED_ALL

        // 或者，你的代理使用 “Forwarded” 标头
        // Request::HEADER_FORWARDED

        // 或者，你是使用 AWS ELB
        // Request::HEADER_X_FORWARDED_AWS_ELB
    );

请求对象有几个 ``Request::HEADER_*`` 常量，可以准确控制来自反向代理的 *哪些* 标头是可信的。
参数是一个比特(bit)字段，因此你也可以传递自己的值（例如 ``0b00110``）。

但是，如果我的反向代理的IP不断变化怎么办？
----------------------------------------------------------

某些反向代理（如AWS Elastic Load Balancing）没有静态IP地址，甚至没有可以使用CIDR表示法来定位的范围。
在这种情况下，你需要 - *非常小心的* - 信任 *所有* 代理。

#. 将你的Web服务器配置为 *不* 响应来自负载均衡器以外的 *任何* 客户端的流量。
   对于AWS，可以使用 `security groups`_ 完成此操作。

#. 一旦你确保流量只来自你可信的反向代理，请将Symfony配置为 *始终* 信任传入的请求：

   .. code-block:: php

       // public/index.php

       // ...
       Request::setTrustedProxies(
           // 信任 *所有* 请求
           array('127.0.0.1', $request->server->get('REMOTE_ADDR')),

           // 如果你正在使用ELB，否则使用上面的常量
           Request::HEADER_X_FORWARDED_AWS_ELB
       );

仅此而已！阻止来自所有不受信任来源的流量至关重要。
如果你允许外部流量，他们可以“仿冒”他们的真实IP地址和其他信息。

.. _`security groups`: http://docs.aws.amazon.com/elasticloadbalancing/latest/classic/elb-security-groups.html
.. _`RFC 7239`: http://tools.ietf.org/html/rfc7239
