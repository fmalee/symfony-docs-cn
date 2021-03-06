提交补丁
==================

Patches are the best way to provide a bug fix or to propose enhancements to
Symfony.

第1步: 配置环境
------------------------------

安装软件
~~~~~~~~~~~~~~~~~~~~~~~~~~

Before working on Symfony, setup a friendly environment with the following
software:

* Git;
* PHP version 5.5.9 or above.

配置Git
~~~~~~~~~~~~~

Set up your user information with your real name and a working email address:

.. code-block:: terminal

    $ git config --global user.name "Your Name"
    $ git config --global user.email you@example.com

.. tip::

    If you are new to Git, you are highly recommended to read the excellent and
    free `ProGit`_ book.

.. tip::

    If your IDE creates configuration files inside the project's directory,
    you can use global ``.gitignore`` file (for all projects) or
    ``.git/info/exclude`` file (per project) to ignore them. See
    `GitHub's documentation`_.

.. tip::

    Windows users: when installing Git, the installer will ask what to do with
    line endings, and suggests replacing all LF with CRLF. This is the wrong
    setting if you wish to contribute to Symfony! Selecting the as-is method is
    your best choice, as Git will convert your line feeds to the ones in the
    repository. If you have already installed Git, you can check the value of
    this setting by typing:

    .. code-block:: terminal

        $ git config core.autocrlf

    This will return either "false", "input" or "true"; "true" and "false" being
    the wrong values. Change it to "input" by typing:

    .. code-block:: terminal

        $ git config --global core.autocrlf input

    Replace --global by --local if you want to set it only for the active
    repository

获取Symfony源代码
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Get the Symfony source code:

* Create a `GitHub`_ account and sign in;

* Fork the `Symfony repository`_ (click on the "Fork" button);

* After the "forking action" has completed, clone your fork locally
  (this will create a ``symfony`` directory):

.. code-block:: terminal

      $ git clone git@github.com:USERNAME/symfony.git

* Add the upstream repository as a remote:

.. code-block:: terminal

      $ cd symfony
      $ git remote add upstream git://github.com/symfony/symfony.git

检查当前的测试
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now that Symfony is installed, check that all unit tests pass for your
environment as explained in the dedicated :doc:`document <tests>`.

第2步: 处理补丁
--------------------------

许可证
~~~~~~~~~~~

Before you start, you must know that all the patches you are going to submit
must be released under the *MIT license*, unless explicitly specified in your
commits.

选择正确的分支
~~~~~~~~~~~~~~~~~~~~~~~

Before working on a patch, you must determine on which branch you need to
work:

* ``3.4``, if you are fixing a bug for an existing feature or want to make a
  change that falls into the :doc:`list of acceptable changes in patch versions
  </contributing/code/maintenance>` (you may have to choose a higher branch if
  the feature you are fixing was introduced in a later version);

 * ``master``, if you are adding a new feature.

.. note::

    All bug fixes merged into maintenance branches are also merged into more
    recent branches on a regular basis. For instance, if you submit a patch
    for the ``3.4`` branch, the patch will also be applied by the core team on
    the ``master`` branch.

创建主题分支
~~~~~~~~~~~~~~~~~~~~~

Each time you want to work on a patch for a bug or on an enhancement, create a
topic branch:

.. code-block:: terminal

    $ git checkout -b BRANCH_NAME master

Or, if you want to provide a bugfix for the ``3.4`` branch, first track the remote
``3.4`` branch locally:

.. code-block:: terminal

    $ git checkout -t origin/3.4

Then create a new branch off the ``3.4`` branch to work on the bugfix:

.. code-block:: terminal

    $ git checkout -b BRANCH_NAME 3.4

.. tip::

    Use a descriptive name for your branch (``ticket_XXX`` where ``XXX`` is the
    ticket number is a good convention for bug fixes).

The above checkout commands automatically switch the code to the newly created
branch (check the branch you are working on with ``git branch``).

在现有项目中使用你的分支
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you want to test your code in an existing project that uses ``symfony/symfony``
or Symfony components, you can use the ``link`` utility provided in the Git repository
you cloned previously.
This tool scans the ``vendor/`` directory of your project, finds Symfony packages it
uses, and replaces them by symbolic links to the ones in the Git repository.

.. code-block:: terminal

    $ php link /path/to/your/project

Before running the ``link`` command, be sure that the dependencies of the project you
want to debug are installed by running ``composer install`` inside it.

处理你的补丁
~~~~~~~~~~~~~~~~~~

Work on the code as much as you want and commit as much as you want; but keep
in mind the following:

* Read about the Symfony :doc:`conventions <conventions>` and follow the
  coding :doc:`standards <standards>` (use ``git diff --check`` to check for
  trailing spaces -- also read the tip below);

* Add unit tests to prove that the bug is fixed or that the new feature
  actually works;

* Try hard to not break backward compatibility (if you must do so, try to
  provide a compatibility layer to support the old way) -- patches that break
  backward compatibility have less chance to be merged;

* Do atomic and logically separate commits (use the power of ``git rebase`` to
  have a clean and logical history);

* Never fix coding standards in some existing code as it makes the code review
  more difficult;

* Write good commit messages (see the tip below).

.. tip::

    When submitting pull requests, `fabbot`_ checks your code
    for common typos and verifies that you are using the PHP coding standards
    as defined in `PSR-1`_ and `PSR-2`_.

    A status is posted below the pull request description with a summary
    of any problems it detects or any Travis CI build failures.

.. tip::

    A good commit message is composed of a summary (the first line),
    optionally followed by a blank line and a more detailed description. The
    summary should start with the Component you are working on in square
    brackets (``[DependencyInjection]``, ``[FrameworkBundle]``, ...). Use a
    verb (``fixed ...``, ``added ...``, ...) to start the summary and don't
    add a period at the end.

准备提交补丁
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When your patch is not about a bug fix (when you add a new feature or change
an existing one for instance), it must also include the following:

* An explanation of the changes in the relevant ``CHANGELOG`` file(s) (the
  ``[BC BREAK]`` or the ``[DEPRECATION]`` prefix must be used when relevant);

* An explanation on how to upgrade an existing application in the relevant
  ``UPGRADE`` file(s) if the changes break backward compatibility or if you
  deprecate something that will ultimately break backward compatibility.

第3步: 提交分支
-------------------------

Whenever you feel that your patch is ready for submission, follow the
following steps.

变基你的补丁
~~~~~~~~~~~~~~~~~

Before submitting your patch, update your branch (needed if it takes you a
while to finish your changes):

.. code-block:: terminal

    $ git checkout master
    $ git fetch upstream
    $ git merge upstream/master
    $ git checkout BRANCH_NAME
    $ git rebase master

.. tip::

    Replace ``master`` with the branch you selected previously (e.g. ``3.4``)
    if you are working on a bugfix

When doing the ``rebase`` command, you might have to fix merge conflicts.
``git status`` will show you the *unmerged* files. Resolve all the conflicts,
then continue the rebase:

.. code-block:: terminal

    $ git add ... # add resolved files
    $ git rebase --continue

Check that all tests still pass and push your branch remotely:

.. code-block:: terminal

    $ git push --force origin BRANCH_NAME

.. _contributing-code-pull-request:

创建拉取请求
~~~~~~~~~~~~~~~~~~~

You can now make a pull request on the ``symfony/symfony`` GitHub repository.

.. tip::

    Take care to point your pull request towards ``symfony:3.4`` if you want
    the core team to pull a bugfix based on the ``3.4`` branch.

To ease the core team work, always include the modified components in your
pull request message, like in:

.. code-block:: text

    [Yaml] fixed something
    [Form] [Validator] [FrameworkBundle] added something

The default pull request description contains a table which you must fill in
with the appropriate answers. This ensures that contributions may be reviewed
without needless feedback loops and that your contributions can be included into
Symfony as quickly as possible.

Some answers to the questions trigger some more requirements:

* If you answer yes to "Bug fix?", check if the bug is already listed in the
  Symfony issues and reference it/them in "Fixed tickets";

* If you answer yes to "New feature?", you must submit a pull request to the
  documentation and reference it under the "Doc PR" section;

* If you answer yes to "BC breaks?", the patch must contain updates to the
  relevant ``CHANGELOG`` and ``UPGRADE`` files;

* If you answer yes to "Deprecations?", the patch must contain updates to the
  relevant ``CHANGELOG`` and ``UPGRADE`` files;

* If you answer no to "Tests pass", you must add an item to a todo-list with
  the actions that must be done to fix the tests;

* If the "license" is not MIT, just don't submit the pull request as it won't
  be accepted anyway.

If some of the previous requirements are not met, create a todo-list and add
relevant items:

.. code-block:: text

    - [ ] fix the tests as they have not been updated yet
    - [ ] submit changes to the documentation
    - [ ] document the BC breaks

If the code is not finished yet because you don't have time to finish it or
because you want early feedback on your work, add an item to todo-list:

.. code-block:: text

    - [ ] finish the code
    - [ ] gather feedback for my changes

As long as you have items in the todo-list, please prefix the pull request
title with "[WIP]".

In the pull request description, give as much details as possible about your
changes (don't hesitate to give code examples to illustrate your points). If
your pull request is about adding a new feature or modifying an existing one,
explain the rationale for the changes. The pull request description helps the
code review and it serves as a reference when the code is merged (the pull
request description and all its associated comments are part of the merge
commit message).

In addition to this "code" pull request, you must also send a pull request to
the `documentation repository`_ to update the documentation when appropriate.

修订你的补丁
~~~~~~~~~~~~~~~~~

Based on the feedback on the pull request, you might need to rework your
patch. Before re-submitting the patch, rebase with ``upstream/master`` or
``upstream/3.4``, don't merge; and force the push to the origin:

.. code-block:: terminal

    $ git rebase -f upstream/master
    $ git push --force origin BRANCH_NAME

.. note::

    When doing a ``push --force``, always specify the branch name explicitly
    to avoid messing other branches in the repo (``--force`` tells Git that
    you really want to mess with things so do it carefully).

Moderators earlier asked you to "squash" your commits. This means you will
convert many commits to one commit. This is no longer necessary today, because
Symfony project uses a proprietary tool which automatically squashes all commits
before merging.

.. _ProGit: https://git-scm.com/book
.. _GitHub: https://github.com/join
.. _`GitHub's Documentation`: https://help.github.com/articles/ignoring-files
.. _Symfony repository: https://github.com/symfony/symfony
.. _dev mailing-list: https://groups.google.com/group/symfony-devs
.. _travis-ci.org: https://travis-ci.org/
.. _`travis-ci.org status icon`: https://about.travis-ci.com/docs/user/status-images/
.. _`travis-ci.org Getting Started Guide`: https://about.travis-ci.com/docs/user/getting-started/
.. _`documentation repository`: https://github.com/symfony/symfony-docs
.. _`fabbot`: https://fabbot.io
.. _`PSR-1`: https://www.php-fig.org/psr/psr-1/
.. _`PSR-2`: https://www.php-fig.org/psr/psr-2/
