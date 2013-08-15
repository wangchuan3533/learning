<?php
$clients = array();
for($i = 0; $i < 10; $i++){
    $client = new swoole_client(SWOOLE_SOCK_TCP, SWOOLE_SOCK_SYNC); //同步阻塞
    $ret = $client->connect('127.0.0.1', 9876, 0.5, 0);
    if(!$ret)
    {
        echo "Over flow. errno=".$client->errCode;
        die("\n");
    }
    $clients[] = $client;
}
sleep(1);
while (1) {
    foreach ($clients as $client) {
        $buf = "";
        for ($i = 0; $i < 1024; $i++) {
            $buf .= chr(rand(33, 125));
        }
        echo "before len=" . strlen($buf) . "\n";
        $client->send($buf);
        $data = $client->recv(8192, 0);
        echo "after len=" . strlen($data) . "\n";
        if (strcmp($buf, $data) != 0) {
            die("mismatch\n");
        }
    }
}
