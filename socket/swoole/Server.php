<?php

class Tcp_Server {

    /*
     *
     */
    protected $_serv;

    /*
     *
     */
    protected $_clients = array();

    /*
     *
     */
    protected $_serverName;

    /*
     *
     */
    static protected $_instance = null;

    /*
     *
     */
    static public function &factory($class, $serverName) {

        if (!(self::$_instance instanceof self)) {
            self::$_instance = new $class($serverName);
        }
        return self::$_instance;
    }

    /*
     *
     */
    public function __construct($serverName) {

        // TODO check param
        $this->_serverName = $serverName;
    }

    /*
     *
     */
    public function init() {

        // 根据服务名称读取配置
        //$param = Config::get(basename(APP_PATH) . ".$this->_serverName." . 'param');//TODO config path
        $param = array(
            'host' => '127.0.0.1',
            'port' => 9876,
            'timeout' => 2.5,
            'poll_thread_num' => 2,
            'writer_num' => 4,
            'worker_num' => 4,
            'backlog' => 128,
            'max_request' => 1000,
            'daemonize' => 0,
        );

        // 创建TCP服务
        $this->_serv = swoole_server_create($param['host'], $param['port'],
                SWOOLE_PROCESS, SWOOLE_SOCK_TCP);

        // 配置Swoole参数
        swoole_server_set($this->_serv, array(
            'timeout' => $param['timeout'],  //select and epoll_wait timeout. 
            'poll_thread_num' => $param['poll_thread_num'], //reactor thread num
            'writer_num' => $param['writer_num'],     //writer thread num
            'worker_num' => $param['worker_num'],    //worker process num
            'backlog' => $param['backlog'],   //listen backlog
            'max_request' => $param['max_request'],
            'daemonize' => $param['daemonize'],
        ));

        // 配置回调函数
        swoole_server_handler($this->_serv, 'onStart', array($this, 'onStart'));
        swoole_server_handler($this->_serv, 'onShutdown', array($this, 'onShutdown'));
        swoole_server_handler($this->_serv, 'onTimer', array($this, 'onTimer'));
        swoole_server_handler($this->_serv, 'onMasterConnect', array($this, 'onMasterConnect'));
        swoole_server_handler($this->_serv, 'onMasterClose', array($this, 'onMasterClose'));
        swoole_server_handler($this->_serv, 'onConnect', array($this, 'onConnect'));
        swoole_server_handler($this->_serv, 'onReceive', array($this, 'onReceive'));
        swoole_server_handler($this->_serv, 'onClose', array($this, 'onClose'));
        swoole_server_handler($this->_serv, 'onWorkerStart', array($this, 'onWorkerStart'));
        swoole_server_handler($this->_serv, 'onWorkerStop', array($this, 'onWorkerStop'));
    }
    
    /*
     *
     */
    public function start() {
        swoole_server_start($this->_serv);
    }

    /*
     *
     */
    protected function log($output) {
        // TODO
        Logger::writeln($output);
    }

    // TODO a lot

    /*
     *
     */
    function onStart($serv) {
        echo "onStart\n";
    }

    function onShutdown($serv) {
        echo "onShutdown\n";
    }

    function onTimer($serv, $interval) {
        echo "onTimer\n";
    }

    function onClose($serv, $clientId, $reactorId) {
        // unset
        echo "onClose\n";
    }

    function onConnect($serv, $clientId, $reactorId) {
        echo "onConnect\n";
    }

    function onWorkerStart($serv, $workerId) {
        echo "onWorkerStart\n";
    }

    function onWorkerStop($serv, $workerId) {
        echo "onWorkerStop\n";
    }

    function onReceive($serv, $clientId, $reactorId, $data) {

        $server = self::$_instance;
        $resp = $server->response($data);
        swoole_server_send($serv, $clientId, $resp);
        echo "onReceive in parrent\n";
    }

    function onMasterClose($serv, $clientId, $reactorId) {
        // TODO unset
        echo "onMasterClose\n";
    }

    function onMasterConnect($serv, $clientId, $reactorId) {
        echo "onMasterConnect\n";
        // TODO
        $this->_clients[] = array($clientId, $reactorId);
    }

}

class Open_Tcp_Server extends Tcp_Server {

    public function __construct($serverName) {

        parent::__construct($serverName);
    }

    function response($request) {
        echo 'response in child';
        return $request;
    }


}
function test() {
    $serv = Tcp_Server::factory('Open_Tcp_Server', 'test');
    $serv->init();
    $serv->start();
}
test();
