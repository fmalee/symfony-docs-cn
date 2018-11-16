.. index::
   single: Form; Form::submit()

如何使用submit()函数处理表单提交
===========================================================

使用 ``handleRequest()`` 方法，你可以处理表单提交::

    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function new(Request $request)
    {
        $form = $this->createFormBuilder()
            // ...
            ->getForm();

        $form->handleRequest($request);

        if ($form->isSubmitted() && $form->isValid()) {
            // perform some action...

            return $this->redirectToRoute('task_success');
        }

        return $this->render('product/new.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. tip::

    要查看有关此方法的更多信息，请阅读 :ref:`form-handling-form-submissions`。

.. _form-call-submit-directly:

手动调用Form::submit()
-------------------------------

在某些情况下，你希望更好地控制提交的表单以及传递哪些数据给该表单。
那么可以不使用 :method:`Symfony\\Component\\Form\\FormInterface::handleRequest`
方法，而是将提交的数据直接传递给 :method:`Symfony\\Component\\Form\\FormInterface::submit`::

    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function new(Request $request)
    {
        $form = $this->createFormBuilder()
            // ...
            ->getForm();

        if ($request->isMethod('POST')) {
            $form->submit($request->request->get($form->getName()));

            if ($form->isSubmitted() && $form->isValid()) {
                // perform some action...

                return $this->redirectToRoute('task_success');
            }
        }

        return $this->render('product/new.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. tip::

    由嵌套字段组成的表单在 :method:`Symfony\\Component\\Form\\FormInterface::submit`
    是一个数组。你也可以直接在该字段上调用
    :method:`Symfony\\Component\\Form\\FormInterface::submit` 来提交单个字段::

        $form->get('firstName')->submit('Fabien');

.. tip::

    通过“PATCH”请求提交表单时，你可能只想更新几个提交的字段。
    要实现此目的，你可以传递一个可选的布尔型的第二个参数到 ``submit()``。
    传递 ``false`` 将删除该表单对象中的任何缺失字段。否则，缺失的字段将别设置为 ``null``。

.. caution::

    当第二个参数 ``$clearMissing`` 是 ``false``，就像"PATCH"方法，验证扩展将只处理提交的字段。
    如果需要验证基础数据，则应手动完成，即使用验证器。
