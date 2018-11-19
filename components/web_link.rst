.. index::
   single: WebLink
   single: Components; WebLink

WebLink组件
======================

   WebLink组件提供了在使用 `HTTP/2服务器推送`_ 和 `资源提示`_ 时管理
   `Web Linking`_ 所需的 ``Link`` HTTP标头的工具。

安装
------------

.. code-block:: terminal

    $ composer require symfony/web-link

或者，你可以克隆 `<https://github.com/symfony/web-link>`_ 仓库。

.. include:: /components/require_autoload.rst.inc

用法
-----

以下示例显示了该组件的操作::

   use Fig\Link\GenericLinkProvider;
   use Fig\Link\Link;
   use Symfony\Component\WebLink\HttpHeaderSerializer;

   $linkProvider = (new GenericLinkProvider())
       ->withLink(new Link('preload', '/bootstrap.min.css'));

   header('Link: '.(new HttpHeaderSerializer())->serialize($linkProvider->getLinks()));

   echo 'Hello';

阅读完整的 :doc:`WebLink文档 </web_link>`，了解该组件的所有功能及其与Symfony框架的集成。

.. _`Web Linking`: https://tools.ietf.org/html/rfc5988
.. _`HTTP/2服务器推送`: https://tools.ietf.org/html/rfc7540#section-8.2
.. _`资源提示`: https://www.w3.org/TR/resource-hints/
