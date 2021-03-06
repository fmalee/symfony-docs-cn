Symfony核心团队
=================

The **Symfony Core** team is the group of developers that determine the
direction and evolution of the Symfony project. Their votes rule if the
features and patches proposed by the community are approved or rejected.

All the Symfony Core members are long-time contributors with solid technical
expertise and they have demonstrated a strong commitment to drive the project
forward.

This document states the rules that govern the Symfony core team. These rules
are effective upon publication of this document and all Symfony Core members
must adhere to said rules and protocol.

核心组织
-----------------

Symfony Core members are divided into groups. Each member can only belong to one
group at a time. The privileges granted to a group are automatically granted to
all higher priority groups.

The Symfony Core groups, in descending order of priority, are as follows:

1. **项目负责人**

* Elects members in any other group;
* Merges pull requests in all Symfony repositories.

2. **合并团队**

* Merge pull requests for the component or components on which they have been
  granted privileges.

3. **决策团队**

* Decide to merge or reject a pull request.

In addition, there are other groups created to manage specific topics:

**安全团队**

* Manage the whole security process (triaging reported vulnerabilities, fixing
  the reported issues, coordinating the release of security fixes, etc.)

**文档团队**

* Manage the whole `symfony-docs repository`_.

活跃的核心成员
~~~~~~~~~~~~~~~~~~~

.. role:: leader
.. role:: merger
.. role:: decider

* **项目负责人**:

  * **Fabien Potencier** (`fabpot`_).

* **合并团队** (``@symfony/mergers`` on GitHub):

  * **Tobias Schultze** (`Tobion`_) can merge into the Routing_,
    OptionsResolver_ and PropertyAccess_ components;

  * **Nicolas Grekas** (`nicolas-grekas`_) can merge into the Cache_, Debug_,
    Process_, PropertyAccess_, VarDumper_ components, PhpUnitBridge_ and
    the DebugBundle_;

  * **Christophe Coevoet** (`stof`_) can merge into all components, bridges and
    bundles;

  * **Kévin Dunglas** (`dunglas`_) can merge into the PropertyInfo_, the Serializer_
    and the WebLink_ components;

  * **Jakub Zalas** (`jakzal`_) can merge into the DomCrawler_ and Intl_
    components;

  * **Christian Flothmann** (`xabbuh`_) can merge into the Yaml_ component;

  * **Javier Eguiluz** (`javiereguiluz`_) can merge into the WebProfilerBundle_;

  * **Grégoire Pineau** (`lyrixx`_) can merge into the Workflow_ component;

  * **Ryan Weaver** (`weaverryan`_) can merge into the Security_ component and
    the SecurityBundle_;

  * **Robin Chalas** (`chalasr`_) can merge into the Console_ and Security_
    components and the SecurityBundle_;

  * **Maxime Steinhausser** (`ogizanagi`_) can merge into Config_, Console_,
    Form_, Serializer_, DependencyInjection_, and HttpKernel_ components;

  * **Tobias Nyholm** (`Nyholm`_) manages the official and contrib recipes
    repositories;

  * **Samuel Rozé** (`sroze`_) can merge into the Messenger_ component.

* **决策团队** (``@symfony/deciders`` on GitHub):

  * **Jordi Boggiano** (`seldaek`_);
  * **Lukas Kahwe Smith** (`lsmith77`_).

* **安全团队** (``@symfony/security`` on GitHub):

  * **Fabien Potencier** (`fabpot`_);
  * **Michael Cullum** (`michaelcullum`_).

* **文档团队** (``@symfony/team-symfony-docs`` on GitHub):

  * **Fabien Potencier** (`fabpot`_);
  * **Ryan Weaver** (`weaverryan`_);
  * **Christian Flothmann** (`xabbuh`_);
  * **Wouter De Jong** (`wouterj`_);
  * **Jules Pietri** (`HeahDude`_);
  * **Javier Eguiluz** (`javiereguiluz`_).

前核心成员
~~~~~~~~~~~~~~~~~~~

They are no longer part of the core team, but we are very grateful for all their
Symfony contributions:

* **Bernhard Schussek** (`webmozart`_);
* **Abdellatif AitBoudad** (`aitboudad`_);
* **Romain Neutron**.

核心会员申请
~~~~~~~~~~~~~~~~~~~~~~~~~~~

At present, new Symfony Core membership applications are not accepted.

核心成员资格撤销
~~~~~~~~~~~~~~~~~~~~~~~~~~

A Symfony Core membership can be revoked for any of the following reasons:

* Refusal to follow the rules and policies stated in this document;
* Lack of activity for the past six months;
* Willful negligence or intent to harm the Symfony project;
* Upon decision of the **Project Leader**.

Should new Symfony Core memberships be accepted in the future, revoked
members must wait at least 12 months before re-applying.

代码开发规则
----------------------

Symfony project development is based on pull requests proposed by any member
of the Symfony community. Pull request acceptance or rejection is decided based
on the votes cast by the Symfony Core members.

拉请求投票政策
~~~~~~~~~~~~~~~~~~~~~~~~~~

* ``-1`` votes must always be justified by technical and objective reasons;

* ``+1`` votes do not require justification, unless there is at least one
  ``-1`` vote;

* Core members can change their votes as many times as they desire
  during the course of a pull request discussion;

* Core members are not allowed to vote on their own pull requests.

拉取请求合并策略
~~~~~~~~~~~~~~~~~~~~~~~~~~~

A pull request **can be merged** if:

* It is a minor change [1]_;

* Enough time was given for peer reviews (at least 2 days for "regular"
  pull requests, and 4 days for pull requests with "a significant impact");

* At least the component's **Merger** or two other Core members voted ``+1``
  and no Core member voted ``-1``.

拉取请求合并处理
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All code must be committed to the repository through pull requests, except for
minor changes [1]_ which can be committed directly to the repository.

**Mergers** must always use the command-line ``gh`` tool provided by the
**Project Leader** to merge the pull requests.

发布策略
~~~~~~~~~~~~~~

The **Project Leader** is also the release manager for every Symfony version.

Symfony核心规则和协议修正案
------------------------------------------

The rules described in this document may be amended at anytime at the
discretion of the **Project Leader**.

.. [1] Minor changes comprise typos, DocBlock fixes, code standards
       violations, and minor CSS, JavaScript and HTML modifications.

.. _PhpUnitBridge: https://github.com/symfony/phpunit-bridge
.. _BrowserKit: https://github.com/symfony/browser-kit
.. _Cache: https://github.com/symfony/cache
.. _Config: https://github.com/symfony/config
.. _Console: https://github.com/symfony/console
.. _Debug: https://github.com/symfony/debug
.. _DebugBundle: https://github.com/symfony/debug-bundle
.. _DependencyInjection: https://github.com/symfony/dependency-injection
.. _DoctrineBridge: https://github.com/symfony/doctrine-bridge
.. _EventDispatcher: https://github.com/symfony/event-dispatcher
.. _DomCrawler: https://github.com/symfony/dom-crawler
.. _Form: https://github.com/symfony/form
.. _HttpFoundation: https://github.com/symfony/http-foundation
.. _HttpKernel: https://github.com/symfony/http-kernel
.. _Icu: https://github.com/symfony/icu
.. _Intl: https://github.com/symfony/intl
.. _LDAP: https://github.com/symfony/ldap
.. _Locale: https://github.com/symfony/locale
.. _Messenger: https://github.com/symfony/messenger
.. _MonologBridge: https://github.com/symfony/monolog-bridge
.. _OptionsResolver: https://github.com/symfony/options-resolver
.. _Process: https://github.com/symfony/process
.. _PropertyAccess: https://github.com/symfony/property-access
.. _PropertyInfo: https://github.com/symfony/property-info
.. _Routing: https://github.com/symfony/routing
.. _Serializer: https://github.com/symfony/serializer
.. _Translation: https://github.com/symfony/translation
.. _Security: https://github.com/symfony/security
.. _SecurityBundle: https://github.com/symfony/security-bundle
.. _Stopwatch: https://github.com/symfony/stopwatch
.. _TwigBridge: https://github.com/symfony/twig-bridge
.. _Validator: https://github.com/symfony/validator
.. _VarDumper: https://github.com/symfony/var-dumper
.. _Workflow: https://github.com/symfony/workflow
.. _Yaml: https://github.com/symfony/yaml
.. _WebProfilerBundle: https://github.com/symfony/web-profiler-bundle
.. _WebLink: https://github.com/symfony/web-link
.. _`symfony-docs repository`: https://github.com/symfony/symfony-docs
.. _`fabpot`: https://github.com/fabpot/
.. _`webmozart`: https://github.com/webmozart/
.. _`Tobion`: https://github.com/Tobion/
.. _`romainneutron`: https://github.com/romainneutron/
.. _`nicolas-grekas`: https://github.com/nicolas-grekas/
.. _`stof`: https://github.com/stof/
.. _`dunglas`: https://github.com/dunglas/
.. _`jakzal`: https://github.com/jakzal/
.. _`Seldaek`: https://github.com/Seldaek/
.. _`lsmith77`: https://github.com/lsmith77/
.. _`weaverryan`: https://github.com/weaverryan/
.. _`aitboudad`: https://github.com/aitboudad/
.. _`xabbuh`: https://github.com/xabbuh/
.. _`javiereguiluz`: https://github.com/javiereguiluz/
.. _`lyrixx`: https://github.com/lyrixx/
.. _`chalasr`: https://github.com/chalasr/
.. _`ogizanagi`: https://github.com/ogizanagi/
.. _`Nyholm`: https://github.com/Nyholm
.. _`sroze`: https://github.com/sroze
.. _`michaelcullum`: https://github.com/michaelcullum
.. _`wouterj`: https://github.com/wouterj
.. _`HeahDude`: https://github.com/HeahDude
