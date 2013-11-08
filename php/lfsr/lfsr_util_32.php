<?php

class LFSR_UTIL {

    // 因为php不支持无符号整数，所以最大位数整数位数少1
    // 32位版本
    const LFSR_MAX_BITS = 31;

    private static $_lfsr_map = array(
        5 => array(5, 4, 3, 2),
        6 => array(6, 5, 3, 2),
        7 => array(7, 6, 5, 4),
        8 => array(8, 6, 5, 4),
        9 => array(9, 8, 6, 5),
        10 => array(10, 9, 7, 6),
        11 => array(11, 10, 9, 7),
        12 => array(12, 11, 8, 6),
        13 => array(13, 12, 10, 9),
        14 => array(14, 13, 11, 9),
        15 => array(15, 14, 13, 11),
        16 => array(16, 14, 13, 11),
        17 => array(17, 16, 15, 14),
        18 => array(18, 17, 16, 13),
        19 => array(19, 18, 17, 14),
        20 => array(20, 19, 16, 14),
        21 => array(21, 20, 19, 16),
        22 => array(22, 19, 18, 17),
        23 => array(23, 22, 20, 18),
        24 => array(24, 23, 21, 20),
        25 => array(25, 24, 23, 22),
        26 => array(26, 25, 24, 20),
        27 => array(27, 26, 25, 22),
        28 => array(28, 27, 24, 22),
        29 => array(29, 28, 27, 25),
        30 => array(30, 29, 26, 24),
        31 => array(31, 30, 29, 28),
    );

    public static function lfsr_next($current, $n_bits) {

        if (!is_int($current) || $current < 1 || $n_bits < 5 || $n_bits > self::LFSR_MAX_BITS) {
            return 0;
        }

        $bit = (($current >> ($n_bits - self::$_lfsr_map[$n_bits][0])) ^ 
                ($current >> ($n_bits - self::$_lfsr_map[$n_bits][1])) ^
                ($current >> ($n_bits - self::$_lfsr_map[$n_bits][2])) ^
                ($current >> ($n_bits - self::$_lfsr_map[$n_bits][3]))) & 0x1;
        $next = (($current >> 1) | ($bit << $n_bits - 1)) & (0x7fffffff/*PHP_INT_MAX*/ >> (self::LFSR_MAX_BITS - $n_bits));
        return $next;
    }

    public static function lfsr_prev($current, $n_bits) {

        if (!is_int($current) || $current < 1 || $n_bits < 5 || $n_bits > self::LFSR_MAX_BITS) {
            return 0;
        }

        $bit = (($current >> ($n_bits - 1)) ^
                ($current >> ($n_bits - 1 - self::$_lfsr_map[$n_bits][1])) ^
                ($current >> ($n_bits - 1 - self::$_lfsr_map[$n_bits][2])) ^
                ($current >> ($n_bits - 1 - self::$_lfsr_map[$n_bits][3]))) & 0x1;
        $prev = (($current << 1) | $bit) & (0x7fffffff/*PHP_INT_MAX*/ >> (self::LFSR_MAX_BITS - $n_bits));
        return $prev;
    }

    public static function lfsr_step($current, $n_bits, $step) {

        if (!is_int($current) || $current < 1 || $n_bits < 5 || $n_bits > self::LFSR_MAX_BITS || !is_int($step) || $step < 1) {
            return 0;
        }
        for (;$step > 0; $step--) {
            $current = self::lfsr_next($current, $n_bits);
        }
        return $current;
    }


    public static function lfsr_rollback($current, $n_bits, $step) {

        if (!is_int($current) || $current < 1 || $n_bits < 5 || $n_bits > self::LFSR_MAX_BITS || !is_int($step) || $step < 1) {
            return 0;
        }
        for (;$step > 0; $step--) {
            $current = self::lfsr_prev($current, $n_bits);
        }
        return $current;
    }

    public static function lfsr_generate($current, $n_bits, $count) {

        $result = array();
        for (;$count > 0; $count--) {
            $ret = self::lfsr_next($current, $n_bits);
            if ($ret < 1) {
                break;
            }
            $result[] = $current = $ret;
        }
        return $result;
    }

}
