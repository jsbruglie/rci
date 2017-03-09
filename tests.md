## Test Suite:

For all tests, compilation and a reset of the Identity Server(64bit example) is recommended. The following should print an empty server list:

```
killall -9 sid64 && ./sid64 && rm sid.txt
make
./rmb
show_servers
```

chmod 777 sid64

To test, always use valgrind:

valgrind --leak-check=yes ./rmb -i 127.0.0.1
valgrind --leak-check=yes ./msgserv -n aperture01 -j 127.0.0.1 -u 9001 -t 10001 -i 127.0.0.1 -r 2
valgrind --leak-check=yes ./msgserv -n aperture02 -j 127.0.0.1 -u 9002 -t 10002 -i 127.0.0.1 -r 2

To find still reachable blocks:
valgrind --leak-check=full --show-leak-kinds=all

### Test 1 - ID is sent between msgserv, msgserv have correct connections

1. Launch sid64
2. Launch rmb and do a show_servers
3. Launch aperture01
4. Launch aperture02, verify message exchange
5. show_servers on aperture01, show_servers on aperture02

### Test 2 - SGET_MESSAGES upon connect gets the correct table back


### Test 3 - Upon publish to a server, all other servers receive that publish via the first


### Test 4 - Testing LogicClocks


### Functional Tests + Protocol check

* rmb <----> Identity Server
* rmb <----> msgserv

join


* msgserv <----> Identity Server