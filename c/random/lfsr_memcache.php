<?php
require('./lfsr_util_32.php');

function lfsr_memcache_init($id) 
{

    $value = array(
        'current' => rand(1, PHP_INT_MAX),
        'bit' => 23,
        'count' => 0,
    );

    $memcache = new Memcache;
    $memcache->connect('localhost', 11211) or die ("Could not connect");
    $memcache->set($id, $value, 0, 0) or die ("Failed to save data at the server");
    $memcache->close();
}

function lfsr_memcache_get_next($id)
{   
    $memcache = new Memcache;
    $memcache->connect('localhost', 11211) or die ("Could not connect");
    $value = $memcache->get($id);
    $value['current'] = LFSR_UTIL::lfsr_next($value['current'], $value['bit']);
    if ($value['current']) {
        $value['count']++;
        $memcache->set($id, $value, 0, 0) or die ("Could not save data at the server");
    }
    $memcache->close();
    return $value['current'];
}

function lfsr_memcache_get($id, $count)
{   
    $memcache = new Memcache;
    $memcache->connect('localhost', 11211) or die ("Could not connect");
    $value = $memcache->get($id);
    $result = LFSR_UTIL::lfsr_generate($value['current'], $value['bit'], $count);
    if ($result) {
        $value['count'] += $count;
        $value['current'] = $result[$count - 1];
        $memcache->set($id, $value, 0, 0) or die ("Could not save data at the server");
    }
    $memcache->close();
    return $result;
}

//lfsr_memcache_init(1234);
/*
$next = lfsr_memcache_get_next(1234);
var_dump($next);
*/
$next = lfsr_memcache_get(1234, 100000);
//var_dump($next);
