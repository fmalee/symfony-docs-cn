.. index::
   single: Form; Embed collection of forms

如何嵌入表单集合
==================================

在本文中，你将学习如何创建一个嵌入许多其他表单集合的表单。
这可能很有用，例如，如果你有一个 ``Task``
类，并且你想在同一个表单内编辑/创建/删除与该任务相关的许多 ``Tag`` 对象。

.. note::

    在本文中，我们松散地假设你使用Doctrine作为数据库存储。
    但如果你没有使用Doctrine（例如Propel或仅仅一个数据库连接），那么它们就非常相似。
    本教程中只有几个部分真正关心“持久性”。

    如果你 *正在* 使用Doctrine，你需要添加Doctrine的元数据，并在任务的 ``tags``
    属性上配置 ``ManyToMany`` 关联映射定义。

首先，假设每个 ``Task`` 属于多个 ``Tag`` 对象。首先创建一个简单的 ``Task`` 类::

    // src/Entity/Task.php
    namespace App\Entity;

    use Doctrine\Common\Collections\ArrayCollection;

    class Task
    {
        protected $description;

        protected $tags;

        public function __construct()
        {
            $this->tags = new ArrayCollection();
        }

        public function getDescription()
        {
            return $this->description;
        }

        public function setDescription($description)
        {
            $this->description = $description;
        }

        public function getTags()
        {
            return $this->tags;
        }
    }

.. note::

    ``ArrayCollection`` 特定于Doctrine，基本和使用一个 ``array``
    相同（但如果你使用Doctrine，那它必须是一个 ``ArrayCollection``）。

现在，创建一个 ``Tag`` 类。如上所述，一个 ``Task`` 可以有很多 ``Tag`` 对象::

    // src/Entity/Tag.php
    namespace App\Entity;

    class Tag
    {
        private $name;

        public function getName()
        {
            return $this->name;
        }

        public function setName($name)
        {
            $this->name = $name;
        }
    }

然后，创建一个表单类，以便用户可以修改一个 ``Tag`` 对象::

    // src/Form/Type/TagType.php
    namespace App\Form\Type;

    use App\Entity\Tag;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;

    class TagType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('name');
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => Tag::class,
            ));
        }
    }

有了这个，你就足以渲染一个标签表单了。
但由于最终目标是允许在任务表单自身内部修改一个 ``Task`` 的标签，因此要为 ``Task`` 类创建一个表单。

请注意，你使用 :doc:`CollectionType </reference/forms/types/collection>`
字段嵌入了一个 ``TagType`` 表单集合::

    // src/Form/Type/TaskType.php
    namespace App\Form\Type;

    use App\Entity\Task;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Form\Extension\Core\Type\CollectionType;

    class TaskType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder->add('description');

            $builder->add('tags', CollectionType::class, array(
                'entry_type' => TagType::class,
                'entry_options' => array('label' => false),
            ));
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => Task::class,
            ));
        }
    }

在你的控制器中，你将从 ``TaskType`` 创建一个新表单::

    // src/Controller/TaskController.php
    namespace App\Controller;

    use App\Entity\Task;
    use App\Entity\Tag;
    use App\Form\Type\TaskType;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;

    class TaskController extends AbstractController
    {
        public function new(Request $request)
        {
            $task = new Task();

            // 虚拟代码 - 这只是为了让任务拥有一些标签
            // 否则，这不会是一个有趣的例子
            $tag1 = new Tag();
            $tag1->setName('tag1');
            $task->getTags()->add($tag1);
            $tag2 = new Tag();
            $tag2->setName('tag2');
            $task->getTags()->add($tag2);
            // 结束虚拟代码

            $form = $this->createForm(TaskType::class, $task);

            $form->handleRequest($request);

            if ($form->isSubmitted() && $form->isValid()) {
                // ... 可以做一些表单处理，比如保存Task和Tag对象
            }

            return $this->render('task/new.html.twig', array(
                'form' => $form->createView(),
            ));
        }
    }

现在，相应的模板既可以渲染任务表单的 ``description`` 字段，也可以渲染已经与 ``Task``
相关的任何标签的所有 ``TagType`` 表单。
在上面的控制器中，我们添加了一些虚拟代码，以便你可以看到这一点（因为首次创建一个 ``Task`` 时没有标签）。

.. code-block:: html+twig

    {# templates/task/new.html.twig #}

    {# ... #}

    {{ form_start(form) }}
        {# 渲染任务的唯一字段：description #}
        {{ form_row(form.description) }}

        <h3>Tags</h3>
        <ul class="tags">
            {# 迭代每个现有标签，并渲染其唯一字段：name #}
            {% for tag in form.tags %}
                <li>{{ form_row(tag.name) }}</li>
            {% endfor %}
        </ul>
    {{ form_end(form) }}

    {# ... #}

当用户提交表单，``tags`` 字段的已提交数据被用于构建一个 ``Tag``
对象的 ``ArrayCollection``，然后被设置到 ``Task`` 实例的 ``tags`` 字段上。

``tags`` 集合可以很自然地通过 ``$task->getTags()``
来访问，然后可以持久化到数据库中，也可以根据需要使用。

到目前为止，这很好用，但这不允许你动态的添加新标签或删除现有标签。
因此，虽然编辑现有标签已经生效，但你的用户实际上还无法添加任何新标签。

.. caution::

    在本文中，你只嵌入了一个集合，但可以不限于此。你可以根据需要将嵌套集合嵌入到多个级别。
    但是，如果在开发设置中使用Xdebug，则可能会收到一个
    ``Maximum function nesting level of '100' reached, aborting!`` 错误。
    这是因为 ``xdebug.max_nesting_level`` PHP设置的默认值为 ``100``。

    如果你一次渲染整个表单（例如 ``form_widget(form)``），此指令将递归限制为100次调用，这可能不足以在模板中渲染该表单。
    要解决此问题，你可以将此指令设置为更高的值（通过 ``php.ini``
    文件或通过 :phpfunction:`ini_set`，例如在 ``public/index.php`` 中）或手动使用
    ``form_row()`` 来渲染每个表单字段。

.. _form-collections-new-prototype:

允许使用“原型”的“新”标签
----------------------------------------

允许用户动态添加新标签意味着你需要使用一些JavaScript。
在前面，你在控制器中向表单添加了两个标签。现在要让用户直接在浏览器中添加所需数量的标签表单。
这将通过一些JavaScript来完成。

你需要做的第一件事是让表单集合知道它将收到未知数​​量的标签。
到目前为止，你已经添加了两个标签，该表单类型预计会收到两个标签，否则会抛出
``This form should not contain extra fields`` 错误。要使其更灵活，请将 ``allow_add``
选项添加到你的集合字段::

    // src/Form/Type/TaskType.php

    // ...
    use Symfony\Component\Form\FormBuilderInterface;

    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        $builder->add('description');

        $builder->add('tags', CollectionType::class, array(
            'entry_type' => TagType::class,
            'entry_options' => array('label' => false),
            'allow_add' => true,
        ));
    }

除了告诉该字段接受任意数量的提交对象外，``allow_add`` 还可以为你提供一个 *"原型"* 变量。
这个“原型”是一个小“模板”，它包含了能够渲染任何新“标签”表单的所有HTML。
要进行渲染，请对模板进行以下更改：

.. code-block:: html+twig

    <ul class="tags" data-prototype="{{ form_widget(form.tags.vars.prototype)|e('html_attr') }}">
        ...
    </ul>

.. note::

    如果你一次性渲染整个“标签”子表单（例如 ``form_row(form.tags)``），那么将在外部的
    ``div`` 上生成 ``data-prototype`` 属性以作为原型，类似于你在上面看到的内容。

.. tip::

    ``form.tags.vars.prototype`` 是一个表单元素，其外观和感觉就像 ``for``
    循环中的各个 ``form_widget(tag)`` 元素一样。这意味着，你可以在其上调用
    ``form_widget()``、``form_row()`` 或 ``form_label()``。
    你甚至可以选择仅渲染其中一个字段（例如 ``name`` 字段）：

    .. code-block:: html+twig

        {{ form_widget(form.tags.vars.prototype.name)|e }}

在已渲染的页面上，结果将如下所示：

.. code-block:: html

    <ul class="tags" data-prototype="&lt;div&gt;&lt;label class=&quot; required&quot;&gt;__name__&lt;/label&gt;&lt;div id=&quot;task_tags___name__&quot;&gt;&lt;div&gt;&lt;label for=&quot;task_tags___name___name&quot; class=&quot; required&quot;&gt;Name&lt;/label&gt;&lt;input type=&quot;text&quot; id=&quot;task_tags___name___name&quot; name=&quot;task[tags][__name__][name]&quot; required=&quot;required&quot; maxlength=&quot;255&quot; /&gt;&lt;/div&gt;&lt;/div&gt;&lt;/div&gt;">

本章节的目标是使用JavaScript读取此属性，并在用户点击“Add a tag”链接时动态的添加新标签表单。
此示例使用jQuery并假设你已将其包含在页面的某个位置。

在页面的某处添加一个``script`` 标签，以便你可以开始编写一些JavaScript。

首先，通过JavaScript添加一个链接到“标签”列表底部。
其次，绑定“click”事件到该链接，以便你可以添加一个新的标签表单（``addTagForm()`` 将在接下来显示）：

.. code-block:: javascript

    var $collectionHolder;

    // 设置 "add a tag" 链接
    var $addTagButton = $('<button type="button" class="add_tag_link">Add a tag</button>');
    var $newLinkLi = $('<li></li>').append($addTagButton);

    jQuery(document).ready(function() {
        // 获取包含标签集合的ul
        $collectionHolder = $('ul.tags');

        // 将 “add a tag” 锚点和 li 添加到该 ul 标签
        $collectionHolder.append($newLinkLi);

        // 计算我们拥有的当前表单输入（例如2），在插入一个新项时使用它作为新索引（例如2）
        $collectionHolder.data('index', $collectionHolder.find(':input').length);

        $addTagButton.on('click', function(e) {
            // 添加一个新标签表单（请参阅下一个代码块）
            addTagForm($collectionHolder, $newLinkLi);
        });
    });

``addTagForm()`` 函数的工作是在点击此链接时使用 ``data-prototype`` 属性来动态添加一个新表单。
``data-prototype`` HTML包含标签的 ``text`` 输入框元素，其 ``name`` 为
``task[tags][__name__][name]``，``id`` 为 ``task_tags___name___name``。
``__name__`` 是一个小“占位符”，你将用一个唯一的递增数字替换它（例如 ``task[tags][3][name]``）。

实现这一切所需的实际代码可能会有很大差异，但这里有一个例子：

.. code-block:: javascript

    function addTagForm($collectionHolder, $newLinkLi) {
        // 获取前面解释过的 data-prototype
        var prototype = $collectionHolder.data('prototype');

        // 获取新索引
        var index = $collectionHolder.data('index');

        var newForm = prototype;
        // 只有在 TaskType 的 tags 字段中未设置 'label' => false 时才需要这个
        // 将原型的HTML中的 '__name__label__' 替换为基于我们拥有的项目数量的数字
        // newForm = newForm.replace(/__name__label__/g, index);

        // 将原型的HTML中的 '__name__' 替换为基于我们拥有的项目数量的数字
        newForm = newForm.replace(/__name__/g, index);

        // 为下一个项做准备，将索引递增
        $collectionHolder.data('index', index + 1);

        // 在页面中的一个li中显示表单，该 li 在 “Add a tag” 链接的 li 之前，
        var $newFormLi = $('<li></li>').append(newForm);
        $newLinkLi.before($newFormLi);
    }

.. note::

    最好在实际的JavaScript文件中分离该JavaScript，而不是像在此处一样在HTML中编写它。

现在，每次用户点击 ``Add a tag`` 链接时，页面上都会出现一个新的子表单。
提交表单时，任何一个新标签表单都将被转换为新 ``Tag`` 对象并添加到 ``Task`` 对象的 ``tags`` 属性中。

.. seealso::

    你可以在这个 `JSFiddle`_ 中找到一个适用的例子。

.. seealso::

    如果要在原型中自定义HTML代码，请阅读 :ref:`form-custom-prototype`。

要更轻松地处理这些新标签，请在 ``Task`` 类中为标签添加一个 "adder" 和一个 "remover" 方法::

    // src/Entity/Task.php
    namespace App\Entity;

    // ...
    class Task
    {
        // ...

        public function addTag(Tag $tag)
        {
            $this->tags->add($tag);
        }

        public function removeTag(Tag $tag)
        {
            // ...
        }
    }

接下来，向 ``tags`` 字段添加一个 ``by_reference`` 选项并将其设置为 ``false``::

    // src/Form/Type/TaskType.php

    // ...
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('tags', CollectionType::class, array(
            // ...
            'by_reference' => false,
        ));
    }

通过这两个更改，在提交表单时，通过调用 ``addTag()`` 方法将每个新的 ``Tag`` 对象添加到 ``Task`` 类中。
在此更改之前，它们是在表单内部通过调用 ``$task->getTags()->add($tag)`` 添加的。
这也不错，但是强制使用“adder”方法可以更轻松地处理这些新的 ``Tag``
对象（特别是如果你正在使用Doctrine，你将会在下面了解更多！）。

.. caution::

    你必须创建 ``addTag()`` 和 ``removeTag()`` 两个方法，否则即使
    ``by_reference`` 为 ``false``，该表单仍将使用 ``setTag()`` 方法。
    你将在本文后面了解有关 ``removeTag()`` 方法的更多信息。

.. sidebar:: Doctrine: Cascading关系和保存“Inverse”方

    要使用Doctrine保存新标签，你需要考虑更多的事情。
    首先，除非你遍历所有新 ``Tag`` 对象并在每个对象上调用
    ``$entityManager->persist($tag)``，否则你将收到来自Doctrine的错误：

        A new entity was found through the relationship
        ``App\Entity\Task#tags`` that was not configured to
        cascade persist operations for entity...

    要解决此问题，你可以选择自动的将持久化操作从 ``Task`` 对象 “cascade” 到任何相关标签。
    为此，请将 ``cascade`` 选项添加到你的 ``ManyToMany`` 元数据中：

    .. configuration-block::

        .. code-block:: php-annotations

            // src/Entity/Task.php

            // ...

            /**
             * @ORM\ManyToMany(targetEntity="Tag", cascade={"persist"})
             */
            protected $tags;

        .. code-block:: yaml

            # src/Resources/config/doctrine/Task.orm.yml
            App\Entity\Task:
                type: entity
                # ...
                oneToMany:
                    tags:
                        targetEntity: Tag
                        cascade:      [persist]

        .. code-block:: xml

            <!-- src/Resources/config/doctrine/Task.orm.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <doctrine-mapping xmlns="http://doctrine-project.org/schemas/orm/doctrine-mapping"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:schemaLocation="http://doctrine-project.org/schemas/orm/doctrine-mapping
                                http://doctrine-project.org/schemas/orm/doctrine-mapping.xsd">

                <entity name="App\Entity\Task">
                    <!-- ... -->
                    <one-to-many field="tags" target-entity="Tag">
                        <cascade>
                            <cascade-persist />
                        </cascade>
                    </one-to-many>
                </entity>
            </doctrine-mapping>

    第二个潜在问题涉及Doctrine关联关系的 `拥有方和从属方`_。
    在此示例中，如果关系的“拥有”方是“任务”，则在将标签正确添加到任务时，持久化将正常工作。
    但是如果拥有方是“标签”，那么你需要做更多的工作以确保已修改为正确的关系。

    诀窍是确保在每个“标签”上设置单个“任务”。
    一种方法是添加一些额外的逻辑到由表单类型调用的 ``addTag()``
    中（如果 ``by_reference`` 已设置为 ``false``）::

        // src/Entity/Task.php

        public function addTag(Tag $tag)
        {
            // 对于一个 many-to-many 关联关系:
            $tag->addTask($this);

            // 对于一个 many-to-one 关联关系:
            $tag->setTask($this);

            $this->tags->add($tag);
        }

    如果你想要使用 ``addTask()``，请确保你有一个看起来像这样的适当的方法::

        // src/Entity/Tag.php

        // ...
        public function addTask(Task $task)
        {
            if (!$this->tasks->contains($task)) {
                $this->tasks->add($task);
            }
        }

.. _form-collections-remove:

允许移除标签
---------------------------

下一步是允许删除集合中的特定项。解决方案类似于允许添加标签。

首先在表单类型中添加 ``allow_delete`` 选项::

    // src/Form/Type/TaskType.php

    // ...
    public function buildForm(FormBuilderInterface $builder, array $options)
    {
        // ...

        $builder->add('tags', CollectionType::class, array(
            // ...
            'allow_delete' => true,
        ));
    }

现在，你需要将一些代码放入到 ``Task`` 的 ``removeTag()`` 方法中::

    // src/Entity/Task.php

    // ...
    class Task
    {
        // ...

        public function removeTag(Tag $tag)
        {
            $this->tags->removeElement($tag);
        }
    }

模板修改
~~~~~~~~~~~~~~~~~~~~~~

``allow_delete`` 选项意味着如果集合的一个项未在提交中出现（sent），则从服务器上的集合中删除相关数据。
为了使其在一个HTML表单中工作，你必须在提交表单之前删除要删除的集合项的DOM元素。

首先，在每个标签表单中添加 "delete this tag" 链接：

.. code-block:: javascript

    jQuery(document).ready(function() {
        // 获取包含标签集合的ul
        $collectionHolder = $('ul.tags');

        // 为所有现有标签表单的li元素添加一个删除链接
        $collectionHolder.find('li').each(function() {
            addTagFormDeleteLink($(this));
        });

        // ... 之前示例中的其余部分
    });

    function addTagForm() {
        // ...

        // 添加一个删除链接到新表单
        addTagFormDeleteLink($newFormLi);
    }

``addTagFormDeleteLink()`` 函数看起来像这样：

.. code-block:: javascript

    function addTagFormDeleteLink($tagFormLi) {
        var $removeFormButton = $('<button type="button">Delete this tag</button>');
        $tagFormLi.append($removeFormButton);

        $removeFormButton.on('click', function(e) {
            // 删除标签表单的li
            $tagFormLi.remove();
        });
    }

从DOM中删除一个标签表单并提交后，已删除的 ``Tag`` 对象将不会包含在传递给 ``setTags()`` 的集合中。
但能不能实际的移除已删除的 ``Tag`` 和 ``Task`` 对象之间的关系，得看你的持久层的配置。

.. sidebar:: Doctrine: 确保数据库的持久性

    当以这种方式删除对象时，你可能需要做一点点工作，以确保 ``Task``
    和已删除的 ``Tag`` 之间的关系被完全移除。

    在Doctrine中，你会拥有一个关系的两个方向：拥有方和从属方。
    通常在这个例子中，你将拥有一个多对多关系，并且已删除的标签将消失并被正确的持久化（也可以毫不费力地完成添加新标签的工作）。

    但是如果你在Task实体上有一个一对多关系，或是拥有一个 ``mappedBy`` 的多对多关系（意味着Task是“从属”方），
    那么你需要做更多的工作以确保已删除的标签被正确持久化。

    在这种情况下，你可以修改控制器以删除已删除的标签上的关系。
    这里假设你有某个处理任务“更新”的 ``edit()`` 动作::

        // src/Controller/TaskController.php

        use App\Entity\Task;
        use Doctrine\Common\Collections\ArrayCollection;

        // ...
        public function edit($id, Request $request, EntityManagerInterface $entityManager)
        {
            if (null === $task = $entityManager->getRepository(Task::class)->find($id)) {
                throw $this->createNotFoundException('No task found for id '.$id);
            }

            $originalTags = new ArrayCollection();

            // 在数据库中创建一个当前Tag对象的ArrayCollection
            foreach ($task->getTags() as $tag) {
                $originalTags->add($tag);
            }

            $editForm = $this->createForm(TaskType::class, $task);

            $editForm->handleRequest($request);

            if ($editForm->isSubmitted() && $editForm->isValid()) {
                // 删除标签和任务之间的关联关系
                foreach ($originalTags as $tag) {
                    if (false === $task->getTags()->contains($tag)) {
                        // 从Tag中删除Task
                        $tag->getTasks()->removeElement($task);

                        // 如果是一个多对一关系，请像这样删除关系
                        // $tag->setTask(null);

                        $entityManager->persist($tag);

                        // 如果你想完全删除标签，你也可以这样做
                        // $entityManager->remove($tag);
                    }
                }

                $entityManager->persist($task);
                $entityManager->flush();

                // 重定向回编辑页面
                return $this->redirectToRoute('task_edit', array('id' => $id));
            }

            // 渲染表单模板
        }

    如你所见，要正确的添加和删除元素可能会非常棘手。
    除非你有一个任务是“拥有”方的多对多的关系，
    否则你需要做额外的工作以确保每个标签对象本身上的关系得到适当更新（无论你是添加新标签还是删除现有标签）。

.. sidebar:: 表单集合的jQuery插件

    `symfony-collection`_ jQuery插件通过提供添加、编辑和删除集合中的元素所需的JavaScript功能来帮助 ``collection`` 表单元素。
    它还提供更高级的功能，例如移动、复制集合中的元素以及自定义一个按钮。

.. _`拥有方和从属方`: http://docs.doctrine-project.org/en/latest/reference/unitofwork-associations.html
.. _`JSFiddle`: http://jsfiddle.net/847Kf/4/
.. _`symfony-collection`: https://github.com/ninsuo/symfony-collection
