# Symfony中文文档

官方：https://github.com/symfony/symfony-docs

参考：http://www.symfonychina.com

## 同步记录

- `master`
  - `1310e0dc917f676f75662d6fe1053b3adaa695b6`（2018-12-04）
    - `components/dom_crawler.rst`
- `4.3`
  - `9a873fbc4c5fc497d1add66d9cb97251d2d55eeb`（2019-07-03）
- `4.2`
  - `1ec78cf5f6fd5d7768c9cbbd8a891b24cea4d3aa`（2019-07-03）
- [SensioFrameworkExtraBundle](https://github.com/sensiolabs/SensioFrameworkExtraBundle/tree/master/Resources/doc)
  - `1032c7077fd1a6f24f98b5a8377938000859f35d`（2018-12-03）

## 汉化进度

### 未汉化

- [常用Bundle](https://symfony.com/doc/bundles/)
- `introduction`
- `compoments` (大部分)
- `reference` (大部分)
- `contributing`
  - `code/bc.rst`
  - `code/bugs.rst`
  - `code/core_team.rst`
  - `code/experimental.rst`
  - `code/maintenance.rst`
  - `code/pull_requests.rst`
  - `code/reproducer.rst`
  - `code/security.rst`
  - `community/review-comments.rst`
  - `community/reviews.rst`
  - `diversity/governance.rst`
- `bundle/` (5)
- `http_cache` (7)
- `service_container/service_subscribers_locators.rst`
- `configuration/environments.rst`
- `routing/custom_route_loader.rst`
- `templating/PHP.rst`
- `setup/bundles.rst`
- `security/custom_authentication_provider.rst`
- `security/ldap.rst`

## 术语约定

- `Application`：应用
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
- `Discriminator`：鉴别器
- `Locale`：语言环境，表示语言和国家/地区
- `Translator`：翻译器
- `Filter`：过滤器
- `Flash Message`：闪存消息
- `Loader`：加载器
- `Constraint`：约束
- `Assert`：断言
- `Validator`：验证器
- `Broker`：代理
- `Header`：标头
- `UpperCamelCase`：大驼峰拼写法
- `CamelCase`：驼峰拼写法
- `snake_case`：蛇形拼写法
- `Skeleton`：骨架
- `Decorator`：装饰器
- `Verbosity`：冗余度
- `Compiler passes`：编译器传递
- `Bridge`：桥接
- `Type Guesser`：类型猜测器
- `Guesser`：猜测器
- `Fragment`：片段
- `Block`：区块
- `Data Transformer`：数据转换器
- `Transformer`：转换器
- `Widget`：部件
- `Prototype`：原型
- `Formatter`：格式化器
- `Channel`：通道
- `Collector`：收集器
- `Insulate`：隔离，绝缘
- `Owning Side`：拥有方
- `Inverse Side`：从属方
- `Data Mapper` 数据映射器
- `Cache Pool` 缓存池
- `Cache Item` 缓存项
- `Cache Adapter` 缓存适配器
- `Cache Hit` 缓存命中
- `Cache Misse` 缓存未命中
- `Contract` 契约
- `Dump` 转储、备份
- `Recomputation Callbacks` 
- `Stampede` 踩踏
- `Multiplexing Responses` 多路复用响应
- `Scope` 作用域
- `Messenger`：信使
- `Middleware`：中间件
- `Transport` 传输
- `Deliver` 投递
- `Sender` 发送方
- `Receiver` 接受方
- `Stamp` 邮票
- `Envelope` 信封
- `Consume` 消费


### 导航调整

为了让文档的导航更清晰，所以对几个 `.. toctree::` 进行了一些调整。

**configuration.rst**

```diff
.. index::
   single: Configuration

- 配置 Symfony (以及环境)
+ 配置

======================================
```

**index.rst**

```diff
    bundles
+    configuration
    console
+    controller
    doctrine

....

    service_container
+    templating
    testing
```

**controller.rst**

```diff
扩展阅读
----------------------------

- .. toctree::
-     :hidden:
-
-     templating

```

**page_creation.rst**

```diff
深入了解HTTP和框架基础知识
----------------------------

- .. toctree::
-     :hidden:
-
-     routing

```

**routing.rst**

```diff
扩展阅读
----------------------------

- .. toctree::
-     :hidden:
-
-     controller

```

**templating.rst**

```diff
.. index::
   single: Templating

- 创建和使用模板
+ 模板

......

扩展阅读
----------------------------

- .. toctree::
-     :hidden:
-
-     configuration

```

> 这些调整不会影响正文内容，只是生成的导航位置不一样

## 生成文档

1. 按照[pip安装](https://pip.pypa.io/en/stable/installing/)文档中的说明安装 [pip](https://pip.pypa.io/en/stable/)；

   > 如果安装了Python 3.X，则自带了 `pip3`。

2. 安装 [Sphinx](http://sphinx-doc.org/) 和 [PHP和Symfony的Sphinx扩展](https://github.com/fabpot/sphinx-php) （根据你的系统，你可能需要以root用户身份执行此命令）：

```bash
$ pip3 install sphinx~=1.3.0 git+https://github.com/fabpot/sphinx-php.git
```

3. 运行以下命令以HTML格式构建文档：

```bash
$ cd _build/
$ make html
```

生成的文档可在 `_build/html` 目录中找到。

### SymfonyCloud

[SymfonyCloud](https://symfony.com/cloud)会自动构建新的拉取请求。

### Docker

你可以使用一下命令在本地生成文档:

```bash
# build the image...
$ docker build . -t symfony-docs

# ...and start the local web server
# (if it's already in use, change the '8080' port by any other port)
$ docker run --rm -p 8080:80 symfony-docs
```

你现在可以在http://127.0.0.1:8080上阅读该文档（如果你使用虚拟机，则可以使用其IP浏览， 例如http://192.168.99.100:8080，而不是localhost）。

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
