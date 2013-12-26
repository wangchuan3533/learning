#!/usr/bin/env bash
PORT=`openssl rand 2 -hex|tr abcdef ABCDEF`
PORT=`echo "16 i 8000 $PORT 1 400 | + p" | dc`
sed -i "s@^remote vps [0-9]*@remote vps $PORT@" /etc/openvpn/client.conf

