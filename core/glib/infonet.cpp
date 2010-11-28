/////////////////////////////////////////////////
// Includes
#include "stdafx.h"
#include "infonet.h"

/////////////////////////////////////////////////
// Thermal-Schedule
TStr TThermalSch::GetFldVal(const PXmlTok& RecXmlTok, const TStr& FldNm) const {
  PXmlTok FldTagTok;
  if (RecXmlTok->IsTagTok(FldNm, FldTagTok)){
    return FldTagTok->GetTokStr(false);}
  else {return "";}
}

TStr TThermalSch::GetFldVal(const PXmlDoc& RecXmlDoc, const TStr& FldNm) const {
  PXmlTok FldTagTok;
  if (RecXmlDoc->IsTagTok(FldNm, FldTagTok)){
    return FldTagTok->GetTokStr(false);}
  else {return "";}
}

TStr TThermalSch::GetFldDateVal(const PXmlTok& RecXmlTok, const TStr& FldNm) const {
  TStr DateVal=TThermalSch::GetFldVal(RecXmlTok, FldNm);
  DateVal.DelChAll('-');
  return DateVal;
}

TStr TThermalSch::GetFldDateVal(const PXmlDoc& RecXmlDoc, const TStr& FldNm) const {
  TStr DateVal=TThermalSch::GetFldVal(RecXmlDoc, FldNm);
  DateVal.DelChAll('-');
  return DateVal;
}

TStrV TThermalSch::GetFldValV(const PXmlTok& RecXmlTok, const TStr& FldNm,
 const TStr& XFldNm, const TStr& SubFldNm) const {
  TStrV FldValV;
  PXmlTok FldTagTok;
  if (RecXmlTok->IsTagTok(FldNm, FldTagTok)){
    if (SubFldNm.Empty()){
      FldTagTok->GetTagValV(XFldNm, false, FldValV);
    } else {
      TXmlTokV XmlTokV; FldTagTok->GetTagTokV(XFldNm, XmlTokV);
      for (int FldN=0; FldN<XmlTokV.Len(); FldN++){
        TStr FldVal=GetFldVal(XmlTokV[FldN], SubFldNm);
        FldValV.Add(FldVal);
      }
    }
  }
  return FldValV;
}

TStr TThermalSch::GetXmlOut_Ok(const TStr& OpNm){
  TChA ChA;
  ChA+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  ChA+="<ParamOut>\n";
  ChA+="<OperationName>";
  ChA+=TXmlLx::GetXmlStrFromPlainStr(OpNm);
  ChA+="</OperationName>\n";
  ChA+="<Success>Ok</Success>\n";
  ChA+="</ParamOut>\n";
  return ChA;
}

TStr TThermalSch::GetXmlOut_Error(const TStr& OpNm, const TStr& MsgStr){
  TChA ChA;
  ChA+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  ChA+="<ParamOut>\n";
  ChA+="<OperationName>";
  ChA+=TXmlLx::GetXmlStrFromPlainStr(OpNm);
  ChA+="</OperationName>\n";
  ChA+="<Success>Error</Success>\n";
  ChA+="<ResultMessage>";
  ChA+=TXmlLx::GetXmlStrFromPlainStr(MsgStr);
  ChA+="</ResultMessage>\n";
  ChA+="</ParamOut>\n";
  return ChA;
}

TStr TThermalSch::ExeCmFromXml(const PSIn& SIn){
  // load xml-document
  PXmlDoc XmlDoc=TXmlDoc::LoadTxt(SIn);
  // process stream or report error
  if (XmlDoc->IsOk()){
    TStr CmTagNm="Operation|OperationName";
    if (XmlDoc->IsTagTok(CmTagNm)){
      // get command name
      TStr CmNm=XmlDoc->GetTagTokStr(CmTagNm);
      // process commands
      if (CmNm=="SetCatalogueTherapyPlaces"){
        return ExeCm_SetCatalogueTherapyPlaces(XmlDoc);
      } else if (CmNm=="SetCatalogueActivities"){
        return ExeCm_SetCatalogueActivities(XmlDoc);
      } else if (CmNm=="SetCatalogueAvailableTherapyPlaces"){
        return ExeCm_SetCatalogueAvailableTherapyPlaces(XmlDoc);
      } else if (CmNm=="SetCatalogueAvailableActivities"){
        return ExeCm_SetCatalogueAvailableActivities(XmlDoc);
      } else if (CmNm=="SetScheduledActivities"){
        return ExeCm_SetScheduledActivities(XmlDoc);
      } else if (CmNm=="SetCatalogueHolidays"){
        return ExeCm_SetCatalogueHolidays(XmlDoc);
      } else if (CmNm=="MakeSchedule"){
        return ExeCm_MakeSchedule(XmlDoc);
      } else if (CmNm=="CheckSchedule"){
        return ExeCm_CheckSchedule(XmlDoc);
      } else {
        // error: invalid command
        TStr CmXmlStr=TXmlLx::GetXmlStrFromPlainStr(CmNm);
        return GetXmlOut_Error(CmXmlStr, TStr("Invalid Command: '")+CmXmlStr+"'.");
      }
    } else {
      // error: command tag not found
      TStr CmTagXmlStr=TXmlLx::GetXmlStrFromPlainStr(CmTagNm);
      return GetXmlOut_Error("NoCommand", TStr("Command tag not found: '")+CmTagXmlStr+"'.");
    }
  } else {
    // error: invalid xml format
    return GetXmlOut_Error("NoCommand", TStr("Invalid XML Format: '")+XmlDoc->GetMsgStr()+"'.");
  }
}

TStr TThermalSch::ExeCmFromXmlFile(const TStr& FNm, const TStr& FPath){
  // get full-file-name
  TStr FullFNm=TStr::GetNrFPath(FPath)+FNm;
  // create file-stream
  bool OpenedP;
  PSIn SIn=TFIn::New(FullFNm, OpenedP);
  // process stream or report error
  if (OpenedP){
    // process stream
    return ExeCmFromXml(SIn);
  } else {
    // error: can not open file
    return GetXmlOut_Error("NoCommand", TStr("Can not open file: '")+FNm+"'.");
  }
}

TStr TThermalSch::ExeCmFromXmlStr(const TStr& Str){
  // create string-stream
  PSIn SIn=TStrIn::New(Str);
  // process stream
  return ExeCmFromXml(SIn);
}

TStr TThermalSch::ExeCm_SetCatalogueTherapyPlaces(const PXmlDoc& XmlDoc){
  TherapyPlacesXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsTherapyPlacesOk(MsgStr)){
    return TThermalSch::GetXmlOut_Ok("SetCatalogueTherapyPlaces");
  } else {
    TherapyPlacesXmlDoc=NULL;
    return MsgStr;
  }
}

TStr TThermalSch::ExeCm_SetCatalogueActivities(const PXmlDoc& XmlDoc){
  ActivitiesXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsActivitiesOk(MsgStr)){
    return TThermalSch::GetXmlOut_Ok("SetCatalogueActivities");
  } else {
    ActivitiesXmlDoc=NULL;
    return MsgStr;
  }
}

TStr TThermalSch::ExeCm_SetCatalogueAvailableTherapyPlaces(const PXmlDoc& XmlDoc){
  AvailableTherapyPlacesXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsAvailableTherapyPlacesOk(MsgStr)){
    return TThermalSch::GetXmlOut_Ok("SetCatalogueAvailableTherapyPlaces");
  } else {
    AvailableTherapyPlacesXmlDoc=NULL;
    return MsgStr;
  }
}

TStr TThermalSch::ExeCm_SetCatalogueAvailableActivities(const PXmlDoc& XmlDoc){
  AvailableActivitiesXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsAvailableActivitiesOk(MsgStr)){
    return TThermalSch::GetXmlOut_Ok("SetCatalogueAvailableActivities");
  } else {
    AvailableActivitiesXmlDoc=NULL;
    return MsgStr;
  }
}

TStr TThermalSch::ExeCm_SetScheduledActivities(const PXmlDoc& XmlDoc){
  ScheduledActivitiesXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsScheduledActivitiesOk(MsgStr)){
    return TThermalSch::GetXmlOut_Ok("SetScheduledActivities");
  } else {
    ScheduledActivitiesXmlDoc=NULL;
    return MsgStr;
  }
}

TStr TThermalSch::ExeCm_SetCatalogueHolidays(const PXmlDoc& XmlDoc){
  HolidaysXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsHolidaysOk(MsgStr)){
    return TThermalSch::GetXmlOut_Ok("SetCatalogueHolidays");
  } else {
    HolidaysXmlDoc=NULL;
    return MsgStr;
  }
}

TStr TThermalSch::ExeCm_MakeSchedule(const PXmlDoc& XmlDoc){
  MakeScheduleXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsMakeScheduleOk(MsgStr)){
    SaveOldCsv();
    TChA CmLn;
    CmLn+="genscher.exe";
    CmLn+=" -dir"; CmLn+=GetMakeSchedule_PatientATPCode();
    CmLn+=" -pass";
    system(CmLn.CStr());
    TThermalSch::SaveCsvToXml("URNIKXXG.TXT", "URNIKXX0.TXT", "Out.Xml");
    TThermalSch::SaveCsvToXmlParamOut("URNIKXXG.TXT", "URNIKXX0.TXT", "ParamOut.Xml");
    TStr XmlStr=TStr::LoadTxt("ParamOut.Xml");
    return XmlStr;
  } else {
    MakeScheduleXmlDoc=NULL;
    return MsgStr;
  }
}

TStr TThermalSch::ExeCm_CheckSchedule(const PXmlDoc& XmlDoc){
  CheckScheduleXmlDoc=XmlDoc;
  TStr MsgStr;
  if (IsCheckScheduleOk(MsgStr)){
    return TThermalSch::GetXmlOut_Ok("CheckSchedule");
  } else {
    CheckScheduleXmlDoc=NULL;
    return MsgStr;
  }
}

bool TThermalSch::IsTherapyPlacesOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (TherapyPlacesXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("SetCatalogueTherapyPlaces", "TherapyPlaces doesn't exists.");
  }
  // get number of therapy-places
  int TherapyPlaces=TThermalSch::GetTherapyPlaces();
  // check individual therapy-places
  for (int TherapyPlaceN=0; TherapyPlaceN<TherapyPlaces; TherapyPlaceN++){
    PXmlTok TherapyPlace=TThermalSch::GetTherapyPlace(TherapyPlaceN);
    if (TherapyPlace->IsTag("TherapyPlace")){
      GetTherapyPlace_Code(TherapyPlace);
      GetTherapyPlace_Type(TherapyPlace);
      GetTherapyPlace_Location(TherapyPlace);
      GetTherapyPlace_Capacity(TherapyPlace);
      GetTherapyPlace_Distance(TherapyPlace);
      GetTherapyPlace_Gender(TherapyPlace);
      GetTherapyPlace_AlternateScheduling(TherapyPlace);
      GetTherapyPlace_FamilyScheduling(TherapyPlace);
      GetTherapyPlace_TherapistType(TherapyPlace);
    } else {
      MsgStr=GetXmlOut_Error("SetCatalogueTherapyPlaces", "Invalid TherapyPlaces.");
      return false;
    }
  }
  return true;
}

int TThermalSch::GetTherapyPlaces() const {
  if (TherapyPlacesXmlDoc.Empty()){return 0;}
  TXmlTokV XmlTokV;
  TherapyPlacesXmlDoc->GetTagTokV("Operation|Parameter|TherapyPlaces|TherapyPlace", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetTherapyPlace(const int& TherapyPlaceN) const {
  TXmlTokV XmlTokV;
  TherapyPlacesXmlDoc->GetTagTokV("Operation|Parameter|TherapyPlaces|TherapyPlace", XmlTokV);
  return XmlTokV[TherapyPlaceN];
}

bool TThermalSch::IsActivitiesOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (ActivitiesXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("SetCatalogueActivities", "Activities doesn't exists.");
  }
  // get number of therapy-places
  int Activities=TThermalSch::GetActivities();
  // check individual therapy-places
  for (int ActivityN=0; ActivityN<Activities; ActivityN++){
    PXmlTok Activity=TThermalSch::GetActivity(ActivityN);
    if (Activity->IsTag("Activity")){
      GetActivity_Code(Activity);
      GetActivity_Type(Activity);
      GetActivity_Name(Activity);
      GetActivity_Priority(Activity);
      GetActivity_Duration(Activity);
      GetActivity_TPDuration(Activity);
      GetActivity_PatientDuration(Activity);
      GetActivity_TherapistDurationPreparation(Activity);
      GetActivity_TherapistDurationExecution(Activity);
      GetActivity_TherapistDurationAdditionalTime(Activity);
      GetActivity_Weariness(Activity);
      GetActivity_Dry(Activity);
    } else {
      MsgStr=GetXmlOut_Error("SetCatalogueActivities", "Invalid Activities.");
      return false;
    }
  }
  return true;
}

int TThermalSch::GetActivities() const {
  if (ActivitiesXmlDoc.Empty()){return 0;}
  TXmlTokV XmlTokV;
  ActivitiesXmlDoc->GetTagTokV("Operation|Parameter|Activities|Activity", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetActivity(const int& ActivityN) const {
  TXmlTokV XmlTokV;
  ActivitiesXmlDoc->GetTagTokV("Operation|Parameter|Activities|Activity", XmlTokV);
  return XmlTokV[ActivityN];
}

bool TThermalSch::IsAvailableTherapyPlacesOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (AvailableTherapyPlacesXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("SetCatalogueAvailableTherapyPlaces", "Available-Therapy-Places doesn't exists.");
    return false;
  }
  // get number of therapy-places
  int AvailableTherapyPlaces=TThermalSch::GetAvailableTherapyPlaces();
  // check individual therapy-places
  for (int AvailableTherapyPlaceN=0; AvailableTherapyPlaceN<AvailableTherapyPlaces; AvailableTherapyPlaceN++){
    PXmlTok AvailableTherapyPlace=TThermalSch::GetAvailableTherapyPlace(AvailableTherapyPlaceN);
    if (AvailableTherapyPlace->IsTag("AvailableTherapyPlace")){
      GetAvailableTherapyPlace_Code(AvailableTherapyPlace);
      int TherapyPlaceSchedules=GetTherapyPlaceSchedules(AvailableTherapyPlace);
      for (int TherapyPlaceScheduleN=0; TherapyPlaceScheduleN<TherapyPlaceSchedules; TherapyPlaceScheduleN++){
        PXmlTok TherapyPlaceSchedule=GetTherapyPlaceSchedule(AvailableTherapyPlace, TherapyPlaceScheduleN);
        if (TherapyPlaceSchedule->IsTag("TherapyPlaceSchedule")){
          GetTherapyPlaceSchedule_WeekDays(TherapyPlaceSchedule);
          GetTherapyPlaceSchedule_OpenFrom(TherapyPlaceSchedule);
          GetTherapyPlaceSchedule_OpenTill(TherapyPlaceSchedule);
          GetTherapyPlaceSchedule_ValidFrom(TherapyPlaceSchedule);
          GetTherapyPlaceSchedule_ValidTo(TherapyPlaceSchedule);
        }
      }
    } else {
      MsgStr=GetXmlOut_Error("SetCatalogueAvailableTherapyPlaces", "Invalid Available-Therapy-Places.");
      return false;
    }
  }
  return true;
}

int TThermalSch::GetAvailableTherapyPlaces() const {
  if (AvailableTherapyPlacesXmlDoc.Empty()){return 0;}
  TXmlTokV XmlTokV;
  AvailableTherapyPlacesXmlDoc->GetTagTokV("Operation|Parameter|AvailableTherapyPlaces|AvailableTherapyPlace", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetAvailableTherapyPlace(const int& AvailableTherapyPlaceN) const {
  TXmlTokV XmlTokV;
  AvailableTherapyPlacesXmlDoc->GetTagTokV("Operation|Parameter|AvailableTherapyPlaces|AvailableTherapyPlace", XmlTokV);
  return XmlTokV[AvailableTherapyPlaceN];
}

int TThermalSch::GetTherapyPlaceSchedules(const PXmlTok& AvailableTherapyPlace) const {
  TXmlTokV XmlTokV;
  AvailableTherapyPlacesXmlDoc->GetTagTokV("Operation|Parameter|AvailableTherapyPlaces|AvailableTherapyPlace|TherapyPlaceSchedules|TherapyPlaceSchedule", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetTherapyPlaceSchedule(const PXmlTok& AvailableTherapyPlace, const int& TherapyPlaceScheduleN) const {
  TXmlTokV XmlTokV;
  AvailableTherapyPlacesXmlDoc->GetTagTokV("Operation|Parameter|AvailableTherapyPlaces|AvailableTherapyPlace|TherapyPlaceSchedules|TherapyPlaceSchedule", XmlTokV);
  return XmlTokV[TherapyPlaceScheduleN];
}

bool TThermalSch::IsAvailableActivitiesOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (AvailableActivitiesXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("SetCatalogueAvailableActivities", "Available-Activities doesn't exists.");
    return false;
  }
  // get number of therapy-places
  int AvailableActivities=TThermalSch::GetAvailableActivities();
  // check individual therapy-places
  for (int AvailableActivityN=0; AvailableActivityN<AvailableActivities; AvailableActivityN++){
    PXmlTok AvailableActivity=TThermalSch::GetAvailableActivity(AvailableActivityN);
    if (AvailableActivity->IsTag("AvailableActivity")){
      GetAvailableActivity_Code(AvailableActivity);
      int ActivitySchedules=GetActivitySchedules(AvailableActivity);
      for (int ActivityScheduleN=0; ActivityScheduleN<ActivitySchedules; ActivityScheduleN++){
        PXmlTok ActivitySchedule=GetActivitySchedule(AvailableActivity, ActivityScheduleN);
        if (ActivitySchedule->IsTag("ActivitySchedule")){
          GetActivitySchedule_WeekDays(ActivitySchedule);
          GetActivitySchedule_StartTime(ActivitySchedule);
          GetActivitySchedule_EndTime(ActivitySchedule);
          GetActivitySchedule_TherapyPlaceType(ActivitySchedule);
          GetActivitySchedule_ValidFrom(ActivitySchedule);
          GetActivitySchedule_ValidTo(ActivitySchedule);
        }
      }
    } else {
      MsgStr=GetXmlOut_Error("SetCatalogueAvailableActivities", "Invalid Available-Activities.");
      return false;
    }
  }
  return true;
}

int TThermalSch::GetAvailableActivities() const {
  if (AvailableActivitiesXmlDoc.Empty()){return 0;}
  TXmlTokV XmlTokV;
  AvailableActivitiesXmlDoc->GetTagTokV("Operation|Parameter|AvailableActivities|AvailableActivity", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetAvailableActivity(const int& AvailableActivityN) const {
  TXmlTokV XmlTokV;
  AvailableActivitiesXmlDoc->GetTagTokV("Operation|Parameter|AvailableActivities|AvailableActivity", XmlTokV);
  return XmlTokV[AvailableActivityN];
}

int TThermalSch::GetActivitySchedules(const PXmlTok& AvailableActivity) const {
  TXmlTokV XmlTokV;
  AvailableActivity->GetTagTokV("ActivitySchedules|ActivitySchedule", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetActivitySchedule(const PXmlTok& AvailableActivity, const int& ActivityScheduleN) const {
  TXmlTokV XmlTokV;
  AvailableActivity->GetTagTokV("ActivitySchedules|ActivitySchedule", XmlTokV);
  return XmlTokV[ActivityScheduleN];
}

bool TThermalSch::IsScheduledActivitiesOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (ScheduledActivitiesXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("SetScheduledActivities", "Scheduled-Activities doesn't exists.");
    return false;
  }
  // get number of therapy-places
  int ScheduledActivities=TThermalSch::GetScheduledActivities();
  // check individual therapy-places
  for (int ScheduledActivityN=0; ScheduledActivityN<ScheduledActivities; ScheduledActivityN++){
    PXmlTok ScheduledActivity=TThermalSch::GetScheduledActivity(ScheduledActivityN);
    if (ScheduledActivity->IsTag("ScheduledActivity")){
      GetScheduledActivity_Code(ScheduledActivity);
      GetScheduledActivity_Code(ScheduledActivity);
      GetScheduledActivity_PatientATPCode(ScheduledActivity);
      GetScheduledActivity_Patient(ScheduledActivity);
      GetScheduledActivity_TherapistCode(ScheduledActivity);
      GetScheduledActivity_TherapyPlaceCode(ScheduledActivity);
      GetScheduledActivity_ActivityCode(ScheduledActivity);
      GetScheduledActivity_StartDateTime(ScheduledActivity);
      GetScheduledActivity_RepetitionNumber(ScheduledActivity);
      GetScheduledActivity_StartingPoint(ScheduledActivity);
      GetScheduledActivity_PatientDuration(ScheduledActivity);
    } else {
      MsgStr=GetXmlOut_Error("SetScheduledActivities", "Invalid Scheduled-Activities.");
      return false;
    }
  }
  return true;
}

int TThermalSch::GetScheduledActivities() const {
  if (ScheduledActivitiesXmlDoc.Empty()){return 0;}
  TXmlTokV XmlTokV;
  ScheduledActivitiesXmlDoc->GetTagTokV("Operation|Parameter|ScheduledActivities|ScheduledActivity", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetScheduledActivity(const int& ScheduledActivityN) const {
  TXmlTokV XmlTokV;
  ScheduledActivitiesXmlDoc->GetTagTokV("Operation|Parameter|ScheduledActivities|ScheduledActivity", XmlTokV);
  return XmlTokV[ScheduledActivityN];
}

bool TThermalSch::IsHolidaysOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (HolidaysXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("SetCatalogueHolidays", "Holidays doesn't exists.");
    return false;
  }
  // get number of therapy-places
  int Holidays=TThermalSch::GetHolidays();
  // check individual therapy-places
  for (int HolidayN=0; HolidayN<Holidays; HolidayN++){
    PXmlTok Holiday=TThermalSch::GetHoliday(HolidayN);
    if (Holiday->IsTag("Holiday")){
      TStr Str=GetHoliday_Date(Holiday);
    } else {
      MsgStr=GetXmlOut_Error("SetCatalogueHolidays", "Invalid Holidays.");
      return false;
    }
  }
  return true;
}

int TThermalSch::GetHolidays() const {
  if (HolidaysXmlDoc.Empty()){return 0;}
  TXmlTokV XmlTokV;
  HolidaysXmlDoc->GetTagTokV("Operation|Parameter|Holidays|Holiday", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetHoliday(const int& HolidayN) const {
  TXmlTokV XmlTokV;
  HolidaysXmlDoc->GetTagTokV("Operation|Parameter|Holidays|Holiday", XmlTokV);
  return XmlTokV[HolidayN];
}

bool TThermalSch::IsMakeScheduleOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (MakeScheduleXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("MakeSchedule", "Make-Schedule doesn't exists.");
    return false;
  }
  // check top-level fields
  GetMakeSchedule_PatientCode();
  GetMakeSchedule_PatientGender();
  GetMakeSchedule_PatientATPCode();
  // get recs
  int PlanRequests=TThermalSch::GetMakeSchedule_PlanRequests();
  // check individual recs
  for (int PlanRequestN=0; PlanRequestN<PlanRequests; PlanRequestN++){
    PXmlTok PlanRequest=TThermalSch::GetMakeSchedule_PlanRequest(PlanRequestN);
    if (PlanRequest->IsTag("PlanRequest")){
      GetMakeSchedule_PlanRequest_ActivityCode(PlanRequest);
      GetMakeSchedule_PlanRequest_Quantity(PlanRequest);
      GetMakeSchedule_PlanRequest_DayQuantity(PlanRequest);
      GetMakeSchedule_PlanRequest_Frequency(PlanRequest);
      GetMakeSchedule_PlanRequest_AlternatingActivityCode(PlanRequest);
      GetMakeSchedule_PlanRequest_ExcludedActivities_ActivityCode(PlanRequest);
      GetMakeSchedule_PlanRequest_ExcludedActivities_Time(PlanRequest);
      GetMakeSchedule_PlanRequest_FamilyActivity(PlanRequest);
      GetMakeSchedule_PlanRequest_TreatmentDate(PlanRequest);
      GetMakeSchedule_PlanRequest_DesiredStartTime(PlanRequest);
      GetMakeSchedule_PlanRequest_DesiredFinishTime(PlanRequest);
    } else {
      MsgStr=GetXmlOut_Error("MakeSchedule", "Invalid Make-Schedule.");
      return false;
    }
  }
  return true;
}

int TThermalSch::GetMakeSchedule_PlanRequests() const {
  if (MakeScheduleXmlDoc.Empty()){return 0;}
  TXmlTokV XmlTokV;
  MakeScheduleXmlDoc->GetTagTokV("Operation|Parameter|PlanRequests|PlanRequest", XmlTokV);
  return XmlTokV.Len();
}

PXmlTok TThermalSch::GetMakeSchedule_PlanRequest(const int& PlanRequestN) const {
  TXmlTokV XmlTokV;
  MakeScheduleXmlDoc->GetTagTokV("Operation|Parameter|PlanRequests|PlanRequest", XmlTokV);
  return XmlTokV[PlanRequestN];
}

bool TThermalSch::IsCheckScheduleOk(TStr& MsgStr) const {
  // check if therapy-places exists
  if (CheckScheduleXmlDoc.Empty()){
    MsgStr=GetXmlOut_Error("CheckSchedule", "Check-Schedule doesn't exists.");
    return false;
  }
  // check top-level fields
  GetCheckSchedule_PatientCode();
  GetCheckSchedule_PatientGender();
  GetCheckSchedule_PatientATPCode();
  GetCheckSchedule_IntRequestedActivityActivityCode();
  GetCheckSchedule_IntRequestedActivityDayQuantity();
  GetCheckSchedule_IntRequestedActivityTreatmentDate();
  GetCheckSchedule_IntRequestedActivityDesiredStartTime();
  GetCheckSchedule_IntRequestedActivityDesiredFinishTime();
  return true;
}

void TThermalSch::SaveCsvToXml(
 const TStr& DefFNm, const TStr& DatFNm, const TStr& XmlFNm){
  // file existence
  if (!TFile::Exists(DefFNm)){
    TFOut XmlSOut(XmlFNm); FILE* XmlFId=XmlSOut.GetFileId();
    fprintf(XmlFId, "%s", GetXmlOut_Error("NoCommand", TStr("Can not open file: '")+DefFNm+"'."));
    return;
  }
  if (!TFile::Exists(DatFNm)){
    TFOut XmlSOut(XmlFNm); FILE* XmlFId=XmlSOut.GetFileId();
    fprintf(XmlFId, "%s", GetXmlOut_Error("NoCommand", TStr("Can not open file: '")+DatFNm+"'."));
    return;
  }
  // definition file
  PSs DefSs=TSs::LoadTxt(ssfCommaSep, DefFNm);
  TStrV FldNmV;
  for (int Y=0; Y<DefSs->GetYLen(); Y++){
    FldNmV.Add(DefSs->GetVal(0, Y));}
  // data file
  PSs DatSs=TSs::LoadTxt(ssfCommaSep, DatFNm);
  TVec<TStrV> RecFldValVV;
  {for (int Y=0; Y<DatSs->GetYLen(); Y++){
    TStrV FldValV;
    for (int X=0; X<DatSs->GetXLen(Y); X++){
      TStr FldVal=DatSs->GetVal(X, Y);
      FldValV.Add(FldVal);
    }
    RecFldValVV.Add(FldValV);
  }}
  // save to xml
  TFOut XmlSOut(XmlFNm); FILE* XmlFId=XmlSOut.GetFileId();
  if ((DefFNm=="URNIKXXG.TXT")&&(DatFNm=="URNIKXX0.TXT")){
    fprintf(XmlFId, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(XmlFId, "<!-- edited with DarMar XML v3.14 by Darko Zupanic & Marko Grobelnik -->\n");
    fprintf(XmlFId, "<Operation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"D:\\App\\zdr\\te\\ParamIN.xsd\">\n");
    fprintf(XmlFId, "<OperationName>SetScheduledActivities</OperationName>\n");
    fprintf(XmlFId, "<Parameter xsi:type=\"tParamInSetScheduledActivities\">\n");
    fprintf(XmlFId, "<ScheduledActivities>\n");
    for (int RecN=0; RecN<RecFldValVV.Len(); RecN++){
      TStrV& FldValV=RecFldValVV[RecN];
      fprintf(XmlFId, "<ScheduledActivity>\n");
      for (int FldN=0; FldN<FldValV.Len(); FldN++){
        TStr FldNm=FldNmV[FldN];
        if (FldNm=="SIFRA101"){FldNm="PatientCode";}
        else if (FldNm=="SIFRA060"){FldNm="ActivityCode";}
        else if (FldNm=="SIFRA052"){FldNm="TherapyPlaceCode";}
        else if (FldNm=="DATUM060"){FldNm="Date";}
        else if (FldNm=="START060"){FldNm="StartTime";}
        else if (FldNm=="NARP_060"){FldNm="PatientDuration";}
        else if (FldNm=="NARA_060"){FldNm="TPDuration";}
        else if (FldNm=="UJEMANJE"){FldNm="Accordance";}
        fprintf(XmlFId, "<%s>%s</%s>\n",
         TXmlLx::GetXmlStrFromPlainStr(FldNm).CStr(),
         TXmlLx::GetXmlStrFromPlainStr(FldValV[FldN]).CStr(),
         TXmlLx::GetXmlStrFromPlainStr(FldNm).CStr());
      }
      fprintf(XmlFId, "</ScheduledActivity>\n");
    }
    fprintf(XmlFId, "</ScheduledActivities>\n");
    fprintf(XmlFId, "</Parameter>\n");
    fprintf(XmlFId, "</Operation>\n");
  } else {
    fprintf(XmlFId, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(XmlFId, "<Data>\n");
    for (int RecN=0; RecN<RecFldValVV.Len(); RecN++){
      TStrV& FldValV=RecFldValVV[RecN];
      fprintf(XmlFId, "<Record>\n");
      for (int FldN=0; FldN<FldValV.Len(); FldN++){
        fprintf(XmlFId, "<Field Name=\"%s\" Value=\"%s\"/>\n",
         TXmlLx::GetXmlStrFromPlainStr(FldNmV[FldN]).CStr(),
         TXmlLx::GetXmlStrFromPlainStr(FldValV[FldN]).CStr());
      }
      fprintf(XmlFId, "</Record>\n");
    }
    fprintf(XmlFId, "</Data>\n");
  }
}

void TThermalSch::SaveCsvToXmlParamOut(
 const TStr& DefFNm, const TStr& DatFNm, const TStr& XmlFNm){
  // file existence
  if (!TFile::Exists(DefFNm)){
    TFOut XmlSOut(XmlFNm); FILE* XmlFId=XmlSOut.GetFileId();
    fprintf(XmlFId, "%s", GetXmlOut_Error("NoCommand", TStr("Can not open file: '")+DefFNm+"'."));
    return;
  }
  if (!TFile::Exists(DatFNm)){
    TFOut XmlSOut(XmlFNm); FILE* XmlFId=XmlSOut.GetFileId();
    fprintf(XmlFId, "%s", GetXmlOut_Error("NoCommand", TStr("Can not open file: '")+DatFNm+"'."));
    return;
  }
  // definition file
  PSs DefSs=TSs::LoadTxt(ssfCommaSep, DefFNm);
  TStrV FldNmV;
  for (int Y=0; Y<DefSs->GetYLen(); Y++){
    FldNmV.Add(DefSs->GetVal(0, Y));}
  // data file
  PSs DatSs=TSs::LoadTxt(ssfCommaSep, DatFNm);
  TVec<TStrV> RecFldValVV;
  {for (int Y=0; Y<DatSs->GetYLen(); Y++){
    TStrV FldValV;
    for (int X=0; X<DatSs->GetXLen(Y); X++){
      TStr FldVal=DatSs->GetVal(X, Y);
      FldValV.Add(FldVal);
    }
    RecFldValVV.Add(FldValV);
  }}
  // save to xml
  TFOut XmlSOut(XmlFNm); FILE* XmlFId=XmlSOut.GetFileId();
  if ((DefFNm=="URNIKXXG.TXT")&&(DatFNm=="URNIKXX0.TXT")){
    fprintf(XmlFId, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(XmlFId, "<ParamOut>\n");
    fprintf(XmlFId, "<OperationName>");
    fprintf(XmlFId, "MakeSchedule");
    fprintf(XmlFId, "</OperationName>\n");
    fprintf(XmlFId, "<Success>Ok</Success>\n");
    fprintf(XmlFId, "<Data>\n");
    fprintf(XmlFId, "<Operation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"D:\\App\\zdr\\te\\ParamIN.xsd\">\n");
    fprintf(XmlFId, "<OperationName>SetScheduledActivities</OperationName>\n");
    fprintf(XmlFId, "<Parameter xsi:type=\"tParamInSetScheduledActivities\">\n");
    fprintf(XmlFId, "<ScheduledActivities>\n");
    for (int RecN=0; RecN<RecFldValVV.Len(); RecN++){
      TStrV& FldValV=RecFldValVV[RecN];
      fprintf(XmlFId, "<ScheduledActivity>\n");
      for (int FldN=0; FldN<FldValV.Len(); FldN++){
        TStr FldNm=FldNmV[FldN];
        if (FldNm=="SIFRA101"){FldNm="PatientCode";}
        else if (FldNm=="SIFRA060"){FldNm="ActivityCode";}
        else if (FldNm=="SIFRA052"){FldNm="TherapyPlaceCode";}
        else if (FldNm=="DATUM060"){FldNm="Date";}
        else if (FldNm=="START060"){FldNm="StartTime";}
        else if (FldNm=="NARP_060"){FldNm="PatientDuration";}
        else if (FldNm=="NARA_060"){FldNm="TPDuration";}
        else if (FldNm=="UJEMANJE"){FldNm="Accordance";}
        fprintf(XmlFId, "<%s>%s</%s>\n",
         TXmlLx::GetXmlStrFromPlainStr(FldNm).CStr(),
         TXmlLx::GetXmlStrFromPlainStr(FldValV[FldN]).CStr(),
         TXmlLx::GetXmlStrFromPlainStr(FldNm).CStr());
      }
      fprintf(XmlFId, "</ScheduledActivity>\n");
    }
    fprintf(XmlFId, "</ScheduledActivities>\n");
    fprintf(XmlFId, "</Parameter>\n");
    fprintf(XmlFId, "</Operation>\n");
    fprintf(XmlFId, "</Data>\n");
    fprintf(XmlFId, "</ParamOut>\n");
  } else {
    fprintf(XmlFId, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(XmlFId, "<Data>\n");
    for (int RecN=0; RecN<RecFldValVV.Len(); RecN++){
      TStrV& FldValV=RecFldValVV[RecN];
      fprintf(XmlFId, "<Record>\n");
      for (int FldN=0; FldN<FldValV.Len(); FldN++){
        fprintf(XmlFId, "<Field Name=\"%s\" Value=\"%s\"/>\n",
         TXmlLx::GetXmlStrFromPlainStr(FldNmV[FldN]).CStr(),
         TXmlLx::GetXmlStrFromPlainStr(FldValV[FldN]).CStr());
      }
      fprintf(XmlFId, "</Record>\n");
    }
    fprintf(XmlFId, "</Data>\n");
  }
}

void TThermalSch::SaveOldCsv() const {
  SaveOldCsv_KatalogZdrDelavcev();
  SaveOldCsv_KatalogAparatov();
  SaveOldCsv_KatalogAktivnosti();
  SaveOldCsv_UrnikZaTerapevte();
  SaveOldCsv_UrnikZaAparate();
  SaveOldCsv_UrnikAktivnosti();
  SaveOldCsv_MozniTerminiVDnevu();
  SaveOldCsv_Prazniki();
  SaveOldCsv_PredpisaneTerapije();
  SaveOldCsv_PlaniraneAktivnosti();
  SaveOldCsv_UrnikXX();
}

void TThermalSch::SaveOldCsv_KatalogZdrDelavcev() const {
  // definition file
  TFOut DefSOut("KA051ZDG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "PRIM_051");
  SaveFldDef(fDef, "SSIF_S23");
  SaveFldDef(fDef, "SIFRA039");
  SaveFldDef(fDef, "STAT_051");
  SaveFldDef(fDef, "SIFRA051");
  SaveFldDef(fDef, "POZ__051");
  SaveFldDef(fDef, "EMSO_051");
  SaveFldDef(fDef, "DATUMIPL");
  SaveFldDef(fDef, "TIP__051");

  // data file
  TFOut DatSOut("KA051ZD0.TXT"); /*FILE* fDat=DatSOut.GetFileId();*/
}

void TThermalSch::SaveOldCsv_KatalogAparatov() const {
  // definition file
  TFOut DefSOut("KA052APG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA052"); // TherapyPlace -> Code
  SaveFldDef(fDef, "SSIF_T05"); // TherapyPlace -> Type
  SaveFldDef(fDef, "OPIS_052"); // TherapyPlace -> Location
  SaveFldDef(fDef, "SSIF_NE4"); // TherapyPlace -> TherapistType
  SaveFldDef(fDef, "ST_P_052"); // TherapyPlace -> Capacity
  SaveFldDef(fDef, "SIFR1052"); // -
  SaveFldDef(fDef, "PODAL052"); // -
  SaveFldDef(fDef, "TR_DO052"); // -
  SaveFldDef(fDef, "RAZD_052"); // TherapyPlace -> Distance
  SaveFldDef(fDef, "SPOL_052"); // TherapyPlace -> Gender
  SaveFldDef(fDef, "ALTERSCH"); // TherapyPlace -> AlternateScheduling
  SaveFldDef(fDef, "FAMILSCH"); // TherapyPlace -> FamilyScheduling

  // data file
  TFOut DatSOut("KA052AP0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int TherapyPlaces=TThermalSch::GetTherapyPlaces();
  for (int TherapyPlaceN=0; TherapyPlaceN<TherapyPlaces; TherapyPlaceN++){
    PXmlTok TherapyPlace=TThermalSch::GetTherapyPlace(TherapyPlaceN);
    SaveFldVal(fDat, GetTherapyPlace_Code(TherapyPlace)); // SIFRA052
    SaveFldVal(fDat, GetTherapyPlace_Type(TherapyPlace)); // SSIF_T05
    SaveFldVal(fDat, GetTherapyPlace_Location(TherapyPlace)); // OPIS_052
    SaveFldVal(fDat, GetTherapyPlace_TherapistType(TherapyPlace)); // SSIF_NE4
    SaveFldVal(fDat, GetTherapyPlace_Capacity(TherapyPlace)); // ST_P_052
    SaveFldVal(fDat, ""); // SIFR1052
    SaveFldVal(fDat, ""); // PODAL052
    SaveFldVal(fDat, ""); // TR_DO052
    SaveFldVal(fDat, GetTherapyPlace_Distance(TherapyPlace)); // RAZD_052
    SaveFldVal(fDat, GetTherapyPlace_Gender(TherapyPlace)); // SPOL_052
    SaveFldVal(fDat, GetTherapyPlace_AlternateScheduling(TherapyPlace));
    SaveFldVal(fDat, GetTherapyPlace_FamilyScheduling(TherapyPlace), true);
  }
}

void TThermalSch::SaveOldCsv_KatalogAktivnosti() const {
  // definition file
  TFOut DefSOut("KA060AOG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA060"); // Activity -> Code
  SaveFldDef(fDef, "SSIF_T05"); // Activity -> Type
  SaveFldDef(fDef, "TRAJ_060"); // Activity -> Duration
  SaveFldDef(fDef, "STMAX060"); // ???TherapyPlace -> Capacity
  SaveFldDef(fDef, "STOPT060"); // -
  SaveFldDef(fDef, "RAZ1_060"); // Activity -> TherapistDuration -> Preparation
  SaveFldDef(fDef, "RAZ2_060"); // Activity -> TherapistDuration -> Execution
  SaveFldDef(fDef, "RAZ3_060"); // Activity -> TherapistDuration -> AdditionalTime
  SaveFldDef(fDef, "NARP_060"); // Activity -> PatientDuration
  SaveFldDef(fDef, "NARA_060"); // Activity -> TPDuration
  SaveFldDef(fDef, "SSIF_T01"); // ???PlanReqest -> Frequency/DesiredStart...FinishTime
  SaveFldDef(fDef, "SS_T2_T3"); // -
  SaveFldDef(fDef, "TEZA_060"); // Activity -> Weariness
  SaveFldDef(fDef, "SUHA_060"); // Activity -> Dry
  SaveFldDef(fDef, "PRIORITY"); // Activity -> Priority

  // data file
  TFOut DatSOut("KA060AO0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int Activities=TThermalSch::GetActivities();
  for (int ActivityN=0; ActivityN<Activities; ActivityN++){
    PXmlTok Activity=TThermalSch::GetActivity(ActivityN);
    SaveFldVal(fDat, GetActivity_Code(Activity)); // SIFRA060
    SaveFldVal(fDat, GetActivity_Type(Activity)); // SSIF_T05
    SaveFldVal(fDat, GetActivity_Duration(Activity)); // TRAJ_060
    SaveFldVal(fDat, ""); // STMAX060
    SaveFldVal(fDat, ""); // STOPT060
    SaveFldVal(fDat, GetActivity_TherapistDurationPreparation(Activity)); // RAZ1_060
    SaveFldVal(fDat, GetActivity_TherapistDurationExecution(Activity)); // RAZ2_060
    SaveFldVal(fDat, GetActivity_TherapistDurationAdditionalTime(Activity)); // RAZ3_060
    SaveFldVal(fDat, GetActivity_PatientDuration(Activity)); // NARP_060
    SaveFldVal(fDat, GetActivity_TPDuration(Activity)); // NARA_060
    SaveFldVal(fDat, ""); // SSIF_T01
    SaveFldVal(fDat, ""); // SS_T2_T3
    SaveFldVal(fDat, GetActivity_Weariness(Activity)); // TEZA_060
    SaveFldVal(fDat, GetActivity_Dry(Activity)); // SUHA_060
    SaveFldVal(fDat, GetActivity_Priority(Activity), true); // PRIORITY
  }
}

void TThermalSch::SaveOldCsv_UrnikZaTerapevte() const {
  // definition file
  TFOut DefSOut("PR350UIG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "TIP__051");
  SaveFldDef(fDef, "DAN__350");
  SaveFldDef(fDef, "STEV_350");
  SaveFldDef(fDef, "DATOD350");
  SaveFldDef(fDef, "DATDO350");

  // data file
  TFOut DatSOut("PR350UI0.TXT"); /*FILE* fDat=DatSOut.GetFileId();*/
}

void TThermalSch::SaveOldCsv_UrnikZaAparate() const {
  // definition file
  TFOut DefSOut("PR351UPG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA052"); // AvailableTherapyPlace -> Code
  SaveFldDef(fDef, "AKTIV351"); // ???AvailableTherapyPlace -> TherapyPlaceSchedule -> Day
  SaveFldDef(fDef, "DATOD351"); // AvailableTherapyPlace -> TherapyPlaceSchedule -> ValidFrom
  SaveFldDef(fDef, "DATDO351"); // AvailableTherapyPlace -> TherapyPlaceSchedule -> ValidTo
  SaveFldDef(fDef, "CASOD351"); // AvailableTherapyPlace -> TherapyPlaceSchedule -> Open -> From
  SaveFldDef(fDef, "CASDO351"); // AvailableTherapyPlace -> TherapyPlaceSchedule -> Open -> Till
  SaveFldDef(fDef, "DAN"); // AvailableTherapyPlace -> TherapyPlaceSchedule -> Days -> Day

  // data file
  TFOut DatSOut("PR351UP0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int AvailableTherapyPlaces=TThermalSch::GetAvailableTherapyPlaces();
  for (int AvailableTherapyPlaceN=0; AvailableTherapyPlaceN<AvailableTherapyPlaces; AvailableTherapyPlaceN++){
    PXmlTok AvailableTherapyPlace=TThermalSch::GetAvailableTherapyPlace(AvailableTherapyPlaceN);
    int TherapyPlaceSchedules=GetTherapyPlaceSchedules(AvailableTherapyPlace);
    for (int TherapyPlaceScheduleN=0; TherapyPlaceScheduleN<TherapyPlaceSchedules; TherapyPlaceScheduleN++){
      PXmlTok TherapyPlaceSchedule=GetTherapyPlaceSchedule(AvailableTherapyPlace, TherapyPlaceScheduleN);
      TStrV DayV=GetTherapyPlaceSchedule_WeekDays(TherapyPlaceSchedule);
      for (int DayN=0; DayN<DayV.Len(); DayN++){
        SaveFldVal(fDat, GetAvailableTherapyPlace_Code(AvailableTherapyPlace)); // SIFRA052
        SaveFldVal(fDat, "D"); // AKTIV351
        SaveFldVal(fDat, GetTherapyPlaceSchedule_ValidFrom(TherapyPlaceSchedule)); // DATOD351
        SaveFldVal(fDat, GetTherapyPlaceSchedule_ValidTo(TherapyPlaceSchedule)); // DATDO351
        SaveFldVal(fDat, GetTherapyPlaceSchedule_OpenFrom(TherapyPlaceSchedule)); // CASOD351
        SaveFldVal(fDat, GetTherapyPlaceSchedule_OpenTill(TherapyPlaceSchedule)); // CASDO351
        SaveFldVal(fDat, DayV[DayN], true); // DAN
      }
    }
  }
}

void TThermalSch::SaveOldCsv_UrnikAktivnosti() const {
  // definition file
  TFOut DefSOut("PR352UAG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA060"); // AvailableActivity -> Code
  SaveFldDef(fDef, "DAN__352"); // AvailableActivity ->  ActivitySchedule -> Day
  SaveFldDef(fDef, "SIFRA352"); // -
  SaveFldDef(fDef, "DATOD352"); // AvailableActivity -> ActivitySchedule -> ValidFrom
  SaveFldDef(fDef, "DATDO352"); // AvailableActivity -> ActivitySchedule -> ValidTo
  SaveFldDef(fDef, "CASOD352"); // AvailableActivity -> ActivitySchedule -> StartTime
  SaveFldDef(fDef, "CASDO352"); // AvailableActivity -> ActivitySchedule -> EndTime
  SaveFldDef(fDef, "SIFRA052"); // AvailableActivity -> ActivitySchedule -> EndTime
  SaveFldDef(fDef, "SPOL052"); // AvailableActivity -> ActivitySchedule -> Attributes -> Gender

  // data file
  TFOut DatSOut("PR352UA0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int AvailableActivities=TThermalSch::GetAvailableActivities();
  for (int AvailableActivityN=0; AvailableActivityN<AvailableActivities; AvailableActivityN++){
    PXmlTok AvailableActivity=TThermalSch::GetAvailableActivity(AvailableActivityN);
    //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", AvailableActivity->GetTokStr().CStr());
    //printf("%s\n", AvailableActivity->GetTokStr().CStr());
    int ActivitySchedules=GetActivitySchedules(AvailableActivity);
    for (int ActivityScheduleN=0; ActivityScheduleN<ActivitySchedules; ActivityScheduleN++){
      PXmlTok ActivitySchedule=GetActivitySchedule(AvailableActivity, ActivityScheduleN);
      //printf("%s\n", ActivitySchedule->GetTokStr().CStr());
      TStrV DayV=GetActivitySchedule_WeekDays(ActivitySchedule);
      for (int DayN=0; DayN<DayV.Len(); DayN++){
        SaveFldVal(fDat, GetAvailableActivity_Code(AvailableActivity)); // SIFRA060
        SaveFldVal(fDat, DayV[DayN]); // DAN__352
        SaveFldVal(fDat, ""); // SIFRA352
        SaveFldVal(fDat, GetActivitySchedule_ValidFrom(ActivitySchedule)); // DATOD352
        SaveFldVal(fDat, GetActivitySchedule_ValidTo(ActivitySchedule)); // DATDO352
        SaveFldVal(fDat, GetActivitySchedule_StartTime(ActivitySchedule)); // CASOD352
        SaveFldVal(fDat, GetActivitySchedule_EndTime(ActivitySchedule)); // CASDO352
        SaveFldVal(fDat, GetActivitySchedule_TherapyPlaceType(ActivitySchedule)); // SIFRA052
        SaveFldVal(fDat, GetActivitySchedule_Gender(ActivitySchedule), true);  // SPOL052
      }
    }
  }
}

void TThermalSch::SaveOldCsv_MozniTerminiVDnevu() const {
  // definition file
  TFOut DefSOut("PR352URG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA352");
  SaveFldDef(fDef, "ZAPST352");
  SaveFldDef(fDef, "URAOD352");
  SaveFldDef(fDef, "URADO352");

  // data file
  TFOut DatSOut("PR352UR0.TXT"); /*FILE* fDat=DatSOut.GetFileId();*/
}

void TThermalSch::SaveOldCsv_Prazniki() const {
  // definition file
  TFOut DefSOut("PRAZNIKG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "DATUM000");

  // data file
  TFOut DatSOut("PRAZNIK0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int Holidays=TThermalSch::GetHolidays();
  for (int HolidayN=0; HolidayN<Holidays; HolidayN++){
    PXmlTok Holiday=TThermalSch::GetHoliday(HolidayN);
    SaveFldVal(fDat, GetHoliday_Date(Holiday), true); // DATUM000
  }
}

void TThermalSch::SaveOldCsv_PredpisaneTerapije() const {
  // definition file
  TFOut DefSOut("PR300NSG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA210"); // Patient - ATPCode
  SaveFldDef(fDef, "SIFRA101"); // Patient -> Code
  SaveFldDef(fDef, "SIFRA060"); // PlanRequest -> ActivityCode
  SaveFldDef(fDef, "ZAPST300"); // -
  SaveFldDef(fDef, "STPON300"); // PlanRequest -> Quantity
  SaveFldDef(fDef, "S_P_D300"); // PlanRequest -> DayQuantity
  SaveFldDef(fDef, "SSIF_T11"); // PlanRequest -> Freqency
  SaveFldDef(fDef, "SIFR1060"); // PlanRequest -> AlternatingActivityCode
  SaveFldDef(fDef, "SIFR2060"); // PlanRequest -> BeforeActivtyCode
  SaveFldDef(fDef, "URAOD300"); // PlanRequest -> DesiredStartTime
  SaveFldDef(fDef, "URADO300"); // PlanRequest -> DesiredFinishTime
  SaveFldDef(fDef, "DATOD300"); // PlanRequest -> TreatmentDate
  SaveFldDef(fDef, "STAT_300"); // -
  SaveFldDef(fDef, "PLAN_300"); // -
  SaveFldDef(fDef, "DATUMIPL"); // -
  SaveFldDef(fDef, "OPER_300"); // -
  SaveFldDef(fDef, "SIFRA051"); // PlanRequest -> TherapistCode
  SaveFldDef(fDef, "SPOL_300"); // Patient -> Attributes -> Gender
  SaveFldDef(fDef, "DRUZINA"); // Patient -> Attributes -> Family
  SaveFldDef(fDef, "ATTRNAME"); // PlanRequest -> Attributes -> Name
  SaveFldDef(fDef, "EXCLACT"); // list of PlanRequest -> ExcludedActivities -> ExcludedActivity -> ActivityCode
  SaveFldDef(fDef, "EXCLTIME"); // list of PlanRequest -> ExcludedActivities -> ExcludedActivity -> Time

  // data file
  TFOut DatSOut("PR300NS0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int PlanRequests=TThermalSch::GetMakeSchedule_PlanRequests();
  for (int PlanRequestN=0; PlanRequestN<PlanRequests; PlanRequestN++){
    PXmlTok PlanRequest=TThermalSch::GetMakeSchedule_PlanRequest(PlanRequestN);
    SaveFldVal(fDat, GetMakeSchedule_PatientATPCode()); // SIFRA210
    SaveFldVal(fDat, GetMakeSchedule_PatientCode()); // SIFRA101
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_ActivityCode(PlanRequest)); // SIFRA060
    SaveFldVal(fDat, ""); // ZAPST300
    //SaveFldVal(fDat, GetMakeSchedule_PlanRequest_Quantity(PlanRequest)); // STPON300
    TStr AlternatingActivityCode=GetMakeSchedule_PlanRequest_AlternatingActivityCode(PlanRequest);
    TStr QuantityStr=GetMakeSchedule_PlanRequest_Quantity(PlanRequest);
    if (AlternatingActivityCode.Empty()){
      SaveFldVal(fDat, GetMakeSchedule_PlanRequest_Quantity(PlanRequest)); // STPON300
    } else {
      int Quantity=2*QuantityStr.GetInt(0);
      SaveFldVal(fDat, TInt::GetStr(Quantity)); // STPON300
    }
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_DayQuantity(PlanRequest)); // S_P_D300
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_Frequency(PlanRequest)); // SSIF_T11
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_AlternatingActivityCode(PlanRequest)); // SIFR1060
    SaveFldVal(fDat, ""); // SIFR2060
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_DesiredStartTime(PlanRequest)); // URAOD300
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_DesiredFinishTime(PlanRequest)); // URADO300
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_TreatmentDate(PlanRequest)); // DATOD300
    SaveFldVal(fDat, ""); // STAT_300
    SaveFldVal(fDat, ""); // PLAN_300
    SaveFldVal(fDat, ""); // DATUMIPL
    SaveFldVal(fDat, ""); // OPER_300
    SaveFldVal(fDat, ""); // SIFRA051
    SaveFldVal(fDat, GetMakeSchedule_PatientGender()); // SPOL_300
    SaveFldVal(fDat, GetMakeSchedule_PatientFamily()); // DRUZINA
    SaveFldVal(fDat, GetMakeSchedule_PlanRequest_AttrNm(PlanRequest)); // ATTRNAME
    SaveFldValV(fDat, GetMakeSchedule_PlanRequest_ExcludedActivities_ActivityCode(PlanRequest)); // EXCLACT
    SaveFldValV(fDat, GetMakeSchedule_PlanRequest_ExcludedActivities_Time(PlanRequest), true); // EXCLTIME
  }
}

void TThermalSch::SaveOldCsv_PlaniraneAktivnosti() const {
  // definition file
  TFOut DefSOut("PR360AKG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA360"); // ScheduledActivity -> Code
  SaveFldDef(fDef, "SIFRA210"); // ScheduledActivity -> Patient -> ATPCode
  SaveFldDef(fDef, "SIFRA101"); // ScheduledActivity -> Patient -> Code
  SaveFldDef(fDef, "SIFRA051"); // ScheduledActivity -> TherapistCode
  SaveFldDef(fDef, "SIFRA052"); // ScheduledActivity -> TherapyPlaceCode
  SaveFldDef(fDef, "SIFRA060"); // ScheduledActivity -> ActivityCode
  SaveFldDef(fDef, "DATUM360"); // ScheduledActivity -> StartDateTime
  SaveFldDef(fDef, "STAT_360"); // -
  SaveFldDef(fDef, "Z_ST_360"); // ScheduledActivity -> RepetitionNumber
  SaveFldDef(fDef, "C_OD_360"); // ScheduledActivity -> StartingPoint
  SaveFldDef(fDef, "TRAJ_060"); // ScheduledActivity -> PatientDuration
  SaveFldDef(fDef, "URA__360"); // ScheduledActivity -> StartDateTime
  SaveFldDef(fDef, "SSIF_T07"); // -

  // data file
  TFOut DatSOut("PR360AK0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int ScheduledActivities=TThermalSch::GetScheduledActivities();
  for (int ScheduledActivityN=0; ScheduledActivityN<ScheduledActivities; ScheduledActivityN++){
    PXmlTok ScheduledActivity=TThermalSch::GetScheduledActivity(ScheduledActivityN);
    SaveFldVal(fDat, GetScheduledActivity_Code(ScheduledActivity)); // SIFRA360
    SaveFldVal(fDat, GetScheduledActivity_PatientATPCode(ScheduledActivity)); // SIFRA210
    SaveFldVal(fDat, GetScheduledActivity_Patient(ScheduledActivity)); // SIFRA101
    SaveFldVal(fDat, GetScheduledActivity_TherapistCode(ScheduledActivity)); // SIFRA051
    SaveFldVal(fDat, GetScheduledActivity_TherapyPlaceCode(ScheduledActivity)); // SIFRA052
    SaveFldVal(fDat, GetScheduledActivity_ActivityCode(ScheduledActivity)); // SIFRA060
    SaveFldVal(fDat, GetScheduledActivity_StartDateTime(ScheduledActivity)); // DATUM360
    SaveFldVal(fDat, ""); // STAT_360
    SaveFldVal(fDat, GetScheduledActivity_RepetitionNumber(ScheduledActivity)); // Z_ST_360
    SaveFldVal(fDat, GetScheduledActivity_StartingPoint(ScheduledActivity)); // C_OD_360
    SaveFldVal(fDat, GetScheduledActivity_PatientDuration(ScheduledActivity)); // TRAJ_060
    SaveFldVal(fDat, GetScheduledActivity_StartDateTime(ScheduledActivity)); // URA__360
    SaveFldVal(fDat, "", true); // SSIF_T07
  }
}

void TThermalSch::SaveOldCsv_UrnikXX() const {
  // definition file
  TFOut DefSOut("URNIKXXG.TXT"); FILE* fDef=DefSOut.GetFileId();
  SaveFldDef(fDef, "SIFRA101");
  SaveFldDef(fDef, "SIFRA060");
  SaveFldDef(fDef, "SIFRA052");
  SaveFldDef(fDef, "DATUM060");
  SaveFldDef(fDef, "START060");
  SaveFldDef(fDef, "NARP_060");
  SaveFldDef(fDef, "NARA_060");
  SaveFldDef(fDef, "UJEMANJE");

  // data file
  TFOut DatSOut("URNIKXX0.TXT"); FILE* fDat=DatSOut.GetFileId();
  int ScheduledActivities=TThermalSch::GetScheduledActivities();
  for (int ScheduledActivityN=0; ScheduledActivityN<ScheduledActivities; ScheduledActivityN++){
    PXmlTok ScheduledActivity=TThermalSch::GetScheduledActivity(ScheduledActivityN);
    SaveFldVal(fDat, GetScheduledActivity_PatientCode(ScheduledActivity)); // SIFRA101
    SaveFldVal(fDat, GetScheduledActivity_ActivityCode(ScheduledActivity)); // SIFRA060
    SaveFldVal(fDat, GetScheduledActivity_TherapyPlaceCode(ScheduledActivity)); // SIFRA052
    SaveFldVal(fDat, GetScheduledActivity_Date(ScheduledActivity)); // DATUM060
    SaveFldVal(fDat, GetScheduledActivity_StartTime(ScheduledActivity)); // START060
    SaveFldVal(fDat, GetScheduledActivity_PatientDuration(ScheduledActivity)); // NARP_060
    SaveFldVal(fDat, GetScheduledActivity_TPDuration(ScheduledActivity)); // NARA_060
    SaveFldVal(fDat, GetScheduledActivity_Accordance(ScheduledActivity), true); // UJEMANJE
  }
}

// DLL functions
PThermalSch TThermalSch::DllThermalSch=TThermalSch::New();

char* TThermalSch::DllExeCmFromXmlFile(char* FNm, char* FPath){
  TStr MsgXmlStr=DllThermalSch->ExeCmFromXmlFile(FNm, FPath);
  return MsgXmlStr.GetCStr();
}

char* TThermalSch::DllExeCmFromXmlStr(char* XmlStr){
  TStr MsgXmlStr=DllThermalSch->ExeCmFromXmlStr(XmlStr);
  return MsgXmlStr.GetCStr();
}

