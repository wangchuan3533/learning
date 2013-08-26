<?php

error_reporting(E_ALL);
set_time_limit(0);

class TcpServer {

    protected $address = '0.0.0.0';
    protected $port = 10000;
    protected $socket;
    protected $base;
    protected $buffers = array();
    protected $clients = array();
    protected $index = 0;

    public function __construct() {
    }

    public function init() {

        $socket = stream_socket_server ("tcp://$this->address:$this->port", $errno, $errstr);
        stream_set_blocking($socket, 0);
        $base = event_base_new();

        $this->socket = $socket;
        $this->base = $base;

    }

    public function start() {

        $socket = $this->socket;
        $base = $this->base;

        $event = event_new();
        event_set($event, $socket, EV_READ | EV_PERSIST, array($this, 'ev_accept'), $base);
        event_base_set($event, $base);
        event_add($event);

        event_base_loop($base);
    }

    protected function ev_accept($socket, $flag, $base) {

        $socket = $this->socket;
        $base = $this->base;
        $index = &$this->index; 
        
        $connection = stream_socket_accept($socket);
        stream_set_blocking($connection, 0);
        
        $buffer = event_buffer_new($connection, array($this, 'ev_read'), NULL, array($this, 'ev_error'), ++$index);
        event_buffer_base_set($buffer, $base);
        event_buffer_timeout_set($buffer, 300, 5);
        event_buffer_watermark_set($buffer, EV_READ, 0, 0xffffff);
        event_buffer_priority_set($buffer, 10);
        event_buffer_enable($buffer, EV_READ | EV_PERSIST);
        
        // we need to save both buffer and connection outside
        $this->clients[$index] = $connection;
        $this->buffers[$index] = $buffer;
        //var_dump($this->clients);
    }

    protected function ev_error($buffer, $error, $index) {
        event_buffer_disable($this->buffers[$index], EV_READ | EV_WRITE);
        event_buffer_free($this->buffers[$index]);
        fclose($this->clients[$index]);
        unset($this->clients[$index], $this->buffers[$index]);
        var_dump($error);
        //var_dump($this->clients);
    }

    protected function ev_read($buffer, $index) {
        while ($read = event_buffer_read($buffer, 1024)) {
            $data = 'it works';
            stream_socket_sendto($this->clients[$index], $this->http_package($data));
            /*
            event_buffer_disable($this->buffers[$index], EV_READ | EV_WRITE);
            event_buffer_free($this->buffers[$index]);
            fclose($this->clients[$index]);
            unset($this->clients[$index], $this->buffers[$index]);
            return;
            */
        }
    }

    protected function http_package($data) {
        $httpStr = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\nServer: Tencent.WeLife.Server\r\nConnection: Close\r\n\r\n" . $data;
        return $httpStr;
    }
}

$server = new TcpServer();
$server->init();
$server->start();
