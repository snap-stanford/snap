//////////////////////////////////////
// Simple-App-Server-Request-Environment
ClassTP(TSAppSrvRqEnv, PSAppSrvRqEnv)//{
private:
	PWebSrv WebSrv;
	int SockId;
	PHttpRq HttpRq;

public:
	TSAppSrvRqEnv(const PWebSrv& _WebSrv, int _SockId, const PHttpRq& _HttpRq):
		WebSrv(_WebSrv), SockId(_SockId), HttpRq(_HttpRq) { }
	static PSAppSrvRqEnv New(const PWebSrv& WebSrv, int SockId, const PHttpRq& HttpRq) { 
		return new TSAppSrvRqEnv(WebSrv, SockId, HttpRq); }

	const PWebSrv& GetWebSrv() const { return WebSrv; }
	int GetSockId() const { return SockId; }
	const PHttpRq& GetHttpRq() const { return HttpRq; }
};

//////////////////////////////////////
// Simple-App-Server-Function
typedef enum { saotXml, saotJSon, saotCustom } TSAppOutType;
ClassTPV(TSAppSrvFun, PSAppSrvFun, TSAppSrvFunV)//{
public:
	static bool IsFldNm(const TStrKdV& FldNmValPrV, const TStr& FldNm);
	static TStr GetFldVal(const TStrKdV& FldNmValPrV, const TStr& FldNm, const TStr& DefFldVal = "");
	static void GetFldValV(const TStrKdV& FldNmValPrV, const TStr& FldNm, TStrV& FldValV);
	static bool IsFldNmVal(const TStrKdV& FldNmValPrV,	const TStr& FldNm, const TStr& FldVal);

private:
	TSAppOutType OutType;

protected:
	PXmlDoc GetErrorRes(const TStr& ErrorStr);

public:
	TSAppSrvFun(const TSAppOutType& _OutType = saotXml): OutType(_OutType) { }
	virtual ~TSAppSrvFun() { }

	// output type
	TSAppOutType GetFunOutType() const { return OutType; }
    // name of the function, corresponds to URL path
    virtual TStr GetFunNm() const = 0;
    // executes function using parameters passed after ? and returns XML doc
	virtual PXmlDoc Exec(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv RqEnv) { 
		EAssert(OutType != saotXml); return TXmlDoc::New(); }
    // executes function using parameters passed after ? and returns JSon doc
	virtual TStr ExecJSon(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv RqEnv) {
		EAssert(OutType != saotJSon); return ""; };
    // executes function using parameters passed after ? and returns JavaScript doc
	virtual PSIn ExecCustom(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv RqEnv,
		TStr& ContTypeStr) { EAssert(OutType != saotCustom); return NULL; };
};

//////////////////////////////////////
// Simple-App-Server
ClassTE(TSAppSrv, TWebSrv)//{
private:
	TStr XmlHdStr;
    THash<TStr, PSAppSrvFun> FunNmToFunH;
public:
    TSAppSrv(const int& PortN, const TSAppSrvFunV& SrvFunV, const PNotify& Notify);
    static PWebSrv New(const int& PortN, const TSAppSrvFunV& SrvFunV, 
        const PNotify& Notify) { return new TSAppSrv(PortN, SrvFunV, Notify); }
    
    virtual void OnHttpRq(const int& SockId, const PHttpRq& HttpRq);
};

//////////////////////////////////////
// Hello-World-Function
ClassTE(TSASFunHelloWorld, TSAppSrvFun)//{
public:
	TSASFunHelloWorld() { }
    static PSAppSrvFun New() { return new TSASFunHelloWorld(); }

    TStr GetFunNm() const { return "hello"; }

    PXmlDoc Exec(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv RqEnv) {
        PXmlTok TopTok = TXmlTok::New("logs");
        TopTok->AddSubTok(TXmlTok::New("HelloWorld"));
        for (int FldN = 0; FldN < FldNmValPrV.Len(); FldN++) {
            PXmlTok ArgTok = TXmlTok::New("arg");
            ArgTok->AddArg(FldNmValPrV[FldN].Key, FldNmValPrV[FldN].Dat);
            TopTok->AddSubTok(ArgTok);
        }
        return TXmlDoc::New(TopTok);
    }
};

//////////////////////////////////////
// Exit-Function
ClassTE(TSASFunExit, TSAppSrvFun)//{
public:
	TSASFunExit() { }
    static PSAppSrvFun New() { return new TSASFunExit(); }

    TStr GetFunNm() const { return "exit"; }

    PXmlDoc Exec(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv RqEnv) {
        // send message to server to quit
        TSysMsg::Quit();
        // send the reply
        PXmlTok TopTok = TXmlTok::New("exit");
        return TXmlDoc::New(TopTok);
    }
};

//////////////////////////////////////
// File-Download-Function
ClassTE(TSASFunFile, TSAppSrvFun)//{
private:
	TStr Url;
	TStr FNm;
	TStr ContType;
	
public:
	TSASFunFile(const TStr& _Url, const TStr& _FNm, const TStr& _ContType): 
	  TSAppSrvFun(saotCustom), Url(_Url), FNm(_FNm), ContType(_ContType) { }
	static PSAppSrvFun New(const TStr& Url, const TStr& FNm, const TStr& ContType) {
		return new TSASFunFile(Url, FNm, ContType); }

    TStr GetFunNm() const { return Url; }

    PSIn ExecCustom(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv RqEnv, TStr& ContTypeStr) {
		ContTypeStr = ContType;
        return TFIn::New(FNm);
    }

	static void LoadFunFileV(const TStr& FPath, TSAppSrvFunV& SrvFunV);
};

//////////////////////////////////////
// RetJpg-Function
ClassTE(TSASFunImg, TSAppSrvFun)//{
private:	
	TStr ContType;	
public:
	TSASFunImg(const TStr& _ContType): TSAppSrvFun(saotCustom), ContType(_ContType) { }
	static PSAppSrvFun New(const TStr& ContType) {
		return new TSASFunImg(ContType); }

    TStr GetFunNm() const { return "seq-jpg"; }

    PSIn ExecCustom(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv RqEnv, TStr& ContTypeStr) {
		ContTypeStr = ContType;
		TStr FNm = GetFldVal(FldNmValPrV, "file");		
        return TFIn::New(FNm);
    }
};


