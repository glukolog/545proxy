#!/bin/bash
#
cd /opt/afree-cpuminer-proxy
ps -fe|grep afree-cpuminer-proxy |grep -v grep
if [ $? -ne 0 ]
then
  echo "" >> run.log
  echo "check-afree-cpuminer-proxy.sh $(date "+%Y-%m-%d %H:%M:%S"): afree-cpuminer-proxy start" >> run.log
  nohup ./afree-cpuminer-proxy >> run.log 2>&1 &
else
  echo "check-afree-cpuminer-proxy.sh $(date "+%Y-%m-%d %H:%M:%S"): afree-cpuminer-proxy running..."
fi
