# Part of $OBJ

## setbit(bitpos)
Sets a bit, from low order.

```
(0b100110).setbit(3).bin() -> 101110
```

## clearbit(bitpos)
Clears a bit, from low order. 

```
(0b100110).clearbit(2).bin() -> 100010
```

## genbits
Generates sequence of bits.

```
(4).genbits() -> 15
(4).genbits().bin() -> 1111
```

## encode (method, [,options])

encode/decode types:
Method | Options | Description
------------ | ------------- | -------------
"RSA" | {n,e,d [,p,q,dp,dq,qi]} | Data size must be exactly byte count of d  minus 32.
"AES256" | {key [,iv]} | key is 32 bits and iv is 16 bits
"ZIP-GRAPA" | |
"BASE64" | |
"ESCAPE" | |
"FLOAT" | |
"JSON" | | Only encode.
"XML" | | Only encode.
"SHAKE256" | | Only encode.
"SHAKE128" | | Only encode.
SHA3-256" | | Only encode.
SHA3-384" | | Only encode.
"SHA3-512" | | Only encode.
"PEM" | | Only decode.

Example of RSA using hard coded RSA key.

```
e = 65537;
d = (0xA4954A19EA13D5FF06C86E7719AB6B6D5F37EF2D0A453713F08CD3FF2847C7BE80F2A3D0159C2EA862997DEE93DC08CD18CDFBD96DCC077B978904D05B2FCA5988A63).uraw();
d = (d + (0x2CEB3B7286ACEF97682F418172B30509BD29A1C56E0244D79CFADD5A60C83D2F28A8B72643CB82AABA08871E094B36E6613B108166768B33F5C4860BD21).uraw()).uint();
n = (0xB47D8ABBD7A33B5AD29C8EFD291433D62695BA043F4130F8E677CD179315FD0DD39989539EC710EB3BCBCACBDED2230DE8575FAEE3ED2835B256ADB1AAEF2F34C430E).uraw();
n = (n + (0xAB9934FAF1977FA83E12B947806CE1F8D06692F02068FD18E4DE952895DDD2D9BDEE923E94257E84B63D21C92EC1F74B8AF4AF9A33608C7E8247F1C820F).uraw()).uint();
g = {"method":"RSA","n":n,"e":e, "d":d};
len = g.n.bytes()-42;
v = "this is a test of this is a test of 86 chars to see if we can encode with RSA. It needs to be exact size chars to see if we can encode with RSA. It needs to be exact size...so need to make it so.".left(len);
v.encode(g).decode(g).str();
```

```
v.encode("AES256",{key:"12345678123456781234567812345678",iv:"1234567812345678"}).decode("AES256",{key:"12345678123456781234567812345678",iv:"1234567812345678"}).str();
v.encode("SHA3-256");
v.encode("SHAKE128");
{1,2,3}.encode("ZIP-GRAPA").decode("ZIP-GRAPA");
v.encode("BASE64").decode("BASE64").str();
```

## decode (method [,options])
See encode.

## sign (method, [,options])

```
curve = "prime256v1";
pub = 0x25074E1882EF3F6230925ADFFEA56EC322F7EB4F23C3DECF12BFF5A24E11BA21C;
prv = 0x5A44899AE665F34833146FD9805222B9E7C22B8BE94B7F9C34A54AA9B9D89890;
g = {"method":"EC","curve":curve,"pub":pub,"prv":prv};
"test".sign(g).verify(g,"test");
```

## signadd (method, value, [,options])
Not implemented yet. Used for pairwise eliptic curves where sigatures can be added.

## verify (method, value, [,options])
See sign.

## verifyrecover (method, [,options])
Recovers signed data.

```
a = "RSA".genkeys();
b = "hello".sign(a);
b.verifyrecover(a).str();
```

## secret (method)
Diffie-Hellman key exchange. Node A generates the staring keys and sends "p" and "g" to node B. Node B then generates its keys using "p" and "g" from node A. Both nodes can then generate the shared secret, and they will be equal.  

```
a = "dh".genkeys()
b = "dh".genkeys({p:a.p,g:a.g})
a.secret(b)==b.secret(a)
```
