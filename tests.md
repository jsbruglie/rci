## Test Suite

For all tests, compilation and a reset of the Identity Server(64bit example) is recommended. The following should print an empty server list:

```
killall -9 sid64 && ./sid64 && rm sid.txt
make
./rmb
show_servers
```

chmod 777 sid64

To test, try to always use valgrind:

```
valgrind --leak-check=yes ./rmb -i 127.0.0.1
valgrind --leak-check=yes ./msgserv -n aperture01 -j 127.0.0.1 -u 9001 -t 10001 -i 127.0.0.1 -r 2
valgrind --leak-check=yes ./msgserv -n aperture02 -j 127.0.0.1 -u 9002 -t 10002 -i 127.0.0.1 -r 2
```

## Testing Sequence

### 1 -- msgserv <---> sid (USE WIRESHARK TO CONFIRM)

1. Launch sid64
2. Launch a msgserv, aperture01
3. On the aperture01 terminal do a join
4. Do a show_servers on aperture01 terminal (should be empty, no tcp connections)

### 2 -- rmb <---> sid (USE WIRESHARK)

1. Launch rmb, lets call it client01
2. Do a show_servers on client01
3. rmb should have a server to publish to now

### 3 -- rmb <---> msgserv 

1. On the rmb, do a publish 'sabem qual o programa das JEEC?'
2. Confirm message (Do a show_messages on aperture01) was received and LC(aperture01)=1
3. On rmb, do show_latest_messages 10
4. Confirm you see a message

### 4 -- rmb <---> msgserv another message

1. Launch another rmb, lets call it client02
2. Do a show_latest_messages 10
3. On client02, do a publish 'vê em jeec.tecnico.ulisboa.pt'
4. Confirm aperture01, confirm messages (Do a show_messages on aperture01) and LC(aperture02)=2

### 5 -- msgserv <---> msgserv share table on start

1. Launch another msgserv, aperture02
2. Confirm the server was registered, do a show_servers on aperture02
3. aperture02 should get the entire table from aperture01, and now LC(aperture02)=3 and LC(aperture01)=2
4. Do a show_messages on both sides

### 6 -- LC Test as performed in the lab 15/03 using scanfs to control TCP propagation between servers
1. Launch 3 msgserv (aperture01, aperture02, aperture03)
2. From an rmb, publish a message A to aperture01
3. Message A is propagated to aperture02
4. Aperture02 receives message B from another rmb (may use nc to force a PUBLISH instead of creating a new rmb for this)
5. Aperture02 propagates B to aperture03
6. Aperture01 propagates A to aperture03
7. LC_aperture01 must be 1, LC_aperture02 must be 3 and LC_aperture03 must be 5 and on aperture03 A must appear before B (on aperture02 aswell)

## Unit tests

### Test 1 - ID is sent between msgserv, msgserv have correct connections

1. Launch sid64
2. Launch rmb and do a show_servers
3. Launch aperture01
4. Launch aperture02, verify ID message exchange
5. show_servers on aperture01, show_servers on aperture02

### Test 2 - SGET_MESSAGES upon connect gets the correct table back

1. Launch sid64
2. Launch rmb and do a show_servers
3. Launch aperture01
4. Do a show_servers on rmb
5. On rmb do a publish message (with spaces) 'A tweet'
6. On rmb do another publish message (with spaces) 'Another tweet'
7. On aperture01, do a show_messages
8. On rmb do a show_latest_messages 10
9. Launches aperture02
10. Do a show_servers on aperture02 (check aperture01 is there)
11. Do a show_messages on aperture02 and confirm the table was sent to aperture02 from aperture01 upon its creation
12. Do a show_servers on aperture01 (check aperture02 is there)

### Test 3 - Upon publish to a server, all other servers receive that publish via the first

1. Launch sid64
2. Launch rmb and do a show_servers
3. Launch aperture01
4. Launch aperture02, verify ID message exchange
5. Check servers on aperture01 and on aperture02, confirm TCP connection
6. From rmb, publish a message 'A tweet'
7. Check aperture01 and aperture02, check which one received from rmb and if message was propagated properly
8. From rmb, publish another message 'Another tweet'
9. Check aperture01 and aperture02, do a show_messages on each and confirm if logic clocks are the same

## Stress Tests

* Send more messages than the server has capacity for (>m)
* Request negative messages
