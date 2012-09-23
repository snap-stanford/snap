/////////////////////////////////////////////////
// GraphViz
namespace TSnap {
namespace TSnapDetail {
void GVizDoLayout(const TStr& GraphInFNm, TStr OutFNm, const TGVizLayout& Layout) {
  TStr LayoutExe = TSnap::TSnapDetail::GVizGetLayoutStr(Layout), Ext = OutFNm.GetFExt(), GvPath;
  #if defined(GLib_WIN)
    GvPath = "C:\\Prog\\GraphViz\\bin\\";
  #else
    GvPath = "/usr/bin/";
    Ext = ".ps";
    OutFNm = OutFNm.GetFMid() + Ext;
  #endif
  IAssert(Ext==".ps" || Ext==".gif" || Ext==".png");
  const TStr ExeCmd = TStr::Fmt("%s -T%s %s -o %s", LayoutExe.CStr(),
    Ext.CStr()+1, GraphInFNm.CStr(), OutFNm.CStr());

  if (system(ExeCmd.CStr())==0) { return; }
  #if defined(GLib_WIN)
  if (system(TStr::Fmt(".\\%s", ExeCmd.CStr()).CStr())==0) { return; }
  #else
  if (system(TStr::Fmt("./%s", ExeCmd.CStr()).CStr())==0) { return; }
  #endif
  if (system(TStr::Fmt("%s%s", GvPath.CStr(), ExeCmd.CStr()).CStr())==0) { return; }
  fprintf(stderr, "[%s:%d] Cat not find GraphViz (%s). Set the PATH.\n", __FILE__, __LINE__, ExeCmd.CStr());
  //#if defined(GLib_WIN)
  //if (ShowPlot) system(TStr::Fmt("start %s", OutFNm.CStr()).CStr());
  //#endif
}

TStr GVizGetLayoutStr(const TGVizLayout& Layout) {
  switch(Layout) {
    case gvlDot : return "dot";
    case gvlNeato : return "neato";
    case gvlTwopi : return "twopi";
    case gvlCirco: return "circo";
    default: Fail;
  }
  return TStr::GetNullStr();
}

} // namespace TSnapDetail
} // namespace TSnap

