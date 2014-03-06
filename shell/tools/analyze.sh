#!/bin/sh
logfile="/data/log/apache-2.0.59-1.0/access_log."`date '+%Y%m%d'`
echo "COUNT,BYTES_ALL,BYTES_AVE,TIME_ALL,TIME_AVE,URI" > /home/admin/logs/`date '+%Y%m%d'`

awk '{
    a[$6]++;
    b[$6] += $7;
    c[$6] += $8;
}
END {
    OFS=","
    for (url in a) {
        print a[url], b[url], b[url]/a[url], c[url], c[url]/a[url], url;
    }
}' $logfile | sort -n -t, -r -k5 >> /home/admin/logs/`date '+%Y%m%d'`
