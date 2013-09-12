#!/usr/bin/env bash
case $1 in
start)
    ./server -c54574 -r54575 -l/var/log/customerlive.tcp.log -p/var/tmp/customerlive.tcp.pid -t20 -d
    ;;
stop)
    cat /var/tmp/customerlive.tcp.pid |xargs kill
    ;;
restart)
    cat /var/tmp/customerlive.tcp.pid |xargs kill
    ./server -c54574 -r54575 -l/var/log/customerlive.tcp.log -p/var/tmp/customerlive.tcp.pid -t20 -d
    ;;
*)
    echo "Usage"
    echo "$0 [start|stop|restart]"
    ;;
esac

