.. index:: Vagrant, Homestead

在Homestead/Vagrant中使用Symfony
====================================

为了开发Symfony应用，你可能希望使用虚拟开发环境而不是内置服务器或WAMP/LAMP。
Homestead_ 是一个易于使用的 Vagrant_ 盒子，可以快速启动和运行一个虚拟环境。

.. tip::

    由于Symfony中的文件系统操作量很大（例如更新缓存文件和写入日志文件），Symfony可能会显著减慢速度。
    要提高速度，请考虑 :ref:`重写缓存和日志目录 <override-cache-dir>`
    到NFS共享之外的位置（例如，使用 :phpfunction:`sys_get_temp_dir`）。
    你可以阅读 `此博客文章`_，了解有关加速Vagrant中的Symfony的更多提示。

安装Vagrant和Homestead
-----------------------------

在使用Homestead之前，你需要按照 `Homestead文档`_ 中的说明来安装和配置Vagrant和Homestead。

设置Symfony应用
--------------------------------

想象一下，你已将Symfony应用安装在本地系统的 ``~/projects/symfony_demo`` 上。
你首先需要Homestead来同步此项目中的文件。
执行 ``homestead edit`` 以编辑Homestead配置并配置 ``~/projects`` 目录：

.. code-block:: yaml

    # ...
    folders:
        - map: ~/projects
          to: /home/vagrant/projects

现在可以在Homestead环境的 ``/home/vagrant/projects`` 中访问PC上的 ``projects/`` 目录。

完成此操作后，在Homestead配置中配置Symfony应用：

.. code-block:: yaml

    # ...
    sites:
        - map: symfony-demo.test
          to: /home/vagrant/projects/symfony_demo/public
          type: symfony4

``type`` 选项告诉Homestead使用适用于Symfony的Nginx配置。
Homestead现在支持Symfony的2/3/4等三个版本。
即 ``type`` 为 ``symfony2`` 时，支持使用 ``app.php`` 和 ``app_dev.php`` 的前端布局；
``type`` 为 ``symfony4`` 时，支持使用 ``index.php`` 的前端布局。

最后，编辑本地计算机上的hosts文件以映射 ``symfony-demo.test`` 到
``192.168.10.10`` （对应Homestead使用的IP）::

    # /etc/hosts (unix) or C:\Windows\System32\drivers\etc\hosts (Windows)
    192.168.10.10 symfony-demo.test

现在，在你的Web浏览器中打开``http://symfony-demo.test``，然后开始享受开发Symfony应用的乐趣吧！

.. seealso::

    要了解Homestead的更多功能，包括Blackfire Profiler集成、自动创建MySQL数据库等，请阅读Homestead文档的 `Daily Usage`_ 章节。

.. _Homestead: https://laravel.com/docs/homestead
.. _Vagrant: https://www.vagrantup.com/
.. _Homestead文档: https://laravel.com/docs/homestead#installation-and-setup
.. _Daily Usage: https://laravel.com/docs/homestead#daily-usage
.. _此博客文章: https://www.whitewashing.de/2013/08/19/speedup_symfony2_on_vagrant_boxes.html
