<?php

class LFSR_UTIL {

    // 因为php不支持无符号整数，所以最大位数整数位数少1
    // 64位机器最高支持63位
    const LFSR_MAX_BITS = 63;

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
        32 => array(32, 30, 26, 25),
        33 => array(33, 32, 29, 27),
        34 => array(34, 31, 30, 26),
        35 => array(35, 34, 28, 27),
        36 => array(36, 35, 29, 28),
        37 => array(37, 36, 33, 31),
        38 => array(38, 37, 33, 32),
        39 => array(39, 38, 35, 32),
        40 => array(40, 37, 36, 35),
        41 => array(41, 40, 39, 38),
        42 => array(42, 40, 37, 35),
        43 => array(43, 42, 38, 37),
        44 => array(44, 42, 39, 38),
        45 => array(45, 44, 42, 41),
        46 => array(46, 40, 39, 38),
        47 => array(47, 46, 43, 42),
        48 => array(48, 44, 41, 39),
        49 => array(49, 45, 44, 43),
        50 => array(50, 48, 47, 46),
        51 => array(51, 50, 48, 45),
        52 => array(52, 51, 49, 46),
        53 => array(53, 52, 51, 47),
        54 => array(54, 51, 48, 46),
        55 => array(55, 54, 53, 49),
        56 => array(56, 54, 52, 49),
        57 => array(57, 55, 54, 52),
        58 => array(58, 57, 53, 52),
        59 => array(59, 57, 55, 52),
        60 => array(60, 58, 56, 55),
        61 => array(61, 60, 59, 56),
        62 => array(62, 59, 57, 56),
        63 => array(63, 62, 59, 58),
    );

    public static function lfsr_next($current, $n_bits) {

        if (!is_int($current) || $current < 1 || $n_bits < 5 || $n_bits > self::LFSR_MAX_BITS) {
            return 0;
        }

        $bit = (($current >> ($n_bits - self::$_lfsr_map[$n_bits][0])) ^ 
                ($current >> ($n_bits - self::$_lfsr_map[$n_bits][1])) ^
                ($current >> ($n_bits - self::$_lfsr_map[$n_bits][2])) ^
                ($current >> ($n_bits - self::$_lfsr_map[$n_bits][3]))) & 0x1;
        $next = (($current >> 1) | ($bit << $n_bits - 1)) & (0x7fffffffffffffff/*PHP_INT_MAX*/ >> (self::LFSR_MAX_BITS - $n_bits));
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
        $prev = (($current << 1) | $bit) & (0x7fffffffffffffff/*PHP_INT_MAX*/ >> (self::LFSR_MAX_BITS - $n_bits));
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

function test() {

    $initial = 1234;
    $result = LFSR_UTIL::lfsr_generate($initial, 23, 10);
    var_dump($result);
    var_dump(LFSR_UTIL::lfsr_rollback($result[9], 23, 10));
}

test();
