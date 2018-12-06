.. index::
   single: Controller; Upload; File

如何上传文件
===================

.. note::

    你可以考虑使用 `VichUploaderBundle`_ 社区bundle，而不是自己处理文件上传。
    该bundle提供所有常见操作（例如文件重命名、保存和删除），并与Doctrine ORM，MongoDB ODM，PHPCR ODM和Propel紧密集成。

想象一下，你的应用中有一个 ``Product`` 实体，并且你想为每个产品添加一个PDF手册。
为此，请在 ``Product`` 实体中添加一个名为 ``brochure`` 的新属性::

    // src/Entity/Product.php
    namespace App\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Validator\Constraints as Assert;

    class Product
    {
        // ...

        /**
         * @ORM\Column(type="string")
         *
         * @Assert\NotBlank(message="Please, upload the product brochure as a PDF file.")
         * @Assert\File(mimeTypes={ "application/pdf" })
         */
        private $brochure;

        public function getBrochure()
        {
            return $this->brochure;
        }

        public function setBrochure($brochure)
        {
            $this->brochure = $brochure;

            return $this;
        }
    }

请注意，``brochure`` 列的类型是 ``string`` 而不是 ``binary``  或 ``blob``，因为它只存储PDF文件名而不是文件内容。

然后，将新的 ``brochure`` 字段添加到管理 ``Product`` 实体的表单中::

    // src/Form/ProductType.php
    namespace App\Form;

    use App\Entity\Product;
    use Symfony\Component\Form\AbstractType;
    use Symfony\Component\Form\FormBuilderInterface;
    use Symfony\Component\OptionsResolver\OptionsResolver;
    use Symfony\Component\Form\Extension\Core\Type\FileType;

    class ProductType extends AbstractType
    {
        public function buildForm(FormBuilderInterface $builder, array $options)
        {
            $builder
                // ...
                ->add('brochure', FileType::class, array('label' => 'Brochure (PDF file)'))
                // ...
            ;
        }

        public function configureOptions(OptionsResolver $resolver)
        {
            $resolver->setDefaults(array(
                'data_class' => Product::class,
            ));
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

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\File\Exception\FileException;
    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Routing\Annotation\Route;
    use App\Entity\Product;
    use App\Form\ProductType;

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
                // $file 将保存上传的PDF文件
                /** @var Symfony\Component\HttpFoundation\File\UploadedFile $file */
                $file = $product->getBrochure();

                $fileName = $this->generateUniqueFileName().'.'.$file->guessExtension();

                // 将文件移动到存储手册的目录
                try {
                    $file->move(
                        $this->getParameter('brochures_directory'),
                        $fileName
                    );
                } catch (FileException $e) {
                    // 处理异常，如果在文件上传过程中发生了某些事情的话
                }

                // 更新 'brochure' 属性以存储PDF文件名而不是其内容
                $product->setBrochure($fileName);

                // ... 持久化 $product 变量或任何其他工作

                return $this->redirect($this->generateUrl('app_product_list'));
            }

            return $this->render('product/new.html.twig', array(
                'form' => $form->createView(),
            ));
        }

        /**
         * @return string
         */
        private function generateUniqueFileName()
        {
            // md5() 降低了由 uniqid() 生成的文件名的相似性，因为它基于时间戳生产
            return md5(uniqid());
        }
    }

现在，创建在控制器中使用的 ``brochures_directory`` 参数，以指定应存储手册的目录：

.. code-block:: yaml

    # config/services.yaml

    # ...
    parameters:
        brochures_directory: '%kernel.project_dir%/public/uploads/brochures'

在上述控制器的代码中需要考虑一些重要的事情：

#. 上传表单时，``brochure`` 属性包含整个PDF文件内容。由于此属性仅存储文件名，因此必须在持久化实体更改之前设置其新值;
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

.. versionadded:: 4.1
    :method:`Symfony\\Component\\HttpFoundation\\File\\UploadedFile::getClientSize`
    方法在Symfony 4.1中已弃用，将在Symfony 5.0中删除。请改用 ``getSize()``。


你可以使用以下代码链接到一个产品的PDF手册：

.. code-block:: html+twig

    <a href="{{ asset('uploads/brochures/' ~ product.brochure) }}">View brochure (PDF)</a>

.. tip::

    在创建一个表单以编辑已经存在的项目时，文件表单类型仍然需要一个
    :class:`Symfony\\Component\\HttpFoundation\\File\\File` 实例。
    由于持久化实体现在只包含文件的相对路径，因此首先必须将配置的上传路径与存储的文件名连接起来并创建一个新的 ``File`` 类::

        use Symfony\Component\HttpFoundation\File\File;
        // ...

        $product->setBrochure(
            new File($this->getParameter('brochures_directory').'/'.$product->getBrochure())
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
            $fileName = md5(uniqid()).'.'.$file->guessExtension();

            try {
                $file->move($this->getTargetDirectory(), $fileName);
            } catch (FileException $e) {
                // ... 如果在文件上传过程中发生了某些事，则处理异常
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

    .. versionadded:: 4.1
        Symfony 4.1中引入了详细的异常类。

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
                http://symfony.com/schema/dic/services/services-1.0.xsd">
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
    use Symfony\Component\HttpFoundation\Request;
    use App\Service\FileUploader;

    // ...
    public function new(Request $request, FileUploader $fileUploader)
    {
        // ...

        if ($form->isSubmitted() && $form->isValid()) {
            $file = $product->getBrochure();
            $fileName = $fileUploader->upload($file);

            $product->setBrochure($fileName);

            // ...
        }

        // ...
    }

使用Doctrine监听器
-------------------------

如果你使用Doctrine来存储Product实体，则可以创建
:doc:`Doctrine监听器 </doctrine/event_listeners_subscribers>` 以在持久化实体时自动上传文件::

    // src/EventListener/BrochureUploadListener.php
    namespace App\EventListener;

    use Symfony\Component\HttpFoundation\File\UploadedFile;
    use Symfony\Component\HttpFoundation\File\File;
    use Doctrine\ORM\Event\LifecycleEventArgs;
    use Doctrine\ORM\Event\PreUpdateEventArgs;
    use App\Entity\Product;
    use App\Service\FileUploader;

    class BrochureUploadListener
    {
        private $uploader;

        public function __construct(FileUploader $uploader)
        {
            $this->uploader = $uploader;
        }

        public function prePersist(LifecycleEventArgs $args)
        {
            $entity = $args->getEntity();

            $this->uploadFile($entity);
        }

        public function preUpdate(PreUpdateEventArgs $args)
        {
            $entity = $args->getEntity();

            $this->uploadFile($entity);
        }

        private function uploadFile($entity)
        {
            // 上传仅适用于产品实体
            if (!$entity instanceof Product) {
                return;
            }

            $file = $entity->getBrochure();

            // 只上传新文件
            if ($file instanceof UploadedFile) {
                $fileName = $this->uploader->upload($file);
                $entity->setBrochure($fileName);
            } elseif ($file instanceof File) {
                // 防止在更新时保存完整文件路径，因为在postLoad监听器上设置了路径
                $entity->setBrochure($file->getFilename());
            }
        }
    }

现在，将此类注册为Doctrine监听器：

.. configuration-block::

    .. code-block:: yaml

        # config/services.yaml
        services:
            _defaults:
                # ... 确保自动装配以启用
                autowire: true
            # ...

            App\EventListener\BrochureUploadListener:
                tags:
                    - { name: doctrine.event_listener, event: prePersist }
                    - { name: doctrine.event_listener, event: preUpdate }

    .. code-block:: xml

        <!-- config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <!-- ... be sure autowiring is enabled -->
                <defaults autowire="true" />
                <!-- ... -->

                <service id="App\EventListener\BrochureUploaderListener">
                    <tag name="doctrine.event_listener" event="prePersist"/>
                    <tag name="doctrine.event_listener" event="preUpdate"/>
                </service>
            </services>
        </container>

    .. code-block:: php

        // config/services.php
        use App\EventListener\BrochureUploaderListener;

        $container->autowire(BrochureUploaderListener::class)
            ->addTag('doctrine.event_listener', array(
                'event' => 'prePersist',
            ))
            ->addTag('doctrine.event_listener', array(
                'event' => 'preUpdate',
            ))
        ;

现在，在持久化一个新的Product实体时会自动执行此监听器。
这样，你可以从控制器中删除与上传相关的所有内容。

.. tip::

    当从数据库中获取实体时，此监听器还可以基于路径创建 ``File`` 实例::

        // ...
        use Symfony\Component\HttpFoundation\File\File;

        // ...
        class BrochureUploadListener
        {
            // ...

            public function postLoad(LifecycleEventArgs $args)
            {
                $entity = $args->getEntity();

                if (!$entity instanceof Product) {
                    return;
                }

                if ($fileName = $entity->getBrochure()) {
                    $entity->setBrochure(new File($this->uploader->getTargetDirectory().'/'.$fileName));
                }
            }
        }

    添加这些行后，配置该监听器以监听 ``postLoad`` 事件。

.. _`VichUploaderBundle`: https://github.com/dustin10/VichUploaderBundle
