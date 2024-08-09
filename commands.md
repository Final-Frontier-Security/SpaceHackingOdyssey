## Start Garak
```cd /home/Operator/Desktop/garak/```\
```./start-garak.sh```\

## Stop Garak
```ctrl+c in window where Garak is running```\
```pkill flask```\

## Put IPTables rules on Ground Station
```ssh -i ~/.ssh/groundstation.pem ubuntu@cosmos.groundstation.earth```
```sudo iptables -t nat -A OUTPUT -p udp -d 10.10.10.10 --dport 5012 -j DNAT --to-destination 10.10.20.20:5012```\
```sudo iptables -t nat -A PREROUTING -p udp -d 10.10.10.10 --dport 5012 -j DNAT --to-destination 10.10.20.20:5012```\