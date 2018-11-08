.. index::
   single: Doctrine; Simple Registration Form
   single: Form; Simple Registration Form
   single: Security; Simple Registration Form

如何实现简单的注册表单
===========================================

创建注册表单与创建任何表单的工作方式相同。
你配置表单以更新某个 ``User`` 模型对象（在此示例中为Doctrine实体），然后保存它。

首先，确保你已安装所有需要的依赖：

.. code-block:: terminal

    $ composer require symfony/orm-pack symfony/form symfony/security-bundle symfony/validator

如果你还没有 ``User`` 实体和一个登录系统，请首先按照 :doc:`/security` 进行配置。

你的 ``User`` 实体可能至少具有以下字段：

``username``
    这将用于登录，除非你希望你的用户
    :ref:`通过电子邮件登录 <registration-form-via-email>` （在这种情况下，此字段是不必要的）。

``email``
    用户信息中比较重要的。你还可以允许用户 :ref:`通过电子邮件登录 <registration-form-via-email>`。

``password``
    加密的密码。

``plainPassword``
    此字段 *不会* 保留:(请注意不在其上方添加 ``@ORM\Column`` ）。
    它暂时存储注册表单中的文本密码。可以验证此字段，然后使用该字段填充 ``password`` 字段。

添加一些验证后，你的类可能如下所示::

    // src/Entity/User.php
    namespace App\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Validator\Constraints as Assert;
    use Symfony\Bridge\Doctrine\Validator\Constraints\UniqueEntity;
    use Symfony\Component\Security\Core\User\UserInterface;

    /**
     * @ORM\Entity
     * @UniqueEntity(fields="email", message="Email already taken")
     * @UniqueEntity(fields="username", message="Username already taken")
     */
    class User implements UserInterface
    {
        /**
         * @ORM\Id
         * @ORM\Column(type="integer")
         * @ORM\GeneratedValue(strategy="AUTO")
         */
        private $id;

        /**
         * @ORM\Column(type="string", length=255, unique=true)
         * @Assert\NotBlank()
         * @Assert\Email()
         */
        private $email;

        /**
         * @ORM\Column(type="string", length=255, unique=true)
         * @Assert\NotBlank()
         */
        private $username;

        /**
         * @Assert\NotBlank()
         * @Assert\Length(max=4096)
         */
        private $plainPassword;

        /**
         * The below length depends on the "algorithm" you use for encoding
         * the password, but this works well with bcrypt.
         *
         * @ORM\Column(type="string", length=64)
         */
        private $password;

        /**
         * @ORM\Column(type="array")
         */
        private $roles;

        public function __construct()
        {
            $this->roles = array('ROLE_USER');
        }

        // 其他属性和方法

        public function getEmail()
        {
            return $this->email;
        }

        public function setEmail($email)
        {
            $this->email = $email;
        }

        public function getUsername()
        {
            return $this->username;
        }

        public function setUsername($username)
        {
            $this->username = $username;
        }

        public function getPlainPassword()
        {
            return $this->plainPassword;
        }

        public function setPlainPassword($password)
        {
            $this->plainPassword = $password;
        }

        public function getPassword()
        {
            return $this->password;
        }

        public function setPassword($password)
        {
            $this->password = $password;
        }

        public function getSalt()
        {
            // bcrypt和argon2i算法不需要单独的salt。
            // 如果你选择不同的编码器，你*可能*需要一个真正的salt。
            return null;
        }

        public function getRoles()
        {
            return $this->roles;
        }

        public function eraseCredentials()
        {
        }
    }

:class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`
需要一些其他的方法，你的 ``security.yaml`` 文件也需要正确配置，以便与 ``User`` 实体一起工作。
有关更完整的示例，请参阅 :doc:`安全指南 </security>`。

.. _registration-password-max:

.. sidebar:: 为什么密码限制是4096？

    请注意，该 ``plainPassword`` 字段的最大长度为4096个字符。
    出于安全目的（`CVE-2013-5750`_），Symfony在加密时将明文密码长度限制为4096个字符。
    添加此约束可确保如果有人尝试超长密码，你的表单会给出一个验证错误。

    在应用中的，你需要在用户提交明文密码的任何位置都添加此约束（例如，更改密码表单）。
    不需要关心这一点的唯一地方是你的登录表单，因为Symfony的安全组件为你处理此问题。

.. _create-a-form-for-the-model:

为实体创建表单
----------------------------

接下来，为 ``User`` 实体创建表单类型::

    // src/Form/UserType.php
    namespace App\Form;

    use App\Entity\User;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Form\Extension\Core\Type\EmailType;
    use Symfony\Component\Form\Extension\Core\Type\TextType;
    use Symfony\Component\Form\Extension\Core\Type\RepeatedType;
    use Symfony\Component\Form\Extension\Core\Type\PasswordType;

    class UserType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('email', EmailType::class)
                ->add('username', TextType::class)
                ->add('plainPassword', RepeatedType::class, array(
                    'type' => PasswordType::class,
                    'first_options'  => array('label' => 'Password'),
                    'second_options' => array('label' => 'Repeat Password'),
                ))
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => User::class,
            ));
        }
    }

只有三个字段：``email``、``username`` 和 ``plainPassword`` （重复确认输入的密码）。

.. tip::

    要了解有关Form组件的更多信息，请阅读 :doc:`/forms` 指南。

处理表单提交
----------------------------

接下来，你需要一个控制器来处理表单渲染和提交。
如果提交了表单，控制器将执行验证并将数据保存到数据库中::

    // src/Controller/RegistrationController.php
    namespace App\Controller;

    use App\Form\UserType;
    use App\Entity\User;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Routing\Annotation\Route;
    use Symfony\Component\Security\Core\Encoder\UserPasswordEncoderInterface;

    class RegistrationController extends AbstractController
    {
        /**
         * @Route("/register", name="user_registration")
         */
        public function register(Request $request, UserPasswordEncoderInterface $passwordEncoder)
        {
            // 1) 生成表单
            $user = new User();
            $form = $this->createForm(UserType::class, $user);

            // 2) 处理提交 (仅在POST时触发)
            $form->handleRequest($request);
            if ($form->isSubmitted() && $form->isValid()) {

                // 3) 加密密码 (你也可以通过Doctrine监听器做此操作)
                $password = $passwordEncoder->encodePassword($user, $user->getPlainPassword());
                $user->setPassword($password);

                // 4) 保存用户!
                $entityManager = $this->getDoctrine()->getManager();
                $entityManager->persist($user);
                $entityManager->flush();

                // ... 其他操作 - 比如给他们发送邮件等等
                // 也可以该用户设置一个闪存消息

                return $this->redirectToRoute('replace_with_some_route');
            }

            return $this->render(
                'registration/register.html.twig',
                array('form' => $form->createView())
            );
        }
    }

要在步骤3中定义用于编码密码的算法，请在安全配置中配置编码器：

.. configuration-block::

    .. code-block:: yaml

        # config/packages/security.yaml
        security:
            encoders:
                App\Entity\User: bcrypt

    .. code-block:: xml

        <!-- config/packages/security.xml -->
        <?xml version="1.0" charset="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <encoder class="App\Entity\User">bcrypt</encoder>
            </config>
        </srv:container>

    .. code-block:: php

        // config/packages/security.php
        use App\Entity\User;

        $container->loadFromExtension('security', array(
            'encoders' => array(
                User::class => 'bcrypt',
            ),
        ));

在这个例子中，使用推荐的 `bcrypt`_ 算法。如果有需要，请查看
:ref:`用户密码编码 <security-encoding-user-password>` 章节。

接下来，创建模板：

.. code-block:: html+twig

    {# templates/registration/register.html.twig #}

    {{ form_start(form) }}
        {{ form_row(form.username) }}
        {{ form_row(form.email) }}
        {{ form_row(form.plainPassword.first) }}
        {{ form_row(form.plainPassword.second) }}

        <button type="submit">Register!</button>
    {{ form_end(form) }}

有关更多详细信息，请参阅 :doc:`/form/form_customization`。

更新数据库的模式
---------------------------

如果你在本教程中更新了 ``User`` 实体，则必须使用以下命令更新数据库模式：

.. code-block:: terminal

    $ php bin/console doctrine:migrations:diff
    $ php bin/console doctrine:migrations:migrate

仅此而已！可以前往 ``/register`` 看看成果了！

.. _registration-form-via-email:

只有电子邮件（无用户名）的注册表单
--------------------------------------------------------

如果你希望用户通过电子邮件而不是用户名登录，则可以将其从你的 ``User`` 实体中完全删除。
然后，在 ``getUsername()`` 中返回 ``email`` 属性::

    // src/Entity/User.php
    // ...

    class User implements UserInterface
    {
        // ...

        public function getUsername()
        {
            return $this->email;
        }

        // ...
    }

接下来，更新 ``security.yaml`` 文件的 ``providers`` 部分，
以便Symfony知道如何在登录时通过 ``email`` 属性加载用户。
请参阅 :ref:`authenticating-someone-with-a-custom-entity-provider`。

添加“接受条款”复选框
--------------------------------

有时，你需要在注册表单上添加“你是否接受条款和条件”复选框。
唯一的诀窍是，你希望将此字段添加到表单中，
而不向你的 ``User`` 实体添加你永远不需要的的 ``termsAccepted`` 新属性。

为此，请在表单中添加一个 ``termsAccepted`` 字段，并将其
:ref:`mapped <reference-form-option-mapped>` 选项设置为 ``false``::

    // src/Form/UserType.php
    // ...
    use Symfony\Component\Validator\Constraints\IsTrue;
    use Symfony\Component\Form\Extension\Core\Type\CheckboxType;
    use Symfony\Component\Form\Extension\Core\Type\EmailType;

    class UserType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                ->add('email', EmailType::class);
                // ...
                ->add('termsAccepted', CheckboxType::class, array(
                    'mapped' => false,
                    'constraints' => new IsTrue(),
                ))
            );
        }
    }

为了能够使用验证，``termsAccepted`` 字段也可以添加 :ref:`约束 <form-option-constraints>` 选项，
即使在 ``User`` 上没有该属性。

成功后手动认证
-------------------------------------

如果你使用的是安保(Guard)认证，则可以在注册成功后 :ref:`自动进行认证 <guard-manual-auth>`。

.. _`CVE-2013-5750`: https://symfony.com/blog/cve-2013-5750-security-issue-in-fosuserbundle-login-form
.. _`bcrypt`: https://en.wikipedia.org/wiki/Bcrypt
