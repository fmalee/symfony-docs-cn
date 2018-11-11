.. index::
    single: DependencyInjection; Debug
    single: Service Container; Debug

如何调试服务容器和列出服务
==================================================

你可以使用控制台找出已在容器注册的服务。要显示所有服务（公有和私有）及其PHP类，请运行：

.. code-block:: terminal

    $ php bin/console debug:container

    # 也可以添加此选项来显示“隐藏服务” (那些ID以点号开头的服务)
    $ php bin/console debug:container --show-hidden

.. versionadded:: 4.1
    Symfony 4.1中引入了隐藏服务和 ``--show-hidden`` 选项。

要查看可用于自动装配的所有可用类型的列表，请运行：

.. code-block:: terminal

    $ php bin/console debug:autowiring

关于单一服务的详细信息
------------------------------------

你可以通过指定其ID来获取有关特定服务的更多详细信息：

.. code-block:: terminal

    $ php bin/console debug:container 'App\Service\Mailer'

    # 显示该服务的参数:
    $ php bin/console debug:container 'App\Service\Mailer' --show-arguments
