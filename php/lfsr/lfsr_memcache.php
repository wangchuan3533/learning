<?php
require('./lfsr_util.php');

class LFSR_MEMCACHE {

    static public function lfsr_memcache_init($id, $n_bits) 
    {
        $value = array(
            'current' => rand(1, PHP_INT_MAX),
            'n_bits' => $n_bits,
            'count' => 1,
        );

        $memcache = new Memcache;
        $memcache->connect('localhost', 11211) or die ("Could not connect");
        $memcache->set($id, json_encode($value), 0, 0) or die ("Failed to save data at the server");
        $memcache->close();
    }

    static public function lfsr_memcache_get_next($id)
    {   
        $memcache = new Memcache;
        $memcache->connect('localhost', 11211) or die ("Could not connect");
        // lock
        while ($memcache->add($id . '_lock', 1, 0, 0) == FALSE);
        $value = $memcache->get($id);
        if ($value) $value = json_decode($value, true);
        $value['current'] = LFSR_Util::lfsr_next($value['current'], $value['n_bits']);
        if ($value['current']) {
            $value['count']++;
            $memcache->set($id, json_encode($value), 0, 0) or die ("Could not save data at the server");
        }
        // unlock
        $memcache->delete($id . '_lock');
        $memcache->close();
        return $value['current'];
    }

    static public function lfsr_memcache_get($id, $count)
    {   

        $memcache = new Memcache;
        $memcache->connect('localhost', 11211) or die ("Could not connect");
        // lock
        while (!($memcache->add($id . '_lock', 1, 0, 0)));
        $value = $memcache->get($id);
        if ($value) $value = json_decode($value, true);
        $result = LFSR_Util::lfsr_generate($value['current'], $value['n_bits'], $count);
        if ($result) {
            $value['count'] += $count;
            $value['current'] = $result[$count - 1];
            $memcache->set($id, json_encode($value), 0, 0) or die ("Could not save data at the server");
        }
        // unlock
        $memcache->delete($id . '_lock');
        $memcache->close();
        return $result;
    }
}
