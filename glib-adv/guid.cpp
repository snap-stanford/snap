//////////////////////////////////////////////
// GUID
#ifdef GLib_WIN

TStr TGuid::GenGuid() {
  GUID Guid;
  EAssert(CoCreateGuid(&Guid) == S_OK);
  TStr GuidStr = TStr::Fmt("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
    Guid.Data1, Guid.Data2, Guid.Data3, Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], 
    Guid.Data4[3], Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7]);
  //printf("Guid = '%s'\n", GuidStr.CStr());
  return GuidStr;
}

#elif defined(GLib_UNIX)

extern "C" {
  #include <uuid/uuid.h>
}

TStr TGuid::GenGuid() {
  uuid_t Uuid;
  uuid_generate_random(Uuid);
  char s[37];
  uuid_unparse(Uuid, s);
  TStr UuidStr = s;
  return UuidStr;
}

#endif
