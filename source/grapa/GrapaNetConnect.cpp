// GrapaNetConnect.cpp
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

#include "GrapaNetConnect.h"
#include "GrapaMem.h"

#include <thread>


///////////////////////////////////////////////////////////////////////////////////////////////////

GrapaNetConnect::GrapaNetConnect()
{
	vReceiveLambda = NULL;
	dReceiveLambda = NULL;
	vConnectLambda = NULL;
	dConnectLambda = NULL;
	vScriptState = NULL;
	mListenMode = false;
	mListenChild = false;
	vObj = NULL;
	dPassLambda = NULL;
	dPassParam = NULL;
}

GrapaNetConnect::~GrapaNetConnect()
{
	Disconnect();
	if (dReceiveLambda)
	{
		dReceiveLambda->CLEAR();
		delete dReceiveLambda;
		dReceiveLambda = NULL;
	}
	if (dConnectLambda)
	{
		dConnectLambda->CLEAR();
		delete dConnectLambda;
		dConnectLambda = NULL;
	}
	if (dPassLambda)
	{
		dPassLambda->CLEAR();
		delete dPassLambda;
		dPassLambda = NULL;
	}
	if (dPassParam)
	{
		dPassParam->CLEAR();
		delete dPassParam;
		dPassParam = NULL;
	}
}

void GrapaNetConnect::Set(GrapaCHAR& pURL)
{
	mURL.FROM(pURL);
}

GrapaError GrapaNetConnect::Connect(GrapaRuleEvent* pUrl)
{
	Disconnect();
	vObj = pUrl;
	mListenMode = false;
	mError = mNet.Connect(mURL);
	return(mError);
}

GrapaError GrapaNetConnect::Bind(GrapaRuleEvent* pUrl)
{
	Disconnect();
	vObj = pUrl;
	mError = mNet.Bind(mURL);
	return(mError);
}

GrapaError GrapaNetConnect::Listen(GrapaNetConnect* pNet)
{
	Disconnect();
	vObj = pNet->vObj;
	mError = pNet->mNet.Listen(mNet);
	return(mError);
}

GrapaError GrapaNetConnect::Disconnect()
{
	if (!mListenChild)
	{
		GrapaRuleEvent* newNet = (GrapaRuleEvent*)mListeners.PopHead();
		while (newNet)
		{
			newNet->CLEAR();
			delete newNet;
			newNet = (GrapaRuleEvent*)mListeners.PopHead();
		}
	}
	mNet.Disconnect();
	Stop();
	return(0);
}

GrapaError GrapaNetConnect::Proxy(GrapaRuleEvent* ptype, GrapaRuleEvent* pid, GrapaRuleEvent* ph)
{
	GrapaCHAR blank;
	return mNet.Proxy(ptype?ptype->mValue: blank, pid?pid->mValue: blank, ph?ph->mValue: blank);
}

GrapaError GrapaNetConnect::Certificate(GrapaRuleEvent* certFile)
{
	return mNet.Certificate(certFile->mValue);
}

static int static_pem_passwd_cb(char* buf, int size, int rwflag, void* u)
{
	buf[0] = '\0';
	int rlen = 0;
	GrapaNetConnect* vThis = (GrapaNetConnect*)u;
	if (vThis == 0)
		return(0);
	GrapaRuleQueue params;
	GrapaRuleEvent* url;
	url = new GrapaRuleEvent();  params.PushTail(url);
	url->mValue.mToken = GrapaTokenType::PTR;
	url->vRulePointer = vThis->dPassParam;
	GrapaScriptExec se(vThis->vScriptState);
	GrapaRuleEvent* result = se.ProcessPlan(&vThis->mNames, vThis->dPassLambda, params.Head(), 1);
	if (result)
	{
		if (result->mValue.mLength > size)
			result->mValue.SetLength(size);
		rlen = (int)result->mValue.mLength;
		if (rlen)
			memcpy(buf, (char*)result->mValue.mBytes, (int)result->mValue.mLength);
		result->CLEAR();
		delete result;
	}
	return rlen;
}

GrapaError GrapaNetConnect::Private(GrapaScriptExec* pScriptExec, GrapaRuleEvent* keyFile, GrapaRuleEvent* passOp, GrapaRuleEvent* passParam)
{
	vScriptState = pScriptExec->vScriptState;
	if (dPassLambda)
	{
		dPassLambda->CLEAR();
		delete dPassLambda;
		dPassLambda = NULL;
	}
	if (passOp)
		dPassLambda = vScriptState->vScriptExec->CopyItem(passOp);
	if (dPassParam)
	{
		dPassParam->CLEAR();
		delete dPassParam;
		dPassParam = NULL;
	}
	if (passParam)
		dPassParam = vScriptState->vScriptExec->CopyItem(passParam);
	return mNet.Private(keyFile->mValue, (void*)static_pem_passwd_cb, (void*)this);
}

GrapaError GrapaNetConnect::Trusted(GrapaRuleEvent* caFile, GrapaRuleEvent* caPath)
{
	return mNet.Trusted(caFile->mValue, caPath->mValue);
}

GrapaError GrapaNetConnect::Verify(GrapaCHAR& cert)
{
	return mNet.Verify(cert);
}

GrapaRuleQueue* GrapaNetConnect::Chain()
{
	return mNet.Chain();
}

GrapaRuleQueue* GrapaNetConnect::Decode(const GrapaCHAR& cert)
{
	return GrapaNet::Decode(cert);
}


GrapaError GrapaNetConnect::OnListen(GrapaScriptExec* pScriptExec, GrapaRuleEvent* pUrl, GrapaRuleEvent* pReceiveLambda, GrapaRuleEvent* pConnectLambda)
{
	Disconnect();
	vObj = pUrl;
	mListenMode = true;
	vScriptState = pScriptExec->vScriptState;
	if (dReceiveLambda)
	{
		dReceiveLambda->CLEAR();
		delete dReceiveLambda;
	}
	if (dConnectLambda)
	{
		dConnectLambda->CLEAR();
		delete dConnectLambda;
	}
	vReceiveLambda = dReceiveLambda = pScriptExec->CopyItem(pReceiveLambda);
	vConnectLambda = dConnectLambda = pScriptExec->CopyItem(pConnectLambda);
	mError = mNet.Bind(mURL);
	if (mError == 0)
		Start(false);
	return(mError);
}

GrapaError GrapaNetConnect::OnReceive(GrapaScriptExec* pScriptExec, GrapaRuleEvent* pReceiveLambda)
{
	WaitCritical();
	vScriptState = pScriptExec->vScriptState;
	if (dReceiveLambda)
	{
		dReceiveLambda->CLEAR();
		delete dReceiveLambda;
	}
	vReceiveLambda = dReceiveLambda = pScriptExec->CopyItem(pReceiveLambda);
	LeaveCritical();
	if (mStop)
		Start(false);
	return(0);
}

void GrapaNetConnect::Starting()
{
}

void GrapaNetConnect::Running()
{
	GrapaError err = 0;

	SendCondition();

	if (mListenMode)
	{
		if (vObj)
		{
			WaitCritical();
			while (!mStop && !err)
			{
				GrapaRuleEvent* newNet = new GrapaRuleEvent(GrapaTokenType::OBJ, 0, NULL);
				newNet->vQueue = new GrapaRuleQueue();
				newNet->vNetConnect = new GrapaNetConnect();
				newNet->vClass = vObj->vClass;
				newNet->vNetConnect->vObj = newNet;
				newNet->vNetConnect->mNames.SetParrent(vObj->vNetConnect->mNames.GetParrent());
				newNet->vNetConnect->mListenChild = true;
				LeaveCritical();
				err = mNet.Listen(newNet->vNetConnect->mNet);
				if (err || mStop)
				{
					mStop = true;
					delete newNet;
					break;
				}
				WaitCritical();
				if (!err && newNet->vNetConnect->mNet.mConnected)
				{
					mListeners.PushTail(newNet);
					newNet->vNetConnect->vScriptState = vScriptState;
					newNet->vNetConnect->vReceiveLambda = vReceiveLambda;
					newNet->vNetConnect->vConnectLambda = vConnectLambda;
					newNet->vNetConnect->Start(false);
				}
				else
				{
					delete newNet;
				}
				newNet = (GrapaRuleEvent*)mListeners.Head();
				while (newNet)
				{
					GrapaRuleEvent* testNet = newNet;
					newNet = newNet->Next();
					if (testNet->vNetConnect->mStop && !testNet->vNetConnect->mNet.mConnected)
					{
						mListeners.PopEvent(testNet);
						testNet->CLEAR();
						delete testNet;
					}
				}
			}
			LeaveCritical();
		}
	}
	else
	{
		GrapaRuleQueue params;
		GrapaRuleEvent* url, * msg, * more;
		url = new GrapaRuleEvent();  params.PushTail(url);
		msg = new GrapaRuleEvent(); msg->mValue.SetSize(4096); params.PushTail(msg);
		more = new GrapaRuleEvent(); params.PushTail(more);
		GrapaScriptExec se(vScriptState);

		url->mValue.mToken = GrapaTokenType::PTR;
		url->vRulePointer = vObj;

		WaitCritical();
		if (vConnectLambda)
		{
			GrapaRuleEvent* result = se.ProcessPlan(&mNames, vConnectLambda, params.Head(), 1);
			if (result)
			{
				result->CLEAR();
				delete result;
			}
		}
		while (!mStop && !err && mNet.mConnected)
		{
			u64 recvlen = 0;
			LeaveCritical();
			err = mNet.Receive(msg->mValue.mBytes, 4096, recvlen);
			if (mStop)
			{
				break;
			}
			WaitCritical();
			msg->mValue.SetLength(recvlen);
			if (err == 0 && vReceiveLambda)
			{
				u64 moreBytes = 0;
				mNet.Pending(moreBytes);
				more->mValue.FROM(GrapaInt(moreBytes).getBytes());
				GrapaRuleEvent* result = se.ProcessPlan(&mNames, vReceiveLambda, params.Head(), 3);
				if (result)
				{
					result->CLEAR();
					delete result;
				}
			}
		}
		if (!mStop && vReceiveLambda)
		{
			msg->mValue.SetLength(0);
			more->mValue.FROM(GrapaInt(0).getBytes());
			GrapaRuleEvent* result = se.ProcessPlan(&mNames, vReceiveLambda, params.Head(), 3);
			if (result)
			{
				result->CLEAR();
				delete result;
				result = NULL;
			}
		}
		LeaveCritical();
	}
}

void GrapaNetConnect::Stopping()
{
	mNet.Disconnect();
}

GrapaError GrapaNetConnect::HttpRead(GrapaScriptExec* pScriptExec)
{
	GrapaError err = 0;
	mNet.vpResponse.SetLength(0);

	GrapaCHAR buf;
	GrapaCHAR res;
	GrapaCHAR res_upper;

	GrapaCHAR head;
	GrapaCHAR body;

	res.SetSize(4096);
	res_upper.SetSize(4096);
	buf.SetSize(4096);
	res.SetLength(0);
	buf.SetLength(0);
	res_upper.SetLength(0);
	while (res.mLength == 0)
	{
		buf.SetLength(0);
		err = mNet.Receive(buf);
		if (err) 
			return err;
		res.Append(buf);
		buf.ToUpper();
		res_upper.Append(buf);
	}

	char* crcr = NULL;
	char* contentlength = NULL;
	char* transferencoding = NULL;

	while (contentlength == NULL && transferencoding == NULL)
	{

		u64 hasmore = 0;
		err = mNet.Pending(hasmore);
		if (err == 0 && hasmore == 0)
		{
#ifdef _THREAD_
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
			err = mNet.Pending(hasmore);
		}
		if (err)
			return err;

		while (hasmore)
		{
			if ((res.mSize - res.mLength) < hasmore)
			{
				res.SetSize(res.mSize + (hasmore - (res.mSize - res.mLength)));
				res_upper.SetSize(res.mSize + (hasmore - (res.mSize - res.mLength)));
			}
			buf.SetLength(0);
			err = mNet.Receive(buf);
			if (err)
				return err;
			res.Append(buf);
			buf.ToUpper();
			res_upper.Append(buf);
			hasmore = 0;
			err = mNet.Pending(hasmore);
			if (err)
				return err;
		}
		crcr = (char*)GrapaMem::MemFind(res_upper.mBytes, res_upper.mLength, (void*)"\r\n\r\n", 4);
		if (crcr)
		{
			contentlength = (char*)GrapaMem::MemFind(res_upper.mBytes, res_upper.mLength, (void*)"CONTENT-LENGTH: ", 16);
			if (crcr && !contentlength)
				transferencoding = (char*)GrapaMem::MemFind(res_upper.mBytes, res_upper.mLength, (void*)"TRANSFER-ENCODING: ", 19);
			if (contentlength)
			{
				u64 len = res_upper.mLength - (contentlength - (char*)res_upper.mBytes);
				if (len > 128)
					len = 128;
				char* crlf = (char*)GrapaMem::MemFind(contentlength + 16, len - 16, (void*)"\r\n", 2);
				u64 len2 = GrapaMem::s2au64(contentlength + 16, crlf - (contentlength + 16));
				len2 += (crcr - (char*)res_upper.mBytes) + 4;
				while (res.mLength < len2)
				{
					buf.SetLength(0);
					err = mNet.Receive(buf);
					if (err)
						return err;
					res.Append(buf);
				}
				mNet.vpResponse.FROM(res);

				return(0);
			}
			else if (transferencoding)
			{
				u64 len = res_upper.mLength - (transferencoding - (char*)res_upper.mBytes);
				if (len > 128)
					len = 128;
				char* chunked = (char*)GrapaMem::MemFind(transferencoding, len, (void*)"CHUNKED", 7);
				hasmore = 0;
				err = mNet.Pending(hasmore);
				if (err)
					return(err);
				while (hasmore)
				{
					if ((res.mSize - res.mLength) < hasmore)
						res.SetSize(res.mSize + (hasmore - (res.mSize - res.mLength)));
					buf.SetLength(0);
					err = mNet.Receive(buf);
					if (err)
						return err;
					res.Append(buf);
					hasmore = 0;
					err = mNet.Pending(hasmore);
					if (err)
						return err;
				}
				crcr = (char*)GrapaMem::MemFind(res.mBytes, res.mLength, (void*)"\r\n\r\n", 4);
				if (chunked)
				{
					GrapaCHAR res2;
					head.FROM((char*)res.mBytes, (u64)(crcr - (char*)res.mBytes));
					res2.FROM((char*)(crcr + 4), (u64)(res.mLength - (crcr - (char*)res.mBytes) - 4));
					res.FROM(res2);

					bool notdone = true;
					while (notdone)
					{
						char* crlf = (char*)GrapaMem::MemFind(res.mBytes, res.mLength, (void*)"\r\n", 2);
						if (crlf == NULL)
						{
							buf.SetLength(0);
							err = mNet.Receive(buf);
							if (err)
								return err;
							res.Append(buf);
						}
						else
						{
							u64 len = crlf - (char*)res.mBytes;
							s64 cl = 0;
							for (u64 i = 0; i < len; i++)
							{
								if (res.mBytes[i] > 'a')
									cl = cl * 16 + ((res.mBytes[i] - 'a') + 10);
								else if (res.mBytes[i] > 'A')
									cl = cl * 16 + ((res.mBytes[i] - 'A') + 10);
								else
									cl = cl * 16 + (res.mBytes[i] - '0');
							}
							if (cl >= 0)
							{
								res2.FROM((char*)(res.mBytes + len + 2), (u64)(res.mLength - len - 2));
								while (res2.mLength < (cl + 2))
								{
									buf.SetLength(0);
									err = mNet.Receive(buf);
									res2.Append(buf);
								}
								body.Append((char*)res2.mBytes, cl);
								res.FROM((char*)(res2.mBytes + cl + 2), res2.mLength - cl - 2);
							}
							if (cl <= 0)
								notdone = false;
						}
					};
					hasmore = 0;
					err = mNet.Pending(hasmore);
					if (err)
						return(err);
					while (hasmore)
					{
						if ((res.mSize - res.mLength) < hasmore)
							res.SetSize(res.mSize + (hasmore - (res.mSize - res.mLength)));
						buf.SetLength(0);
						err = mNet.Receive(buf);
						if (err)
							return err;
						res.Append(buf);
						hasmore = 0;
						err = mNet.Pending(hasmore);
						if (err)
							return err;
					}
					if (res.mLength)
					{
						if (body.mLength == 0)
							body.FROM(res);
						else
						{
							head.Append("\r\n");
							head.Append(res);
						}
					};

					mNet.vpResponse.FROM(head);
					mNet.vpResponse.Append("\r\n\r\n");
					mNet.vpResponse.Append(body);
					return(0);
				}
				else
				{
					mNet.vpResponse.FROM(res);
					return(0);
				}
			}
		}

	}

	return -1;
}

GrapaRuleEvent* GrapaNetConnect::HttpMessage(GrapaScriptExec* vScriptExec, GrapaNames* pNameSpace)
{
	GrapaRuleEvent* result = NULL;

	u64 pl = mNet.vpResponse.mLength;
	char* pc = (char*)mNet.vpResponse.mBytes;

	while (pl >= 2 && (pc[0]=='\r' && pc[1]=='\n'))
	{
		pl -= 2;
		pc += 2;
	}
	
	if (pl <= 2)
		return result;

	result = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "", "");
	result->vQueue = new GrapaRuleQueue();

	char* cr = NULL;

	cr = (char*)GrapaMem::MemFind(pc, pl, (void*)"\r\n", 2);
	if (cr == NULL)
		return result;
	u64 statusL = (u64)(cr - pc);
	char* statusC = pc;
	pl -= (statusL + 2);
	pc += (statusL + 2);

	GrapaRuleEvent* v = NULL;
	GrapaRuleEvent* v2 = NULL;

	cr = NULL;
	v = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "status", "");
	v->vQueue = new GrapaRuleQueue();
		cr = (statusC && statusL) ? (char*)GrapaMem::MemFind(statusC, statusL, (void*)" ", 1) : NULL;
		if (cr)
		{
			v2 = new GrapaRuleEvent(GrapaTokenType::STR, 0, "version", "");
			v2->mValue.FROM(statusC, ((u64)(cr - statusC)));
			v->vQueue->PushTail(v2);
			statusL -= (((u64)(cr - statusC)) + 1);
			statusC += (((u64)(cr - statusC)) + 1);
		}
		cr = (statusC && statusL) ? (char*)GrapaMem::MemFind(statusC, statusL, (void*)" ", 1) : NULL;
		if (cr)
		{
			v2 = new GrapaRuleEvent(GrapaTokenType::STR, 0, "code", "");
			v2->mValue.FROM(statusC, ((u64)(cr - statusC)));
			v->vQueue->PushTail(v2);
			statusL -= (((u64)(cr - statusC)) + 1);
			statusC += (((u64)(cr - statusC)) + 1);
		}
		if (statusC)
		{
			v2 = new GrapaRuleEvent(GrapaTokenType::STR, 0, "description", "");
			v2->mValue.FROM(statusC, statusL);
			v->vQueue->PushTail(v2);
		}
	result->vQueue->PushTail(v);

	bool isPlain = false;
	bool isHtml = false;
	bool isXml = false;
	bool isJson = false;
	GrapaCHAR space(" ");

	cr = NULL;
	cr = (pc && pl) ? (char*)GrapaMem::MemFind(pc, pl, (void*)"\r\n\r\n", 4) : NULL;
	if (cr)
	{
		char* headerC = pc;
		u64 headerL = (u64)(cr - pc);
		pl -= (headerL + 4);
		pc += (headerL + 4);
		v = new GrapaRuleEvent(GrapaTokenType::LIST, 0, "headers", "");
		v->vQueue = new GrapaRuleQueue();
		while (headerC && headerL)
		{
			cr = (char*)GrapaMem::MemFind(headerC, headerL, (void*)"\r\n", 2);
			if (cr == NULL && headerL)
				cr = headerC;
			else if (cr == NULL)
				break;
			if (cr)
			{
				char* cr2 = (char*)GrapaMem::MemFind(headerC, cr - headerC, (void*)": ", 2);
				if (cr2)
				{
					GrapaCHAR cn(headerC, cr2 - headerC);
					cn.ToLower();

					s64 index = 0;
					v2 = v->vQueue->Search(cn,index);
					if (v2==NULL)
					{
						v2 = new GrapaRuleEvent(0, cn, GrapaCHAR(""));
						v->vQueue->PushTail(v2);
					}

					if (cn.Cmp("content-type") == 0)
					{
						if (v2->vQueue == NULL)
						{
							v2->mValue.mToken = GrapaTokenType::ARRAY;
							v2->vQueue = new GrapaRuleQueue();
						}
						char* hc = cr2 + 2;
						u64 hl = ((u64)(cr - cr2)) - 2;
						while (hc && hl)
						{
							char* hc2 = (char*)GrapaMem::MemFind(hc, hl, (void*)";", 1);
							if (hc2 == NULL && hl)
								hc2 = &hc[hl];
							if (hc2 == NULL)
								break;
							while (hl && hc[0] == ' ')
							{
								hl -= 1;
								hc += 1;
							}
							if (hl)
							{
								GrapaRuleEvent* v3 = new GrapaRuleEvent(0, GrapaCHAR(""), GrapaCHAR(hc, (u64)(hc2 - hc)));
								v3->mValue.Trim(space);
								v2->vQueue->PushTail(v3);
								if (hl == (u64)(hc2 - hc))
								{
									hl = 0;
									hc = NULL;
								}
								else
								{
									hl -= ((u64)(hc2 - hc)) + 1;
									hc += ((u64)(hc2 - hc)) + 1;
								}
								if (v3->mValue.Cmp("text/plain") == 0) isPlain = true;
								else if (v3->mValue.Cmp("text/html") == 0) isHtml = true;
								else if (v3->mValue.Cmp("text/xml") == 0) isXml = true;
								else if (v3->mValue.Cmp("application/xml") == 0) isXml = true;
								else if (v3->mValue.Cmp("application/xhtml+xml") == 0) isXml = true;
								else if (v3->mValue.Cmp("text/json") == 0) isJson = true;
								else if (v3->mValue.Cmp("application/json") == 0) isJson = true;
							}
						}
					}
					else if (cn.Cmp("set-cookie") == 0)
					{
						if (v2->vQueue == NULL)
						{
							v2->mValue.mToken = GrapaTokenType::LIST;
							v2->vQueue = new GrapaRuleQueue();
						}
						char* hc = cr2 + 2;
						u64 hl = ((u64)(cr - cr2)) - 2;
						while (hc && hl)
						{
							char* hc2 = (char*)GrapaMem::MemFind(hc, hl, (void*)";", 1);
							if (hc2 == NULL && hl)
								hc2 = &hc[hl];
							if (hc2 == NULL)
								break;
							while (hl && hc[0] == ' ')
							{
								hl -= 1;
								hc += 1;
							}
							if (hl)
							{
								char* cc = hc;
								u64 cl = (u64)(hc2 - hc);
								char* cp = (char*)GrapaMem::MemFind(cc, cl, (void*)"=", 1);
								if (cp)
								{
									GrapaCHAR cname(cc, cp - cc);
									cname.Trim(space);
									GrapaRuleEvent* v3 = v2->vQueue->Search(cname,index);
									if (v3 == NULL)
									{
										v3 = new GrapaRuleEvent(0, GrapaCHAR(cc, cp - cc), GrapaCHAR(cp + 1, cl - (cp - cc) - 1));
										v3->mValue.Trim(space);
										v2->vQueue->PushTail(v3);
									}
									else
									{
										v3->mValue.FROM(GrapaCHAR(cp + 1, cl - (cp - cc) - 1));
										v3->mValue.Trim(space);
									}
								}
								else
								{
									GrapaCHAR cname(hc, (u64)(hc2 - hc));
									cname.Trim(space);
									GrapaRuleEvent* v3 = v2->vQueue->Search(cname, index);
									if (v3 == NULL)
									{
										GrapaRuleEvent* v3 = new GrapaRuleEvent(0, cname, GrapaCHAR());
										v3->mName.Trim(space);
										v3->SetNull();
										v2->vQueue->PushTail(v3);
									}
									else
									{
										v3->mValue.SetNull();
									}
								}
								if (hl == (u64)(hc2 - hc))
								{
									hl = 0;
									hc = NULL;
								}
								else
								{
									hl -= ((u64)(hc2 - hc)) + 1;
									hc += ((u64)(hc2 - hc)) + 1;
								}
							}
						}
					}
					else
					{
						v2->mValue.FROM(cr2 + 2, ((u64)(cr - cr2)) - 2);
					}
				}
				if (cr == headerC)
				{
					headerL = 0;
					headerC = NULL;
				}
				else
				{
					headerL -= (((u64)(cr - headerC)) + 2);
					headerC += (((u64)(cr - headerC)) + 2);
				}
			}
		}
		result->vQueue->PushTail(v);
	}

	if (pc && pl)
	{
		u64 bodyL = pl;
		char* bodyC = pc;

		if (isHtml)
		{
			while (bodyC && bodyL && (bodyC[0] == ' ' || bodyC[0] == '\r' || bodyC[0] == '\n' || bodyC[0] == '\t'))
			{
				bodyL -= 1;
				bodyC += 1;
			}
			if (bodyC && bodyL && bodyC[0] == '<')
			{
				GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$htmlcreate"));
				GrapaCHAR val("$&");
				val.Append(bodyC, bodyL);
				val.Append("$&");
				GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, val, rulexx, 0, GrapaCHAR());
				v = vScriptExec->ProcessPlan(pNameSpace, plan);
				if (plan)
				{
					plan->CLEAR();
					delete plan;
				}
			}
			else
			{
				v = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
				v->mValue.FROM(bodyC, bodyL);
			}
		}
		else if (isXml)
		{
			while (bodyC && bodyL && (bodyC[0] == ' ' || bodyC[0] == '\r' || bodyC[0] == '\n' || bodyC[0] == '\t'))
			{
				bodyL -= 1;
				bodyC += 1;
			}
			if (bodyC && bodyL && bodyC[0] == '<')
			{
				GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$xmlcreate"));
				GrapaCHAR val("$&");
				val.Append(bodyC, bodyL);
				val.Append("$&");
				GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, val, rulexx, 0, GrapaCHAR());
				v = vScriptExec->ProcessPlan(pNameSpace, plan);
				if (plan)
				{
					plan->CLEAR();
					delete plan;
				}
			}
			else
			{
				v = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
				v->mValue.FROM(bodyC, bodyL);
			}
		}
		else if (isJson)
		{
			GrapaRuleEvent* rulexx = vScriptExec->vScriptState->SearchVariable(pNameSpace, GrapaCHAR("$function"));
			GrapaCHAR val(bodyC, bodyL);
			GrapaRuleEvent* plan = vScriptExec->Plan(pNameSpace, val, rulexx, 0, GrapaCHAR());
			v = vScriptExec->ProcessPlan(pNameSpace, plan);
			if (plan)
			{
				plan->CLEAR();
				delete plan;
			}
		}
		else
		{
			v = new GrapaRuleEvent(GrapaTokenType::STR, 0, "", "");
			v->mValue.FROM(bodyC, bodyL);
		}

		v->mName.FROM("body");
		result->vQueue->PushTail(v);
	}

	return result;
}


