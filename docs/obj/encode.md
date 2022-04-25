References
- [GrapaEncode.h header](../../source/grapa/GrapaEncode.h)
- [GrapaEncode.cpp source](../../source/grapa/GrapaEncode.cpp)
- [$math](../sys/math.md)

## key = method.genkeys(options)

### "rsa"
[RSA_generate_key](https://www.openssl.org/docs/man1.0.2/man3/RSA_generate_key_ex.html)

options
- bits (default=1024)
- e (default=65537)

```
grapa: />"rsa".genkeys({bits:1024, e:65537});
{"method":"rsa","n":153343860242731826718997454922543948246319786484738710266065697186566851303969929744525118374830275413173691343298483355967655653707943038260978178989870480981813864975853444332967984168167971315164681129567478872720168346499340635885246889112254404613308384241361659956685429052350776397599870929204283799727,"e":65537,"d":79055022445659402007335810450096775552716887040295814635390111095145249324919236710228881006612913550893380981980039628730017876494656017726225944634797947775686645431513668671692142235308537648915581979668299541155091041081197126102417671890829285804060548022166826534910977714874080213766562525384158510233,"p":12927549674691586875163180066905699166335405091224131481044213102404425715203497636737871644864054092888178130162387310855633573554296177378467548678038211,"q":11861788513791973340894513760762175330214406628819766321709624619984772343399257289407679201931012919105862007733677390652191686844294289003006359142734757,"dmp1":421535524281183471202888685825983476791106713458748019820734598773795836754655758574680435556624251895906688804141503018119366871927780201379146917389683,"dmq1":2422782566269730917515509730405152493557075348785897919990311353328900660523711156690284782596831391964097667508781383817846985978877936930195814936366441,"iqmp":4751564343152764182762436394942179636976979980491913680362394619885466773528151935033468867078538523266366855637186571425698958155097556787751966441250519}

grapa: />nodeA = "rsa".genkeys();
grapa: />tstr = "this is a test of this is a test".left(nodeA.n.bytes()-42);
grapa: />enc = tstr.encode(nodeA);
grapa: />enc;
0xC492D258D8213815771868C08197E89834FD553EF26DE0FD1E957E37282B5F75805A0538EA47C93A0B767999CD0B034176FBEB85960211D9D12AB598FBBA0D54162E2AECE5F5DFB0168D8ADD5BC5CA33964216FDE2E5C7525AFABD7DE0111C4C2F253A1C6F14810EF30E93E99364220D58DF55A970994A9DFC0EBD3850000917
grapa: />enc.decode(nodeA).str();
this is a test of this is a test

grapa: />nodeA = "rsa".genkeys();
grapa: />sig = "test".sign(nodeA)
grapa: />sig
0xA86168258CCCAF03D502AF220D29C56DF689E29A881792203A30F55E9F5DC620D9BA72A69FA7BB8E5DB6DB40E74DC7F37AE10E94267EB69BD178310F9AE713E36559B789A2A1D88D80D7B7631C53C8F0D65DD74F2129F3C065F0CFB7982013625E22EF052AF829A44C07CDCA33334229B6A23058ECA50D6B7C7B8DCB54AD6402
grapa: />sig.verify(nodeA,"test")
1
grapa: />sig.verify(nodeA,"test2")
0
grapa: />sig.verifyrecover(nodeA).str()
test
```

### "dh" - Diffie-Hellman key exchange
[DH_generate_key](https://www.openssl.org/docs/man1.0.2/man3/DH_generate_key.html)

options
- bits (default=1024)
- g (default=2)
- p (from other node)

```
grapa: />"dh".genkeys({bits:1024, g:2});
{"method":"dh","p":155060894798916672436294955495808726921562912418691946735299981488630632728917641139720322303293804373154953309741508291858037753320020908034625492504226207533520275246218001191529318164644965512204584496862649121961720039055278601771796332158129094227178861323999156687238560468911736859258982900594036495419,"g":2,"pub":32452604788320682188849350932531192941087970270462344515271303858654517731576279803429572719652061802609399632204622690115682073041731861373289670753479025573602436383635155882438039975820160596493133927013148569271558813144843665089688971452982482929762826761875748093517340605310796965292388347247601517927,"prv":85108738216648176726657906323697936988639742459594685112244281495890189161229363404509137286685931354323324249119565623639407205879234061660167646352177376062162362276676470273884923897379280600025611946952284789763517326405782930856345819438343010650014807653972154357078710716080052243407709981947131030820}

grapa: />nodeA = "dh".genkeys();
grapa: />nodeB = "dh".genkeys({p:nodeA.p, g:nodeA.g});
grapa: />nodeA.secret({method:nodeB.method, pub:nodeB.pub});
41411399464032750193815617500918575158768059683378407298669269983997192654320169347707063521253943511427030448681025344568888491017373725518403783432343686587216160306110643977085792902295674526020669661500697334078419157529204684347818535354141358922052361205641014829108645061072280570433902877036291304733
grapa: />nodeB.secret({method:nodeA.method, pub:nodeA.pub});
41411399464032750193815617500918575158768059683378407298669269983997192654320169347707063521253943511427030448681025344568888491017373725518403783432343686587216160306110643977085792902295674526020669661500697334078419157529204684347818535354141358922052361205641014829108645061072280570433902877036291304733
grapa: />nodeA.secret({method:b.method, pub:nodeB.pub}) == b.secret({method:nodeA.method, pub:nodeA.pub});
true
```

### "ec"
[EC_KEY_generate_key](https://www.openssl.org/docs/man1.0.2/man3/EC_KEY_generate_key.html)

options
- curve (default="prime256v1")

curve
- "secp224r1"
- "prime256v1"

```
grapa: />"ec".genkeys()
{"method":"ec","curve":"prime256v1","pub":349854664772403873294259339017252697866055805081789380369591127264988512574728,"prv":109253108998027222598743004106159085619823908146992540608529858792290817953562}

grapa: />"ec".genkeys({curve:"secp224r1", bits:256})
{"method":"EC","curve":"secp224r1","pub":94005469537357239932833357907049107441874311457299400778270964405582,"prv":5192160124916624753659544220564622206602718051022940651336084962189}

grapa: />nodeA = "ec".genkeys({curve:"prime256v1"});
grapa: />sig = "test".sign(nodeA)
grapa: />sig
0x304402207601D7909310BC4DE38DB17D8FE52AB71B7A21CE2976238E1A4870CCD5EE8FE202207072F45DBF7C35D46CE23AAEC4BF89B2FA62E158DD1FA69B28FEF935ACC01807
grapa: />sig.verify(nodeA,"test")
1
grapa: />sig.verify(nodeA,"test2")
0

grapa: />nodeA = "ec".genkeys({curve:"prime256v1"});
grapa: />nodeB = "ec".genkeys({curve:nodeA.curve});
grapa: />nodeA.secret({method:nodeB.method, curve:nodeB.curve, pub:nodeB.pub});
-32833883556403339571501877545325088412797888782971835903758327481485027662328
grapa: />nodeB.secret({method:nodeA.method, curve:nodeA.curve, pub:nodeA.pub});
-32833883556403339571501877545325088412797888782971835903758327481485027662328
grapa: />nodeA.secret({method:nodeB.method, curve:nodeB.curve, pub:nodeB.pub}) == nodeB.secret({method:nodeA.method, curve:nodeA.curve, pub:nodeA.pub});
true
```

### "bc"
[EVP_CipherInit_ex](https://www.openssl.org/docs/man1.0.2/man3/EVP_CipherInit_ex.html)

options
- cipher (default="aes-256-cbc")
- key
- iv
- digest (default="sha512")
- count
- salt
- data

cyper
- "des-ecb"
- "des-ede"
- "des-ede3"
- "des-ede-ecb"
- "des-ede3-ecb"
- "des-cfb64"
- "des-cfb1"
- "des-cfb8"
- "des-ede-cfb64"
- "des-ede3-cfb64"
- "des-ede3-cfb1"
- "des-ede3-cfb8"
- "des-ofb"
- "des-ede-ofb"
- "des-ede3-ofb"
- "des-cbc"
- "des-ede-cbc"
- "des-ede3-cbc"
- "desx-cbc"
- "des-ede3-wrap"
- "rc4"
- "rc4-40"
- "rc4-hmac-md5"
- "idea-ecb"
- "idea-cfb64"
- "idea-ofb"
- "idea-cbc"
- "rc2-ecb"
- "rc2-cbc"
- "rc2-40-cbc"
- "rc2-64-cbc"
- "rc2-cfb64"
- "rc2-ofb"
- "bf-ecb"
- "bf-cbc"
- "bf-cfb64"
- "bf-ofb"
- "cast5-ecb"
- "cast5-cbc"
- "cast5-cfb64"
- "cast5-ofb"
- "rc5-32-12-16-cbc"
- "rc5-32-12-16-ecb"
- "rc5-32-12-16-cfb64"
- "rc5-32-12-16-ofb"
- "aes-128-ecb"
- "aes-128-cbc"
- "aes-128-cfb1"
- "aes-128-cfb8"
- "aes-128-cfb128"
- "aes-128-ofb"
- "aes-128-ctr"
- "aes-128-ccm"
- "aes-128-gcm"
- "aes-128-xts"
- "aes-128-wrap"
- "aes-128-wrap-pad"
- "aes-128-ocb"
- "aes-192-ecb"
- "aes-192-cbc"
- "aes-192-cfb1"
- "aes-192-cfb8"
- "aes-192-cfb128"
- "aes-192-ofb"
- "aes-192-ctr"
- "aes-192-ccm"
- "aes-192-gcm"
- "aes-192-wrap"
- "aes-192-wrap-pad"
- "aes-192-ocb"
- "aes-256-ecb"
- "aes-256-cbc"
- "aes-256-cfb1"
- "aes-256-cfb8"
- "aes-256-cfb128"
- "aes-256-ofb"
- "aes-256-ctr"
- "aes-256-ccm"
- "aes-256-gcm"
- "aes-256-xts"
- "aes-256-wrap"
- "aes-256-wrap-pad"
- "aes-256-ocb"
- "aes-128-cbc-hmac-sha1"
- "aes-256-cbc-hmac-sha1"
- "aes-128-cbc-hmac-sha256"
- "aes-256-cbc-hmac-sha256"
- "aria-128-ecb"
- "aria-128-cbc"
- "aria-128-cfb1"
- "aria-128-cfb8"
- "aria-128-cfb128"
- "aria-128-ctr"
- "aria-128-ofb"
- "aria-128-gcm"
- "aria-128-ccm"
- "aria-192-ecb"
- "aria-192-cbc"
- "aria-192-cfb1"
- "aria-192-cfb8"
- "aria-192-cfb128"
- "aria-192-ctr"
- "aria-192-ofb"
- "aria-192-gcm"
- "aria-192-ccm"
- "aria-256-ecb"
- "aria-256-cbc"
- "aria-256-cfb1"
- "aria-256-cfb8"
- "aria-256-cfb128"
- "aria-256-ctr"
- "aria-256-ofb"
- "aria-256-gcm"
- "aria-256-ccm"
- "camellia-128-ecb"
- "camellia-128-cbc"
- "camellia-128-cfb1"
- "camellia-128-cfb8"
- "camellia-128-cfb128"
- "camellia-128-ofb"
- "camellia-128-ctr"
- "camellia-192-ecb"
- "camellia-192-cbc"
- "camellia-192-cfb1"
- "camellia-192-cfb8"
- "camellia-192-cfb128"
- "camellia-192-ofb"
- "camellia-192-ctr"
- "camellia-256-ecb"
- "camellia-256-cbc"
- "camellia-256-cfb1"
- "camellia-256-cfb8"
- "camellia-256-cfb128"
- "camellia-256-ofb"
- "camellia-256-ctr"
- "chacha20"
- "chacha20-poly1305"
- "seed-ecb"
- "seed-cbc"
- "seed-cfb128"
- "seed-ofb"
- "sm4-ecb"
- "sm4-cbc"
- "sm4-cfb128"
- "sm4-ofb"
- "sm4-ctr"

```
grapa: />nodeA = "bc".genkeys({cipher:"aes-256-cbc", key:"thisisatest"})
grapa: />nodeA
{"method":"bc","cipher":"aes-256-cbc","key":0x7468697369736174657374,"iv":null}
grapa: />enc = "this is a test".encode(nodeA);
grapa: />enc
0x4E12DB388BBE0173A35A82D6972C0BBA
grapa: />enc.decode(nodeA).str();
this is a test

grapa: />enc = "this is a test".encode({method:"bc", cipher:"aes-256-cbc", key:"thisisatest"});
grapa: />enc
0x4E12DB388BBE0173A35A82D6972C0BBA
grapa: />enc.decode({method:"bc", cipher:"aes-256-cbc", key:"thisisatest"}).str();
this is a test
```

### "md"

digest
- "md4"
- "md5"
- "md5-sha1"
- "blake2b512"
- "blake2s256"
- "sha1"
- "sha224"
- "sha256"
- "sha384"
- "sha512"
- "sha512-224"
- "sha512-256"
- "sha3-224"
- "sha3-256"
- "sha3-384"
- "sha3-512"
- "shake128"
- "shake256"
- "mdc2"
- "ripemd160"
- "whirlpool"
- "sm3"

```
grapa: />nodeA = "md".genkeys({digest:"sha3-256"});
grapa: />nodeA;
{"method":"md","digest":"sha3-256"}
grapa: />"test".encode(nodeA);
0x36F028580BB02CC8272A9A020F4200E346E276AE664E45EE80745574E2F5AB80

grapa: />"test".encode({method:"md", digest:"sha3-256"});
0x36F028580BB02CC8272A9A020F4200E346E276AE664E45EE80745574E2F5AB80
```

### "rpk"
[EVP_PKEY_keygen_init](https://www.openssl.org/docs/man1.0.2/man3/EVP_PKEY_keygen_init.html)

options
- alg (default="ED25519")
- pub
- prv

alg
- "ED25519"
- "X25519"
- "ED448"
- "X448"

```
grapa: />"rpk".genkeys({alg:"ED25519"})
{"method":"rpk","alg":"ed25519","pub":0x60BEEACB0D9B67B617E98B3DFFF047B0951413A5817685E6FD2125853B07DDB5,"prv":0x1C8390953C4E0B5B5720E55C3C4A1C21FF4B105C61B69ADAA065C7E655722B59}

grapa: />g="rpk".genkeys()
grapa: />g
{"method":"rpk","alg":"ed25519","pub":0x1780BB5267C0EC8A10C2A99A836930991B909608045F123668596C9AF28F2FC,"prv":0xF0A90D5C818DF2E62B0C3B49DF668501A77B3D6E1C8DAD98F7F49FDE5DE2C723}
grapa: />"test".sign(g)
0xA3FDB87097F7ED99F5B1E15387E392DCF7870C32904544D9039941A1C9D6A111B48392931F6447BBEC6FDDB7CCB2E3408060E504379B3E19DC27F21F4E579A08
grapa: />"test".sign(g).verify(g,"test")
1
grapa: />"test".sign(g).verify(g,"test2")
0

grapa: />nodeA = "rpk".genkeys({alg:"X25519"});
grapa: />nodeB = "rpk".genkeys({alg:nodeA.alg});
grapa: />nodeA.secret({method:nodeB.method, alg:nodeB.alg, pub:nodeB.pub});
33649166166276908480251638309469536773081960395558481192191946345661575379713
grapa: />nodeB.secret({method:nodeA.method, alg:nodeA.alg, pub:nodeA.pub});
33649166166276908480251638309469536773081960395558481192191946345661575379713
grapa: />nodeA.secret({method:nodeB.method, alg:nodeB.alg, pub:nodeB.pub}) == nodeB.secret({method:nodeA.method, alg:nodeA.alg, pub:nodeA.pub});
true
```

## encdata = rawdata.encode (method, options)

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
g = {"method":"rsa","n":n,"e":e, "d":d};
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


## rawdata = encdata.decode (method, options)
See encode.

## sig = rawdata.sign (method, options)

```
curve = "prime256v1";
pub = 0x25074E1882EF3F6230925ADFFEA56EC322F7EB4F23C3DECF12BFF5A24E11BA21C;
prv = 0x5A44899AE665F34833146FD9805222B9E7C22B8BE94B7F9C34A54AA9B9D89890;
g = {"method":"ec","curve":curve,"pub":pub,"prv":prv};
"test".sign(g).verify(g,"test");
```

## sig = sig.signadd (method, value, options)
Not implemented yet. Used for pairwise eliptic curves where sigatures can be added.

## ispass = sig.verify (method, value, options)
See sign.

## rawdata = sig.verifyrecover (method, options)
Recovers signed data.

```
a = "rsa".genkeys();
b = "hello".sign(a);
b.verifyrecover(a).str();
```

## value = key.secret (key)
Diffie-Hellman key exchange. Node A generates the staring keys and sends "p" and "g" to node B. Node B then generates its keys using "p" and "g" from node A. Both nodes can then generate the shared secret, and they will be equal.  

```
grapa: />nodeA = "dh".genkeys();
grapa: />nodeB = "dh".genkeys({p:nodeA.p, g:nodeA.g});
grapa: />nodeA.secret({method:nodeB.method, pub:nodeB.pub});
41411399464032750193815617500918575158768059683378407298669269983997192654320169347707063521253943511427030448681025344568888491017373725518403783432343686587216160306110643977085792902295674526020669661500697334078419157529204684347818535354141358922052361205641014829108645061072280570433902877036291304733
grapa: />nodeB.secret({method:nodeA.method, pub:nodeA.pub});
41411399464032750193815617500918575158768059683378407298669269983997192654320169347707063521253943511427030448681025344568888491017373725518403783432343686587216160306110643977085792902295674526020669661500697334078419157529204684347818535354141358922052361205641014829108645061072280570433902877036291304733
grapa: />nodeA.secret({method:b.method, pub:nodeB.pub}) == b.secret({method:nodeA.method, pub:nodeA.pub});
true

grapa: />nodeA = "ec".genkeys({curve:"prime256v1"});
grapa: />nodeB = "ec".genkeys({curve:nodeA.curve});
grapa: />nodeA.secret({method:nodeB.method, curve:nodeB.curve, pub:nodeB.pub});
-32833883556403339571501877545325088412797888782971835903758327481485027662328
grapa: />nodeB.secret({method:nodeA.method, curve:nodeA.curve, pub:nodeA.pub});
-32833883556403339571501877545325088412797888782971835903758327481485027662328
grapa: />nodeA.secret({method:nodeB.method, curve:nodeB.curve, pub:nodeB.pub}) == nodeB.secret({method:nodeA.method, curve:nodeA.curve, pub:nodeA.pub});
true

grapa: />nodeA = "rpk".genkeys({alg:"X25519"});
grapa: />nodeB = "rpk".genkeys({alg:nodeA.alg});
grapa: />nodeA.secret({method:nodeB.method, alg:nodeB.alg, pub:nodeB.pub});
33649166166276908480251638309469536773081960395558481192191946345661575379713
grapa: />nodeB.secret({method:nodeA.method, alg:nodeA.alg, pub:nodeA.pub});
33649166166276908480251638309469536773081960395558481192191946345661575379713
grapa: />nodeA.secret({method:nodeB.method, alg:nodeB.alg, pub:nodeB.pub}) == nodeB.secret({method:nodeA.method, alg:nodeA.alg, pub:nodeA.pub});
true
```

