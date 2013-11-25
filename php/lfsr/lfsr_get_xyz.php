<?php
require('./lfsr_util.php');

if (count($argv) != 3) {
    echo "Usage:\n";
    echo "$argv[0] [card_id] [user_id]\n";
    exit;
}

$card_id = $argv[1];
$user_id = $argv[2];
$memcache = new Memcache;
$memcache->connect('localhost', 11211) or die ("Could not connect");

$key = 'user_' . $card_id . '_' . $user_id;
$current = $memcache->get($key);
if (!$current) {
    $current = array(
        'offset' => rand(1, 0xfffff),
        'index' => 0,
    );
}
$next = array(
    'offset' => LFSR_UTIL::lfsr_next($current['offset'], 20),
    'index' => $current['index'] + 1,
);

$ret = $memcache->set($key, $next, 0, 0);

if (!$ret) {
    die("memcache set fail\n");
}

$value = (int)$memcache->get($current['offset']);
$value ^= $card_id;
if (!$value) {
    die("memcache get fail");
}

$result = array(
    'x' => $current['offset'] & 0x3ff,
    'y' => ($current['offset'] & 0xffc00) >> 10,
    'v' => $value,
    'index' => $current['index'],
);
var_dump($result);
$memcache->close();
