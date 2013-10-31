<?php

#24位随机数生成器
function next_no($cur) {

    $cur &= 0x7fffff;
    $bit  = (($cur >> 0) ^ ($cur >> 1) ^ ($cur >> 3) ^ ($cur >> 5) ) & 1;
    $cur =  (($cur >> 1) | ($bit << 22)) & 0x7fffff;
    return $cur;
}
$cardid = 0x1234;
// Create 100 byte shared memory block with system id of 0xff3

$shm_id = shmop_open($cardid, "c", 0644, 4 * 3);
if (!$shm_id) {
        echo "Couldn't create shared memory segment\n";
}

// Get shared memory block's size
$shm_size = shmop_size($shm_id);
//echo "SHM Block Size: " . $shm_size . " has been created.\n";

// Lets write a test string into shared memory
if (count($argv) > 1 && $argv[1] === 'create') {
    $offset = 0x1;
    $end = 0x1234;
    $count = 0;
    $initial = pack("V3", $offset, $end, $count);
    $shm_bytes_written = shmop_write($shm_id, $initial, 0);
    if ($shm_bytes_written != strlen($initial)) {
            echo "Couldn't write the entire length of data\n";
    }
}
// Now lets read the string back
$my_string = shmop_read($shm_id, 0, $shm_size);
if (!$my_string) {
        echo "Couldn't read from shared memory block\n";
}
$unpacked = unpack('Voffset/Vend/Vcount', $my_string);
if (count($argv) > 1 && ($num = (int)$argv[1])) {

    for (;$num > 0;$num--) {
        $unpacked['offset'] = next_no($unpacked['offset']);
        $unpacked['count']++;
        $out[] = 10000000 - $unpacked['offset'];
    }
}
$_next = pack("V3", $unpacked['offset'], $unpacked['end'], $unpacked['count']);
$shm_bytes_written = shmop_write($shm_id, $_next, 0);

if (count($argv) > 1 && $argv[1] === 'delete') {
    //Now lets delete the block and close the shared memory segment
    if (!shmop_delete($shm_id)) {
            echo "Couldn't mark shared memory block for deletion.";
    }
}
var_dump($out);
shmop_close($shm_id);
?>
