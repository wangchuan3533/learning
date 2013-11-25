<?php
require('./lfsr_memcache.php');

if (count($argv) > 1 && $argv[1] == 'init') {
    LFSR_Memcache::lfsr_memcache_init(1234, 63);
    exit;
}

//$next = LFSR_Memcache::lfsr_memcache_get_next(1234);
for ($i = 0; $i < 10; $i++) {
    $next = LFSR_Memcache::lfsr_memcache_get(1234, 10000);
    foreach ($next as $v) {
        echo "$v\n";
    }
    usleep(10);
}
