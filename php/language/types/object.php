<?php
$bar = array(
    'foo' => 'bar',
    'bar' => 'foo',
);
$obj = (object)$bar;
var_dump($obj);
