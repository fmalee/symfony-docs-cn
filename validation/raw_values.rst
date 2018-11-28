.. index::
    single: Validation; Validating raw values

如何验证原始值（标量值和数组）
=====================================================

通常，你将验证整个对象。但有时，你只想验证一个简单的值 - 比如验证字符串是否是有效的电子邮件地址。
为此，你可以直接实例化验证器。从控制器内部看起来就像这样::

    // ...
    use Symfony\Component\Validator\Constraints as Assert;
    use Symfony\Component\Validator\Validator\ValidatorInterface;

    // ...
    public function addEmail($email, ValidatorInterface $validator)
    {
        $emailConstraint = new Assert\Email();
        // 所有约束“选项”都可以以这种方式设置
        $emailConstraint->message = 'Invalid email address';

        // 使用验证器来验证该值
        $errors = $validator->validate(
            $email,
            $emailConstraint
        );

        if (0 === count($errors)) {
            // ... 这 *是* 一个有效的电子邮件地址，可以做点事情
        } else {
            // 这 *不是* 一个有效的电子邮件地址
            $errorMessage = $errors[0]->getMessage();

            // ... 使用该错误信息做些事情
        }

        // ...
    }

通过调用验证器的 ``validate()``，你可以传入一个原始值和要验证该值的约束对象。
:doc:`约束参考 </reference/constraints>`
章节提供了所有可用约束的完整列表 - 以及每个约束的完整类名称。

使用 ``Collection`` 约束可以验证一个数组::

    use Symfony\Component\Validator\Validation;
    use Symfony\Component\Validator\Constraints as Assert;

    $validator = Validation::createValidator();

    $constraint = new Assert\Collection(array(
        // 该键对应于输入数组中的键
        'name' => new Assert\Collection(array(
          'first_name' => new Assert\Length(array('min' => 101)),
          'last_name' => new Assert\Length(array('min' => 1)),
        )),
        'email' => new Assert\Email(),
        'simple' => new Assert\Length(array('min' => 102)),
        'gender' => new Assert\Choice(array(3, 4)),
        'file' => new Assert\File(),
        'password' => new Assert\Length(array('min' => 60)),
    ));

    $violations = $validator->validate($input, $constraint);

``validate()`` 方法返回一个
:class:`Symfony\\Component\\Validator\\ConstraintViolationList`
对象，其行为就像一个错误数组。
集合中的每个错误都是一个
:class:`Symfony\\Component\\Validator\\ConstraintViolation`
对象，而他的 ``getMessage()`` 方法保存着对应的错误消息。
