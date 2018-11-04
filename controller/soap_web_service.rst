.. index::
    single: Web Services; SOAP

.. _how-to-create-a-soap-web-service-in-a-symfony2-controller:

如何在Symfony控制器中创建SOAP Web服务
========================================================

设置控制器以充当SOAP服务器是由几个工具辅助的。这些工具希望您安装PHP SOAP扩展。由于PHP SOAP扩展当前无法生成WSDL，因此您必须从头开始创建一个或使用第三方生成器。
Setting up a controller to act as a SOAP server is aided by a couple
tools. Those tools expect you to have the `PHP SOAP`_ extension installed.
As the PHP SOAP extension cannot currently generate a WSDL, you must either
create one from scratch or use a 3rd party generator.

.. note::

    There are several SOAP server implementations available for use with
    PHP. `Zend SOAP`_ and `NuSOAP`_ are two examples. Although the PHP SOAP
    extension is used in these examples, the general idea should still
    be applicable to other implementations.
    有几种SOAP服务器实现可用于PHP。Zend SOAP和NuSOAP就是两个例子。尽管在这些示例中使用了PHP SOAP扩展，但总体思路仍应适用于其他实现。

SOAP通过将PHP对象的方法暴露给外部实体（即使用SOAP服务的人）来工作。首先，创建一个类 - HelloService表示您将在SOAP服务中公开的功能。在这种情况下，SOAP服务将允许客户端调用一个被调用的方法hello，该方法 恰好发送一封电子邮件：
SOAP works by exposing the methods of a PHP object to an external entity
(i.e. the person using the SOAP service). To start, create a class - ``HelloService`` -
which represents the functionality that you'll expose in your SOAP service.
In this case, the SOAP service will allow the client to call a method called
``hello``, which happens to send an email::

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

接下来，确保您的新类已注册为服务。如果您使用的是默认服务配置，则无需执行任何操作！
Next, make sure that your new class is registered as a service. If you're using
the :ref:`default services configuration <service-container-services-load-example>`,
you don't need to do anything!

最后，下面是一个能够处理SOAP请求的控制器示例。因为index()可以访问/soap，所以可以通过/soap?wsdl以下方式检索WSDL文档：
Finally, below is an example of a controller that is capable of handling a SOAP
request. Because ``index()`` is accessible via ``/soap``, the WSDL document
can be retrieved via ``/soap?wsdl``::

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

记下对ob_start()和的电话ob_get_clean()。这些方法控制输出缓冲，允许您“捕获”回声输出$server->handle()。这是必要的，因为Symfony希望您的控制器返回一个Response输出为“内容”的对象。您还必须记住将“Content-Type”标头设置为“text / xml”，因为这是客户端所期望的。因此，您可以使用ob_start()开始缓冲STDOUT并使用ob_get_clean()将回显的输出转储到Response的内容中并清除输出缓冲区。最后，你准备好了Response。
Take note of the calls to ``ob_start()`` and ``ob_get_clean()``. These
methods control `output buffering`_ which allows you to "trap" the echoed
output of ``$server->handle()``. This is necessary because Symfony expects
your controller to return a ``Response`` object with the output as its "content".
You must also remember to set the "Content-Type" header to "text/xml", as
this is what the client will expect. So, you use ``ob_start()`` to start
buffering the STDOUT and use ``ob_get_clean()`` to dump the echoed output
into the content of the Response and clear the output buffer. Finally, you're
ready to return the ``Response``.

下面是使用NuSOAP客户端调用服务的示例。此示例假定index()上述控制器中的方法可通过以下路径访问/soap：
Below is an example calling the service using a `NuSOAP`_ client. This example
assumes that the ``index()`` method in the controller above is accessible via
the route ``/soap``::

    $soapClient = new \SoapClient('http://example.com/index.php/soap?wsdl');

    $result = $soapClient->call('hello', array('name' => 'Scott'));

下面是一个示例WSDL。
An example WSDL is below.

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
.. _`output buffering`: https://php.net/manual/en/book.outcontrol.php
.. _`Zend SOAP`: http://framework.zend.com/manual/current/en/modules/zend.soap.server.html
