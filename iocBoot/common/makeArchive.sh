#!/bin/sh

rm $IOC_DATA/$1/iocInfo/IOC.pvlist

while ! test -f $IOC_DATA/$1/iocInfo/IOC.pvlist
do
    sleep 1    
done

cat $IOC_DATA/$1/iocInfo/IOC.pvlist | grep -v "wave" | grep -v string | grep -v subArray | grep -v sub  | grep -v bsa | grep -v compress | grep -v fanout | grep -v UNLH | grep -v CNTRST | grep -v seq | cut -f1 -d "," | sed  's/$/ 10 scan/' > $IOC_DATA/$1/archive/MAIN.archive
