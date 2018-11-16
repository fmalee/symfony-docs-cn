.. index::
    single: Forms; With no class

如何使用没有数据类的表单
======================================

在大多数情况下，一个表单绑定到一个对象，并且该表单的字段获取并将其数据存储在该对象的属性上。
这正是你在本文中使用的 ``Task`` 类中看到的内容。

但有时，你可能只想使用没有类的表单，并以数组形式获取提交的数据。
``getData()`` 方法完全允许你这样做::

    // 确保你已在类上方导入Request命名空间
    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function contact(Request $request)
    {
        $defaultData = array('message' => 'Type your message here');
        $form = $this->createFormBuilder($defaultData)
            ->add('name', TextType::class)
            ->add('email', EmailType::class)
            ->add('message', TextareaType::class)
            ->add('send', SubmitType::class)
            ->getForm();

        $form->handleRequest($request);

        if ($form->isSubmitted() && $form->isValid()) {
            // 数据将是带有“name”，“email”和“message”键的数组
            $data = $form->getData();
        }

        // ... render the form
    }

默认情况下，表单实际上假定你要使用数据数组而不是对象。
有两种方法可以更改此行为并将表单绑定到一个对象：

#. 创建表单时传递一个对象（作为 ``createFormBuilder()`` 的第一个参数或 ``createForm()`` 的第二个参数）;

#. 在表单上声明 ``data_class`` 选项。

如果你 *不* 执行其中任何一项，则表单将以数组形式返回数据。
在此示例中，由于 ``$defaultData`` 不是对象（并且未设置 ``data_class`` 选项），因此 ``$form->getData()`` 最终返回一个数组。

.. tip::

    你还可以直接通过请求对象访问POST的值（在本例中为“name”），如下所示::

        $request->request->get('name');

    但是请注意，在大多数情况下，使用 ``getData()`` 方法是一个更好的选择，因为它返回的是经Form组件转换后的数据（通常是对象）。

添加验证
-----------------

唯一缺失的部分是验证。
通常，在调用 ``$form->handleRequest($request)`` 时，通过读取应用于该类的约束来验证一个对象。
如果你的表单已映射到一个对象（即你正在使用 ``data_class`` 选项或将已将一个对象传递给你的表单），这几乎总是你要使用的方法。
有关详细信息，请参阅 :doc:`/validation`。

.. _form-option-constraints:

但是，如果表单未映射到对象，而你想要检索一个提交数据的简单数组，那么如何向该表单数据添加约束？

答案是自己设置约束，并将它们附加到各个字段。
在 :doc:`这篇验证文档 </validation/raw_values>` 中对整体方法进行了更多介绍，但这里有一个简短的例子::

    use Symfony\Component\Validator\Constraints\Length;
    use Symfony\Component\Validator\Constraints\NotBlank;
    use Symfony\Component\Form\Extension\Core\Type\TextType;

    $builder
       ->add('firstName', TextType::class, array(
           'constraints' => new Length(array('min' => 3)),
       ))
       ->add('lastName', TextType::class, array(
           'constraints' => array(
               new NotBlank(),
               new Length(array('min' => 3)),
           ),
       ))
    ;

.. tip::

    如果你正使用验证组，则需要在创建表单时引用 ``Default`` 组，或者在要添加的约束上设置正确的验证组。

    .. code-block:: php

        new NotBlank(array('groups' => array('create', 'update')));

.. tip::

    如果表单未映射到对象，则使用 ``Symfony\Component\Validator\Constraints\Valid``
    约束验证提交的数据数组中的每个对象，除非你 :doc:`禁用验证 </form/disabling_validation>`。
