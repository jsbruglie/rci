# rci
Computer Networks and Internet

## Questions:

* Need to add some sort of checking that the user should never do a publish message in rmb before show servers? Or do a "show_servers" routine at the beggining of the publish message command? 

## TODO:
* Get rmb to publish a msg to msgserv successfully (test that msgserv receives a publish message), need to setup UDP server on msgserver
* Get msgserver and rmb to communicate
* Use a thread for the interfaces and another for the respective server services in msgserv (rmb can also have the same thing)
