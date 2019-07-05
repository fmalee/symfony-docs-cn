.. index::
   single: Migration

迁移旧应用到Symfony
============================================

如果你现有的应用不是使用Symfony构建的，那么你可能希望移动该应用的某些部分，而无需完全重写现有逻辑。
对于这些情况，有一种称为 `绞杀者应用`_ 的模式 。
此模式的基本思想是创建一个新应用，然后逐步接管旧应用的功能。
这种迁移方案可以通过Symfony以各种方式实现，并且与重写相比具有一些优势，
例如能够在旧应用中引入新功能，并通过避免新应用的“大爆炸”释放来降低风险。

.. admonition:: Screencast
    :class: screencast

    在会议期间有时会讨论从旧应用迁移到Symfony的主题。
    例如，`Modernizing with Symfony`_ 的谈话重申了本页的一些要点。

先决条件
-------------

在开始将Symfony引入旧应用之前，必须确保旧应用和环境满足某些要求。
在开始迁移过程之前做出决策并准备好环境对于其成功至关重要。

.. note::

    以下步骤不要求你准备好新的Symfony应用，事实上，在旧应用中预先引入这些更改可能更安全。

选择目标Symfony版本
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

最重要的是，这意味着你必须决定要迁移到哪个版本，无论是当前稳定版本还是长期支持版本（LTS）。
主要的区别是，为了使用支持的版本，你需要多久升级一次。
在迁移环境中，其他因素（如支持的PHP版本或对所使用的库/包的支持）也可能产生强烈影响。
使用最新的稳定版本可能会为你提供更多功能，
但它还需要你更频繁地更新以确保你将获得BUG修复和安全补丁的支持，并且你必须更快地修复弃用以便能够升级。

.. tip::

    升级到Symfony时，你可能也想使用 :doc:`Flex </setup/flex>`。
    请记住，它主要侧重于根据有关目录结构的最佳实践来引导一个新的Symfony应用。
    当你处理旧应用的约束时，你可能无法遵循这些约束，从而使Flex变得不那么有用。

首先，你的环境需要能够同时支持两个应用的最低要求。
换句话说，当你希望使用的Symfony版本需要PHP 7.1，而你现有的应用尚不支持此PHP版本时，你可能必须升级你的旧项目。
你可以找到运行Symfony的 :doc:`需求 </reference/requirements>`，并将它们与当前应用的环境进行比较，以确保你能够在同一系统上运行这两个应用。
拥有一个尽可能靠近生产环境的测试系统，你可以在现有的Symfony项目旁边安装一个新的Symfony项目，并检查它能否正常工作，这样将为你提供更可靠的结果。

.. tip::

    如果你当前的项目是在较旧的PHP版本（如PHP 5.x）上运行，则升级到最新版本可以在不改变代码的情况下提高性能。

设置Composer
~~~~~~~~~~~~~~~~~~~

你需要注意的另一点是两个应用中的依赖之间的冲突。
如果你现有的应用已经使用Symfony中常用的Symfony组件或库（如Doctrine ORM，Swiftmailer或Twig），那么这一点尤为重要。
确保兼容性的一种好方法是对项目的依赖使用相同的 ``composer.json``。

一旦你引入了用于管理项目依赖的composer，你就可以使用其自动加载器来确保你不会因为现有框架的自定义自动加载而遇到任何冲突。
这通常需要为你的 ``composer.json`` 添加一个 `自动加载`_
节点，并根据你的应用对其进行配置，然后使用以下内容替换你的自定义逻辑::

    require __DIR__.'/vendor/autoload.php';

从旧应用中移除全局状态
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在较旧的PHP应用中，依赖全局状态甚至在运行时期间改变它是很常见的。
这可能会对新引入的Symfony应用产生副作用。
换句话说，应该重构旧应用中依赖于全局变量的代码，以允许两个系统同时工作。
由于现在依赖于全局状态被认为是一种反模式，因此你甚至可能希望在进行任何集成之前就开始研究它。

设置环境
~~~~~~~~~~~~~~~~~~~~~~~~~~

根据所使用的库、项目所基于的原始框架以及最重要的是项目的年龄，你可能需要采取其他步骤。
因为在过去的几年中，PHP本身经历了许多你的代码可能还没有赶上的改进。
只要你现有的代码和新的Symfony项目都可以在同一个系统上并行运行，那么你就处于良好的状态。
所有这些步骤都不需要你刚刚引入的Symfony，并且已经为实现现有代码的现代化提供了一些机会。

建立一个回归的安全网
-----------------------------------------

在你可以安全地更改现有代码之前，必须确保不会中断任何内容。
选择迁移的一个原因是确保应用处于可以始终运行的状态。
确保工作状态的最佳方法是建立自动化测试。

旧应用要么根本没有测试套件，要么代码覆盖率很低，这是很常见的。
为该代码引入单元测试可能不划算，因为旧代码可能被Symfony组件的功能所取代，或者可能会适应新的应用。
此外，遗留代码往往很难编写测试，从而使该过程变得缓慢和麻烦。

与其提供确保每个类按预期工作的低级测试，不如编写高级测试来确保面向用户的任何东西至少在表面上工作，这是有意义的。
这些类型的测试通常被称为端到端测试，因为它们涵盖了整个应用，从用户在浏览器中看到的内容到正在运行的代码以及连接的服务（如数据库）。
要实现自动化，你必须确保你的系统的测试实例尽可能容易地运行，并确保外部系统不会更改你的生产环境，
例如，提供一个包含来自生产系统的（匿名）数据的独立测试数据库，或者能够为你的测试环境设置一个包含基本数据集的新架构。
由于这些测试不太依赖于隔离可测试代码，而是着眼于相互连接的系统，因此在进行迁移时编写它们通常更容易、更高效。
然后，你可以限制在新应用中必须更改或替换的代码部分上编写较低级别的测试，以确保从一开始就可以对其进行测试。

有一些针对你可以使用的端到端测试的工具，例如
`Symfony Panther`_，或者你可以在初始设置完成后立即在新的Symfony应用中编写 :doc:`功能测试 </testing>`。
例如，你可以添加所谓的烟雾测试，它只通过检查返回的HTTP状态代码或从页面查找文本片段来确保某个路径是可访问的。

将Symfony引入旧应用
-----------------------------------------------

以下说明仅概述了设置Symfony应用的常见任务，该应用在路由不可访问时回退到旧版应用。
你的里程可能会有所不同，你可能需要调整其中的一部分，甚至提供额外的配置或改装，以使其适用于你的应用。
本指南不应该是全面的，而是旨在成为一个起点。

.. tip::

    如果你遇到困难或需要其他帮助，只要你需要针对你所面临的问题获得具体反馈，你就可以联系
    :doc:`Symfony社区 </contributing/community/index>`。

在一个前端控制器中启动Symfony
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

纵观典型的PHP应用如何引导时，有两种主要方法。
如今，大多数框架都提供了一个所谓的前端控制器，让它充当入口点。
无论你要执行应用中的哪个URL路径，每个请求都将被发送到此前端控制器，然后前端控制器将确定要加载应用的哪些部分，例如要调用的控制器和动作。
这也是Symfony所采用的方法，即使用 ``public/index.php`` 作为前端控制器。
但是是在较旧的应用中，不同的路径由不同的PHP文件处理是很常见的。

在任何情况下，你都必须创建一个 ``public/index.php`` 来启动symfony应用，方法是从
``FrameworkBundle`` 指令复制该文件，或者使用Flex并请求（requiring）FrameworkBundle。
你可能还必须更新Web服务器（例如Apache或nginx）以始终使用此前端控制器。
你可以查看 :doc:`Web服务器的配置 </setup/web_server_configuration>`
以获取其大概配置的示例。例如，使用Apache时，你可以使用重写规则来确保忽略PHP文件，而只调用
``index.php``：

.. code-block:: apache

    RewriteEngine On

    RewriteCond %{REQUEST_URI}::$1 ^(/.+)/(.*)::\2$
    RewriteRule ^(.*) - [E=BASE:%1]

    RewriteCond %{ENV:REDIRECT_STATUS} ^$
    RewriteRule ^index\.php(?:/(.*)|$) %{ENV:BASE}/$1 [R=301,L]

    RewriteRule ^index\.php - [L]

    RewriteCond %{REQUEST_FILENAME} -f
    RewriteCond %{REQUEST_FILENAME} !^.+\.php$
    RewriteRule ^ - [L]

    RewriteRule ^ %{ENV:BASE}/index.php [L]

此更改将确保从现在开始Symfony应用是第一个处理所有请求的应用。
下一步是确保旧应用启动，并在Symfony无法处理以前由旧应用管理的路径时接管该路径。
从这一点来看，许多策略都是可行的，每个项目都需要其独特的迁移方法。
本指南显示了常用方法的两个示例，你可以将它们用作自己方法的基础：

* `带有Legacy Bridge的前端控制器`_，它使旧应用不受影响，并允许将其分阶段迁移到Symfony应用。
* `传统路由加载器`_，旧应用分阶段集成到Symfony中，具有完全集成的最终结果。

带有Legacy Bridge的前端控制器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

一旦你有一个正在运行的Symfony应用来接管所有请求，就可以使用进入旧系统的一些逻辑来扩展原始前端控制器脚本，以完成对旧应用的回放。
该文件可能如下所示::

    // public/index.php
    use App\Kernel;
    use App\LegacyBridge;
    use Symfony\Component\Debug\Debug;
    use Symfony\Component\HttpFoundation\Request;

    require dirname(__DIR__).'/config/bootstrap.php';

    /*
     * 内核将始终全局可用，以允许你从旧应用访问他，同时能通过它访问服务容器。
     * 这将允许在旧应用中引入新功能。
     */
    global $kernel;

    if ($_SERVER['APP_DEBUG']) {
        umask(0000);

        Debug::enable();
    }

    if ($trustedProxies = $_SERVER['TRUSTED_PROXIES'] ?? $_ENV['TRUSTED_PROXIES'] ?? false) {
        Request::setTrustedProxies(
          explode(',', $trustedProxies),
          Request::HEADER_X_FORWARDED_ALL ^ Request::HEADER_X_FORWARDED_HOST
        );
    }

    if ($trustedHosts = $_SERVER['TRUSTED_HOSTS'] ?? $_ENV['TRUSTED_HOSTS'] ?? false) {
        Request::setTrustedHosts([$trustedHosts]);
    }

    $kernel = new Kernel($_SERVER['APP_ENV'], (bool) $_SERVER['APP_DEBUG'], dirname(__DIR__));
    $request = Request::createFromGlobals();
    $response = $kernel->handle($request);

    /*
     * LegacyBridge将负责确定是启动旧应用还是将Symfony的响应发送回客户端。
     */
    $scriptFile = LegacyBridge::prepareLegacyScript($request, $response, __DIR__);
    if ($scriptFile !== null) {
        require $scriptFile;
    } else {
        $response->send();
    }
    $kernel->terminate($request, $response);

它与原始文件有2个主要偏差：

Line 15
  首先，``$kernel`` 是全局可用的。这允许你在旧应用中使用Symfony的功能，并允许访问Symfony应用中配置的服务。
  这有助于你准备自己的代码，以便在转换之前在Symfony应用中更好地工作。
  例如，通过使用Symfony组件替换过时或冗余的库。

Line 38 - 47
  不是直接发送Symfony响应，而是调用一个 ``LegacyBridge``
  来决定是否应该启动旧应用并使用它来创建响应。

这个传统桥接器负责确定应该加载哪个文件以处理旧应用的逻辑。
这可以是类似于Symfony的 ``public/index.php``
的前端控制器，也可以是基于当前路由的特定脚本文件。
这个LegacyBridge的基本轮廓可能看起来像这样::

    // src/LegacyBridge.php
    namespace App;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;

    class LegacyBridge
    {
        public static function prepareLegacyScript(Request $request, Response $response, string $publicDirectory): string
        {
            // 如果Symfony成功地处理了路由，你就不必做任何事情。
            if (false === $response->isNotFound()) {
                return;
            }

            // 了解如何从旧应用映射到所需的脚本文件，并可能（重新）设置一些环境变量。
            $legacyScriptFilename = ...;

            return $legacyScriptFilename;
        }
    }

这是你可以采用的最通用的方法，无论你以前的系统是什么，它都可能有效。
你可能需要考虑某些“怪癖”，但由于你的原始应用仅在Symfony处理完请求后启动，因此减少了产生副作用和任何干扰的机会。

由于旧脚本是在全局变量作用域内调用的，它将减少对旧代码的副作用，而旧代码有时可能需要全局作用域内的变量。
同时，因为你的Symfony应用将始终首先启动，你可以通过 ``$kernel``
变量来访问容器，然后获取任何服务（使用
:method:`Symfony\\Component\\HttpKernel\\KernelInterface::getContainer`）。
如果要为旧应用引入新功能，而不将整个操作切换到新应用，这将非常有用。
如果你想在不将整个动作切换到新应用的情况下将新功能引入到旧应用中，这将非常有用。
例如，你现在可以在旧应用中使用Symfony Translator，或者使用Doctrine来重构旧查询，而不是使用旧的数据库逻辑。
这也将允许你逐步改进旧代码，从而更容易将其转换到新的Symfony应用。

主要的缺点是，两个系统没有很好地相互集成，导致一些冗余和可能重复的代码。
例如，由于Symfony应用已经处理完请求，因此你无法利用内核事件或利用Symfony的路由来确定要调用的旧脚本。

传统路由加载器
~~~~~~~~~~~~~~~~~~~

与之前的LegacyBridge方案的主要区别在于，逻辑在Symfony应用中移动。
它消除了一些冗余，并允许我们从Symfony内部与旧应用的各个部分进行交互，而不是相反的方式。

.. tip::

    下面的路由加载器只是你可能必须针对旧应用进行调整的常规示例。
    你可以通过在 :doc:`路由 </routing>` 一文中阅读它来熟悉这些概念。

传统路由加载器是 :doc:`一个自定义路由加载器 </routing/custom_route_loader>`。
传统路由加载器具有与之前的LegacyBridge类似的功能，但它是在Symfony的路由组件中注册的服务::

    // src/Legacy/LegacyRouteLoader.php
    namespace App\Legacy;

    use Symfony\Component\Config\Loader\Loader;
    use Symfony\Component\Routing\Route;
    use Symfony\Component\Routing\RouteCollection;

    class LegacyRouteLoader extends Loader
    {
        // ...

        public function load($resource, $type = null)
        {
            $collection = new RouteCollection();
            $finder = new Finder();
            $finder->files()->name('*.php');

            /** @var SplFileInfo $legacyScriptFile */
            foreach ($finder->in($this->webDir) as $legacyScriptFile) {
                // 这假定所有旧文件都使用“.php”作为扩展名
                $filename = basename($legacyScriptFile->getRelativePathname(), '.php');
                $routeName = sprintf('app.legacy.%s', str_replace('/', '__', $filename));

                $collection->add($routeName, new Route($legacyScriptFile->getRelativePathname(), [
                    '_controller' => 'App\Controller\LegacyController::loadLegacyScript',
                    'requestPath' => '/' . $legacyScriptFile->getRelativePathname(),
                    'legacyScript' => $legacyScriptFile->getPathname(),
                ]));
            }

            return $collection;
        }
    }

你还必须按照 :doc:`自定义路由加载器 </routing/custom_route_loader>` 中的说明在应用的
``routing.yaml`` 中注册加载器。根据你的配置，你可能还必须使用标签服务。
之后，你应该能够查看路由配置中的所有旧路由，例如，当你调用 ``debug:router`` 命令时：

.. code-block:: terminal

    $ php bin/console debug:router

要使用这些路由，你需要创建一个处理这些路由的控制器。
你可能已经注意到上一个代码示例中的 ``_controller``
属性，该属性告诉Symfony每当它尝试访问我们的旧路由时调用哪个控制器。
然后，控制器本身可以使用其他路由属性（即 ``requestPath`` 和
``legacyScript``）来确定要调用的脚本并将输出封装在一个响应类中::

    // src/Controller/LegacyController.php
    namespace App\Controller;

    use Symfony\Component\HttpFoundation\StreamedResponse;

    class LegacyController
    {
        public function loadLegacyScript(string $requestPath, string $legacyScript)
        {
            return StreamedResponse::create(
                function () use ($requestPath, $legacyScript) {
                    $_SERVER['PHP_SELF'] = $requestPath;
                    $_SERVER['SCRIPT_NAME'] = $requestPath;
                    $_SERVER['SCRIPT_FILENAME'] = $legacyScript;

                    chdir(dirname($legacyScript));

                    require $legacyScript;
                }
            );
        }
    }

该控制器将设置一些旧应用可能需要的服务器变量。
这将模拟直接调用的旧脚本，以防它依赖于这些变量（例如，在确定相对路径或文件名时）。
最后，该动作需要旧脚本，它基本上像以前一样调用原始脚本，但它在我们当前应用的作用域内运行，而不是在全局作用域内运行。

这种方法存在一些风险，因为它不再在全局作用域内运行。
但是，由于旧代码现在在一个控制器动作中运行，因此你可以从新的Symfony应用访问许多功能，包括使用Symfony的事件生命周期的机会。
例如，这允许你使用安全组件及其防火墙将旧应用的认证和授权转移到Symfony应用。

.. _`绞杀者应用`: https://www.martinfowler.com/bliki/StranglerApplication.html
.. _`自动加载`: https://getcomposer.org/doc/04-schema.md#autoload
.. _`Modernizing with Symfony`: https://youtu.be/YzyiZNY9htQ
.. _`Symfony Panther`: https://github.com/symfony/panther
