.. index::
   single: Controller; Upload; File

如何上传文件
===================

.. note::

    你可以考虑使用 `VichUploaderBundle`_ 社区bundle，而不是自己处理文件上传。
    该bundle提供所有常见操作（例如文件重命名、保存和删除），并与Doctrine ORM，MongoDB ODM，PHPCR ODM和Propel紧密集成。

想象一下，你的应用中有一个 ``Product`` 实体，并且你想为每个产品添加一个PDF手册。
为此，请在 ``Product`` 实体中添加一个名为 ``brochureFilename`` 的新属性::

    // src/Entity/Product.php
    namespace App\Entity;

    use Doctrine\ORM\Mapping as ORM;

    class Product
    {
        // ...

        /**
         * @ORM\Column(type="string")
         */
        private $brochureFilename;

        public function getBrochureFilename()
        {
            return $this->brochureFilename;
        }

        public function setBrochureFilename($brochureFilename)
        {
            $this->brochureFilename = $brochureFilename;

            return $this;
        }
    }

请注意，``brochureFilename`` 列的类型是 ``string`` 而不是 ``binary``
或 ``blob``，因为它仅存储PDF文件名而不是文件内容。

下一步是向管理 ``Product`` 实体的表单添加新字段。这必须是一个 ``FileType``
字段，以便浏览器可以显示文件上传的小部件。
使其工作的技巧是将该表单字段添加为“未映射”，因此Symfony不会尝试从相关实体获取/设置其值::

    // src/Form/ProductType.php
    namespace App\Form;

    use App\Entity\Product;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\Extension\Core\Type\FileType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Validator\Constraints\File;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('brochure', FileType::class, [
                    'label' => 'Brochure (PDF file)',

                    // 未映射表示此字段未与任何实体属性关联
                    'mapped' => false,

                    // 设置为可选，这样每次编辑产品详细信息时都不必重新上载PDF文件。
                    'required' => false,

                    // 未映射字段不能使用关联实体中的注释来定义它的验证，因此可以使用PHP约束类
                    'constraints' => [
                        new File([
                            'maxSize' => '1024k',
                            'mimeTypes' => [
                                'application/pdf',
                                'application/x-pdf',
                            ],
                            'mimeTypesMessage' => 'Please upload a valid PDF document',
                        ])
                    ],
                ])
                // ...
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults([
                'data_class' => Product::class,
            ]);
        }
    }

现在，更新渲染表单的模板以显示新的 ``brochure`` 字段，要添加的确切模板代码取决于应用中
:doc:`自定义表单渲染 </form/form_customization>` 的具体方法：

.. code-block:: html+twig

    {# templates/product/new.html.twig #}
    <h1>Adding a new product</h1>

    {{ form_start(form) }}
        {# ... #}

        {{ form_row(form.brochure) }}
    {{ form_end(form) }}

最后，你需要更新处理表单的控制器的代码::

    // src/Controller/ProductController.php
    namespace App\Controller;

    use App\Entity\Product;
    use App\Form\ProductType;
    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\File\Exception\FileException;
    use Symfony\Component\HttpFoundation\File\UploadedFile;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Routing\Annotation\Route;

    class ProductController extends AbstractController
    {
        /**
         * @Route("/product/new", name="app_product_new")
         */
        public function new(Request $request)
        {
            $product = new Product();
            $form = $this->createForm(ProductType::class, $product);
            $form->handleRequest($request);

            if ($form->isSubmitted() && $form->isValid()) {
                /** @var UploadedFile $brochureFile */
                $brochureFile = $form['brochure']->getData();

                // 此条件是必需的，因为 'brochure' 字段不是必需的
                // 因此，只有在上传文件时才能处理此PDF文件
                if ($brochureFile) {
                    $originalFilename = pathinfo($brochureFile->getClientOriginalName(), PATHINFO_FILENAME);
                    // 这里需要安全地将文件名作为URL的一部分包含进来。
                    $safeFilename = transliterator_transliterate('Any-Latin; Latin-ASCII; [^A-Za-z0-9_] remove; Lower()', $originalFilename);
                    $newFilename = $safeFilename.'-'.uniqid().'.'.$brochureFile->guessExtension();

                    // 将文件移动到存储手册的目录中
                    try {
                        $brochureFile->move(
                            $this->getParameter('brochures_directory'),
                            $newFilename
                        );
                    } catch (FileException $e) {
                        // ... 如果在文件上传过程中发生问题，则处理异常
                    }

                    // 更新 'brochureFilename' 属性以存储PDF文件名而不是其内容
                    $product->setBrochureFilename($newFilename);
                }

                // ... 持久化 $product 变量或任何其他工作

                return $this->redirect($this->generateUrl('app_product_list'));
            }

            return $this->render('product/new.html.twig', [
                'form' => $form->createView(),
            ]);
        }
    }

现在，创建在控制器中使用的 ``brochures_directory`` 参数，以指定应存储手册的目录：

.. code-block:: yaml

    # config/services.yaml

    # ...
    parameters:
        brochures_directory: '%kernel.project_dir%/public/uploads/brochures'

在上述控制器的代码中需要考虑一些重要的事情：

#. 在Symfony应用中，上传的文件是
   :class:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile` 类的对象。
   该类提供处理上传文件时最常用操作的方法；
#. 众所周知的最佳安全实践是永远不要相信用户提供的输入。这也适用于你的访问者上传的文件。
   ``UploadedFile`` 类提供了一些方法来获得原始文件的扩展名（
   :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getExtension`
   ），原始文件的大小（
   :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientSize`
   ）和原文件名（
   :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientOriginalName`
   ）。但是，它们一般是 *不安全* 的，因为恶意用户可能会篡改该信息。这就是为什么总是生成一个唯一的名称并使用
   :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::guessExtension`
   方法让Symfony根据文件MIME类型猜出正确的扩展名的原因;

.. deprecated:: 4.1

    :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientSize`
    方法在Symfony 4.1中已弃用，将在Symfony 5.0中删除。请改用 ``getSize()``。

你可以使用以下代码链接到一个产品的PDF手册：

.. code-block:: html+twig

    <a href="{{ asset('uploads/brochures/' ~ product.brochureFilename) }}">View brochure (PDF)</a>

.. tip::

    在创建一个表单以编辑已经存在的项目时，文件表单类型仍然需要一个
    :class:`Symfony\\Component\\HttpFoundation\\File\\File` 实例。
    由于持久化实体现在只包含文件的相对路径，因此首先必须将配置的上传路径与存储的文件名连接起来并创建一个新的 ``File`` 类::

        use Symfony\Component\HttpFoundation\File\File;
        // ...

        $product->setBrochureFilename(
            new File($this->getParameter('brochures_directory').'/'.$product->getBrochureFilename())
        );

创建上传服务
----------------------------

为了避免控制器中的逻辑使控制器越来越大，你可以将上传逻辑提取到单独的服务中::

    // src/Service/FileUploader.php
    namespace App\Service;

    use Symfony\Component\HttpFoundation\File\Exception\FileException;
    use Symfony\Component\HttpFoundation\File\UploadedFile;

    class FileUploader
    {
        private $targetDirectory;

        public function __construct($targetDirectory)
        {
            $this->targetDirectory = $targetDirectory;
        }

        public function upload(UploadedFile $file)
        {
            $originalFilename = pathinfo($file->getClientOriginalName(), PATHINFO_FILENAME);
            $safeFilename = transliterator_transliterate('Any-Latin; Latin-ASCII; [^A-Za-z0-9_] remove; Lower()', $originalFilename);
            $fileName = $safeFilename.'-'.uniqid().'.'.$file->guessExtension();

            try {
                $file->move($this->getTargetDirectory(), $fileName);
            } catch (FileException $e) {
                // ... 如果在文件上传过程中发生问题，则处理异常
            }

            return $fileName;
        }

        public function getTargetDirectory()
        {
            return $this->targetDirectory;
        }
    }

.. tip::

    除了一个通用的 :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\FileException`
    类外，还有其他异常类可以处理上传失败的文件：
    :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\CannotWriteFileException`，
    :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\ExtensionFileException`，
    :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\FormSizeFileException`，
    :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\IniSizeFileException`，
    :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\NoFileException`，
    :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\NoTmpDirFileException`，
    以及 :class:`Symfony\\Component\\HttpFoundation\\File\\Exception\\PartialFileException`。

然后，将此类定义为服务：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            # ...

            App\Service\FileUploader:
                arguments:
                    $targetDirectory: '%brochures_directory%'

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                https://symfony.com/schema/dic/services/services-1.0.xsd">
            <!-- ... -->

            <service id="App\FileUploader">
                <argument>%brochures_directory%</argument>
            </service>
        </container>

    .. code-block:: php

        // config/services.php
        use App\Service\FileUploader;

        $container->autowire(FileUploader::class)
            ->setArgument('$targetDirectory', '%brochures_directory%');

现在你已准备好在控制器中使用此服务::

    // src/Controller/ProductController.php
    use App\Service\FileUploader;
    use Symfony\Component\HttpFoundation\Request;

    // ...
    public function new(Request $request, FileUploader $fileUploader)
    {
        // ...

        if ($form->isSubmitted() && $form->isValid()) {
            /** @var UploadedFile $brochureFile */
            $brochureFile = $form['brochure']->getData();
            if ($brochureFile) {
                $brochureFileName = $fileUploader->upload($brochureFile);
                $product->setBrochureFilename($brochureFileName);
            }

            // ...
        }

        // ...
    }

使用Doctrine监听器
-------------------------

本文的先前版本解释了如何使用 :doc:`Doctrine监听器 </doctrine/event_listeners_subscribers>`
处理文件上载。但是，不再建议这样做，因为不应将Doctrine事件用于你的域逻辑。

此外，Doctrine监听器通常依赖于内部的Doctrine行为，这些行为在将来的版本中可能会发生变化。
同样，它们可能会无意中引入性能问题（因为你的监听器会持久存储导致其他实体被更改和持久化的实体）。

作为替代方案，你可以使用 :doc:`Symfony事件、监听器和订阅器 </event_dispatcher>`。

.. _`VichUploaderBundle`: https://github.com/dustin10/VichUploaderBundle
