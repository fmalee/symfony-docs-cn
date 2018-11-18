将信息从Twig传递到脚本
===========================================

在Symfony应用中，你可能会发现需要将一些动态数据（例如用户信息）从Twig传递到脚本代码。
传递动态配置的一个好方法是将信息存储在 ``data`` 属性中，稍后在脚本中读取它们。例如：

.. code-block:: twig

    <div class="js-user-rating" data-is-authenticated="{{ app.user ? 'true' : 'false' }}">
        <!-- ... -->
    </div>

在脚本中获取该内容：

.. code-block:: javascript

    document.addEventListener('DOMContentLoaded', function() {
        var userRating = document.querySelector('.js-user-rating');
        var isAuthenticated = userRating.dataset.isAuthenticated;

        // 或使用jQuery
        //var isAuthenticated = $('.js-user-rating').data('isAuthenticated');
    });

.. note::

    当 `从脚本中访问data属性`_ 时，该属性名称要从划线式（dash-style）转换为驼峰拼写法。
    例如，``data-is-authenticated`` 变成 ``isAuthenticated``；
    ``data-number-of-reviews`` 变成 ``numberOfReviews``。

``data-`` 属性值没有大小限制，因此可以存储任何内容。
在Twig中，使用 ``html_attr`` 转义策略以避免弄乱HTML属性。
例如，如果你的 ``User`` 对象有一些 ``getProfileData()`` 方法返回一个数组，你可以执行以下操作：

.. code-block:: twig

    <div data-user-profile="{{ app.user ? app.user.profileData|json_encode|e('html_attr') }}">
        <!-- ... -->
    </div>

.. _`从脚本中访问data属性`: https://developer.mozilla.org/en-US/docs/Learn/HTML/Howto/Use_data_attributes
