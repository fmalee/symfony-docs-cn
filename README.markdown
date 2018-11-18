# Symfony中文文档

官方：https://github.com/symfony/symfony-docs

参考：http://www.symfonychina.com

## 同步记录

- `master`
  - `2ebb3d5057c8024355ce173f6d0e0ea215053c71`（2018-11-17）
  - `3edbcad1e97744bc86cdcb58df63916985775bc8`（2018-11-01）
  - `28bd6e337977ff0e6908316f0749429a9dbaf392`（2018-10-31）
  - `e350b3efcfadbb26ab51768e5f6a2bcb07d9dc14`（2018-10-26）
  - `df318b284e22352b7678daeaa32ab06eb0fbcc0c`（2018-10-25）
  - `b43a26fc1c8408de31e79f1abf91e0201e92e487`（2018-10-19）

## 汉化进度

### 未汉化

- `bundle/` (5)
- `compoments`
- `configuration` 
  - `environments.rst`
  - `micro_kernel_trait.rst`
  - `multiple_kernels.rst`
- `console`
  - `style.rst`
- `create_framework`
- `form`
  - `create_custom_field_type.rst`
  - `create_form_type_extension.rst`
  - `data_transformers.rst`
  - `dynamic_form_modification.rst`
  - `embedded.rst`
  - `events.rst`
  - `form_collections.rst`
  - `form_customization.rst`
  - `form_dependencies.rst`
  - `form_themes.rst`
  - `type_guesser.rst`
  - `unit_testing.rst`
- `http_cache`
  - `cache_invalidation.rst`
  - `cache_vary.rst`
  - `esi.rst`
  - `expiration.rst`
  - `form_csrf_caching.rst`
  - `validation.rst`
  - `varnish.rst`
- `introduction`
- `logging`
- `profiler`
  - `data_collector.rst`
- `reference`
- `routing`
  - `custom_route_loader.rst`
  - `external_resources.rst`
  - `hostname_pattern.rst`
  - `redirect_in_config.rst`
  - `requirements.rst`
- `security`
  - `access_control.rst`
  - `custom_authentication_provider.rst`
  - `form_login.rst`
  - `impersonating_user.rst`
  - `json_login_setup.rst`
  - `ldap.rst`
  - `multiple_guard_authenticators.rst`
  - `user_provider.rst`
- `service_container`
  - `alias_private.rst`
  - `autowiring.rst`
  - `compiler_passes.rst`
  - `configurators.rst`
  - `definitions.rst`
  - `expression_language.rst`
  - `factories.rst`
  - `parent_services.rst`
  - `service_decoration.rst`
  - `service_subscribers_locators.rst`
  - `tags.rst`
- `session`
  - `locale_sticky_session.rst`
  - `php_bridge.rst`
- `setup`
  - `built_in_web_server.rst`
  - `bundles.rst`
  - `homestead.rst`
  - `web_server_configuration.rst`
- `templating`
  - `PHP.rst`
- `testing`
  - `bootstrap.rst`
  - `database.rst`
  - `doctrine.rst`
  - `http_authentication.rst`
  - `insulating_clients.rst`
  - `profiling.rst`
- `translation`
  - `debug.rst`
  - `lint.rst`
  - `locale.rst`
- `validation`
  - `custom_constraint.rst`
  - `groups.rst`
  - `raw_values.rst`
  - `sequence_provider.rst`
  - `severity.rst`
  - `translations.rst`
- `workflow`
  - `dumping-workflows.rst`
  - `state-machines.rst`
  - `usage.rst`

## 术语约定

- `Route`：路由
- `Controller`：控制器
- `Services`：服务
- `Service Container`：服务容器
- `Dependency Injection`：依赖注入
- `Type-Hint`：类型约束
- `Fully-Qualified`：完全限定
- `Centralize`：集中化
- `Recipes`：指令
- `Meatdata`：元数据
- `Schema`：模式
- `Repository`：仓库
- `Entity`：实体
- `Entity Manger`：实体管理器
- `Query Builder `：查询生成器
- `Wire`：装配
- `Autowire`：自动装配
- `Override`：重写，覆盖
- `Public`：公有
- `Private`：私有
- `Auto-Tag`：自动标记
- `Tag`：标签
- `Tagged`：标记
- `Resource`：资源
- `Asset`：资源，特指CSS，JS等Web资源
- `Guesser`：猜测器
- `Transformer`：转换器、变压器
- `Authentication`：认证
- `Authenticator`：认证器
- `Authorization`：授权
- `Role`：角色
- `Firewall`：防火墙
- `Provider`：提供器
- `User Provider`：用户提供器
- `Authentication Provider`：认证提供器
- `Guard Authenticator`：安保认证器
- `Remember Me`：保持登录
- `Impersonation`：模拟
- `Dummy`：虚拟
- `Profiler`：调试器
- `Voter`：表决器
- `Hierarchy`：层级
- `Logger`：日志器
- `Handler`：处理器
- `Channel`：通道
- `Helper`：辅助函数，辅助程序
- `Section`：切片
- `Migration`：迁移
- `Event`：事件
- `Subscriber`：订阅器
- `Listener`：监听器
- `Dispatch`：调度、派遣
- `Exception`：异常
- `Workflow`：工作流
- `Serializer`：序列化器
- `Normalizer`：规范化器
- `Locale`：语言环境，表示语言和国家/地区
- `Translator`：翻译器
- `Filter`：过滤器
- `Flash Message`：闪存消息
- `Loader`：加载器
- `Constraint`：约束
- `Assert`：断言
- `Validator`：验证器
- `Messenger`：信使
- `Middleware`：中间件
- `Broker`：代理
- `Header`：标头
- `UpperCamelCase`：大驼峰拼写法
- `CamelCase`：驼峰拼写法
- `snake_case`：蛇形拼写法
- `Skeleton`：骨架
- `Decorator`：装饰器
- `Verbosity`：冗余度

## 语法问题

- 对于加粗等语法，需要在 `**` 前后要加上空格，这在中文里很影响美观和阅读。
  - 加粗：`**`
  - 斜体：`*`
  - 注释链接：\`链接\`_
    - 链接的标题可以直接翻译，需要修改两个地方。
- 代码：需要使用 `::`

## 汉化步骤

### 建立仓库

- 克隆官方文档：`symfony-docs`。
- 在文档说明中记录最后提交的UUID和时间。
- 导出官方文档的 `master` 的全部文件到 `doc-cn` 目录。
- 进入 `doc-cn` 目录，初始化仓库。
- 提交所有文件到 `master` 分支。

### 汉化文档

- 进入中文文档的仓库：`doc-cn`。
- 检出  `master` 分支。
- 在 `master` 分支持续汉化文档。

### 更新文档

#### 导出对比版本

- 在仓库外建立一个临时目录：`update`。
- 从 `symfony-docs` 拉取最新的 `master` 记录。
  - 导出 `master` 的全部文件到 `/update/new`。
  - 在文档说明中记录最后提交的UUID和时间。
- 从  `symfony-docs`  中检出 `doc-cn` 仓库记录的最后同步的版本。
  - 导出该版本的全部文件到 `/update/old`。
- 从 `doc-cn` 导出 `master` 分支的全部文件到 `/update/doc`。
- 用 `Beyond Compare`  比较 `doc` 和 `old`
  - 导出差异文件(也就是已经汉化的文件)到 `/update/cn`。
  - 删除 `/update/doc`。

#### 更新未汉化文件

- 删除  `doc-cn` 下 `master` 工作区的所有文件
- 复制  `/update/new` 全部文件到 `doc-cn` 的 `master` 工作区，提交修改。
- 合并  `/update/cn` 全部文件到 `doc-cn` 的 `master` 工作区，提交修改。

- 删除 `/update/cn`。

> 该步骤适合汉化文件不多，而官方更新又比较多的情况。
> 这样就只需要用汉化文件和官方文件对比，工作量少了很多。
>
> 如果官方更新文件不多，则可略过此步骤

#### 更新 `新增/删除` 文件

- 进入 `doc-cn` 仓库，从 `master` 分支创建并检出新分支：`master-new`
- 用 `Beyond Compare`  比较 `new` 和 `old`
- 进入 `master-new` 分支
  - 添加 `Beyond Comper` 中新增的文件
  - 删除 `Beyond Comper` 中已删除的文件
  - 用 `git status` 检查文件状态
- 在 `Beyond Compare` 中和  `git status` 比对
  - 逐一对比两边文件变化
    - 如果Git中已完成，则将 `/update/new` 同步(`复制/删除`)到 `/update/old`。
  - 提交  `doc-cn` 的 `master-new` 分支

#### 更新修改文件

- 根据  `Beyond Compare` 在 `master-new` 分支中合并对应文件。
  - 如果文件比较多，可以一次合并少量文件。
- 汉化合并的内容
- 用 `git status` 检查文件状态
- 在 `Beyond Compare` 中和  `git status` 比对
  - 从 `/update/new`  复制已经合并的文件到 `/update/old`
  - 直至 `git status` 的文件在 `Beyond Compare` 中没有差异
- 提交 `master-new` 分支
- 继续根据 `Beyond Compare` 在 `master-new` 分支中合并对应文件。
- 直至 `Beyond Compare` 中两个目录没有差异，就表示合并已经全部完成。
- 删除 `update` 目录
- 合并 `master-new` 分支到 `master`
- 删除 `master-new` 分支
- 更新完成

这么多的步骤，有两个原因：

- 如果差异太多，一时完成不了同步，可以下次继续进行。
- 防止差异比较多的情况下遗漏某些文件未同步。

> 从 `Beyond Compare` 复制内容到 `Atom` 时，会多出缩进的格式，注意删除。

### 添加版本

首先，**更新现有中文文档**！

其次，选择和需要添加的版本**比较相近**的已经汉化的版本。如已经汉化的 `master(4.2)` 和要添加的 `4.1`。

最后，根据两个版本进行**差异比较**来对比更新。

以添加 `4.1` 版本为例：

- 在仓库外建立一个临时目录：`update`

- 从 `symfony-docs` 拉取最新的 `master` 记录
  - 导出 `master` 的全部文件到 `/update/master`
- 从  `symfony-docs`  中检出最新的 `4.1` 分支
  - 导出该版本的全部文件到 `/update/4.1`
  - 在文档说明中记录最后提交的UUID和时间
- 进入 `doc-cn` 仓库，从 `master` 分支创建并检出新分支：`4.1`
- 用 `Beyond Compare`  比较 `master` 和 `4.1`
- 先更新状态为 `新增/删除` 的文件
  - 具体操作参考 **更新文档** 的对应步骤
- 更新差异文件
  - 具体操作参考 **更新文档** 的对应步骤
- 删除 `update` 目录
- 添加版本完成

> 如果文件太多，一时无法完成汉化：
>
> - 则需要再次导出 `symfony-docs` 的 `master` 和 `4.1` 的所有文件
> - 然后再次用 `Beyond Compare`  对比
> - 在 `doc-cn` 的 `4.1` 分支中进行对比补漏
> - 汉化完一个文件，就同步 `Beyond Compare`  里的文件。
> - 如此反复直至汉化完全完成。

## 生成文档

1. 按照[pip安装](https://pip.pypa.io/en/stable/installing/)文档中的说明安装 [pip](https://pip.pypa.io/en/stable/)；

2. 安装 [Sphinx](http://sphinx-doc.org/) 和 [PHP和Symfony的Sphinx扩展](https://github.com/fabpot/sphinx-php) （根据你的系统，你可能需要以root用户身份执行此命令）：

```bash
$ pip install sphinx~=1.3.0 git+https://github.com/fabpot/sphinx-php.git
```

3. 运行以下命令以HTML格式构建文档：

```bash
$ cd _build/
$ make html
```

生成的文档可在 `_build/html` 目录中找到。

### Platform.sh

[Platform.sh](https://platform.sh)会自动构建新的拉取请求。

### Docker

你可以使用一下命令在本地生成文档:

```bash
# build the image...
$ docker build . -t symfony-docs

# ...and serve it locally on http//:127.0.0.1:8080
# (if it's already in use, change the '8080' port by any other port)
$ docker run --rm -p 8080:80 symfony-docs
```

### 中文搜索

Sphinx官方已经支持中文搜索。

- 安装[结巴中文分词](https://link.jianshu.com/?t=https://github.com/fxsjy/jieba)

  ```bash
  $ pip install jieba
  ```

- 接下来修改sphinx的 `conf.py` 文件，为项目设置为中文的搜索配置。

  ```
  html_search_language = 'zh'
  ```

  > sphinx的位置可以使用`pip -V`来获取。

- 可选配置

  ```ini
  # 根据需要设置jieba的词典路径
  html_search_options = {'dict': '/usr/lib/jieba.txt'}
  ```

## 文档主题

推荐使用 `Doctrine` 项目文档的主题进行修改配置

- https://github.com/doctrine/doctrine-sphinx-theme