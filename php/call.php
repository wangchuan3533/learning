<?php
class test {

    private $name;

    public function __construct($name) {
        $this->name = $name;
    }

    public static function factory($name) {
        return new self($name);
    }

    public function __call($method, $args) {
        echo "$method called:\n";
        var_dump($args);
    }
}
//$a = test::factory('hello');
$clazz = 't' . 'e' . 's' . 't';
$a = new $clazz('hello');
$a->hello('world');
