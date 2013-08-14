<?php
class Tcp_Client {
    
    protected $_client;

    public function __construct() {
        $this->_client = new swoole_client(SWOOLE_SOCK_TCP, SWOOLE_SOCK_SYNC); //同步阻塞
    }

    public function connect($host, $port, $timeout) {
        $ret = $this->_client->connect($host, $port, $time, 0);
        if (!$ret) {
            echo "Connect Server fail.errCode=".$client->errCode;
        }
    }

    public function close() {
    }

    public function send($data) {
    }

    public function receive() {
    }
}
