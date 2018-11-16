.. note::

    为了使用注释加载器，你应该使用Composer安装 ``doctrine/annotations`` 和 ``doctrine/cache`` 包。

.. tip::

    注释类不会自动加载，因此你必须使用类似于此类的一个类加载器来加载它们::

        use Composer\Autoload\ClassLoader;
        use Doctrine\Common\Annotations\AnnotationRegistry;

        /** @var ClassLoader $loader */
        $loader = require __DIR__.'/../vendor/autoload.php';

        AnnotationRegistry::registerLoader([$loader, 'loadClass']);

        return $loader;
