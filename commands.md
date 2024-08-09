## Setup Garak
Unzip archive
```cd <archive>/backend/app```\
```export FLASK_APP=run.py```\
```flask db init```\
```flask db migrate -m "initial"```\
```flask db upgrade```\
```python setup-db.py```

## Start Garak
```cd /home/Operator/Desktop/garak/```\
```./start-garak.sh```

## Stop Garak
```ctrl+c in window where Garak is running```\
```pkill flask```

## Put IPTables rules on Ground Station
```ssh -i ~/.ssh/groundstation.pem ubuntu@cosmos.groundstation.earth```
```sudo iptables -t nat -A OUTPUT -p udp -d 10.10.10.10 --dport 5012 -j DNAT --to-destination 10.10.20.20:5012```\
```sudo iptables -t nat -A PREROUTING -p udp -d 10.10.10.10 --dport 5012 -j DNAT --to-destination 10.10.20.20:5012```

## Remove IPTables rules from Ground Station
```ssh -i ~/.ssh/groundstation.pem ubuntu@cosmos.groundstation.earth```
```sudo iptables -t nat -D OUTPUT -p udp -d 10.10.10.10 --dport 5012 -j DNAT --to-destination 10.10.20.20:5012```\
```sudo iptables -t nat -D PREROUTING -p udp -d 10.10.10.10 --dport 5012 -j DNAT --to-destination 10.10.20.20:5012```

## Give Garak access to raw sockets (for cloaking device)
```sudo setcap cap_net_raw+ep $(realpath $(which python3))```