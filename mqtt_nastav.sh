mosquitto_pub -t "/thermctl-in/TERM D1/tds/set/0/name" -m "vnitrni"
mosquitto_pub -t "/thermctl-in/TERM D1/tds/associate" -m "0"

mosquitto_pub -t "/thermctl-in/TERM D1/ring/set/0/name" -m "hlavni"
mosquitto_pub -t "/thermctl-in/TERM D1/ring/set/0/tds" -m "0"
mosquitto_pub -t "/thermctl-in/TERM D1/ring/set/0/active" -m "1"
