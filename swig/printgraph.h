
template<class PGraph>
void PrintGraphStatTable(const PGraph& G, TStr OutFNm, TStr Desc="") {
  TFltPrV DegCCfV;
  int64 ClosedTriads, OpenTriads;
  int FullDiam;
  double EffDiam;
  TSnap::PrintInfo(G, OutFNm);
  TExeTm ExeTm; printf("C");
  const double CCF = TSnap::GetClustCf(G, DegCCfV, ClosedTriads, OpenTriads);
  printf("[%s]D", ExeTm.GetStr());
  TSnap::GetBfsEffDiam(G, 1000, false, EffDiam, FullDiam);
  printf("[%s]CC", ExeTm.GetStr());
  PGraph WCC = TSnap::GetMxWcc(G);
  PGraph SCC = TSnap::GetMxScc(G);
  printf("[%s]\n", ExeTm.GetStr());
  FILE* F = stdout;
  if (! OutFNm.Empty()) {
    F = fopen(TStr::Fmt("%s.html", OutFNm.CStr()).CStr(), "wt"); }
  fprintf(F, "\n");
  fprintf(F, "<table id=\"datatab\" summary=\"Dataset statistics\">\n");
  fprintf(F, "  <tr> <th colspan=\"2\">Dataset statistics</th> </tr>\n");
  fprintf(F, "  <tr><td>Nodes</td> <td>%d</td></tr>\n", G->GetNodes());
  fprintf(F, "  <tr><td>Edges</td> <td>%d</td></tr>\n", G->GetEdges());
  fprintf(F, "  <tr><td>Nodes in largest WCC</td> <td>%d (%.3f)</td></tr>\n", WCC->GetNodes(), WCC->GetNodes()/double(G->GetNodes()));
  fprintf(F, "  <tr><td>Edges in largest WCC</td> <td>%d (%.3f)</td></tr>\n", WCC->GetEdges(), WCC->GetEdges()/double(G->GetEdges()));
  fprintf(F, "  <tr><td>Nodes in largest SCC</td> <td>%d (%.3f)</td></tr>\n", SCC->GetNodes(), SCC->GetNodes()/double(G->GetNodes()));
  fprintf(F, "  <tr><td>Edges in largest SCC</td> <td>%d (%.3f)</td></tr>\n", SCC->GetEdges(), SCC->GetEdges()/double(G->GetEdges()));
  fprintf(F, "  <tr><td>Average clustering coefficient</td> <td>%.4f</td></tr>\n", CCF);
  fprintf(F, "  <tr><td>Number of triangles</td> <td>%s</td></tr>\n", TUInt64(ClosedTriads).GetStr().CStr());
  fprintf(F, "  <tr><td>Fraction of closed triangles</td> <td>%.4g</td></tr>\n", ClosedTriads/double(ClosedTriads+OpenTriads));
  fprintf(F, "  <tr><td>Diameter (longest shortest path)</td> <td>%d</td></tr>\n", FullDiam);
  fprintf(F, "  <tr><td>90-percentile effective diameter</td> <td>%.2g</td></tr>\n", EffDiam);
  fprintf(F, "</table>\n");
  fprintf(F, "<br>\n");
  if (! OutFNm.Empty()) {
    fprintf(F, "\n<table id=\"datatab\" summary=\"Table of datasets\">\n");
    fprintf(F, "<tr>\n");
    fprintf(F, "  <th>File</th>\n");
    fprintf(F, "  <th>Description</th>\n");
    fprintf(F, "</tr>\n");
    fprintf(F, "<tr>\n");
    fprintf(F, "  <td><a href=\"%s.txt.gz\">%s.txt.gz</a></td>\n", OutFNm.CStr(), OutFNm.CStr());
    fprintf(F, "  <td>%s</td>\n", Desc.CStr());
    fprintf(F, "</tr>\n");
    fprintf(F, "</table>\n");
    fclose(F);
    TSnap::SaveEdgeList(G, OutFNm+".txt", Desc);
  }
}

