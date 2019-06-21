.. index::
    single: Web Server

配置Web服务器
========================

开发Symfony应用的首选方法是使用 :doc:`Symfony的本地Web服务器 </setup/symfony_server>`。

但是，在生产环境中运行应用时，你需要使用功能齐全的Web服务器。
本文介绍了将Symfony与Apache或Nginx一起使用的几种方法。

使用Apache时，你可以将PHP配置为一个
:ref:`Apache模块 <web-server-apache-mod-php>`，或使用
:ref:`PHP FPM <web-server-apache-fpm>` 来配置FastCGI。
FastCGI也是 :ref:`在Nginx中 <web-server-nginx>` 使用PHP的首选方式。

.. sidebar:: 公共目录

    公共目录是所有应用的公共和静态文件的主页，包括图像、样式表和脚本文件。
    它也是前控制器（``index.php``）所在的地方。

    配置Web服务器时，公共目录充当文档根目录。
    在下面的示例中，``public/`` 目录将成为文档根目录。这个目录是 ``/var/www/project/public/``。

    如果你的托管服务提供商要求你将 ``public/``
    目录更改为其他位置（例如 ``public_html/``），请确保
    :ref:`重写 public/ 目录的位置 <override-web-dir>`。

.. _web-server-apache-mod-php:

添加重写规则
--------------------

最简单的方法是通过执行以下命令来安装Apache指令：

.. code-block:: terminal

    $ composer require symfony/apache-pack

该指令在 ``public/`` 目录中安装了一个包含重写规则的 ``.htaccess`` 文件。

.. tip::

    通过将重写规则从 ``.htaccess`` 文件移动到Apache配置的 ``VirtualHost`` 区块，然后将
    ``AllowOverride All`` 更改为 ``AllowOverride None``， 可以实现性能提升。

Apache和mod_php/PHP-CGI
---------------------------

使你的应用在Apache下运行的 **最低配置** 是：

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        DocumentRoot /var/www/project/public
        <Directory /var/www/project/public>
            AllowOverride All
            Order Allow,Deny
            Allow from All
        </Directory>

        # 如果你将资源安装为符号链接
        # 或在编译 LESS/Sass/CoffeeScript 资产时遇到问题，请取消以下注释行
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined
    </VirtualHost>

.. tip::

    如果你的系统支持 ``APACHE_LOG_DIR`` 变量，你可能希望使用
    ``${APACHE_LOG_DIR}/`` 而不是硬编码 ``/var/log/apache2/``。

使用以下 **优化配置** 来禁用 ``.htaccess`` 以支持并提高Web服务器性能：

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        DocumentRoot /var/www/project/public
        <Directory /var/www/project/public>
            AllowOverride None
            Order Allow,Deny
            Allow from All

            FallbackResource /index.php
        </Directory>

        # 如果你将资源安装为符号链接
        # 或在编译 LESS/Sass/CoffeeScript 资产时遇到问题，请取消以下注释行
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        # 可选地禁用资产目录的回退(fallback)资源，
        # 这将允许Apache在找不到文件时返回一个404错误，而不是将请求传递给Symfony
        <Directory /var/www/project/public/bundles>
            FallbackResource disabled
        </Directory>
        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined

        # 可选地设置应用中使用的环境变量的值
        #SetEnv APP_ENV prod
        #SetEnv APP_SECRET <app-secret-id>
        #SetEnv DATABASE_URL "mysql://db_user:db_pass@host:3306/db_name"
    </VirtualHost>

.. tip::

    如果你使用的是 **php-cgi**，Apache默认情况下不会将HTTP基本用户名和密码传递给PHP。
    要解决此限制，你应使用以下配置代码段：

    .. code-block:: apache

        RewriteRule .* - [E=HTTP_AUTHORIZATION:%{HTTP:Authorization}]

Apache2.4和mod_php/PHP-CGI
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在Apache 2.4中， ``Order Allow,Deny`` 已被替换为 ``Require all granted``。
因此，你需要修改你的 ``Directory`` 权限设置，如下所示：

.. code-block:: apache

    <Directory /var/www/project/public>
        Require all granted
        # ...
    </Directory>

有关高级的Apache配置选项，请阅读官方的 `Apache文档`_。

.. _web-server-apache-fpm:

Apache和PHP-FPM
-------------------

要在Apache中使用PHP-FPM，首先必须你拥有FastCGI进程管理器
``php-fpm`` 的二进制文件和Apache的FastCGI模块（例如，在基于Debian的系统上，你必须安装
``libapache2-mod-fastcgi`` 和 ``php7.1-fpm`` 包）。

PHP-FPM使用所谓的 *池* 来处理传入的FastCGI请求。
你可以在FPM配置中配置任意数量的池。
在一个池中，你可以配置要监听的TCP套接字（IP和端口）或Unix域套接字。
每个池也可以在一个不同的UID和GID下运行：

.. code-block:: ini

    ; 名为 www 的一个池
    [www]
    user = www-data
    group = www-data

    ; 使用一个Unix域套接字
    listen = /var/run/php/php7.1-fpm.sock

    ; 或监听一个TCP套接字
    listen = 127.0.0.1:9000

Apache2.4和mod_proxy_fcgi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

如果你运行的是Apache 2.4，则可以使用 ``mod_proxy_fcgi`` 将传入的请求传递给PHP-FPM。
配置PHP-FPM以监听TCP或Unix套接字，在Apache配置中启用
``mod_proxy`` 和 ``mod_proxy_fcgi``，并使用 ``SetHandler`` 指令将PHP文件的请求传递给PHP-FPM：

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        # 取消以下注释行以强制Apache将认证标头传递给PHP：
        # PHP-FPM和FastCGI中的“basic_auth”的必需要求
        #
        # SetEnvIfNoCase ^Authorization$ "(.+)" HTTP_AUTHORIZATION=$1

        # 对于Apache 2.4.9 或更高版本
        # 使用SetHandler避免了将ProxyPassMatch
        # 与mod_rewrite或mod_autoindex结合使用时出现的问题
        <FilesMatch \.php$>
            SetHandler proxy:fcgi://127.0.0.1:9000
            # Apache 2.4.10 或更高版本的Unix套接字
            # SetHandler proxy:unix:/path/to/fpm.sock|fcgi://dummy
        </FilesMatch>

        # 如果你使用2.4.9以下的Apache版本，则必须考虑更新或使用此版本
        # ProxyPassMatch ^/(.*\.php(/.*)?)$ fcgi://127.0.0.1:9000/var/www/project/public/$1

        # 如果在文档根目录的子路径上运行Symfony应用，则必须相应地更改此正则表达式：
        # ProxyPassMatch ^/path-to-app/(.*\.php(/.*)?)$ fcgi://127.0.0.1:9000/var/www/project/public/$1

        DocumentRoot /var/www/project/public
        <Directory /var/www/project/public>
            # 启用 .htaccess 重写
            AllowOverride All
            Require all granted
        </Directory>

        # 如果你将资源安装为符号链接
        # 或在编译 LESS/Sass/CoffeeScript 资产时遇到问题，请取消以下注释行
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined
    </VirtualHost>

Apache2.2和PHP-FPM
~~~~~~~~~~~~~~~~~~~~~~~

在Apache 2.2或更低版本，你无法使用 ``mod_proxy_fcgi``。
你必须使用 `FastCgiExternalServer`_ 指令。因此，你的Apache配置应如下所示：

.. code-block:: apache

    <VirtualHost *:80>
        ServerName domain.tld
        ServerAlias www.domain.tld

        AddHandler php7-fcgi .php
        Action php7-fcgi /php7-fcgi
        Alias /php7-fcgi /usr/lib/cgi-bin/php7-fcgi
        FastCgiExternalServer /usr/lib/cgi-bin/php7-fcgi -host 127.0.0.1:9000 -pass-header Authorization

        DocumentRoot /var/www/project/public
        <Directory /var/www/project/public>
            # 启用 .htaccess 重写
            AllowOverride All
            Order Allow,Deny
            Allow from all
        </Directory>

        # 如果你将资源安装为符号链接
        # 或在编译 LESS/Sass/CoffeeScript 资产时遇到问题，请取消以下注释行
        # <Directory /var/www/project>
        #     Options FollowSymlinks
        # </Directory>

        ErrorLog /var/log/apache2/project_error.log
        CustomLog /var/log/apache2/project_access.log combined
    </VirtualHost>

如果你更喜欢使用Unix套接字，则必须使用 ``-socket`` 选项来代替：

.. code-block:: apache

    FastCgiExternalServer /usr/lib/cgi-bin/php7-fcgi -socket /var/run/php/php7.1-fpm.sock -pass-header Authorization

.. _web-server-nginx:

Nginx
-----

让你的应用在Nginx下运行的 **最低配置** 是：

.. code-block:: nginx

    server {
        server_name domain.tld www.domain.tld;
        root /var/www/project/public;

        location / {
            # 尝试直接提供文件，否则回退到 index.php
            try_files $uri /index.php$is_args$args;
        }

        location ~ ^/index\.php(/|$) {
            fastcgi_pass unix:/var/run/php/php7.2-fpm.sock;
            fastcgi_split_path_info ^(.+\.php)(/.*)$;
            include fastcgi_params;

            # 可选地设置应用中使用的环境变量的值
            # fastcgi_param APP_ENV prod;
            # fastcgi_param APP_SECRET <app-secret-id>;
            # fastcgi_param DATABASE_URL "mysql://db_user:db_pass@host:3306/db_name";

            # 当你使用符号链接将文档根目录链接到你的应用的当前版本时，
            # 你应该将实际的应用路径而不是符号链接的路径传递给PHP-FPM。
            # 否则，PHP的OPcache可能无法正确检测PHP文件的变更
            # (查看 https://github.com/zendtech/ZendOptimizerPlus/issues/126
            # 以获得更多信息).
            fastcgi_param SCRIPT_FILENAME $realpath_root$fastcgi_script_name;
            fastcgi_param DOCUMENT_ROOT $realpath_root;
            # 这将阻止包含前端控制器的URI。以下URI将会是404：
            # http://domain.tld/index.php/some-path
            # 移除该内部指令以允许这样的URI
            internal;
        }

        # 对于与前端控制器不匹配的所有其他php文件，返回404
        # 这样可以禁止访问你不想被访问的其他php文件。
        location ~ \.php$ {
            return 404;
        }

        error_log /var/log/nginx/project_error.log;
        access_log /var/log/nginx/project_access.log;
    }

.. note::

    根据你的PHP-FPM配置，``fastcgi_pass`` 也可以是 ``fastcgi_pass 127.0.0.1:9000``。

.. tip::

    在公共目录中 **仅** 执行 ``index.php``。所有其他以 “.php” 结尾的文件将被拒绝。

    如果你的公共目录中有其他需要执行的PHP文件，请务必将它们包含在上面的 ``location`` 区块中。

.. caution::

    部署到生产后，请确保你 *无法* 访问 ``index.php``
    脚本（即 ``http://example.com/index.php``）。

.. note::

    默认情况下，Symfony应用包含多个 ``.htaccess`` 文件来配置重定向并防止对某些敏感目录的未授权访问。
    这些文件仅在使用Apache时有用，因此你可以在使用Nginx时安全地删除它们。

有关高级Nginx配置选项，请阅读官方的 `Nginx文档`_。

.. _`Apache文档`: https://httpd.apache.org/docs/
.. _`FastCgiExternalServer`: https://docs.oracle.com/cd/B31017_01/web.1013/q20204/mod_fastcgi.html#FastCgiExternalServer
.. _`Nginx文档`: https://www.nginx.com/resources/wiki/start/topics/recipes/symfony/
