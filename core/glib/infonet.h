#ifndef infonet_h
#define infonet_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Thermal-Schedule
ClassTP(TThermalSch, PThermalSch)//{
private:
  PXmlDoc TherapyPlacesXmlDoc;
  PXmlDoc ActivitiesXmlDoc;
  PXmlDoc AvailableTherapyPlacesXmlDoc;
  PXmlDoc AvailableActivitiesXmlDoc;
  PXmlDoc ScheduledActivitiesXmlDoc;
  PXmlDoc HolidaysXmlDoc;
  PXmlDoc MakeScheduleXmlDoc;
  PXmlDoc CheckScheduleXmlDoc;
public:
  TThermalSch(){}
  static PThermalSch New(){return PThermalSch(new TThermalSch());}
  TThermalSch(TSIn&){Fail;}
  static PThermalSch Load(TSIn& SIn){return new TThermalSch(SIn);}
  void Save(TSOut&){Fail;}

  TThermalSch& operator=(const TThermalSch&){Fail; return *this;}

  // field-retrieval
  TStr GetFldVal(const PXmlTok& RecXmlTok, const TStr& FldNm) const;
  TStr GetFldVal(const PXmlDoc& RecXmlDoc, const TStr& FldNm) const;
  TStr GetFldDateVal(const PXmlTok& RecXmlTok, const TStr& FldNm) const;
  TStr GetFldDateVal(const PXmlDoc& RecXmlDoc, const TStr& FldNm) const;
  TStrV GetFldValV(const PXmlTok& RecXmlTok, const TStr& FldNm,
   const TStr& XFldNm, const TStr& SubFldNm=TStr()) const;

  // xml-output
  static TStr GetXmlOut_Ok(const TStr& OpNm);
  static TStr GetXmlOut_Error(const TStr& OpNm, const TStr& MsgStr);

  // execute generic commands
  TStr ExeCmFromXml(const PSIn& SIn);
  TStr ExeCmFromXmlFile(const TStr& FNm, const TStr& FPath);
  TStr ExeCmFromXmlStr(const TStr& Str);

  // execute specific commands
  TStr ExeCm_SetCatalogueTherapyPlaces(const PXmlDoc& XmlDoc);
  TStr ExeCm_SetCatalogueActivities(const PXmlDoc& XmlDoc);
  TStr ExeCm_SetCatalogueAvailableTherapyPlaces(const PXmlDoc& XmlDoc);
  TStr ExeCm_SetCatalogueAvailableActivities(const PXmlDoc& XmlDoc);
  TStr ExeCm_SetScheduledActivities(const PXmlDoc& XmlDoc);
  TStr ExeCm_SetCatalogueHolidays(const PXmlDoc& XmlDoc);
  TStr ExeCm_MakeSchedule(const PXmlDoc& XmlDoc);
  TStr ExeCm_CheckSchedule(const PXmlDoc& XmlDoc);

  // therapy places
  bool IsTherapyPlacesOk(TStr& MsgStr) const;
  int GetTherapyPlaces() const;
  PXmlTok GetTherapyPlace(const int& TherapyPlaceN) const;
  // field-retrieval
  TStr GetTherapyPlace_Code(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "Code");}
  TStr GetTherapyPlace_Type(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "Type");}
  TStr GetTherapyPlace_Location(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "Location");}
  TStr GetTherapyPlace_Capacity(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "Capacity");}
  TStr GetTherapyPlace_Distance(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "Distance");}
  TStr GetTherapyPlace_Gender(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "Gender");}
  TStr GetTherapyPlace_AlternateScheduling(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "AlternateScheduling");}
  TStr GetTherapyPlace_FamilyScheduling(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "FamilyScheduling");}
  TStr GetTherapyPlace_TherapistType(const PXmlTok& TherapyPlace) const {
    return GetFldVal(TherapyPlace, "TherapistType");}

  // activities
  bool IsActivitiesOk(TStr& MsgStr) const;
  int GetActivities() const;
  PXmlTok GetActivity(const int& ActivityN) const;
  // field-retrieval
  TStr GetActivity_Code(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "Code");}
  TStr GetActivity_Type(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "Type");}
  TStr GetActivity_Name(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "Name");}
  TStr GetActivity_Priority(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "Priority");}
  TStr GetActivity_Duration(const PXmlTok& Activity) const {
    TStr DurationSecsStr=GetFldVal(Activity, "Duration");
    int DurationSecs=DurationSecsStr.GetInt(0);
    int DurationMins=DurationSecs/60;
    return TInt::GetStr(DurationMins);}
  TStr GetActivity_TPDuration(const PXmlTok& Activity) const {
    TStr DurationSecsStr=GetFldVal(Activity, "TPDuration");
    int DurationSecs=DurationSecsStr.GetInt(0);
    int DurationMins=DurationSecs/60;
    return TInt::GetStr(DurationMins);}
  TStr GetActivity_PatientDuration(const PXmlTok& Activity) const {
    TStr DurationSecsStr=GetFldVal(Activity, "PatientDuration");
    int DurationSecs=DurationSecsStr.GetInt(0);
    int DurationMins=DurationSecs/60;
    return TInt::GetStr(DurationMins);}
  TStr GetActivity_TherapistDurationPreparation(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "TherapistDuration|Preparation");}
  TStr GetActivity_TherapistDurationExecution(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "TherapistDuration|Execution");}
  TStr GetActivity_TherapistDurationAdditionalTime(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "TherapistDuration|AdditionalTime");}
  TStr GetActivity_Weariness(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "Weariness");}
  TStr GetActivity_Dry(const PXmlTok& Activity) const {
    return GetFldVal(Activity, "Dry");}

  // available therapy places
  bool IsAvailableTherapyPlacesOk(TStr& MsgStr) const;
  int GetAvailableTherapyPlaces() const;
  PXmlTok GetAvailableTherapyPlace(const int& AvailableTherapyPlaceN) const;
  TStr GetAvailableTherapyPlace_Code(const PXmlTok& AvailableTherapyPlace) const {
    return GetFldVal(AvailableTherapyPlace, "Code");}
  int GetTherapyPlaceSchedules(const PXmlTok& AvailableTherapyPlace) const;
  PXmlTok GetTherapyPlaceSchedule(
   const PXmlTok& AvailableTherapyPlace, const int& TherapyPlaceScheduleN) const;
  // therapy-place-schedule
  TStrV GetTherapyPlaceSchedule_WeekDays(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldValV(TherapyPlaceSchedule, "Days", "Day");}
  TStr GetTherapyPlaceSchedule_OpenFrom(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldVal(TherapyPlaceSchedule, "OpenTimes|Open|From");}
  TStr GetTherapyPlaceSchedule_OpenTill(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldVal(TherapyPlaceSchedule, "OpenTimes|Open|Till");}
  TStr GetTherapyPlaceSchedule_ValidFrom(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldDateVal(TherapyPlaceSchedule, "ValidFrom");}
  TStr GetTherapyPlaceSchedule_ValidTo(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldDateVal(TherapyPlaceSchedule, "ValidTo");}

  // available activities
  bool IsAvailableActivitiesOk(TStr& MsgStr) const;
  int GetAvailableActivities() const;
  PXmlTok GetAvailableActivity(const int& AvailableActivityN) const;
  TStr GetAvailableActivity_Code(const PXmlTok& AvailableActivity) const {
    return GetFldVal(AvailableActivity, "Code");}
  int GetActivitySchedules(const PXmlTok& AvailableActivity) const;
  PXmlTok GetActivitySchedule(
   const PXmlTok& AvailableActivity, const int& ActivityScheduleN) const;
  // activity-schedule
  TStrV GetActivitySchedule_WeekDays(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldValV(TherapyPlaceSchedule, "Days", "Day");}
  TStr GetActivitySchedule_StartTime(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldVal(TherapyPlaceSchedule, "StartTime");}
  TStr GetActivitySchedule_EndTime(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldVal(TherapyPlaceSchedule, "EndTime");}
  TStr GetActivitySchedule_TherapyPlaceType(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldVal(TherapyPlaceSchedule, "TherapyPlaceType");}
  TStr GetActivitySchedule_Gender(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldVal(TherapyPlaceSchedule, "Attributes|Gender");}
  TStr GetActivitySchedule_ValidFrom(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldDateVal(TherapyPlaceSchedule, "ValidFrom");}
  TStr GetActivitySchedule_ValidTo(const PXmlTok& TherapyPlaceSchedule) const {
    return GetFldDateVal(TherapyPlaceSchedule, "ValidTo");}

  // scheduled-activities
  bool IsScheduledActivitiesOk(TStr& MsgStr) const;
  int GetScheduledActivities() const;
  PXmlTok GetScheduledActivity(const int& ScheduledActivityN) const;
  // field-retrieval
  TStr GetScheduledActivity_Code(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "Code");}
  TStr GetScheduledActivity_PatientATPCode(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "ATPCode");}
  TStr GetScheduledActivity_Patient(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "Code");}
  TStr GetScheduledActivity_TherapistCode(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "TherapistCode");}
  TStr GetScheduledActivity_StartDateTime(const PXmlTok& ScheduledActivity) const {
    return GetFldDateVal(ScheduledActivity, "StartDateTime");}
  TStr GetScheduledActivity_RepetitionNumber(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "RepetitionNumber");}
  TStr GetScheduledActivity_StartingPoint(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "StartingPoint");}

  TStr GetScheduledActivity_PatientCode(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "PatientCode");}
  TStr GetScheduledActivity_ActivityCode(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "ActivityCode");}
  TStr GetScheduledActivity_TherapyPlaceCode(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "TherapyPlaceCode");}
  TStr GetScheduledActivity_Date(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "Date");}
  TStr GetScheduledActivity_StartTime(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "StartTime");}
  TStr GetScheduledActivity_PatientDuration(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "PatientDuration");}
  TStr GetScheduledActivity_TPDuration(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "TPDuration");}
  TStr GetScheduledActivity_Accordance(const PXmlTok& ScheduledActivity) const {
    return GetFldVal(ScheduledActivity, "Accordance");}

  // holidays
  bool IsHolidaysOk(TStr& MsgStr) const;
  int GetHolidays() const;
  PXmlTok GetHoliday(const int& HolidayN) const;
  // field-retrieval
  TStr GetHoliday_Date(const PXmlTok& Holiday) const {
    return GetFldDateVal(Holiday, "");}

  // make schedule
  bool IsMakeScheduleOk(TStr& MsgStr) const;
  TStr GetMakeSchedule_PatientCode() const {
    return GetFldVal(MakeScheduleXmlDoc, "Operation|Parameter|Patient|Code");}
  TStr GetMakeSchedule_PatientGender() const {
    return GetFldVal(MakeScheduleXmlDoc, "Operation|Parameter|Patient|Attributes|Gender");}
  TStr GetMakeSchedule_PatientFamily() const {
    return GetFldVal(MakeScheduleXmlDoc, "Operation|Parameter|Patient|Attributes|Family");}
  TStr GetMakeSchedule_PatientATPCode() const {
    return GetFldVal(MakeScheduleXmlDoc, "Operation|Parameter|Patient|ATPCode");}
  int GetMakeSchedule_PlanRequests() const;
  PXmlTok GetMakeSchedule_PlanRequest(const int& PlanRequestN) const;
  TStr GetMakeSchedule_PlanRequest_ActivityCode(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "ActivityCode");}
  TStr GetMakeSchedule_PlanRequest_Quantity(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "Quantity");}
  TStr GetMakeSchedule_PlanRequest_DayQuantity(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "DayQuantity");}
  TStr GetMakeSchedule_PlanRequest_Frequency(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "Frequency");}
  TStr GetMakeSchedule_PlanRequest_AlternatingActivityCode(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "AlternatingActivityCode");}
  TStrV GetMakeSchedule_PlanRequest_ExcludedActivities_ActivityCode(const PXmlTok& PlanRequest) const {
    return GetFldValV(PlanRequest, "ExcludedActivities", "ExcludedActivity", "ActivityCode");}
  TStrV GetMakeSchedule_PlanRequest_ExcludedActivities_Time(const PXmlTok& PlanRequest) const {
    TStrV TimeStrV=GetFldValV(PlanRequest, "ExcludedActivities", "ExcludedActivity", "Time");
    for (int TimeStrN=0; TimeStrN<TimeStrV.Len(); TimeStrN++){
      TimeStrV[TimeStrN]=TInt::GetStr(TimeStrV[TimeStrN].GetInt(0)/60);}
    return TimeStrV;
  }
  TStr GetMakeSchedule_PlanRequest_FamilyActivity(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "FamilyActivity");}
  TStr GetMakeSchedule_PlanRequest_TreatmentDate(const PXmlTok& PlanRequest) const {
    return GetFldDateVal(PlanRequest, "TreatmentDate");}
  TStr GetMakeSchedule_PlanRequest_DesiredStartTime(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "DesiredStartTime");}
  TStr GetMakeSchedule_PlanRequest_DesiredFinishTime(const PXmlTok& PlanRequest) const {
    return GetFldVal(PlanRequest, "DesiredFinishTime");}
  TStr GetMakeSchedule_PlanRequest_AttrNm(const PXmlTok& PlanRequest) const {
    TStr FldVal=GetFldVal(PlanRequest, "Attributes|Name");
    if (FldVal=="Family"){return "DRUZINA";}
    if (FldVal=="Gender"){return "SPOL_300";}
    return FldVal.GetUc();}

  // check schedule
  bool IsCheckScheduleOk(TStr& MsgStr) const;
  TStr GetCheckSchedule_PatientCode() const {
    return GetFldVal(CheckScheduleXmlDoc, "Operation|Parameter|Patient|Code");}
  TStr GetCheckSchedule_PatientGender() const {
    return GetFldVal(CheckScheduleXmlDoc, "Operation|Parameter|Patient|Gender");}
  TStr GetCheckSchedule_PatientATPCode() const {
    return GetFldVal(CheckScheduleXmlDoc, "Operation|Parameter|Patient|ATPCode");}
  TStr GetCheckSchedule_IntRequestedActivityActivityCode() const {
    return GetFldVal(CheckScheduleXmlDoc, "Operation|Parameter|IntRequestedActivity|ActivityCode");}
  TStr GetCheckSchedule_IntRequestedActivityDayQuantity() const {
    return GetFldVal(CheckScheduleXmlDoc, "Operation|Parameter|IntRequestedActivity|DayQuantity");}
  TStr GetCheckSchedule_IntRequestedActivityTreatmentDate() const {
    return GetFldDateVal(CheckScheduleXmlDoc, "Operation|Parameter|IntRequestedActivity|TreatmentDate");}
  TStr GetCheckSchedule_IntRequestedActivityDesiredStartTime() const {
    return GetFldVal(CheckScheduleXmlDoc, "Operation|Parameter|IntRequestedActivity|DesiredStartTime");}
  TStr GetCheckSchedule_IntRequestedActivityDesiredFinishTime() const {
    return GetFldVal(CheckScheduleXmlDoc, "Operation|Parameter|IntRequestedActivity|DesiredFinishTime");}

  // field saving
  void SaveFldDef(FILE* FId, const TStr& FldNm) const {
    fprintf(FId, "\"%s\"\r\n", FldNm.CStr());}
  void SaveFldVal(FILE* FId, const TStr& FldVal, const bool& EolnP=false) const {
    fprintf(FId, "\"%s\"", FldVal.CStr());
    if (EolnP){fprintf(FId, "\r\n");} else {fprintf(FId, ",");}}
  void SaveFldValV(FILE* FId, const TStrV& FldValV, const bool& EolnP=false) const {
    fprintf(FId, "\"");
    for (int FldValN=0; FldValN<FldValV.Len(); FldValN++){
      if (FldValN!=0){fprintf(FId, ",");}
      fprintf(FId, "%s", FldValV[FldValN].CStr());
    }
    fprintf(FId, "\"");
    if (EolnP){fprintf(FId, "\r\n");} else {fprintf(FId, ",");}}

  // save to CSV & XML
  static void SaveCsvToXml(
   const TStr& DefFNm, const TStr& DatFNm, const TStr& OutXmlFNm);
  static void SaveCsvToXmlParamOut(
   const TStr& DefFNm, const TStr& DatFNm, const TStr& OutXmlFNm);
  void SaveOldCsv() const;
  void SaveOldCsv_KatalogZdrDelavcev() const;
  void SaveOldCsv_KatalogAparatov() const;
  void SaveOldCsv_KatalogAktivnosti() const;
  void SaveOldCsv_UrnikZaTerapevte() const;
  void SaveOldCsv_UrnikZaAparate() const;
  void SaveOldCsv_UrnikAktivnosti() const;
  void SaveOldCsv_MozniTerminiVDnevu() const;
  void SaveOldCsv_Prazniki() const;
  void SaveOldCsv_PredpisaneTerapije() const;
  void SaveOldCsv_PlaniraneAktivnosti() const;
  void SaveOldCsv_UrnikXX() const;

  // DLL functions
  static PThermalSch DllThermalSch;
  static void DllNewThermalSch(){
    DllThermalSch=TThermalSch::New();}
  static char* DllExeCmFromXmlFile(char* FNm, char* FPath);
  static char* DllExeCmFromXmlStr(char* XmlStr);
};

#endif
