.. index::
    single: Web Services; SOAP

.. _how-to-create-a-soap-web-service-in-a-symfony2-controller:

如何在Symfony控制器中创建SOAP Web服务
========================================================

设置控制器以充当SOAP服务器是由几个工具辅助的。
这些工具希望你安装 `PHP SOAP`_ 展。
由于PHP SOAP扩展当前无法生成WSDL，因此你必须从头开始创建一个或使用第三方生成器。

.. note::

    有几种SOAP服务器实现可用于PHP。`Zend SOAP`_ 和 `NuSOAP`_ 就是两个例子。
    尽管在这些示例中使用了PHP SOAP扩展，但总体思路仍应适用于其他实现。

SOAP通过将PHP对象的方法暴露给外部实体（即使用SOAP服务的人）来工作。
首先，创建一个类 - ``HelloService`` - 表示你将在SOAP服务中公开的功能。
在这个例子中，SOAP服务将允许客户端调用一个 ``hello`` 方法，该方法用于发送一封电子邮件::

    // src/Service/HelloService.php
    namespace App\Service;

    class HelloService
    {
        private $mailer;

        public function __construct(\Swift_Mailer $mailer)
        {
            $this->mailer = $mailer;
        }

        public function hello($name)
        {

            $message = new \Swift_Message('Hello Service')
                ->setTo('me@example.com')
                ->setBody($name.' says hi!');

            $this->mailer->send($message);

            return 'Hello, '.$name;
        }
    }

接下来，确保你的新类已注册为服务。如果你使用的是
:ref:`默认服务配置 <service-container-services-load-example>`，则无需执行任何操作！

最后，下面是一个能够处理SOAP请求的控制器示例。
因为通过 ``/soap`` 可以访问 ``index()``，所以可以通过 ``/soap?wsdl`` 检索WSDL文档::

    namespace App\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\AbstractController;
    use Symfony\Component\HttpFoundation\Response;
    use Symfony\Component\Routing\Annotation\Route;
    use App\Service\HelloService;

    class HelloServiceController extends AbstractController
    {
        /**
         * @Route("/soap")
         */
        public function index(HelloService $helloService)
        {
            $soapServer = new \SoapServer('/path/to/hello.wsdl');
            $soapServer->setObject($helloService);

            $response = new Response();
            $response->headers->set('Content-Type', 'text/xml; charset=ISO-8859-1');

            ob_start();
            $soapServer->handle();
            $response->setContent(ob_get_clean());

            return $response;
        }
    }

记下对 ``ob_start()`` 和 ``ob_get_clean()`` 的调用。
这些方法控制 `输出缓冲`_，允许你“捕获” ``$server->handle()`` 输出的回声(echoed)。
这是必要的，因为Symfony希望你的控制器返回一个输出为“内容”的 ``Response`` 对象。
你还必须记住将“Content-Type”标头设置为“text/xml”，因为这是客户端所期望的。
因此，你可以使用 ``ob_start()`` 开始缓冲STDOUT并使用 ``ob_get_clean()``
将回显(echoed)的输出(dump)转储到响应的内容中并清除输出缓冲区。
最后，你准备好返回 ``Response`` 了。

下面是使用 `NuSOAP`_ 客户端调用服务的示例。
此示例假定可以通过 ``/soap`` 路由访问上述控制器中的 ``index()`` 方法::

    $soapClient = new \SoapClient('http://example.com/index.php/soap?wsdl');

    $result = $soapClient->call('hello', array('name' => 'Scott'));

下面是一个WSDL示例。

.. code-block:: xml

    <?xml version="1.0" encoding="ISO-8859-1"?>
    <definitions xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
        xmlns:xsd="http://www.w3.org/2001/XMLSchema"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
        xmlns:tns="urn:arnleadservicewsdl"
        xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/"
        xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/"
        xmlns="http://schemas.xmlsoap.org/wsdl/"
        targetNamespace="urn:helloservicewsdl">

        <types>
            <xsd:schema targetNamespace="urn:hellowsdl">
                <xsd:import namespace="http://schemas.xmlsoap.org/soap/encoding/" />
                <xsd:import namespace="http://schemas.xmlsoap.org/wsdl/" />
            </xsd:schema>
        </types>

        <message name="helloRequest">
            <part name="name" type="xsd:string" />
        </message>

        <message name="helloResponse">
            <part name="return" type="xsd:string" />
        </message>

        <portType name="hellowsdlPortType">
            <operation name="hello">
                <documentation>Hello World</documentation>
                <input message="tns:helloRequest"/>
                <output message="tns:helloResponse"/>
            </operation>
        </portType>

        <binding name="hellowsdlBinding" type="tns:hellowsdlPortType">
            <soap:binding style="rpc" transport="http://schemas.xmlsoap.org/soap/http"/>
            <operation name="hello">
                <soap:operation soapAction="urn:arnleadservicewsdl#hello" style="rpc"/>

                <input>
                    <soap:body use="encoded" namespace="urn:hellowsdl"
                        encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
                </input>

                <output>
                    <soap:body use="encoded" namespace="urn:hellowsdl"
                        encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
                </output>
            </operation>
        </binding>

        <service name="hellowsdl">
            <port name="hellowsdlPort" binding="tns:hellowsdlBinding">
                <soap:address location="http://example.com/index.php/soap" />
            </port>
        </service>
    </definitions>

.. _`PHP SOAP`: https://php.net/manual/en/book.soap.php
.. _`NuSOAP`: http://sourceforge.net/projects/nusoap
.. _`输出缓冲`: https://php.net/manual/en/book.outcontrol.php
.. _`Zend SOAP`: http://framework.zend.com/manual/current/en/modules/zend.soap.server.html
