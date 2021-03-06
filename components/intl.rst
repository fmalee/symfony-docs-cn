.. index::
   single: Intl
   single: Components; Intl

The Intl Component
==================

    A PHP replacement layer for the C `intl extension`_ that also provides
    access to the localization data of the `ICU library`_.

.. caution::

    The replacement layer is limited to the locale "en". If you want to use
    other locales, you should `install the intl extension`_ instead.

.. seealso::

    This article explains how to use the Intl features as an independent component
    in any PHP application. Read the :doc:`/translation` article to learn about
    how to internationalize and manage the user locale in Symfony applications.

Installation
------------

.. code-block:: terminal

    $ composer require symfony/intl

Alternatively, you can clone the `<https://github.com/symfony/intl>`_ repository.

.. include:: /components/require_autoload.rst.inc

If you install the component via Composer, the following classes and functions
of the intl extension will be automatically provided if the intl extension is
not loaded:

* :phpclass:`Collator`
* :phpclass:`IntlDateFormatter`
* :phpclass:`Locale`
* :phpclass:`NumberFormatter`
* :phpfunction:`intl_error_name`
* :phpfunction:`intl_is_failure`
* :phpfunction:`intl_get_error_code`
* :phpfunction:`intl_get_error_message`

When the intl extension is not available, the following classes are used to
replace the intl classes:

* :class:`Symfony\\Component\\Intl\\Collator\\Collator`
* :class:`Symfony\\Component\\Intl\\DateFormatter\\IntlDateFormatter`
* :class:`Symfony\\Component\\Intl\\Locale\\Locale`
* :class:`Symfony\\Component\\Intl\\NumberFormatter\\NumberFormatter`
* :class:`Symfony\\Component\\Intl\\Globals\\IntlGlobals`

Composer automatically exposes these classes in the global namespace.

Writing and Reading Resource Bundles
------------------------------------

The :phpclass:`ResourceBundle` class is not currently supported by this component.
Instead, it includes a set of readers and writers for reading and writing
arrays (or array-like objects) from/to resource bundle files. The following
classes are supported:

* `TextBundleWriter`_
* `PhpBundleWriter`_
* `BinaryBundleReader`_
* `PhpBundleReader`_
* `BufferedBundleReader`_
* `StructuredBundleReader`_

Continue reading if you are interested in how to use these classes. Otherwise
skip this section and jump to `Accessing ICU Data`_.

TextBundleWriter
~~~~~~~~~~~~~~~~

The :class:`Symfony\\Component\\Intl\\ResourceBundle\\Writer\\TextBundleWriter`
writes an array or an array-like object to a plain-text resource bundle. The
resulting .txt file can be converted to a binary .res file with the
:class:`Symfony\\Component\\Intl\\ResourceBundle\\Compiler\\BundleCompiler`
class::

    use Symfony\Component\Intl\ResourceBundle\Writer\TextBundleWriter;
    use Symfony\Component\Intl\ResourceBundle\Compiler\BundleCompiler;

    $writer = new TextBundleWriter();
    $writer->write('/path/to/bundle', 'en', array(
        'Data' => array(
            'entry1',
            'entry2',
            // ...
        ),
    ));

    $compiler = new BundleCompiler();
    $compiler->compile('/path/to/bundle', '/path/to/binary/bundle');

The command "genrb" must be available for the
:class:`Symfony\\Component\\Intl\\ResourceBundle\\Compiler\\BundleCompiler` to
work. If the command is located in a non-standard location, you can pass its
path to the
:class:`Symfony\\Component\\Intl\\ResourceBundle\\Compiler\\BundleCompiler`
constructor.

PhpBundleWriter
~~~~~~~~~~~~~~~

The :class:`Symfony\\Component\\Intl\\ResourceBundle\\Writer\\PhpBundleWriter`
writes an array or an array-like object to a .php resource bundle::

    use Symfony\Component\Intl\ResourceBundle\Writer\PhpBundleWriter;

    $writer = new PhpBundleWriter();
    $writer->write('/path/to/bundle', 'en', array(
        'Data' => array(
            'entry1',
            'entry2',
            // ...
        ),
    ));

BinaryBundleReader
~~~~~~~~~~~~~~~~~~

The :class:`Symfony\\Component\\Intl\\ResourceBundle\\Reader\\BinaryBundleReader`
reads binary resource bundle files and returns an array or an array-like object.
This class currently only works with the `intl extension`_ installed::

    use Symfony\Component\Intl\ResourceBundle\Reader\BinaryBundleReader;

    $reader = new BinaryBundleReader();
    $data = $reader->read('/path/to/bundle', 'en');

    var_dump($data['Data']['entry1']);

PhpBundleReader
~~~~~~~~~~~~~~~

The :class:`Symfony\\Component\\Intl\\ResourceBundle\\Reader\\PhpBundleReader`
reads resource bundles from .php files and returns an array or an array-like
object::

    use Symfony\Component\Intl\ResourceBundle\Reader\PhpBundleReader;

    $reader = new PhpBundleReader();
    $data = $reader->read('/path/to/bundle', 'en');

    var_dump($data['Data']['entry1']);

BufferedBundleReader
~~~~~~~~~~~~~~~~~~~~

The :class:`Symfony\\Component\\Intl\\ResourceBundle\\Reader\\BufferedBundleReader`
wraps another reader, but keeps the last N reads in a buffer, where N is a
buffer size passed to the constructor::

    use Symfony\Component\Intl\ResourceBundle\Reader\BinaryBundleReader;
    use Symfony\Component\Intl\ResourceBundle\Reader\BufferedBundleReader;

    $reader = new BufferedBundleReader(new BinaryBundleReader(), 10);

    // actually reads the file
    $data = $reader->read('/path/to/bundle', 'en');

    // returns data from the buffer
    $data = $reader->read('/path/to/bundle', 'en');

    // actually reads the file
    $data = $reader->read('/path/to/bundle', 'fr');

StructuredBundleReader
~~~~~~~~~~~~~~~~~~~~~~

The :class:`Symfony\\Component\\Intl\\ResourceBundle\\Reader\\StructuredBundleReader`
wraps another reader and offers a
:method:`Symfony\\Component\\Intl\\ResourceBundle\\Reader\\StructuredBundleReaderInterface::readEntry`
method for reading an entry of the resource bundle without having to worry
whether array keys are set or not. If a path cannot be resolved, ``null`` is
returned::

    use Symfony\Component\Intl\ResourceBundle\Reader\BinaryBundleReader;
    use Symfony\Component\Intl\ResourceBundle\Reader\StructuredBundleReader;

    $reader = new StructuredBundleReader(new BinaryBundleReader());

    $data = $reader->read('/path/to/bundle', 'en');

    // produces an error if the key "Data" does not exist
    var_dump($data['Data']['entry1']);

    // returns null if the key "Data" does not exist
    var_dump($reader->readEntry('/path/to/bundle', 'en', array('Data', 'entry1')));

Additionally, the
:method:`Symfony\\Component\\Intl\\ResourceBundle\\Reader\\StructuredBundleReaderInterface::readEntry`
method resolves fallback locales. For example, the fallback locale of "en_GB" is
"en". For single-valued entries (strings, numbers etc.), the entry will be read
from the fallback locale if it cannot be found in the more specific locale. For
multi-valued entries (arrays), the values of the more specific and the fallback
locale will be merged. In order to suppress this behavior, the last parameter
``$fallback`` can be set to ``false``::

    var_dump($reader->readEntry(
        '/path/to/bundle',
        'en',
        array('Data', 'entry1'),
        false
    ));

Accessing ICU Data
------------------

The ICU data is located in several "resource bundles". You can access a PHP
wrapper of these bundles through the static
:class:`Symfony\\Component\\Intl\\Intl` class. At the moment, the following
data is supported:

* `Language and Script Names`_
* `Country Names`_
* `Locales`_
* `Currencies`_

Language and Script Names
~~~~~~~~~~~~~~~~~~~~~~~~~

The translations of language and script names can be found in the language
bundle::

    use Symfony\Component\Intl\Intl;

    \Locale::setDefault('en');

    $languages = Intl::getLanguageBundle()->getLanguageNames();
    // => array('ab' => 'Abkhazian', ...)

    $language = Intl::getLanguageBundle()->getLanguageName('de');
    // => 'German'

    $language = Intl::getLanguageBundle()->getLanguageName('de', 'AT');
    // => 'Austrian German'

    $scripts = Intl::getLanguageBundle()->getScriptNames();
    // => array('Arab' => 'Arabic', ...)

    $script = Intl::getLanguageBundle()->getScriptName('Hans');
    // => 'Simplified'

All methods accept the translation locale as the last, optional parameter,
which defaults to the current default locale::

    $languages = Intl::getLanguageBundle()->getLanguageNames('de');
    // => array('ab' => 'Abchasisch', ...)

Country Names
~~~~~~~~~~~~~

The translations of country names can be found in the region bundle::

    use Symfony\Component\Intl\Intl;

    \Locale::setDefault('en');

    $countries = Intl::getRegionBundle()->getCountryNames();
    // => array('AF' => 'Afghanistan', ...)

    $country = Intl::getRegionBundle()->getCountryName('GB');
    // => 'United Kingdom'

All methods accept the translation locale as the last, optional parameter,
which defaults to the current default locale::

    $countries = Intl::getRegionBundle()->getCountryNames('de');
    // => array('AF' => 'Afghanistan', ...)

Locales
~~~~~~~

The translations of locale names can be found in the locale bundle::

    use Symfony\Component\Intl\Intl;

    \Locale::setDefault('en');

    $locales = Intl::getLocaleBundle()->getLocaleNames();
    // => array('af' => 'Afrikaans', ...)

    $locale = Intl::getLocaleBundle()->getLocaleName('zh_Hans_MO');
    // => 'Chinese (Simplified, Macau SAR China)'

All methods accept the translation locale as the last, optional parameter,
which defaults to the current default locale::

    $locales = Intl::getLocaleBundle()->getLocaleNames('de');
    // => array('af' => 'Afrikaans', ...)

Currencies
~~~~~~~~~~

The translations of currency names and other currency-related information can
be found in the currency bundle::

    use Symfony\Component\Intl\Intl;

    \Locale::setDefault('en');

    $currencies = Intl::getCurrencyBundle()->getCurrencyNames();
    // => array('AFN' => 'Afghan Afghani', ...)

    $currency = Intl::getCurrencyBundle()->getCurrencyName('INR');
    // => 'Indian Rupee'

    $symbol = Intl::getCurrencyBundle()->getCurrencySymbol('INR');
    // => '???'

    $fractionDigits = Intl::getCurrencyBundle()->getFractionDigits('INR');
    // => 2

    $roundingIncrement = Intl::getCurrencyBundle()->getRoundingIncrement('INR');
    // => 0

All methods (except for
:method:`Symfony\\Component\\Intl\\ResourceBundle\\CurrencyBundleInterface::getFractionDigits`
and
:method:`Symfony\\Component\\Intl\\ResourceBundle\\CurrencyBundleInterface::getRoundingIncrement`)
accept the translation locale as the last, optional parameter, which defaults
to the current default locale::

    $currencies = Intl::getCurrencyBundle()->getCurrencyNames('de');
    // => array('AFN' => 'Afghanische Afghani', ...)

That's all you need to know for now. Have fun coding!

Learn more
----------

.. toctree::
    :maxdepth: 1
    :glob:

    /reference/forms/types/country
    /reference/forms/types/currency
    /reference/forms/types/language
    /reference/forms/types/locale

.. _Packagist: https://packagist.org/packages/symfony/intl
.. _Icu component: https://packagist.org/packages/symfony/icu
.. _intl extension: https://php.net/manual/en/book.intl.php
.. _install the intl extension: https://php.net/manual/en/intl.setup.php
.. _ICU library: http://site.icu-project.org/
