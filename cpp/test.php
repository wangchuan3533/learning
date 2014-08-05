<?php
class A {
    public function p() {
        echo 'a';
    }
}

class B extends A {
    public function p() {
        echo 'b';
    }
}
$c = new B();
$c->p();

