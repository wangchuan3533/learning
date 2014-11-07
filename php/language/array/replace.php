<?php
$a = array(
    'a' => 'b',
    'c' => array(
        'd' => 'e',
        'e' => 'f',
    ),
);
$b = array(
    'c' => array(
        'e' => 'e',
        'f' => 'e',
        'g' => 'f',
    ),
);

var_export(array_replace_recursive($a, $b));
