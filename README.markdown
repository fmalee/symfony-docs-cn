# Symfony中文文档

官方：https://github.com/symfony/symfony-docs

参考：http://www.symfonychina.com

## 同步记录

- `master`
  - `df318b284e22352b7678daeaa32ab06eb0fbcc0c`（2018-10-25）
  - `b43a26fc1c8408de31e79f1abf91e0201e92e487`（2018-10-19）

## 汉化进展

- [Quick Tour](https://symfony.com/doc/current/quick_tour/index.html)
- [Best Practices](https://symfony.com/doc/current/best_practices/index.html)
- [Getting Started](https://symfony.com/doc/current/setup.html)
- Guides
  - Service Container

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
- `Wire`：自动装配
- `Autowire`：自动装配
- `Override`：重写，覆盖
- `Public`：公有
- `Private`：私有
- `Auto-Tag`：自动标记
- `Tag`：标签
- `Tagged`：标记
- `Resource`：资源
- `Asset`：资源，特指CSS，JS等Web资源
- `Authentication`：认证
- `Authenticator`：认证器
- `Authorization`：授权
- `Role`：角色
- `Firewall`：防火墙
- `Provider`：提供者
- `User Provider`：用户提供者
- `Authentication Provider`：认证提供者
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

- 在仓库外建立一个临时目录：`update`。
- 从 `symfony-docs` 拉取最新的 `master` 记录。
  - 导出 `master` 的全部文件到 `/update/master`。
  - 在文档说明中记录最后提交的UUID和时间。
- 从  `symfony-docs`  中检出 `doc-cn` 记录的最后同步的版本。
  - 导出该版本的全部文件到 `/update/master-cn`。
- 进入 `doc-cn` 仓库，从 `master` 分支创建并检出新分支：`master-new`
- 用 `Beyond Compare`  比较 `master` 和 `master-cn`
- 先更新状态为 `新增`、`删除` 的文件
  - 进入 `master-new` 分支
    - 添加 `Beyond Comper` 中新增的文件
    - 删除 `Beyond Comper` 中已删除的文件
    - 用 `git status` 检查文件状态
  - 在 `Beyond Compare` 中和  `git status` 比对
    - 逐一对比两边文件变化
      - 如果Git中已完成，则将 `/update/master` 同步(`复制/删除`)到 `/update/master-cn`。
    - 提交  `doc-cn` 的 `master-new` 分支
- 更新差异文件。
  - 根据  `Beyond Compare` 在 `master-new` 分支中合并对应文件。
    - 如果文件比较多，可以一次合并少量文件。
  - 汉化合并的内容
  - 用 `git status` 检查文件状态
  - 在 `Beyond Compare` 中和  `git status` 比对
    - 从 `/update/master`  复制已经合并的文件到 `/update/master-cn`
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

首先，**更新现有文档**！

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
- 先更新状态为 `新增`、`删除` 的文件
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

1. Install [pip](https://pip.pypa.io/en/stable/) as explained in the [pip installation](https://pip.pypa.io/en/stable/installing/) article;

2. Install [Sphinx](http://sphinx-doc.org/) and [Sphinx Extensions for PHP and Symfony](https://github.com/fabpot/sphinx-php) (depending on your system, you may need to execute this command as root user):

```bash
$ pip install sphinx~=1.3.0 git+https://github.com/fabpot/sphinx-php.git
```

3. Run the following command to build the documentation in HTML format:

```bash
$ cd _build/
$ make html
```

The generated documentation is available in the `_build/html` directory.

### Platform.sh

Pull requests are automatically built by [Platform.sh](https://platform.sh).

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

- https://github.com/ryan-roemer/sphinx-bootstrap-theme
