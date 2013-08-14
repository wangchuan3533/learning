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
    static $_instance = null;

    /*
     *
     */
    protected $_class = '';

    /*
     *
     */
    public function __construct($serverName) {

        // TODO check param
        $this->_serverName = $serverName;
        $this->_class = __CLASS__;
    }

    /*
     *
     */
    public function init() {

        // 根据服务名称读取配置
        $param = Config::get(basename(APP_PATH) . ".$this->_serverName." . 'param');//TODO config path

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
        swoole_server_handler($this->_serv, 'onConnect', array($this->_class, 'onConnect'));
        swoole_server_handler($this->_serv, 'onReceive', array($this->_class, 'onReceive'));
        swoole_server_handler($this->_serv, 'onClose', array($this->_class, 'onClose'));
        swoole_server_handler($this->_serv, 'onWorkerStart', array($this->_class, 'onWorkerStart'));
        swoole_server_handler($this->_serv, 'onWorkerStop', array($this->_class, 'onWorkerStop'));
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

    static function onClose($serv, $clientId, $reactorId) {
        // unset
        echo "onClose\n";
    }

    static function onConnect($serv, $clientId, $reactorId) {
        echo "onConnect\n";
    }

    static function onWorkerStart($serv, $workerId) {
        echo "onWorkerStart\n";
    }

    static function onWorkerStop($serv, $workerId) {
        echo "onWorkerStop\n";
    }

    static function onReceive($serv, $clientId, $reactorId, $data) {

        echo "onReceive\n";
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

    /*
     *
     */
    static function response($request) {
        return $request;
    }
}

