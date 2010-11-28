/////////////////////////////////////////////////
// String-Cache
TStr TStrCache::GetStr(const TStr& Str){
  int StrId=-1; Rqs++;
  if (!StrH.IsKey(Str, StrId)){
    StrId=StrH.AddKey(Str);}
  else {Hits++;}
  return StrH.GetKey(StrId);
}

