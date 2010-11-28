//////////////////////////////////////
// Simple-App-Server-Function
bool TSAppSrvFun::IsFldNm(const TStrKdV& FldNmValPrV, const TStr& FldNm) {
	const int ValN = FldNmValPrV.SearchForw(TStrKd(FldNm, ""));
	return (ValN != -1);
}

TStr TSAppSrvFun::GetFldVal(const TStrKdV& FldNmValPrV, 
		const TStr& FldNm, const TStr& DefFldVal) {

	const int ValN = FldNmValPrV.SearchForw(TStrKd(FldNm, ""));
	return (ValN == -1) ? DefFldVal : FldNmValPrV[ValN].Dat;	
}

void TSAppSrvFun::GetFldValV(const TStrKdV& FldNmValPrV, const TStr& FldNm, TStrV& FldValV) {
	FldValV.Clr();
	int ValN = FldNmValPrV.SearchForw(TStrKd(FldNm, ""));
	while (ValN != -1) {
		FldValV.Add(FldNmValPrV[ValN].Dat);
		ValN = FldNmValPrV.SearchForw(TStrKd(FldNm, ""), ValN + 1);
	}
}

bool TSAppSrvFun::IsFldNmVal(const TStrKdV& FldNmValPrV, 
		const TStr& FldNm, const TStr& FldVal) {

	int ValN = FldNmValPrV.SearchForw(TStrKd(FldNm, ""));
	while (ValN != -1) {
		if (FldNmValPrV[ValN].Dat == FldVal) { return true; }
		ValN = FldNmValPrV.SearchForw(TStrKd(FldNm, ""), ValN + 1);
	}
	return false;
}

PXmlDoc TSAppSrvFun::GetErrorRes(const TStr& ErrorStr) {
	PXmlTok TopTok = TXmlTok::New("error", ErrorStr);
	return TXmlDoc::New(TopTok);
}

//////////////////////////////////////////////////////////////////////////
// Simple-App-Server
TSAppSrv::TSAppSrv(const int& PortN, const TSAppSrvFunV& SrvFunV, 
        const PNotify& Notify): TWebSrv(PortN, true, Notify) {

    // initiaize hash-table with mappings
    for (int SrvFunN = 0; SrvFunN < SrvFunV.Len(); SrvFunN++) {
        PSAppSrvFun SrvFun =  SrvFunV[SrvFunN];
        FunNmToFunH.AddDat(SrvFun->GetFunNm(), SrvFun);
    }
	// initialize XML header
	XmlHdStr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n";
}

void TSAppSrv::OnHttpRq(const int& SockId, const PHttpRq& HttpRq) {
    PHttpResp HttpResp;
    try {
        // check http-request correctness - return if error
        EAssertR(HttpRq->IsOk(), "Bad HTTP request!");
        // check url correctness - return if error
        PUrl RqUrl = HttpRq->GetUrl();
        EAssertR(RqUrl->IsOk(), "Bad request URL!");
        // extract function name
        PUrl HttpRqUrl = HttpRq->GetUrl();
        TStr FunNm = HttpRqUrl->GetPathSeg(0);
        EAssertR(FunNmToFunH.IsKey(FunNm) || FunNm.Empty(), "Unknown function '" + FunNm + "' !");
        // extract parameters
        TStrKdV FldNmValPrV;
        PUrlEnv HttpRqUrlEnv = HttpRq->GetUrlEnv();
        const int Keys = HttpRqUrlEnv->GetKeys();
        for (int KeyN = 0; KeyN < Keys; KeyN++) {
            TStr KeyNm = HttpRqUrlEnv->GetKeyNm(KeyN);
            const int Vals = HttpRqUrlEnv->GetVals(KeyN);
            for (int ValN = 0; ValN < Vals; ValN++) {
                TStr Val = HttpRqUrlEnv->GetVal(KeyN, ValN);
                FldNmValPrV.Add(TStrKd(KeyNm, Val));
            }
        }
        // log the call
        TStr TimeNow = TTm::GetCurLocTm().GetWebLogDateTimeStr(true);
        GetNotify()->OnStatus(TStr::Fmt("[%s] Request %s", TimeNow.CStr(), FunNm.CStr()));
		// prepare request environment
		PSAppSrvRqEnv RqEnv = TSAppSrvRqEnv::New(this, SockId, HttpRq);
		PSIn BodySIn; TStr ContTypeVal;
		if (!FunNm.Empty()) {
			// call function
			PSAppSrvFun SrvFun = FunNmToFunH.GetDat(FunNm);
			if (SrvFun->GetFunOutType() == saotXml) {
				PXmlDoc ResXmlDoc = SrvFun->Exec(FldNmValPrV, RqEnv);        
				TStr ResXmlStr; ResXmlDoc->SaveStr(ResXmlStr);
				//ResXmlDoc->SaveTxt(TFile::GetUniqueFNm("test.xml"));
				BodySIn = TMIn::New(XmlHdStr + ResXmlStr);
				ContTypeVal = THttp::TextXmlFldVal;
			} else if (SrvFun->GetFunOutType() == saotJSon) {
				TStr ResStr = SrvFun->ExecJSon(FldNmValPrV, RqEnv);
				BodySIn = TMIn::New(ResStr);
				//ContTypeVal = THttp::TextHtmlFldVal;
				ContTypeVal = THttp::AppJSonFldVal;
			} else {
				BodySIn = SrvFun->ExecCustom(FldNmValPrV, RqEnv, ContTypeVal);
			}
		} else {
			PXmlTok TopTok = TXmlTok::New("registered-functions");
			int KeyId = FunNmToFunH.FFirstKeyId();
			while (FunNmToFunH.FNextKeyId(KeyId)) {
				PXmlTok FunTok = TXmlTok::New("function");
				FunTok->AddArg("name", FunNmToFunH.GetKey(KeyId));
				TopTok->AddSubTok(FunTok);
			}
			PXmlDoc ResXmlDoc = TXmlDoc::New(TopTok);
			TStr ResXmlStr; ResXmlDoc->SaveStr(ResXmlStr);
			BodySIn = TMIn::New(XmlHdStr + ResXmlStr);
			ContTypeVal = THttp::TextXmlFldVal;
		}
        // prepare response
        HttpResp = THttpResp::New(THttp::OkStatusCd, 
            ContTypeVal, false, BodySIn);
        // send response
    } catch (PExcept Except) {
		PXmlTok TopTok = TXmlTok::New("error");
		TopTok->AddSubTok(TXmlTok::New("message", Except->GetMsgStr()));
		TopTok->AddSubTok(TXmlTok::New("location", Except->GetLocStr()));
		PXmlDoc ErrorXmlDoc = TXmlDoc::New(TopTok); 
        TStr ResXmlStr; ErrorXmlDoc->SaveStr(ResXmlStr);
        HttpResp = THttpResp::New(THttp::OkStatusCd, 
            THttp::TextHtmlFldVal, false, 
			TMIn::New(XmlHdStr + ResXmlStr));
    } catch (...) {
		PXmlDoc ErrorXmlDoc = TXmlDoc::New(TXmlTok::New("error")); 
        TStr ResXmlStr; ErrorXmlDoc->SaveStr(ResXmlStr);
        HttpResp = THttpResp::New(THttp::OkStatusCd, 
            THttp::TextHtmlFldVal, false, 
			TMIn::New(XmlHdStr + ResXmlStr));
    }
    SendHttpResp(SockId, HttpResp); 
}

//////////////////////////////////////
// File-Download-Function
void TSASFunFile::LoadFunFileV(const TStr& FPath, TSAppSrvFunV& SrvFunV) {
	TFFile File(FPath, "", false); TStr FNm;
	while (File.Next(FNm)) {
		TStr FExt = FNm.GetFExt();
		TStr FUrl = FNm.GetSubStr(FPath.Len());
		FUrl.ChangeChAll('\\', '/');
		printf("%s %s %s\n", FNm.CStr(), FExt.CStr(), FUrl.CStr());
		if (FExt == ".htm") { SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::TextHtmlFldVal)); }
		else if (FExt == ".html") { SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::TextHtmlFldVal)); }
		else if (FExt == ".js") { SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::TextJavaScriptFldVal)); }
		else if (FExt == ".css") { SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::TextCssFldVal)); }
		else if (FExt == ".jpg") { SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::ImageJpgFldVal)); }
		else if (FExt == ".jpeg") { SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::ImageJpgFldVal)); }
		else if (FExt == ".gif") { SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::ImageGifFldVal)); }
		else {
			printf("Unknown MIME type for extension '%s' for file '%s'", FExt.CStr(), FNm.CStr());
			SrvFunV.Add(TSASFunFile::New(FUrl, FNm, THttp::AppOctetFldVal)); 
		}
	}	
}
