.. index::
    single: Messenger; Getting results / Working with command & query buses

从处理器中获取结果
---------------------------------

处理一个消息时，:class:`Symfony\\Component\\Messenger\\Middleware\\HandleMessageMiddleware`
为处理消息的每个对象添加一个
:class:`Symfony\\Component\\Messenger\\Stamp\\HandledStamp`。你可以使用它来获取处理器返回的值::

    use Symfony\Component\Messenger\MessageBusInterface;
    use Symfony\Component\Messenger\Stamp\HandledStamp;

    $envelope = $messageBus->dispatch(SomeMessage());

    // 获取上一个消息处理器返回的值
    $handledStamp = $envelope->last(HandledStamp::class);
    $handledStamp->getResult();

    // 或获取有关所有处理器的信息
    $handledStamps = $envelope->all(HandledStamp::class);

还存在一个 :class:`Symfony\\Component\\Messenger\\HandleTrait`
以便于利用Messenger总线来满足同步需求。
:method:`Symfony\\Component\\Messenger\\HandleTrait::handle`
方法确保只有一个处理器被注册并返回其结果。

使用命令 & 查询总线
----------------------------------

Messenger组件可用于CQRS构架，其中命令和查询总线是应用的核心部分。阅读Martin Fowler的
`关于CQRS的文章`_ ，以了解更多以及 :doc:`如何配置多个总线 </messenger/multiple_buses>`。

由于查询通常是同步的并且预计会被处理一次，因此从处理器获取结果是一种常见的需求。

为避免样板代码，你可以在任何具有 ``$messageBus`` 属性的类中使用 ``HandleTrait``::

    // src/Action/ListItems.php
    namespace App\Action;

    use App\Message\ListItemsQuery;
    use App\MessageHandler\ListItemsQueryResult;
    use Symfony\Component\Messenger\HandleTrait;
    use Symfony\Component\Messenger\MessageBusInterface;

    class ListItems
    {
        use HandleTrait;

        public function __construct(MessageBusInterface $messageBus)
        {
            $this->messageBus = $messageBus;
        }

        public function __invoke()
        {
            $result = $this->query(new ListItemsQuery(/* ... */));

            // 对结果做点什么
            // ...
        }

        // 创建这样的方法是可选的，但允许对结果进行类型提示
        private function query(ListItemsQuery $query): ListItemsResult
        {
            return $this->handle($query);
        }
    }

因此，你可以使用Trait来创建命令和查询总线类。例如，你可以创建一个特殊的 ``QueryBus``
类，并将其注入到需要查询总线行为的任何位置，以来取代 ``MessageBusInterface``::

    // src/MessageBus/QueryBus.php
    namespace App\MessageBus;

    use Symfony\Component\Messenger\Envelope;
    use Symfony\Component\Messenger\HandleTrait;
    use Symfony\Component\Messenger\MessageBusInterface;

    class QueryBus
    {
        use HandleTrait;

        public function __construct(MessageBusInterface $messageBus)
        {
            $this->messageBus = $messageBus;
        }

        /**
         * @param object|Envelope $query
         *
         * @return mixed The handler returned value
         */
        public function query($query)
        {
            return $this->handle($query);
        }
    }

.. _`关于CQRS的文章`: https://martinfowler.com/bliki/CQRS.html
