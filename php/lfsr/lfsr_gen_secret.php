<?php
require('./lfsr_util.php');
$memcache = new Memcache;
$memcache->connect('localhost', 11211) or die ("Could not connect");
$seed = rand(1, 1 << 31);
for ($i = 0, $next = $seed; $i < 0x100000; $i++) {
    $next = LFSR_UTIL::lfsr_next($next, 32);
    $memcache->set($i, $next, 0, 0) or die ("Failed to save data at the server");
}
$memcache->close();
