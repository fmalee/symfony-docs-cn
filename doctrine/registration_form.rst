.. index::
   single: Doctrine; Simple Registration Form
   single: Form; Simple Registration Form
   single: Security; Simple Registration Form

如何实现注册表单
===========================================

创建注册表单的基础知识与任何普通表单相同。毕竟，你正在使用它创建一个对象（用户）。
但是，由于这与安全有关，因此还有一些其他方面的事项。这篇文章将解释这一切。

开始之前
----------------------

要创建注册表单，请确保准备好以下3件事：

**1) 安装MakerBundle**

确保安装了MakerBundle：

.. code-block:: terminal

    $ composer require --dev symfony/maker-bundle

如果你需要任何其他依赖项，MakerBundle将在你运行每个命令时告诉你。

**2) 创建用户类**

如果你已经拥有一个
:ref:`User类 <create-user-class>`，那太好了！如果没有，你可以通过运行命令生成一个：

.. code-block:: terminal

    $ php bin/console make:user

有关详细信息，请参阅 :ref:`create-user-class`。

**3) (可选) 创建安保认证器**

如果要在注册后自动对用户进行认证，请在生成注册表单之前创建安保认证器。有关详细信息，请参阅主安全页面上的
:ref:`firewalls-authentication` 部分。

添加注册系统
------------------------------

最简单的方法是使用 ``make:registration-form`` 命令来构建注册表单：

.. versionadded:: 1.11

    ``make:registration-form`` 是在MakerBundle 1.11.0中引入的。

.. code-block:: terminal

    $ php bin/console make:registration-form

这个命令需要知道几件事 - 比如你的 ``User``
类和关于该类属性的信息。问题将根据你的设置而有所不同，因为命令会尽可能的进行预测。

命令完成后，恭喜！你拥有了一个可供你自定义的功能性注册表系统。生成的文件将类似于你在下面看到的内容。

RegistrationFormType
~~~~~~~~~~~~~~~~~~~~

注册表单的表单类将如下所示::

    namespace App\Form;

    use App\Entity\User;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\PasswordType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Validator\Constraints\Length;
    use Symfony\Component\Validator\Constraints\NotBlank;

    class RegistrationFormType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('email')
                ->add('plainPassword', PasswordType::class, [
                    // 不是直接设置到对象上，而是在控制器中读取和编码
                    'mapped' => false,
                    'constraints' => [
                        new NotBlank([
                            'message' => 'Please enter a password',
                        ]),
                        new Length([
                            'min' => 6,
                            'minMessage' => 'Your password should be at least {{ limit }} characters',
                            'max' => 4096,
                        ]),
                    ],
                ])
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'data_class' => User::class,
            ]);
        }
    }

.. _registration-password-max:

.. sidebar:: 为什么密码限制是4096？

    请注意，该 ``plainPassword`` 字段的最大长度为4096个字符。
    出于安全目的（`CVE-2013-5750`_），Symfony在加密时将明文密码长度限制为4096个字符。
    添加此约束可确保如果有人尝试超长密码，你的表单会给出一个验证错误。

    在应用中的，你需要在用户提交明文密码的任何位置都添加此约束（例如，更改密码表单）。
    不需要关心这一点的唯一地方是你的登录表单，因为Symfony的安全组件为你处理此问题。

RegistrationController
~~~~~~~~~~~~~~~~~~~~~~

控制器会构建表单，并在提交时加密普通密码以及保存用户::

    namespace App\Controller;

    use App\Entity\User;
    use App\Form\RegistrationFormType;
    use App\Security\StubAuthenticator;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing\Annotation\Route;
    use Symfony\Component\Security\Core\Encoder\UserPasswordEncoderInterface;
    use Symfony\Component\Security\Guard\GuardAuthenticatorHandler;

    class RegistrationController extends AbstractController
    {
        /**
         * @Route("/register", name="app_register")
         */
        public function register(Request $request, UserPasswordEncoderInterface $passwordEncoder): Response
        {
            $user = new User();
            $form = $this->createForm(RegistrationFormType::class, $user);
            $form->handleRequest($request);

            if ($form->isSubmitted() && $form->isValid()) {
                // 加密文本密码
                $user->setPassword(
                    $passwordEncoder->encodePassword(
                        $user,
                        $form->get('plainPassword')->getData()
                    )
                );

                $entityManager = $this->getDoctrine()->getManager();
                $entityManager->persist($user);
                $entityManager->flush();

                // 做你需要的任何其他事情，比如发送电子邮件

                return $this->redirectToRoute('app_homepage');
            }

            return $this->render('registration/register.html.twig', [
                'registrationForm' => $form->createView(),
            ]);
        }
    }

register.html.twig
~~~~~~~~~~~~~~~~~~

用于渲染该表单的模板：

.. code-block:: html+twig

    {% extends 'base.html.twig' %}

    {% block title %}Register{% endblock %}

    {% block body %}
        <h1>Register</h1>

        {{ form_start(registrationForm) }}
            {{ form_row(registrationForm.email) }}
            {{ form_row(registrationForm.plainPassword) }}

            <button class="btn">Register</button>
        {{ form_end(registrationForm) }}
    {% endblock %}

添加“接受条款”复选框
--------------------------------

有时，你需要在注册表单上添加“你是否接受条款和条件”复选框。
唯一的诀窍是，你希望将此字段添加到表单中，
而不向你的 ``User`` 实体添加你永远不需要的的 ``termsAccepted`` 新属性。

为此，请在表单中添加一个 ``termsAccepted`` 字段，并将其
:ref:`mapped <reference-form-option-mapped>` 选项设置为 ``false``::

    // src/Form/UserType.php
    // ...
    use Symfony\Component\Form\Extension\Core\Type\CheckboxType;
    use Symfony\Component\Form\Extension\Core\Type\EmailType;
    use Symfony\Component\Validator\Constraints\IsTrue;

    class UserType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('email', EmailType::class)
                // ...
                ->add('termsAccepted', CheckboxType::class, [
                    'mapped' => false,
                    'constraints' => new IsTrue(),
                ])
            ;
        }
    }

为了能够使用验证，``termsAccepted`` 字段也可以添加 :ref:`约束 <form-option-constraints>` 选项，
即使在 ``User`` 上没有该属性。

成功后手动认证
-------------------------------------

如果你使用的是安保(Guard)认证，则可以在注册成功后
:ref:`自动进行认证 <guard-manual-auth>`。生成器可能已配置你的控制器以利用此功能。

.. _`CVE-2013-5750`: https://symfony.com/blog/cve-2013-5750-security-issue-in-fosuserbundle-login-form
