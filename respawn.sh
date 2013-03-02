#!/bin/sh

while [ 1 ]; do
        if [ -x /home/linaro/agrcv/ozdoken-pancar ]
        then
                (/home/linaro/agrcv/ozdoken-pancar 2>&1) >> /dev/null
        else
                sleep 2;
        fi
done
