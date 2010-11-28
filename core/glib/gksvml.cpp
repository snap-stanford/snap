/////////////////////////////////////////////////
// Includes
#include "stdafx.h"
#include "gksvml.h"

/////////////////////////////////////////////////
// Svg-Gks

TVmlGks::TVmlGks(const TStr& OutFName, const TStr& _Title, const TInt& _Width, const TInt& _Height) :
 OutF(NULL), OutFNm(OutFName), Title(_Title), Width(_Width), Height(_Height) {
  if (OutFNm.Empty()) OutFNm = "gksvml.html";
  if (Title.Empty()) Title = "VML GKS";
  if (Width == 0) Width = 1000;
  if (Height == 0) Height = 1000;
  PutHtmlHeader();
}

void TVmlGks::PutHtmlHeader() const {
  if (OutF == NULL) OutF = fopen(OutFNm.CStr(), "wt");
  IAssert(OutF != NULL);
  fprintf(OutF, "<html xmlns:v=\"urn:schemas-microsoft-com:vml\" xmlns:IE>\n<head>\n"
    "<object id=\"VMLRender\" classid=\"CLSID:10072CEC-8CC1-11D1-986E-00A0C955B42E\"></object>\n"
    "<style>\n  v\\:* { behavior: url(#default#VML); }\n</style>\n"
    "<title>%s</title>\n"
    "</head>\n<body>\n"
    "<v:group style=\"width: 99.5%%; height: 99.5%%; left: 0px; top: 0px\" coordsize==\"%d,%d\">\n"
    "<v:rect style=\"left:0; top:0; width:%d; height:%d;\" fillcolor=\"#ffffff\" strokeweight=\"1px\" strokecolor=\"#000000\" />\n",
    Title.CStr(), Width, Height, Width, Height);
  Flush();
}

void TVmlGks::PutHtmlFooter() const {
  IAssert(OutF != NULL);
  fprintf(OutF, "\n</v:group>\n</body>\n</html>\n");
  Flush();
}

