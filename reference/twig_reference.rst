.. index::
    single: Symfony Twig extensions

.. _symfony2-twig-extensions:

Symfony的Twig扩展
=======================

Twig is the default template engine for Symfony. By itself, it already contains
a lot of built-in functions, filters, tags and tests. You can learn more about
them from the `Twig Reference`_.

The Symfony framework adds quite a few extra :ref:`functions <reference-twig-functions>`,
:ref:`filters <reference-twig-filters>`, :ref:`tags <reference-twig-tags>`
and :ref:`tests <reference-twig-tests>` to seamlessly integrate the
various Symfony components with Twig templates. The following sections
describe these extra features.

.. tip::

    Technically, most of the extensions live in the `Twig Bridge`_. That code
    might give you some ideas when you need to write your own Twig extension
    as described in :doc:`/templating/twig_extension`.

.. note::

    This reference only covers the Twig extensions provided by the Symfony
    framework. You are probably using some other bundles as well, and
    those might come with their own extensions not covered here.

.. tip::

    The `Twig Extensions repository`_ contains some additional Twig extensions
    that do not belong to the Twig core, so you might want to have a look at
    the `Twig Extensions documentation`_.

.. _reference-twig-functions:

Functions
---------

.. _reference-twig-function-render:

render
~~~~~~

.. code-block:: twig

    {{ render(uri, options = []) }}

``uri``
    **type**: ``string`` | ``ControllerReference``
``options`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Makes a request to the given internal URI or controller and returns the result.
It's commonly used to :doc:`embed controllers in templates </templating/embedding_controllers>`.

.. code-block:: twig

    {# if the controller is associated with a route, use the path() or
       url() functions to generate the URI used by render() #}
    {{ render(path('latest_articles', {num: 5})) }}
    {{ render(url('latest_articles', {num: 5})) }}

    {# if you don't want to expose the controller with a public URL, use
       the controller() function to define the controller to be executed #}
    {{ render(controller('App\\Controller\\DefaultController::latestArticles', {num: 5})) }}

The render strategy can be specified in the ``strategy`` key of the options.

.. _reference-twig-function-render-esi:

render_esi
~~~~~~~~~~

.. code-block:: twig

    {{ render_esi(uri, options = []) }}

``uri``
    **type**: ``string`` | ``ControllerReference``
``options`` *(optional)*
    **type**: ``array`` **default**: ``[]``

It's similar to the `render`_ function and defines the same arguments. However,
it generates an ESI tag when :doc:`ESI support </http_cache/esi>` is enabled or
falls back to the behavior of `render`_ otherwise.

.. tip::

    The ``render_esi()`` function is an example of the shortcut functions
    of ``render``. It automatically sets the strategy based on what's given
    in the function name, e.g. ``render_hinclude()`` will use the hinclude.js
    strategy. This works for all ``render_*()`` functions.

controller
~~~~~~~~~~

.. code-block:: twig

    {{ controller(controller, attributes = [], query = []) }}

``controller``
    **type**: ``string``
``attributes`` *(optional)*
    **type**: ``array`` **default**: ``[]``
``query`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Returns an instance of ``ControllerReference`` to be used with functions
like :ref:`render() <reference-twig-function-render>` and
:ref:`render_esi() <reference-twig-function-render-esi>`.

asset
~~~~~

.. code-block:: twig

    {{ asset(path, packageName = null) }}

``path``
    **type**: ``string``
``packageName`` *(optional)*
    **type**: ``string`` | ``null`` **default**: ``null``

Returns a public path to ``path``, which takes into account the base path
set for the package and the URL path. More information in
:ref:`templating-assets`. Symfony provides various cache busting
implementations via the :ref:`reference-framework-assets-version`,
:ref:`reference-assets-version-strategy`, and
:ref:`reference-assets-json-manifest-path` configuration options.

asset_version
~~~~~~~~~~~~~~

.. code-block:: twig

    {{ asset_version(packageName = null) }}

``packageName`` *(optional)*
    **type**: ``string`` | ``null`` **default**: ``null``

Returns the current version of the package, more information in
:ref:`templating-assets`.

form
~~~~

.. code-block:: twig

    {{ form(view, variables = []) }}

``view``
    **type**: ``FormView``
``variables`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Renders the HTML of a complete form, more information in
:ref:`the Twig Form reference <reference-forms-twig-form>`.

form_start
~~~~~~~~~~

.. code-block:: twig

    {{ form_start(view, variables = []) }}

``view``
    **type**: ``FormView``
``variables`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Renders the HTML start tag of a form, more information in
:ref:`the Twig Form reference <reference-forms-twig-start>`.

form_end
~~~~~~~~

.. code-block:: twig

    {{ form_end(view, variables = []) }}

``view``
    **type**: ``FormView``
``variables`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Renders the HTML end tag of a form together with all fields that have not
been rendered yet, more information in
:ref:`the Twig Form reference <reference-forms-twig-end>`.

form_widget
~~~~~~~~~~~

.. code-block:: twig

    {{ form_widget(view, variables = []) }}

``view``
    **type**: ``FormView``
``variables`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Renders a complete form or a specific HTML widget of a field, more information
in :ref:`the Twig Form reference <reference-forms-twig-widget>`.

form_errors
~~~~~~~~~~~

.. code-block:: twig

    {{ form_errors(view) }}

``view``
    **type**: ``FormView``

Renders any errors for the given field or the global errors, more information
in :ref:`the Twig Form reference <reference-forms-twig-errors>`.

form_label
~~~~~~~~~~

.. code-block:: twig

    {{ form_label(view, label = null, variables = []) }}

``view``
    **type**: ``FormView``
``label`` *(optional)*
    **type**: ``string`` **default**: ``null``
``variables`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Renders the label for the given field, more information in
:ref:`the Twig Form reference <reference-forms-twig-label>`.

form_help
~~~~~~~~~~

.. code-block:: twig

    {{ form_help(view) }}

``view``
    **type**: ``FormView``

Renders the help text for the given field.

form_row
~~~~~~~~

.. code-block:: twig

    {{ form_row(view, variables = []) }}

``view``
    **type**: ``FormView``
``variables`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Renders the row (the field's label, errors and widget) of the given field,
more information in :ref:`the Twig Form reference <reference-forms-twig-row>`.

form_rest
~~~~~~~~~

.. code-block:: twig

    {{ form_rest(view, variables = []) }}

``view``
    **type**: ``FormView``
``variables`` *(optional)*
    **type**: ``array`` **default**: ``[]``

Renders all fields that have not yet been rendered, more information in
:ref:`the Twig Form reference <reference-forms-twig-rest>`.

csrf_token
~~~~~~~~~~

.. code-block:: twig

    {{ csrf_token(intention) }}

``intention``
    **type**: ``string``

Renders a CSRF token. Use this function if you want CSRF protection without
creating a form.

is_granted
~~~~~~~~~~

.. code-block:: twig

    {{ is_granted(role, object = null, field = null) }}

``role``
    **type**: ``string``, ``string[]``
``object`` *(optional)*
    **type**: ``object``
``field`` *(optional)*
    **type**: ``string``

Returns ``true`` if the current user has the given role. If several roles are
passed in an array, ``true`` is returned if the user has at least one of
them.

Optionally, an object can be passed to be used by the voter. More information
can be found in :ref:`security-template`.

logout_path
~~~~~~~~~~~

.. code-block:: twig

    {{ logout_path(key = null) }}

``key`` *(optional)*
    **type**: ``string``

Generates a relative logout URL for the given firewall. If no key is provided,
the URL is generated for the current firewall the user is logged into.

logout_url
~~~~~~~~~~

.. code-block:: twig

    {{ logout_url(key = null) }}

``key`` *(optional)*
    **type**: ``string``

Equal to the `logout_path`_ function, but it'll generate an absolute URL
instead of a relative one.

path
~~~~

.. code-block:: twig

    {{ path(name, parameters = [], relative = false) }}

``name``
    **type**: ``string``
``parameters`` *(optional)*
    **type**: ``array`` **default**: ``[]``
``relative`` *(optional)*
    **type**: ``boolean`` **default**: ``false``

Returns the relative URL (without the scheme and host) for the given route.
If ``relative`` is enabled, it'll create a path relative to the current
path. More information in :ref:`templating-pages`.

.. seealso::

    Read :doc:`/routing` to learn more about the Routing component.

url
~~~

.. code-block:: twig

    {{ url(name, parameters = [], schemeRelative = false) }}

``name``
    **type**: ``string``
``parameters`` *(optional)*
    **type**: ``array`` **default**: ``[]``
``schemeRelative`` *(optional)*
    **type**: ``boolean`` **default**: ``false``

Returns the absolute URL (with scheme and host) for the given route. If
``schemeRelative`` is enabled, it'll create a scheme-relative URL. More
information in :ref:`templating-pages`.

.. seealso::

    Read :doc:`/routing` to learn more about the Routing component.

absolute_url
~~~~~~~~~~~~

.. code-block:: jinja

    {{ absolute_url(path) }}

``path``
    **type**: ``string``

Returns the absolute URL from the passed relative path. For example, assume
you're on the following page in your app:
``http://example.com/products/hover-board``.

.. code-block:: jinja

    {{ absolute_url('/human.txt') }}
    {# http://example.com/human.txt #}

    {{ absolute_url('products_icon.png') }}
    {# http://example.com/products/products_icon.png #}

relative_path
~~~~~~~~~~~~~

.. code-block:: jinja

    {{ relative_path(path) }}

``path``
    **type**: ``string``

Returns the relative path from the passed absolute URL. For example, assume
you're on the following page in your app:
``http://example.com/products/hover-board``.

.. code-block:: jinja

    {{ relative_path('http://example.com/human.txt') }}
    {# ../human.txt #}

    {{ relative_path('http://example.com/products/products_icon.png') }}
    {# products_icon.png #}

expression
~~~~~~~~~~

Creates an :class:`Symfony\\Component\\ExpressionLanguage\\Expression` in
Twig.

.. _reference-twig-filters:

Filters
-------

.. _reference-twig-humanize-filter:

humanize
~~~~~~~~

.. code-block:: twig

    {{ text|humanize }}

``text``
    **type**: ``string``

Makes a technical name human readable (i.e. replaces underscores by spaces
or transforms camelCase text like ``helloWorld`` to ``hello world``
and then capitalizes the string).

trans
~~~~~

.. code-block:: twig

    {{ message|trans(arguments = [], domain = null, locale = null) }}

``message``
    **type**: ``string``
``arguments`` *(optional)*
    **type**: ``array`` **default**: ``[]``
``domain`` *(optional)*
    **type**: ``string`` **default**: ``null``
``locale`` *(optional)*
    **type**: ``string`` **default**: ``null``

Translates the text into the current language. More information in
:ref:`Translation Filters <translation-filters>`.

transchoice
~~~~~~~~~~~

.. code-block:: twig

    {{ message|transchoice(count, arguments = [], domain = null, locale = null) }}

``message``
    **type**: ``string``
``count``
    **type**: ``integer``
``arguments`` *(optional)*
    **type**: ``array`` **default**: ``[]``
``domain`` *(optional)*
    **type**: ``string`` **default**: ``null``
``locale`` *(optional)*
    **type**: ``string`` **default**: ``null``

Translates the text with pluralization support. More information in
:ref:`Translation Filters <translation-filters>`.

yaml_encode
~~~~~~~~~~~

.. code-block:: twig

    {{ input|yaml_encode(inline = 0, dumpObjects = false) }}

``input``
    **type**: ``mixed``
``inline`` *(optional)*
    **type**: ``integer`` **default**: ``0``
``dumpObjects`` *(optional)*
    **type**: ``boolean`` **default**: ``false``

Transforms the input into YAML syntax. See :ref:`components-yaml-dump` for
more information.

yaml_dump
~~~~~~~~~

.. code-block:: twig

    {{ value|yaml_dump(inline = 0, dumpObjects = false) }}

``value``
    **type**: ``mixed``
``inline`` *(optional)*
    **type**: ``integer`` **default**: ``0``
``dumpObjects`` *(optional)*
    **type**: ``boolean`` **default**: ``false``

Does the same as `yaml_encode() <yaml_encode>`_, but includes the type in
the output.

abbr_class
~~~~~~~~~~

.. code-block:: twig

    {{ class|abbr_class }}

``class``
    **type**: ``string``

Generates an ``<abbr>`` element with the short name of a PHP class (the
FQCN will be shown in a tooltip when a user hovers over the element).

abbr_method
~~~~~~~~~~~

.. code-block:: twig

    {{ method|abbr_method }}

``method``
    **type**: ``string``

Generates an ``<abbr>`` element using the ``FQCN::method()`` syntax. If
``method`` is ``Closure``, ``Closure`` will be used instead and if ``method``
doesn't have a class name, it's shown as a function (``method()``).

format_args
~~~~~~~~~~~

.. code-block:: twig

    {{ args|format_args }}

``args``
    **type**: ``array``

Generates a string with the arguments and their types (within ``<em>`` elements).

format_args_as_text
~~~~~~~~~~~~~~~~~~~

.. code-block:: twig

    {{ args|format_args_as_text }}

``args``
    **type**: ``array``

Equal to the `format_args`_ filter, but without using HTML tags.

file_excerpt
~~~~~~~~~~~~

.. code-block:: twig

    {{ file|file_excerpt(line, srcContext = 3) }}

``file``
    **type**: ``string``
``line``
    **type**: ``integer``
``srcContext`` *(optional)*
    **type**: ``integer``

Generates an excerpt of a code file around the given ``line`` number. The
``srcContext`` argument defines the total number of lines to display around the
given line number (use ``-1`` to display the whole file).

format_file
~~~~~~~~~~~

.. code-block:: twig

    {{ file|format_file(line, text = null) }}

``file``
    **type**: ``string``
``line``
    **type**: ``integer``
``text`` *(optional)*
    **type**: ``string`` **default**: ``null``

Generates the file path inside an ``<a>`` element. If the path is inside
the kernel root directory, the kernel root directory path is replaced by
``kernel.project_dir`` (showing the full path in a tooltip on hover).

format_file_from_text
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: twig

    {{ text|format_file_from_text }}

``text``
    **type**: ``string``

Uses `format_file`_ to improve the output of default PHP errors.

file_link
~~~~~~~~~

.. code-block:: twig

    {{ file|file_link(line) }}

``file``
    **type**: ``string``
``line``
    **type**: ``integer``

Generates a link to the provided file and line number using
a preconfigured scheme.

file_relative
~~~~~~~~~~~~~

.. versionadded:: 4.2
    The ``file_relative`` filter was introduced in Symfony 4.2.

.. code-block:: twig

    {{ file|file_relative }}

``file``
    **type**: ``string``

It transforms the given absolute file path into a new file path relative to
project's root directory:

.. code-block:: twig

    {{ '/var/www/blog/templates/admin/index.html.twig'|file_relative }}
    {# if project root dir is '/var/www/blog/', it returns 'templates/admin/index.html.twig' #}

If the given file path is out of the project directory, a ``null`` value
will be returned.

.. _reference-twig-tags:

Tags
----

form_theme
~~~~~~~~~~

.. code-block:: twig

    {% form_theme form resources %}

``form``
    **type**: ``FormView``
``resources``
    **type**: ``array`` | ``string``

Sets the resources to override the form theme for the given form view instance.
You can use ``_self`` as resources to set it to the current resource. More
information in :doc:`/form/form_customization`.

trans
~~~~~

.. code-block:: twig

    {% trans with vars from domain into locale %}{% endtrans %}

``vars`` *(optional)*
    **type**: ``array`` **default**: ``[]``
``domain`` *(optional)*
    **type**: ``string`` **default**: ``string``
``locale`` *(optional)*
    **type**: ``string`` **default**: ``string``

Renders the translation of the content. More information in :ref:`translation-tags`.

transchoice
~~~~~~~~~~~

.. code-block:: twig

    {% transchoice count with vars from domain into locale %}{% endtranschoice %}

``count``
    **type**: ``integer``
``vars`` *(optional)*
    **type**: ``array`` **default**: ``[]``
``domain`` *(optional)*
    **type**: ``string`` **default**: ``null``
``locale`` *(optional)*
    **type**: ``string`` **default**: ``null``

Renders the translation of the content with pluralization support, more
information in :ref:`translation-tags`.

trans_default_domain
~~~~~~~~~~~~~~~~~~~~

.. code-block:: twig

    {% trans_default_domain domain %}

``domain``
    **type**: ``string``

This will set the default domain in the current template.

stopwatch
~~~~~~~~~

.. code-block:: jinja

    {% stopwatch 'name' %}...{% endstopwatch %}

This will time the run time of the code inside it and put that on the timeline
of the WebProfilerBundle.

.. _reference-twig-tests:

Tests
-----

selectedchoice
~~~~~~~~~~~~~~

.. code-block:: twig

    {% if choice is selectedchoice(selectedValue) %}

``choice``
    **type**: ``ChoiceView``
``selectedValue``
    **type**: ``string``

Checks if ``selectedValue`` was checked for the provided choice field. Using
this test is the most effective way.

rootform
~~~~~~~~

.. code-block:: twig

    {% if form is rootform %}
        {# ... #}
    {% endif %}

``form``
    **type**: ``FormView``

Checks if the given ``form`` does not have a parent form view. This is the only
safe way of testing it because checking if the form contains a field called
``parent`` is not reliable.

Global Variables
----------------

.. _reference-twig-global-app:

app
~~~

The ``app`` variable is available everywhere and gives access to many commonly
needed objects and values. It is an instance of
:class:`Symfony\\Bundle\\FrameworkBundle\\Templating\\GlobalVariables`.

The available attributes are:

* ``app.user``, a PHP object representing the current user;
* ``app.request``, a :class:`Symfony\\Component\\HttpFoundation\\Request` object;
* ``app.session``, a :class:`Symfony\\Component\\HttpFoundation\\Session\\Session` object;
* ``app.environment``, a string with the name of the execution environment;
* ``app.debug``, a boolean telling whether the debug mode is enabled in the app;
* ``app.token``, a :class:`Symfony\\Component\\Security\\Core\\Authentication\\Token\\TokenInterface`
  object representing the security token
* ``app.flashes``, returns flash messages from the session

.. _`Twig Reference`: https://twig.symfony.com/doc/2.x/#reference
.. _`Twig Extensions repository`: https://github.com/twigphp/Twig-extensions
.. _`Twig Extensions documentation`: http://twig-extensions.readthedocs.io/en/latest/
.. _`Twig Bridge`: https://github.com/symfony/symfony/tree/master/src/Symfony/Bridge/Twig/Extension
