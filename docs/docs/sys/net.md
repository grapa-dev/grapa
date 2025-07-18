# $net

/*
Thread Safety:
Grapa is fully thread safe in all supported environments (command line, Grapa shell, and Python/GrapaPy). All built-in operations—including map, filter, reduce, $thread, and $net—are safe to use concurrently. Users do not need to take any special precautions for thread safety in these environments.

*Note: Only if Grapa is integrated directly into a non-thread-safe C++ host (not anticipated for normal users) would additional thread safety considerations arise.*
*/

Also see [$OBJ document](../obj/document.md)

Provides a socket library, cross functional with all platforms supported. 

## type()
Returns $net.

## mac()
```
> $net().mac()
{"name":"Ethernet","ip":"104.0.97.19","mac":FB05B43EFC09ABC8}
```

## interfaces()

```
> $net().interfaces()
{
  {"name":"Ethernet","mac":"FB05B43EFC09ABC8","family":"IPV6","address":"xxx:xxx:xxx:xxx::xxx"},      
  {"name":"Ethernet","mac":"FB05B43EFC09ABC8","family":"IPV6","address":"xxx:xxx:xxxx:xxx:xxxx:xxxx:xxxx:xxx"}
  {"name":"Ethernet","mac":"FB05B43EFC09ABC8","family":"IPV4","address":"104.0.97.19"},
}
```

## connect(url)

```
n2 = $net();
n2.connect('localhost:12345');
```
## bind(url)
Using bind/listen requires setting up at least 2 net objects. One for bind and the other for listen. See listen().
```
n1 = $net();
n1.bind(':12345');
```

## listen(net)
Pass in a bound net object. Listen is blocking, so a $thread may be needed to manage - or separate threads if more than one listener.

For those familiar with using sockets in C++, the $net class is a representation of sockets, and use bind/listen in the same way.

```
n1 = $net();
n1.bind(':12345');
n2 = $net();
n2.listen('@n1);
```

## onlisten(url,messageHandler [,connectHandler [,count:1]])

Accomplishes the same as bind/listen, but handled in a background thread and much easier to setup.

When a connection is initiated, a new thread and new network object is created, and that new network object binds to the connect. The connectHandler is then called to provide a way to initialize data structures, and than the messageHandler is called for incoming data. The connectHandler receives 1 parameter - an updateable variable. The messageHandler recieves 2 parameters - the message and a hasmore flag. If the hasmore is 0, the data can be processed. If the message length is zero, the remote connection terminated and messageHandler should cleanup as the thread will be closing. 

The count defines the number of listeners - but fixed to 1 for now. Once a connection is established, the listener automatically restarts. For reasonable workloads, the 1 should be sufficient. For now if more is needed, use Bind/Listen - this though with require the use of $thread. 

The following sets up a simple web service. Use postman to post messages to verify.

```
processPost = op(in)
{
	{processed:in};
}

postHandler = op(in) 
{
	$local.data = in.split("\r").join("");
	$local.len = data.len() - data.split("\n\n")[0].len() - 2;
	if (len<0) len=0;
	$local.body = data.right(len);
	$local.rstr = processPost(body).str();
	"HTTP/1.1 200 OK\r\nContent-Type: text/json\r\nContent-Length: "+rstr.len().str()+"\r\n\r\n"+rstr;
};

postConnectHandler = op(netSession)
{
	netSession.data = "";
};

postMessageHandler = op(netSession,message,hasmore)
{
	netSession.data += message;
	if (hasmore==0)
	{
		netSession.send(postHandler(netSession.data));
		netSession.data = "";
	};
};

n=$net();
n.onlisten(':12345',postMessageHandler,postConnectHandler);
```

To very, try the following.
```
n2 = $net();
err = n2.connect('localhost:12345');
err = n2.send('POST / HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{try:55}');
n2.nreceive();
n2.receive();
n2.disconnect();
```

## disconnect()
Disconnects the session. Disconnecting a listener will disconnect all sessions the listener is a parent of.

## certificate([file])
Sets $net instance to SSL mode with certificate file.

Use certificate() to revert back to non-SSL.

See SSL_CTX_use_certificate_chain_file.

## private(file [,passOp [,param]])
Server in SSL mode requires a private key file. 

If the private key file is password protected, also provide an $OP for the SSL routines to request the password. If a param is added, the callback will include the param. 

See SSL_CTX_use_PrivateKey_file.

## trusted(file, path)

See SSL_CTX_load_verify_locations.

## verify()

Call after connecting with SSL.

Returns a list:
	result
	certificate

See the following for result codes:
https://www.openssl.org/docs/man1.0.2/man1/verify.html

Use decode("PEM") to decode the certificate.
```
> x = n2.verify();
> x.certificate.decode("PEM");
```

## chain()

Certificate chain.

Use decode(PEM) to decode.
```
> x = n2.chain();
> x[0].decode("PEM");
```

## host()

After running the sample in onlisten, try the following.
```
n.host();
{"url":":12345","host":"computer name","ip":"XX.XX.XX.XX","port":12345,"family":2,"connected":0,"bound":1}
```

## send(message)
See example in onlisten.

NOTE: If using SSL and the remote system breaks the connection, first send will return no error. The second send will return an error. This is not the case for read - where the first read after a broken connection will return an error and close the connection.

## receive()
Blocks until data is received. Use nreceive() first to verify data exists. Or use onreceive.

See example in onlisten.

## pending()
Number of bytes that can be received.

If an SSL connection, the byte count will be what is pending in the SSL encrypted buffer, which is different than the unencrypted data. 

See example in onlisten.

## onreceive(handler)
Handler will be called when data is received.

```
receiveHandler = op(netSession,message,hasmore)
{
	netSession.data += message;
	if (hasmore==0)
	{
		(netSession.data+"\n").echo();
		(@<prompt>)();
		netSession.data = "";
	};
};

n2 = $net();
n2.connect('localhost:12345');
n2.onreceive(receiveHandler);
n2.send('POST / HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{try:55}');
n2.send('POST / HTTP/1.1\r\nContent-Type: application/json\r\n\r\n{try:400}');
n2.disconnect();
```

## httpread()

## httpsend(method,entity,header,body)

## httpmessage(raw)
