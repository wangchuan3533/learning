<?php
/**
 * Tcp_Exception 
 * 
 *   作者: 王川(wangchuan@tencentwsh.com) 
 *   创建时间: Wed Aug 14 09:42:05 CST 2013
 *   修改记录: 
 * 
 *  $Id$
 */
class Tcp_Exception extends Exception {

    public function __construct($message, $code = 0) {

        if (is_a($message, 'Exception')) {
            parent::__construct($message->getMessage(), intval($message->getCode()));
        } else {
            parent::__construct($message, intval($code));
        }
    }

}
