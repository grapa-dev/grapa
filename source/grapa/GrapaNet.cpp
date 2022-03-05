// GrapaNet.cpp
/*
Copyright 2022 Chris Ernest Matichuk

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.
*/
////////////////////////////////////////////////////////////////////////////////

#include "GrapaMem.h"
#include "GrapaNet.h"
#include "GrapaState.h"
#include "GrapaSystem.h"
#include "GrapaState.h"

#include <openssl/err.h>

extern GrapaSystem* gSystem;

// http://msdn.microsoft.com/en-us/library/ms740673(v=vs.85).aspx

// http://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/socket.h.html

#if defined(__MINGW32__) || defined(__GNUC__)
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h> 
#include <sys/types.h>
#include <ifaddrs.h>
//#include <net/if_dl.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#include <ws2def.h>
#endif


#ifdef __APPLE__
#include <sys/sysctl.h>
#include <net/if_dl.h>
#endif

// https://wiki.openssl.org/index.php/Simple_TLS_Server


typedef struct _AcureNetPrivate {
	struct addrinfo mHints;
	struct sockaddr_storage mSockAddr;
	char mServerInfo[46];
#if defined(__MINGW32__) || defined(__GNUC__)
	int mSocket;
#endif
#ifdef _WIN32
	WSADATA wsaData;
	SOCKET mSocket;
#endif
}AcureNetPrivate;

///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaNet::GrapaNet()
{
	vInstance = (void*)GrapaMem::Create(sizeof(AcureNetPrivate));

	mStarted = false;
	mConnected = false;
	mBound = false;
	((AcureNetPrivate*)vInstance)->mSocket = 1;
#ifdef _WIN32
	memset(&((AcureNetPrivate*)vInstance)->wsaData, 0, sizeof(((AcureNetPrivate*)vInstance)->wsaData));
	memset(&((AcureNetPrivate*)vInstance)->mHints, 0, sizeof(((AcureNetPrivate*)vInstance)->mHints));
#endif
	mHostName[0] = 0;
	mIpName[0] = 0;
	((AcureNetPrivate*)vInstance)->mServerInfo[0] = 0;
	memset(&((AcureNetPrivate*)vInstance)->mSockAddr, 0, sizeof(((AcureNetPrivate*)vInstance)->mSockAddr));
	mFamily = 0;
	mPort = 0;
	mMAC = 0;
	mName[0] = 0;

	vIsSSL = false;
	vIsBlocking = false;
    vCriticalError = false;
	vPassOp = NULL;
	vPassParam = NULL;

	vDelCTX = NULL;
	vXCTX = NULL;
	vBIO = NULL;
	aBIO = NULL;
	vSSL = NULL;
	BIO_sock_init();

	vIsProxy = false;
}

GrapaNet::~GrapaNet()
{
	Disconnect();
	Shutdown();
	if (vInstance)
	{
		GrapaMem::Delete(vInstance);
		vInstance = NULL;
	}
}

GrapaError GrapaNet::Startup()
{
#ifdef _WIN32
	if (!mStarted && WSAStartup(MAKEWORD(2, 2), &((AcureNetPrivate*)vInstance)->wsaData) != 0) return(-1);
#endif
	mStarted = true;
	return(0);
}

void GrapaNet::Shutdown()
{
	if (!mStarted) return;
#ifdef _WIN32
	WSACleanup();
#endif
	mStarted = false;
}

void GrapaNet::GetService(const GrapaCHAR& pURL, GrapaCHAR& pNodeName, GrapaCHAR& pServiceName, bool& isIPv6)
{
	GrapaCHAR u(pURL);
	pNodeName.SetLength(0);
	pServiceName.SetLength(0);
	isIPv6 = false;
	char* s1 = strchr((char*)u.mBytes, '[');
	char* s2 = s1 ? strchr(s1, ']') : NULL;
	if (s2)
	{
		isIPv6 = true;
		pNodeName.SetLength(s2 - s1 - 1);
		memcpy((char*)pNodeName.mBytes, &s1[1], (size_t)pNodeName.mLength);
		s1 = strchr(s2, ':');
	}
	else
	{
		s1 = (char*)u.mBytes;
		s2 = strchr((char*)u.mBytes, ':');
		if (s2)
		{
			pNodeName.SetLength(s2 - s1);
			memcpy((char*)pNodeName.mBytes, &s1[0], (size_t)pNodeName.mLength);
			s1 = s2;
		}
		else
		{
			pNodeName.FROM(u);
			s1 = NULL;
		}
	}
	if (s1)
	{
		pServiceName.SetLength(u.mLength - (((char*)u.mBytes) - s1) - 1);
		memcpy((char*)pServiceName.mBytes, &s1[1], (size_t)pServiceName.mLength);
	}
	pNodeName.Trim();
	pServiceName.Trim();
}

GrapaError GrapaNet::Connect(const GrapaCHAR& pURL)
{
	vpResponse.SetLength(0);
	bool isIPv6 = false;
	GrapaCHAR pNodeName, pServiceName;
	GetService(pURL, pNodeName, pServiceName, isIPv6);
	char* nodeName = pNodeName.mLength ? (char*)pNodeName.mBytes : NULL;
	char* serviceName = pServiceName.mLength ? (char*)pServiceName.mBytes : NULL;
	if (mBound || mConnected) return(-1);
	if (Startup()) return(-1);
	int iResult = 0;


	if (gSystem->mLinkInitialized && vIsSSL)
	{
		vXCTX = vDelCTX = SSL_CTX_new(TLS_client_method());
		vCriticalError = false;
		if (vCertFile.mLength)
			iResult = SSL_CTX_use_certificate_chain_file(vXCTX, (char*)vCertFile.mBytes);
		if (vKeyFile.mLength)
			iResult = SSL_CTX_use_PrivateKey_file(vXCTX, (char*)vKeyFile.mBytes, SSL_FILETYPE_PEM);
		if (vPassOp)
		{
			SSL_CTX_set_default_passwd_cb(vXCTX, (pem_password_cb*)vPassOp);
			SSL_CTX_set_default_passwd_cb_userdata(vXCTX, vPassParam);
		}
		if (vcaFile.mLength || vcaPath.mLength)
			iResult = SSL_CTX_load_verify_locations(vXCTX, (char*)vcaFile.mBytes, (char*)vcaPath.mBytes);

		SSL_CTX_set_ciphersuites(vXCTX,"TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_CCM_SHA256:TLS_AES_128_CCM_8_SHA256");

		//BIO_new_socket(0, 1);
		//BIO_new_ssl(vXCTX, 1);
		//vBIO = BIO_push(sslbio, socketbio);


		if (vIsProxy)
		{
			// https://gist.github.com/CarlEkerot/94ddbdc5baf4e53157e8

			vSSL = SSL_new(vXCTX);
			if (vSSL == NULL)
			{
				Disconnect();
				return(-1);
			}
			SSL_set_connect_state(vSSL);
			vBIO = BIO_new(BIO_f_ssl());
			BIO_set_ssl(vBIO, vSSL, BIO_CLOSE);

			BIO* sbio = BIO_new_connect((char*)vPh.mBytes);
			if (sbio == NULL)
			{
				Disconnect();
				return(-1);
			}
			BIO_push(vBIO, sbio);

			SSL_set_mode(vSSL, SSL_MODE_AUTO_RETRY);
			BIO_set_conn_mode(vBIO, BIO_SOCK_KEEPALIVE | BIO_SOCK_NODELAY);

			iResult = BIO_do_connect(sbio);
			if (iResult != 1)
			{
				Disconnect();
				return(-1);
			}

			GrapaCHAR s;
			s.Append("CONNECT ");
			s.Append(nodeName);
			s.Append(":");
			s.Append(serviceName);
			s.Append(" HTTP/1.1\r\n");
			s.Append("Host: ");
			s.Append(nodeName);
			s.Append("\r\n");
			s.Append("Proxy-Authorization: basic ");
			s.Append((char*)vpId.mBytes);
			s.Append("\r\n\r\n");
			GrapaError err;
			u64 sendlen = 0;
			err = BIO_write(sbio,(char*)s.mBytes, (int)s.mLength);
			if (err > 0)
			{
				sendlen = err;
			}
			else
			{
				Disconnect();
				return(-1);
			}
			u64 recvlen = 0;
			vpResponse.SetLength(32000);
			err = BIO_read(sbio,(char*)vpResponse.mBytes, (int)vpResponse.mLength-1);
			if (err > 0)
			{
				recvlen = err;
				vpResponse.SetSize(recvlen);
				vpResponse.SetLength(recvlen);
				char* sp = (char*)GrapaMem::MemFind(vpResponse.mBytes, ' ', vpResponse.mLength);
				if (sp && GrapaMem::MemCompare(sp, " 200 ", 5) != 0)
				{
					Disconnect();
					return(-1);
				}
			}
			else
			{
				Disconnect();
				return(-1);
			}
			iResult = SSL_connect(vSSL);
			if (iResult != 1)
			{
				Disconnect();
				return(-1);
			}
		}
		else
		{
			vBIO = BIO_new_ssl_connect(vXCTX);
			if (vBIO == NULL)
			{
				Disconnect();
				return(-1);
			}

			BIO_get_ssl(vBIO, &vSSL);
			if (vSSL == NULL)
			{
				Disconnect();
				return(-1);
			}
			SSL_set_mode(vSSL, SSL_MODE_AUTO_RETRY);

			BIO_set_conn_hostname(vBIO, nodeName);
			BIO_set_conn_port(vBIO, serviceName);
			BIO_set_conn_mode(vBIO, BIO_SOCK_KEEPALIVE | BIO_SOCK_NODELAY);
			iResult = BIO_do_connect(vBIO);
			if (iResult != 1)
			{
				Disconnect();
				return(-1);
			}

			iResult = BIO_do_handshake(vBIO);
			if (iResult != 1)
			{
				Disconnect();
				return(-1);
			}

		}

		mConnected = true;
		GetPeerName();
		return(0);
	}

	if (vIsProxy)
	{
		bool prIsIPv6 = false;
		GrapaCHAR prNode, prService;
		GetService(vPh, prNode, prService, prIsIPv6);
		struct addrinfo* result = NULL;
		memset(&((AcureNetPrivate*)vInstance)->mHints, 0, sizeof(struct addrinfo));
		((AcureNetPrivate*)vInstance)->mHints.ai_family = AF_INET;    // AF_INET, AF_INET6, AF_BTH
		if (prIsIPv6)
			((AcureNetPrivate*)vInstance)->mHints.ai_family = AF_INET6;
		((AcureNetPrivate*)vInstance)->mHints.ai_socktype = SOCK_STREAM;
		((AcureNetPrivate*)vInstance)->mHints.ai_flags = 0;
		((AcureNetPrivate*)vInstance)->mHints.ai_protocol = IPPROTO_TCP;  // IPPROTO_TCP, BTHPROTO_RFCOMM
		((AcureNetPrivate*)vInstance)->mHints.ai_canonname = NULL;
		((AcureNetPrivate*)vInstance)->mHints.ai_addr = NULL;
		((AcureNetPrivate*)vInstance)->mHints.ai_next = NULL;
		if (getaddrinfo((char*)prNode.mBytes, (char*)prService.mBytes, &((AcureNetPrivate*)vInstance)->mHints, &result) != 0) return(-1);
		((AcureNetPrivate*)vInstance)->mSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (((AcureNetPrivate*)vInstance)->mSocket == -1)
		{
			freeaddrinfo(result);
			return(-1);
		}
		iResult = connect(((AcureNetPrivate*)vInstance)->mSocket, result->ai_addr, (int)result->ai_addrlen);
		freeaddrinfo(result);
		if (iResult < 0)
		{
#if defined(__MINGW32__) || defined(__GNUC__)
			close(((AcureNetPrivate*)vInstance)->mSocket);
#else
#ifdef _WIN32
			closesocket(((AcureNetPrivate*)vInstance)->mSocket);
#endif
#endif
			((AcureNetPrivate*)vInstance)->mSocket = -1;
			return(-1);
		}

		mConnected = true;
		GetPeerName();

		GrapaCHAR s;
		s.Append("CONNECT ");
		s.Append(nodeName);
		s.Append(":");
		s.Append(serviceName);
		s.Append(" HTTP/1.1\r\n");
		s.Append("Host: ");
		s.Append(nodeName);
		s.Append("\r\n");
		s.Append("Proxy-Authorization: basic ");
		s.Append((char*)vpId.mBytes);
		s.Append("\r\n\r\n");
		Send(s);
		GrapaError err;
		u64 recvlen = 0;
		vpResponse.SetLength(32000);
		err = Receive((u8*)vpResponse.mBytes, (int)vpResponse.mLength - 1, recvlen);
		if (err > 0)
		{
			recvlen = err;
			vpResponse.SetSize(recvlen);
			vpResponse.SetLength(recvlen);
			char* sp = (char*)GrapaMem::MemFind(vpResponse.mBytes, ' ', vpResponse.mLength);
			if (sp && GrapaMem::MemCompare(sp, " 200 ", 5) != 0)
			{
				Disconnect();
				return(-1);
			}
		}
		else
		{
			Disconnect();
			return(-1);
		}

		return(0);
	}

	struct addrinfo* result = NULL;
	memset(&((AcureNetPrivate*)vInstance)->mHints, 0, sizeof(struct addrinfo));
	((AcureNetPrivate*)vInstance)->mHints.ai_family = AF_INET;    // AF_INET, AF_INET6, AF_BTH
	if (isIPv6)
		((AcureNetPrivate*)vInstance)->mHints.ai_family = AF_INET6;
	((AcureNetPrivate*)vInstance)->mHints.ai_socktype = SOCK_STREAM;
	((AcureNetPrivate*)vInstance)->mHints.ai_flags = 0;
	((AcureNetPrivate*)vInstance)->mHints.ai_protocol = IPPROTO_TCP;  // IPPROTO_TCP, BTHPROTO_RFCOMM
	((AcureNetPrivate*)vInstance)->mHints.ai_canonname = NULL;
	((AcureNetPrivate*)vInstance)->mHints.ai_addr = NULL;
	((AcureNetPrivate*)vInstance)->mHints.ai_next = NULL;
	if (getaddrinfo(nodeName, serviceName, &((AcureNetPrivate*)vInstance)->mHints, &result) != 0) return(-1);
	((AcureNetPrivate*)vInstance)->mSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (((AcureNetPrivate*)vInstance)->mSocket == -1)
	{
		freeaddrinfo(result);
		return(-1);
	}
	iResult = connect(((AcureNetPrivate*)vInstance)->mSocket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	if (iResult < 0)
	{
#if defined(__MINGW32__) || defined(__GNUC__)
		close(((AcureNetPrivate*)vInstance)->mSocket);
#else
#ifdef _WIN32
		closesocket(((AcureNetPrivate*)vInstance)->mSocket);
#endif
#endif
		((AcureNetPrivate*)vInstance)->mSocket = -1;
		return(-1);
	}

	mConnected = true;
	GetPeerName();
	return(0);
}

GrapaError GrapaNet::Bind(const GrapaCHAR& pURL)
{
	bool isIPv6 = false;
	GrapaCHAR pNodeName, pServiceName;
	GetService(pURL, pNodeName, pServiceName, isIPv6);
	char* nodeName = pNodeName.mLength ? (char*)pNodeName.mBytes : NULL;
	char* serviceName = pServiceName.mLength ? (char*)pServiceName.mBytes : NULL;
	if (mBound || mConnected) return(-1);
	if (Startup()) return(-1);
	int iResult = 0;

	if (gSystem->mLinkInitialized && vIsSSL)
	{
		vXCTX = vDelCTX = SSL_CTX_new(TLS_server_method());
        vCriticalError = false;
		if (vCertFile.mLength)
			iResult = SSL_CTX_use_certificate_chain_file(vXCTX, (char*)vCertFile.mBytes);
		if (vKeyFile.mLength)
			iResult = SSL_CTX_use_PrivateKey_file(vXCTX, (char*)vKeyFile.mBytes, SSL_FILETYPE_PEM);
		if (vPassOp)
		{
			SSL_CTX_set_default_passwd_cb(vXCTX, (pem_password_cb*)vPassOp);
			SSL_CTX_set_default_passwd_cb_userdata(vXCTX, vPassParam);
		}
		if (vcaFile.mLength || vcaPath.mLength)
			iResult = SSL_CTX_load_verify_locations(vXCTX, (char*)vcaFile.mBytes, (char*)vcaPath.mBytes);

		vBIO = BIO_new_ssl(vXCTX,0);
		if (vBIO == NULL)
		{
			Disconnect();
			return(-1);
		}
		BIO_get_ssl(vBIO, &vSSL);
		if (vSSL == NULL)
		{
			Disconnect();
			return(-1);
		}
		SSL_set_mode(vSSL, SSL_MODE_AUTO_RETRY);

		//vBIO = BIO_push(BIO_new(BIO_f_buffer()), vBIO);
        
		aBIO = BIO_new_accept(serviceName);
		BIO_set_accept_bios(aBIO, vBIO);

		BIO_set_accept_name(aBIO, nodeName);
		BIO_set_accept_port(aBIO, serviceName);
		BIO_set_bind_mode(aBIO, BIO_SOCK_REUSEADDR | BIO_SOCK_KEEPALIVE | BIO_SOCK_NODELAY);

		iResult = BIO_do_accept(aBIO);
		if (iResult != 1)
		{
			Disconnect();
			return(-1);
		}

		mBound = true;
		GetLocalName();
		return(0);
	}


	struct addrinfo *result=NULL;
	memset(&((AcureNetPrivate*)vInstance)->mHints, 0, sizeof(struct addrinfo));
	((AcureNetPrivate*)vInstance)->mHints.ai_family = AF_INET; // AF_INET, AF_INET6, AF_BTH
	if (isIPv6)
		((AcureNetPrivate*)vInstance)->mHints.ai_family = AF_INET6;
	((AcureNetPrivate*)vInstance)->mHints.ai_socktype = SOCK_STREAM;
	((AcureNetPrivate*)vInstance)->mHints.ai_flags = AI_PASSIVE;
	((AcureNetPrivate*)vInstance)->mHints.ai_protocol = IPPROTO_TCP; // IPPROTO_TCP, BTHPROTO_RFCOMM
	((AcureNetPrivate*)vInstance)->mHints.ai_canonname = NULL;
	((AcureNetPrivate*)vInstance)->mHints.ai_addr = NULL;
	((AcureNetPrivate*)vInstance)->mHints.ai_next = NULL;
	iResult = getaddrinfo(nodeName, serviceName, &((AcureNetPrivate*)vInstance)->mHints, &result);
	if (iResult != 0) return(-1);
	((AcureNetPrivate*)vInstance)->mSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (((AcureNetPrivate*)vInstance)->mSocket == -1)
	{
		freeaddrinfo(result);
		return(-1);
	}
	iResult = bind(((AcureNetPrivate*)vInstance)->mSocket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	if (iResult < 0)
	{
#ifdef _WIN32
		closesocket(((AcureNetPrivate*)vInstance)->mSocket);
#else
		close(((AcureNetPrivate*)vInstance)->mSocket);
#endif
		((AcureNetPrivate*)vInstance)->mSocket = -1;
	}
	mBound = true;
	GetLocalName();
	return(0);
}

GrapaError GrapaNet::Listen(GrapaNet& pClient)
{
	vpResponse.SetLength(0);
	if (pClient.mBound || pClient.mConnected) return(-1);
	if (pClient.Startup()) return(-1);
	if (!mBound) return(-1);
	int iResult = 0;

	if (gSystem->mLinkInitialized && vIsSSL)
	{
		vIsBlocking = true;
		iResult = BIO_do_accept(aBIO);
		WaitCritical();
		LeaveCritical();
		if (!mBound)
		{
			int so = BIO_get_fd(aBIO, NULL);
			if (so != -1)
			{
#if defined(__MINGW32__) || defined(__GNUC__)
				close(so);
#endif
#ifdef _WIN32
				closesocket(so);
#endif
			}
			if (vBIO)
				BIO_free_all(vBIO);
			aBIO = NULL;
			vBIO = NULL;
			vSSL = NULL;
			if (vDelCTX)
			{
				SSL_CTX_free(vDelCTX);
				vDelCTX = NULL;
				vXCTX = NULL;
			}
			vIsBlocking = false;
			mBound = false;
			return(-1);
		}
		vIsBlocking = false;
		if (iResult != 1)
			return(-1);
		pClient.vBIO = BIO_pop(aBIO);
		pClient.vIsSSL = vIsSSL;
        pClient.vCriticalError = false;
		BIO_get_ssl(pClient.vBIO, &pClient.vSSL);
		pClient.vXCTX = vXCTX;
        
        //BIO_push(BIO_new(BIO_f_buffer()),pClient.vBIO);
        
		iResult = BIO_do_handshake(pClient.vBIO);
		if (iResult != 1)
		{
			pClient.Disconnect();
			return(-1);
		}
		pClient.mConnected = true;
		pClient.GetPeerName();
		return(0);
	}

	((AcureNetPrivate*)pClient.vInstance)->mHints = ((AcureNetPrivate*)vInstance)->mHints;
	((AcureNetPrivate*)pClient.vInstance)->mSocket = -1;
	iResult = listen(((AcureNetPrivate*)vInstance)->mSocket, SOMAXCONN);
	if (iResult < 0)
		return(-1);

	((AcureNetPrivate*)pClient.vInstance)->mSocket = accept(((AcureNetPrivate*)vInstance)->mSocket, 0L, 0L);
	if (((AcureNetPrivate*)pClient.vInstance)->mSocket == -1)
		return(-1);

	pClient.mConnected = true;
	pClient.GetPeerName();
	return(0);
}

GrapaError GrapaNet::GetPeerName()
{
	if (!mConnected && !mBound) return(-1);

	if (gSystem->mLinkInitialized && vIsSSL)
	{
		int so = BIO_get_fd(vBIO, NULL);
		socklen_t addrlen = sizeof(((AcureNetPrivate*)vInstance)->mSockAddr);
		memset(&((AcureNetPrivate*)vInstance)->mSockAddr, 0, sizeof(((AcureNetPrivate*)vInstance)->mSockAddr));
		int iResult = getpeername(so, (sockaddr*)&((AcureNetPrivate*)vInstance)->mSockAddr, &addrlen);
		mFamily = ((AcureNetPrivate*)vInstance)->mSockAddr.ss_family;
		if (((AcureNetPrivate*)vInstance)->mSockAddr.ss_family == AF_INET)
			mPort = ntohs(((struct sockaddr_in*) & ((AcureNetPrivate*)vInstance)->mSockAddr)->sin_port);
		else
			mPort = ntohs(((struct sockaddr_in6*) & ((AcureNetPrivate*)vInstance)->mSockAddr)->sin6_port);
		iResult = gethostname(mHostName, sizeof(mHostName));
		if (((AcureNetPrivate*)vInstance)->mSockAddr.ss_family == AF_INET)
			inet_ntop(AF_INET, &(((struct sockaddr_in*) & ((AcureNetPrivate*)vInstance)->mSockAddr)->sin_addr), mIpName, INET_ADDRSTRLEN);
		else
			inet_ntop(AF_INET6, &(((struct sockaddr_in6*) & ((AcureNetPrivate*)vInstance)->mSockAddr)->sin6_addr), mIpName, INET6_ADDRSTRLEN);
		return(0);
	}

	socklen_t addrlen = sizeof(((AcureNetPrivate*)vInstance)->mSockAddr);
	memset(&((AcureNetPrivate*)vInstance)->mSockAddr, 0, sizeof(((AcureNetPrivate*)vInstance)->mSockAddr));
	int iResult = getpeername(((AcureNetPrivate*)vInstance)->mSocket, (sockaddr*)&((AcureNetPrivate*)vInstance)->mSockAddr, &addrlen);
	mFamily = ((AcureNetPrivate*)vInstance)->mSockAddr.ss_family;
	if (((AcureNetPrivate*)vInstance)->mSockAddr.ss_family == AF_INET)
		mPort = ntohs(((struct sockaddr_in *)&((AcureNetPrivate*)vInstance)->mSockAddr)->sin_port);
	else
		mPort = ntohs(((struct sockaddr_in6 *)&((AcureNetPrivate*)vInstance)->mSockAddr)->sin6_port);
	iResult = gethostname(mHostName, sizeof(mHostName));
	if (((AcureNetPrivate*)vInstance)->mSockAddr.ss_family == AF_INET)
		inet_ntop(AF_INET, &(((struct sockaddr_in *)&((AcureNetPrivate*)vInstance)->mSockAddr)->sin_addr), mIpName, INET_ADDRSTRLEN);
	else
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)&((AcureNetPrivate*)vInstance)->mSockAddr)->sin6_addr), mIpName, INET6_ADDRSTRLEN);
	//iResult = getnameinfo((sockaddr*)&mSockAddr, addrlen, mFullName, sizeof(mFullName), mServerInfo, sizeof(mServerInfo), NI_NUMERICSERV);
	return(0);
}

GrapaError GrapaNet::GetLocalName()
{
	if (!mConnected && !mBound) return(-1);

	if (gSystem->mLinkInitialized && vIsSSL)
	{
		int so = BIO_get_fd(vBIO, NULL);
		socklen_t addrlen = sizeof(((AcureNetPrivate*)vInstance)->mSockAddr);
		memset(&((AcureNetPrivate*)vInstance)->mSockAddr, 0, sizeof(((AcureNetPrivate*)vInstance)->mSockAddr));
		int iResult = getsockname(so, (sockaddr*)&((AcureNetPrivate*)vInstance)->mSockAddr, &addrlen);
		mFamily = ((AcureNetPrivate*)vInstance)->mSockAddr.ss_family;
		if (((AcureNetPrivate*)vInstance)->mSockAddr.ss_family == AF_INET)
			mPort = ntohs(((struct sockaddr_in*) & ((AcureNetPrivate*)vInstance)->mSockAddr)->sin_port);
		else
			mPort = ntohs(((struct sockaddr_in6*) & ((AcureNetPrivate*)vInstance)->mSockAddr)->sin6_port);
		iResult = gethostname(mHostName, sizeof(mHostName));
		//iResult = getnameinfo((sockaddr*)&mSockAddr, addrlen, mFullName, sizeof(mFullName), mServerInfo, sizeof(mServerInfo), NI_NUMERICSERV);
		GetMac(mName, mIpName, mMAC);
		return(0);
	}

	socklen_t addrlen = sizeof(((AcureNetPrivate*)vInstance)->mSockAddr);
	memset(&((AcureNetPrivate*)vInstance)->mSockAddr, 0, sizeof(((AcureNetPrivate*)vInstance)->mSockAddr));
	int iResult = getsockname(((AcureNetPrivate*)vInstance)->mSocket, (sockaddr*)&((AcureNetPrivate*)vInstance)->mSockAddr, &addrlen);
	mFamily = ((AcureNetPrivate*)vInstance)->mSockAddr.ss_family;
	if (((AcureNetPrivate*)vInstance)->mSockAddr.ss_family == AF_INET)
		mPort = ntohs(((struct sockaddr_in *)&((AcureNetPrivate*)vInstance)->mSockAddr)->sin_port);
	else
		mPort = ntohs(((struct sockaddr_in6 *)&((AcureNetPrivate*)vInstance)->mSockAddr)->sin6_port);
	iResult = gethostname(mHostName, sizeof(mHostName));
	//iResult = getnameinfo((sockaddr*)&mSockAddr, addrlen, mFullName, sizeof(mFullName), mServerInfo, sizeof(mServerInfo), NI_NUMERICSERV);
	GetMac(mName, mIpName, mMAC);
	return(0);
}

GrapaError GrapaNet::GetMac(char* pName, char* pIp, u64& pMAC)
{
	pMAC = 0;
	pIp[0] = 0;
	pName[0] = 0;
#if defined(__MINGW32__) || defined(__GNUC__)
	struct ifaddrs *ifap;
	if (0 == getifaddrs(&ifap))
	{
		struct ifaddrs* ifptr = ifap;
		while(ifptr)
		{
			if ((ifptr->ifa_flags & IFF_UP) && (ifptr->ifa_flags & IFF_BROADCAST) && (ifptr->ifa_flags & IFF_RUNNING) && (!(ifptr->ifa_flags & IFF_LOOPBACK)) && ((struct sockaddr_storage*)ifptr->ifa_addr)->ss_family == AF_INET) { 
				strcpy(pName, ifptr->ifa_name);
				if (((struct sockaddr_storage*)ifptr->ifa_addr)->ss_family == AF_INET)
					inet_ntop(AF_INET, &(((struct sockaddr_in *)ifptr->ifa_addr)->sin_addr), pIp, INET_ADDRSTRLEN);
				else
					inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)ifptr->ifa_addr)->sin6_addr), pIp, INET6_ADDRSTRLEN);
				((u8*)&pMAC)[0] = ((unsigned char)ifptr->ifa_addr->sa_data[0] & 0xFD) | ((~(unsigned char)ifptr->ifa_addr->sa_data[0]) & 0x02);
				((u8*)&pMAC)[1] = (unsigned char)ifptr->ifa_addr->sa_data[1];
				((u8*)&pMAC)[2] = (unsigned char)ifptr->ifa_addr->sa_data[2];
				((u8*)&pMAC)[3] = 0xFF;
				((u8*)&pMAC)[4] = 0xFE;
				((u8*)&pMAC)[5] = (unsigned char)ifptr->ifa_addr->sa_data[3];
				((u8*)&pMAC)[6] = (unsigned char)ifptr->ifa_addr->sa_data[4];
				((u8*)&pMAC)[7] = (unsigned char)ifptr->ifa_addr->sa_data[5];
				pMAC = BE_S64(pMAC);

#ifdef __APPLE__
				int                 mgmtInfoBase[6];
				char                *msgBuffer = NULL;
				size_t              length=0;
				unsigned char       macAddress[6];
				GrapaError             err=0;

				// Setup the management Information Base (mib)
				mgmtInfoBase[0] = CTL_NET;        // Request network subsystem
				mgmtInfoBase[1] = AF_ROUTE;       // Routing table info
				mgmtInfoBase[2] = 0;
				mgmtInfoBase[3] = AF_LINK;        // Request link layer information
				mgmtInfoBase[4] = NET_RT_IFLIST;  // Request all configured interfaces

				// With all configured interfaces requested, get handle index
				if ((mgmtInfoBase[5] = if_nametoindex("en0")) == 0)
					err = -1;
				else
				{
					// Get the size of the data available (store in len)
					if (sysctl(mgmtInfoBase, 6, NULL, &length, NULL, 0) < 0)
						err = -1;
					else
					{
						// Alloc memory based on above call
						if ((msgBuffer = (char*)malloc(length)) == NULL)
							err = -1;
						else
						{
							// Get system information, store in buffer
							if (sysctl(mgmtInfoBase, 6, msgBuffer, &length, NULL, 0) < 0)
								err = -1;
						}
					}
				}

				if (err == 0)
				{
					struct if_msghdr    *interfaceMsgStruct = (struct if_msghdr *) msgBuffer;
					struct sockaddr_dl  *socketStruct = (struct sockaddr_dl *) (interfaceMsgStruct + 1);
					memcpy(&macAddress, socketStruct->sdl_data + socketStruct->sdl_nlen, 6);
					((u8*)&pMAC)[0] = ((unsigned char)macAddress[0] & 0xFD) | ((~(unsigned char)macAddress[0]) & 0x02);
					((u8*)&pMAC)[1] = (unsigned char)macAddress[1];
					((u8*)&pMAC)[2] = (unsigned char)macAddress[2];
					((u8*)&pMAC)[3] = 0xFF;
					((u8*)&pMAC)[4] = 0xFE;
					((u8*)&pMAC)[5] = (unsigned char)macAddress[3];
					((u8*)&pMAC)[6] = (unsigned char)macAddress[4];
					((u8*)&pMAC)[7] = (unsigned char)macAddress[5];
					pMAC = BE_S64(pMAC);
				}

				if (msgBuffer) free(msgBuffer);

#endif
				/*struct ifreq ifr;
				strcpy(ifr.ifr_name, ifptr->ifa_name);
				int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
				if (sock != -1) {
				if (0 == ioctl(sock, SIOCGIFHWADDR, &ifr)) {
				((u8*)&pMAC)[0] = ((unsigned char)ifr.ifr_addr.sa_data[0] & 0xFD) | ((~(unsigned char)ifr.ifr_addr.sa_data[0]) & 0x02);
				((u8*)&pMAC)[1] = (unsigned char)ifr.ifr_addr.sa_data[1];
				((u8*)&pMAC)[2] = (unsigned char)ifr.ifr_addr.sa_data[2];
				((u8*)&pMAC)[3] = 0xFF;
				((u8*)&pMAC)[4] = 0xFE;
				((u8*)&pMAC)[5] = (unsigned char)ifr.ifr_addr.sa_data[3];
				((u8*)&pMAC)[6] = (unsigned char)ifr.ifr_addr.sa_data[4];
				((u8*)&pMAC)[7] = (unsigned char)ifr.ifr_addr.sa_data[5];
				pMAC = BE_S64(pMAC);
				}
				close(sock);
				}*/
				break;
			}
			ifptr = ifptr->ifa_next;
		}
		freeifaddrs(ifap);
	}
#else
#ifdef _WIN32
	ULONG flags = 0;
	ULONG family = AF_INET;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG outBufLen = 0;

	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;

	ULONG dwRetVal = GetAdaptersAddresses(family, flags, NULL, NULL, &outBufLen);
	pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
	dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
	if (dwRetVal == NO_ERROR)
	{
		pCurrAddresses = pAddresses;
		while (pCurrAddresses)
		{
			if ((pCurrAddresses->OperStatus&IfOperStatusUp) && (pCurrAddresses->IfType == MIB_IF_TYPE_ETHERNET || pCurrAddresses->IfType == IF_TYPE_IEEE80211))
			{
				switch (pCurrAddresses->PhysicalAddressLength)
				{
				case 6:
					((u8*)&pMAC)[0] = (pCurrAddresses->PhysicalAddress[0] & 0xFD) | ((~pCurrAddresses->PhysicalAddress[0]) & 0x02);
					((u8*)&pMAC)[1] = pCurrAddresses->PhysicalAddress[1];
					((u8*)&pMAC)[2] = pCurrAddresses->PhysicalAddress[2];
					((u8*)&pMAC)[3] = 0xFF;
					((u8*)&pMAC)[4] = 0xFE;
					((u8*)&pMAC)[5] = pCurrAddresses->PhysicalAddress[3];
					((u8*)&pMAC)[6] = pCurrAddresses->PhysicalAddress[4];
					((u8*)&pMAC)[7] = pCurrAddresses->PhysicalAddress[5];
					break;
				case 8:
					((u8*)&pMAC)[0] = pCurrAddresses->PhysicalAddress[0];
					((u8*)&pMAC)[1] = pCurrAddresses->PhysicalAddress[1];
					((u8*)&pMAC)[2] = pCurrAddresses->PhysicalAddress[2];
					((u8*)&pMAC)[3] = pCurrAddresses->PhysicalAddress[3];
					((u8*)&pMAC)[4] = pCurrAddresses->PhysicalAddress[4];
					((u8*)&pMAC)[5] = pCurrAddresses->PhysicalAddress[5];
					((u8*)&pMAC)[6] = pCurrAddresses->PhysicalAddress[6];
					((u8*)&pMAC)[7] = pCurrAddresses->PhysicalAddress[7];
					break;
				}
				pMAC = BE_S64(pMAC);
				if (pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr->sa_family == AF_INET)
					inet_ntop(AF_INET, &(((struct sockaddr_in *)pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr)->sin_addr), pIp, INET_ADDRSTRLEN);
				else
					inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr)->sin6_addr), pIp, INET6_ADDRSTRLEN);
				size_t r = wcstombs(pName, pCurrAddresses->FriendlyName, 255);
				break;
			}
			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	if (pAddresses)
		free(pAddresses);
#endif
#endif
	return(0);
}

GrapaRuleEvent* GrapaNet::GetInterfaces()
{
	GrapaError err = 0;

	GrapaRuleEvent* result = new GrapaRuleEvent();
	result->mValue.mToken = GrapaTokenType::LIST;
	result->vQueue = new GrapaRuleQueue();

	u64 pMAC = 0;
	char pIp[256];
	char pName[256];
	char pFam[256];

#if defined(__MINGW32__) || defined(__GNUC__)
	struct ifaddrs *ifap;
	if (0 == getifaddrs(&ifap))
	{
		struct ifaddrs* ifptr = ifap;
		while (ifptr)
		{
			if ((ifptr->ifa_flags & IFF_UP) && (ifptr->ifa_flags & IFF_BROADCAST) && (ifptr->ifa_flags & IFF_RUNNING) && (!(ifptr->ifa_flags & IFF_LOOPBACK)) 
					&& (
							((struct sockaddr_storage*)ifptr->ifa_addr)->ss_family == AF_INET
							||	((struct sockaddr_storage*)ifptr->ifa_addr)->ss_family == AF_INET6
					)
					) {
				strcpy(pName, ifptr->ifa_name);
				if (((struct sockaddr_storage*)ifptr->ifa_addr)->ss_family == AF_INET)
				{
					inet_ntop(AF_INET, &(((struct sockaddr_in *)ifptr->ifa_addr)->sin_addr), pIp, INET_ADDRSTRLEN);
					strcpy(pFam,"IPV4");
				}
				else if (((struct sockaddr_storage*)ifptr->ifa_addr)->ss_family == AF_INET6)
				{
					inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)ifptr->ifa_addr)->sin6_addr), pIp, INET6_ADDRSTRLEN);
					strcpy(pFam,"IPV6");
				}
				else
				{
					strcpy(pIp,"?");
					strcpy(pFam,"?");
				}
				((u8*)&pMAC)[0] = ((unsigned char)ifptr->ifa_addr->sa_data[0] & 0xFD) | ((~(unsigned char)ifptr->ifa_addr->sa_data[0]) & 0x02);
				((u8*)&pMAC)[1] = (unsigned char)ifptr->ifa_addr->sa_data[1];
				((u8*)&pMAC)[2] = (unsigned char)ifptr->ifa_addr->sa_data[2];
				((u8*)&pMAC)[3] = 0xFF;
				((u8*)&pMAC)[4] = 0xFE;
				((u8*)&pMAC)[5] = (unsigned char)ifptr->ifa_addr->sa_data[3];
				((u8*)&pMAC)[6] = (unsigned char)ifptr->ifa_addr->sa_data[4];
				((u8*)&pMAC)[7] = (unsigned char)ifptr->ifa_addr->sa_data[5];
				pMAC = BE_S64(pMAC);

#ifdef __APPLE__
				int                 mgmtInfoBase[6];
				char                *msgBuffer = NULL;
				size_t              length = 0;
				unsigned char       macAddress[6];
				GrapaError             err = 0;

				// Setup the management Information Base (mib)
				mgmtInfoBase[0] = CTL_NET;        // Request network subsystem
				mgmtInfoBase[1] = AF_ROUTE;       // Routing table info
				mgmtInfoBase[2] = 0;
				mgmtInfoBase[3] = AF_LINK;        // Request link layer information
				mgmtInfoBase[4] = NET_RT_IFLIST;  // Request all configured interfaces

				// With all configured interfaces requested, get handle index
				if ((mgmtInfoBase[5] = if_nametoindex(pName))==0) //"en0")) == 0)
					err = -1;
				else
				{
					// Get the size of the data available (store in len)
					if (sysctl(mgmtInfoBase, 6, NULL, &length, NULL, 0) < 0)
						err = -1;
					else
					{
						// Alloc memory based on above call
						if ((msgBuffer = (char*)malloc(length)) == NULL)
							err = -1;
						else
						{
							// Get system information, store in buffer
							if (sysctl(mgmtInfoBase, 6, msgBuffer, &length, NULL, 0) < 0)
								err = -1;
						}
					}
				}

				if (err == 0)
				{
					struct if_msghdr    *interfaceMsgStruct = (struct if_msghdr *) msgBuffer;
					struct sockaddr_dl  *socketStruct = (struct sockaddr_dl *) (interfaceMsgStruct + 1);
					memcpy(&macAddress, socketStruct->sdl_data + socketStruct->sdl_nlen, 6);
					((u8*)&pMAC)[0] = ((unsigned char)macAddress[0] & 0xFD) | ((~(unsigned char)macAddress[0]) & 0x02);
					((u8*)&pMAC)[1] = (unsigned char)macAddress[1];
					((u8*)&pMAC)[2] = (unsigned char)macAddress[2];
					((u8*)&pMAC)[3] = 0xFF;
					((u8*)&pMAC)[4] = 0xFE;
					((u8*)&pMAC)[5] = (unsigned char)macAddress[3];
					((u8*)&pMAC)[6] = (unsigned char)macAddress[4];
					((u8*)&pMAC)[7] = (unsigned char)macAddress[5];
					pMAC = BE_S64(pMAC);
				}

				if (msgBuffer) free(msgBuffer);

#endif
				//u64 id = 0;
				//GrapaCHAR name(pName); name.Append("."); name.Append(pFam);
				//err = g.CreateEntry(g.mRootTable, GrapaDB::RTABLE_TREE, name, id);
				//err = g.SetField(g.mRootTable, GrapaDB::RTABLE_TREE, id, "$NAME", GrapaCHAR(pName));
				GrapaBYTE x;
				GrapaInt(pMAC).getBytes().ToHex(x);
				//err = g.SetField(g.mRootTable, GrapaDB::RTABLE_TREE, id, "$MAC", x);
				//err = g.SetField(g.mRootTable, GrapaDB::RTABLE_TREE, id, "$FAMILY", GrapaCHAR(pFam));
				//err = g.SetField(g.mRootTable, GrapaDB::RTABLE_TREE, id, "$VALUE", GrapaCHAR(pIp));
				GrapaRuleEvent* item = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
				item->mValue.mToken = GrapaTokenType::LIST;
				item->vQueue = new GrapaRuleQueue();
				result->vQueue->PushTail(item);
				item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("name"), GrapaCHAR(pName)));
				item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("mac"), GrapaCHAR(x)));
				item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("family"), GrapaCHAR(pFam)));
				item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("address"), GrapaCHAR(pIp)));
			}
			ifptr = ifptr->ifa_next;
		}
		freeifaddrs(ifap);
	}
#else
#ifdef _WIN32
	ULONG flags = 0;
	ULONG family = AF_UNSPEC;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG outBufLen = 0;

	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;

	ULONG dwRetVal = GetAdaptersAddresses(family, flags, NULL, NULL, &outBufLen);
	pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
	dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
	if (dwRetVal == NO_ERROR)
	{
		pCurrAddresses = pAddresses;
		while (pCurrAddresses)
		{
			if ((pCurrAddresses->OperStatus&IfOperStatusUp) && (pCurrAddresses->IfType == MIB_IF_TYPE_ETHERNET || pCurrAddresses->IfType == IF_TYPE_IEEE80211))
			{
				switch (pCurrAddresses->PhysicalAddressLength)
				{
				case 6:
					((u8*)&pMAC)[0] = (pCurrAddresses->PhysicalAddress[0] & 0xFD) | ((~pCurrAddresses->PhysicalAddress[0]) & 0x02);
					((u8*)&pMAC)[1] = pCurrAddresses->PhysicalAddress[1];
					((u8*)&pMAC)[2] = pCurrAddresses->PhysicalAddress[2];
					((u8*)&pMAC)[3] = 0xFF;
					((u8*)&pMAC)[4] = 0xFE;
					((u8*)&pMAC)[5] = pCurrAddresses->PhysicalAddress[3];
					((u8*)&pMAC)[6] = pCurrAddresses->PhysicalAddress[4];
					((u8*)&pMAC)[7] = pCurrAddresses->PhysicalAddress[5];
					break;
				case 8:
					((u8*)&pMAC)[0] = pCurrAddresses->PhysicalAddress[0];
					((u8*)&pMAC)[1] = pCurrAddresses->PhysicalAddress[1];
					((u8*)&pMAC)[2] = pCurrAddresses->PhysicalAddress[2];
					((u8*)&pMAC)[3] = pCurrAddresses->PhysicalAddress[3];
					((u8*)&pMAC)[4] = pCurrAddresses->PhysicalAddress[4];
					((u8*)&pMAC)[5] = pCurrAddresses->PhysicalAddress[5];
					((u8*)&pMAC)[6] = pCurrAddresses->PhysicalAddress[6];
					((u8*)&pMAC)[7] = pCurrAddresses->PhysicalAddress[7];
					break;
				}
				pMAC = BE_S64(pMAC);
				size_t r = wcstombs(pName, pCurrAddresses->FriendlyName, 255);
				pUnicast = pCurrAddresses->FirstUnicastAddress;
				while (pUnicast)
				{
					if (pUnicast->Flags&IP_ADAPTER_ADDRESS_DNS_ELIGIBLE)
					{

						if (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
						{
							inet_ntop(AF_INET, &(((struct sockaddr_in *)pUnicast->Address.lpSockaddr)->sin_addr), pIp, INET_ADDRSTRLEN);
							strcpy(pFam, "IPV4");
						}
						else if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6)
						{
							inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)pUnicast->Address.lpSockaddr)->sin6_addr), pIp, INET6_ADDRSTRLEN);
							strcpy(pFam, "IPV6");
						}
						else
						{
							strcpy(pIp, "?");
							strcpy(pFam, "?");
						}
						u64 id = 0;
						GrapaBYTE x;
						GrapaInt(pMAC).getBytes().ToHex(x);
						GrapaRuleEvent* item = new GrapaRuleEvent(0, GrapaCHAR(), GrapaCHAR());
						item->mValue.mToken = GrapaTokenType::LIST;
						item->vQueue = new GrapaRuleQueue();
						result->vQueue->PushTail(item);
						item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("name"), GrapaCHAR(pName)));
						item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("mac"), GrapaCHAR(x)));
						item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("family"), GrapaCHAR(pFam)));
						item->vQueue->PushTail(new GrapaRuleEvent(0, GrapaCHAR("address"), GrapaCHAR(pIp)));
					}
					pUnicast = pUnicast->Next;
				}
			}
			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	if (pAddresses)
		free(pAddresses);
#endif
#endif
	return(result);
}

GrapaError GrapaNet::Disconnect()
{
	GrapaError err = 0;
	char b[4096];

	if (gSystem->mLinkInitialized && vXCTX)
	{
		WaitCritical();
		if (vSSL && !vCriticalError)
		   
		{
            bool isDone = false;
								  
			while (true)
			{
                err = SSL_shutdown(vSSL);
                if (err == 0)
                {
                    isDone = true;
                    break;
                }
                else if (err == 1)
                {
                    err = 0;
                    isDone = true;
                    break;
                }
                err = SSL_get_error(vSSL, err);
                switch (err)
				{
				case SSL_ERROR_SYSCALL:
                    vCriticalError = true;
                    isDone = true;
                    break;
				case SSL_ERROR_SSL:
					vCriticalError = true;
                    isDone = true;
					break;
				case SSL_ERROR_WANT_READ:
					SSL_read(vSSL, b, 4096);
					continue;
				case SSL_ERROR_WANT_WRITE:
					SSL_write(vSSL, b, 0);
					continue;
                default:
                    isDone = true;
				}
                if (isDone)
                    break;
			}
		}
		if (aBIO)
		{
			int so = BIO_get_fd(aBIO, NULL);
			if (so != -1)
			{
#if defined(__MINGW32__) || defined(__GNUC__)
				shutdown(so, SHUT_RDWR);
#endif
                
#ifdef __APPLE__
                close(so);
#endif

#ifdef _WIN32
				shutdown(so, SD_BOTH);
				closesocket(so);
#endif
			}
		}
		if (vBIO)
		{
			int so = BIO_get_fd(vBIO, NULL);
			if (so != -1)
			{
#if defined(__MINGW32__) || defined(__GNUC__)
				shutdown(so, SHUT_RDWR);
				//close(so);
#endif
#ifdef _WIN32
				shutdown(so, SD_BOTH);
				closesocket(so);
#endif
			}
		}
		if (vIsBlocking)
		{
			mConnected = false;
			mBound = false;
			LeaveCritical();
			return(0);
		}
		if (vBIO)
			BIO_free_all(vBIO);
		aBIO = NULL;
		vBIO = NULL;
		vSSL = NULL;
		if (vDelCTX)
		{
			SSL_CTX_free(vDelCTX);
			vDelCTX = NULL;
			vXCTX = NULL;
		}
		mConnected = false;
		mBound = false;
		LeaveCritical();
		return(0);
	}

	if (mConnected || mBound)
	{

		WaitCritical();
#if defined(__MINGW32__) || defined(__GNUC__)
		if (shutdown(((AcureNetPrivate*)vInstance)->mSocket, SHUT_RDWR) < 0)
		{
		}
		close(((AcureNetPrivate*)vInstance)->mSocket);
#endif
#ifdef _WIN32
		if (shutdown(((AcureNetPrivate*)vInstance)->mSocket, SD_BOTH) < 0)
		{
		}
		closesocket(((AcureNetPrivate*)vInstance)->mSocket);
#endif
		((AcureNetPrivate*)vInstance)->mSocket = -1;
		mConnected = false;
		mBound = false;
		LeaveCritical();
	}
	return(0);
}

GrapaError GrapaNet::Send(u8* sendbuf, u64 sendbuflen, u64& sendlen)
{
	if (!mConnected)
		return(-1);
	if (sendbuflen==0)
		return(0);

	GrapaError err = 0;
	sendlen = 0;
	GrapaError iResult = 0;

	if (gSystem->mLinkInitialized && vXCTX)
	{
		err = -1;
		if (vBIO)
		{
            WaitCritical();
            LeaveCritical();
			iResult = SSL_write(vSSL, (char*)sendbuf, (int)sendbuflen);
			if (iResult > 0)
				sendlen = iResult;
			err = SSL_get_error(vSSL, iResult);
			if (err != 0)
			{
                 switch (err)
                 {
                 case SSL_ERROR_SYSCALL:
                    vCriticalError = true;
                     break;
                 case SSL_ERROR_SSL:
                     vCriticalError = true;
                     break;
                 case SSL_ERROR_WANT_READ:
                     break;
                 case SSL_ERROR_WANT_WRITE:
                     break;
                 case 0:
                     break;
                default:
                     break;
                 }
				Disconnect();
				return(err);
			}

        }
		return(err);
	}

	iResult = send(((AcureNetPrivate*)vInstance)->mSocket, (char*)sendbuf, (int)sendbuflen, 0);
	if (iResult > 0)
		sendlen = iResult;
	else
	{
		err = iResult; //send failed
#ifdef _WIN32
		err = WSAGetLastError();
#endif
	}

	return(err);
}

GrapaError GrapaNet::Receive(u8* recvbuf, u64 recvbuflen, u64& recvlen)
{
	if (!mConnected)
		return(-1);
	if (recvbuflen==0)
		return(0);
		
	GrapaError err = 0;
	recvlen = 0;
	GrapaError iResult = 0;

	if (gSystem->mLinkInitialized && vXCTX)
	{
		err = -1;
		if (vSSL)
		{
			err = 0;
			if (vIsBlocking)
				return(-1);
			vIsBlocking = true;
            WaitCritical();
            LeaveCritical();
			iResult = SSL_read(vSSL, (char*)recvbuf, (int)recvbuflen);
			WaitCritical();
			LeaveCritical();
			if (iResult > 0)
				recvlen = iResult;
			err = SSL_get_error(vSSL, iResult);
            switch (err)
            {
            case SSL_ERROR_SYSCALL:
                vCriticalError = true;
                break;
            case SSL_ERROR_SSL:
                vCriticalError = true;
                break;
            case SSL_ERROR_WANT_READ:
                break;
            case SSL_ERROR_WANT_WRITE:
                break;
            case 0:
                break;
            default:
                break;
            }
			if (!mConnected)
			{
				int so = BIO_get_fd(vBIO, NULL);
				if (so != -1)
				{
#if defined(__MINGW32__) || defined(__GNUC__)
					close(so);
#endif
#ifdef _WIN32
					closesocket(so);
#endif
				}
				if (vBIO)
					BIO_free_all(vBIO);
				aBIO = NULL;
				vBIO = NULL;
				vSSL = NULL;
				if (vDelCTX)
				{
					SSL_CTX_free(vDelCTX);
					vDelCTX = NULL;
					vXCTX = NULL;
				}
				vIsBlocking = false;
				mConnected = false;
				return(-1);
			}
			vIsBlocking = false;
			if (err && mConnected)
			{
				Disconnect();
			}
		}
		return(err);
	}

	iResult = recv(((AcureNetPrivate*)vInstance)->mSocket, (char*)recvbuf, (int)recvbuflen, 0);
	if (iResult > 0)
		recvlen = iResult;
	else
	{
		err = iResult; //recv failed
#ifdef _WIN32
		err = WSAGetLastError();
#endif
	}
	return(err);
}

GrapaError GrapaNet::Send(const GrapaBYTE& sendbuf)
{
	u64 sent;
	return Send(sendbuf.mBytes, sendbuf.mLength, sent);
}

GrapaError GrapaNet::Receive(GrapaBYTE& recvbuf)
{
	u64 recvlen=0;
	GrapaError err = Receive((u8*)recvbuf.mBytes, (int)recvbuf.mSize, recvlen);
	recvbuf.SetLength(recvlen);
	return(err);
}

GrapaError GrapaNet::Pending(u64& hasmore)
{
	if (!mConnected)
		return(-1);
	GrapaError err = 0;
	GrapaError iResult=0;
	hasmore = 0;
	u_long iMode = 0;

	if (gSystem->mLinkInitialized && vXCTX)
	{
		err = -1;
		if (vBIO)
		{
			err = 0;
			int so = BIO_get_fd(vBIO, NULL);
#if defined(__MINGW32__) || defined(__GNUC__)
			iResult = ioctl(so, FIONREAD, &iMode);
#endif
#ifdef _WIN32
			iResult = ioctlsocket(so, FIONREAD, &iMode);
#endif
			hasmore = iMode;
			if (iResult < 0)
			{
                hasmore = 0;
				err = iResult; //recv failed
#ifdef _WIN32
				err = WSAGetLastError();
#endif
			}
		}
		return(err);
	}

#if defined(__MINGW32__) || defined(__GNUC__)
	iResult = ioctl(((AcureNetPrivate*)vInstance)->mSocket, FIONREAD, &iMode); hasmore = iMode;
#endif
#ifdef _WIN32
	iResult = ioctlsocket(((AcureNetPrivate*)vInstance)->mSocket, FIONREAD, &iMode);
	hasmore = iMode;
#endif

	if (iResult < 0)
	{
		err = iResult; //recv failed
#ifdef _WIN32
		err = WSAGetLastError();
#endif
	}

	return(err);
}

GrapaError GrapaNet::Proxy(GrapaCHAR& ptype, GrapaCHAR& pid, GrapaCHAR& ph)
{
	GrapaError err = 0;
	Disconnect();
	vIsProxy = ptype.mLength != 0 && ph.mLength != 0;
	vpType.FROM(ptype);
	vpId.FROM(pid);
	vPh.FROM(ph);
	return(err);
}

GrapaError GrapaNet::Certificate(GrapaCHAR& certFile)
{
	GrapaError err = 0;
	Disconnect();
	vIsSSL = certFile.mLength!=0;
	vCertFile.FROM(certFile);

	if (!gSystem->mLinkInitialized)
	{
		SSL_load_error_strings();
		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
		(void)SSL_library_init();
		gSystem->mLinkInitialized = true;
	}

	if (gSystem->mLinkInitialized && certFile.mLength)
	{
		SSL_CTX* tSSL = SSL_CTX_new(TLS_server_method()); 
		//int SSL_CTX_use_certificate_ASN1(tSSL, int len, unsigned char* d);
		//int SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, tSSL, unsigned char* d, long len);
		//int SSL_CTX_use_PrivateKey(tSSL, EVP_PKEY * pkey);
		//int SSL_CTX_use_certificate(tSSL, X509 * x);;
		//long SSL_CTX_add_extra_chain_cert(tSSL, X509 * x509);
		err = SSL_CTX_use_certificate_chain_file(tSSL, (char*)vCertFile.mBytes);
		err = (err == 1) ? 0 : -1;
		SSL_CTX_free(tSSL);
		if (err)
			vIsSSL = false;
	}
	return(err);
}

GrapaError GrapaNet::Private(GrapaCHAR& keyFile, void* passOp, void* passParam)
{
	GrapaError err = 0;
	vKeyFile.FROM(keyFile);
	vPassOp = passOp;
	vPassParam = passParam;
	if (gSystem->mLinkInitialized && vKeyFile.mLength)
	{
		SSL_CTX* tSSL = SSL_CTX_new(TLS_server_method());
		err = SSL_CTX_use_PrivateKey_file(tSSL, (char*)vKeyFile.mBytes, SSL_FILETYPE_PEM);
		err = (err == 1) ? 0 : -1;
		SSL_CTX_free(tSSL);
	}
	return(err);
}

GrapaError GrapaNet::Trusted(GrapaCHAR& caFile, GrapaCHAR& caPath)
{
	GrapaError err = 0;
	vcaFile.FROM(caFile);
	vcaPath.FROM(caPath);
	if (gSystem->mLinkInitialized && (vcaFile.mLength || vcaPath.mLength))
	{
		SSL_CTX* tSSL = SSL_CTX_new(TLS_server_method());
		err = SSL_CTX_load_verify_locations(vXCTX, (char*)vcaFile.mBytes, (char*)vcaPath.mBytes);
		err = (err == 1) ? 0 : -1;
		SSL_CTX_free(tSSL);
	}
	return(err);
}

GrapaError GrapaNet::Verify(GrapaCHAR& cert)
{
	GrapaError err = 0;
	cert.SetLength(0);
	X509* servercert = SSL_get_peer_certificate(vSSL);
	if (servercert)
	{
		cert.SetLength(0);
		X509_to_PEM(servercert, cert);
		err = SSL_get_verify_result(vSSL);
		X509_free(servercert);
	}
	return(err);
}

GrapaRuleQueue* GrapaNet::Chain()
{
	GrapaError err = 0;
	GrapaRuleQueue* r = new GrapaRuleQueue();

	STACK_OF(X509)* serverchain = SSL_get_peer_cert_chain(vSSL);

	if (serverchain)
	{
		int n = sk_X509_num(serverchain);
		for (int i = 0; i < n; i++)
		{
			X509* servercert = sk_X509_value(serverchain, i);
			GrapaCHAR cert;
			X509_to_PEM(servercert, cert);
			r->PushTail(new GrapaRuleEvent(GrapaTokenType::STR, GrapaCHAR(""), cert));
		}
	}

	return(r);
}

// https://cpp.hotexamples.com/examples/-/-/SSL_get_error/cpp-ssl_get_error-function-examples.html
// https://stackoverflow.com/questions/23137368/x509-structure-to-human-readable-string
//     localKeyID: 94 7D DF 3D A7 A5 8D 3F FF 73 B5 CF 44 96 4F FC DC DC 51 BB 
// friendlyName: Company UAT Client SSL Certificate

GrapaRuleQueue* GrapaNet::Decode(const GrapaCHAR& cert)
{
	GrapaError err = 0;
	GrapaRuleQueue* r = new GrapaRuleQueue();

	X509* servercert = PEM_to_X509(cert);

	if (servercert)
	{
		X509_NAME* subject = X509_get_subject_name(servercert);
		if (subject)
		{
			GrapaCHAR d;
			d.SetSize(2048);
			X509_NAME_oneline(subject, (char*)d.mBytes, d.mSize);
			int sz = strlen((char*)d.mBytes);
			d.SetSize(sz);
			d.SetLength(sz);
			r->PushTail(new GrapaRuleEvent(GrapaTokenType::STR, GrapaCHAR("subject_name"), d));
		}
		subject = X509_get_issuer_name(servercert);
		if (subject)
		{
			GrapaCHAR d;
			d.SetSize(2048);
			X509_NAME_oneline(subject, (char*)d.mBytes, d.mSize);
			int sz = strlen((char*)d.mBytes);
			d.SetSize(sz);
			d.SetLength(sz);
			r->PushTail(new GrapaRuleEvent(GrapaTokenType::STR, GrapaCHAR("issuer_name"), d));
		}
		X509_free(servercert);
	}

	return(r);
}

void GrapaNet::X509_to_PEM(X509* cert, GrapaCHAR& pem)
{
	pem.SetLength(0);

	BIO* bio = NULL;

	if (NULL == cert)
		return;

	bio = BIO_new(BIO_s_mem());
	if (NULL == bio) {
		return;
	}

	if (0 == PEM_write_bio_X509(bio, cert)) {
		BIO_free(bio);
		return;
	}

	BUF_MEM* bptr;
	BIO_get_mem_ptr(bio, &bptr);
	int length = bptr->length;

	pem.SetSize(bptr->length +1);

	int len = BIO_read(bio, pem.mBytes, bptr->length);
	if (len > 0)
		pem.SetLength(len);

	BIO_free(bio);
}

X509* GrapaNet::PEM_to_X509(const GrapaCHAR& pem)
{
	X509* cert = NULL;
	BIO* bio = NULL;

	if (pem.mLength==0) {
		return NULL;
	}

	bio = BIO_new_mem_buf(pem.mBytes, pem.mLength);
	if (NULL == bio) {
		return NULL;
	}

	cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
	BIO_free(bio);
	return cert;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
