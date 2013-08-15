#!/usr/bin/env bash
ps aux|grep php-debug|grep Server|awk '{print $2}'|xargs kill
