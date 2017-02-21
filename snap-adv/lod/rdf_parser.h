#ifndef SNAP_RDF_PARSER_H
#define SNAP_RDF_PARSER_H

#include "stdafx.h"

/**
 * Author: Klemen Simonic
 *
 * A simple RDF parser. Each line should be in one of the following two formats:
 * -  <url> <url> <url>
 * -  <url> <url> "string"
 * where the first field represents a subject, the second a predicate and the 
 * third an object. The first two fields need to be enclosed with < > characters, 
 * while the third field can be enclosed with < > or " " characters. There can 
 * be any number of whitespaces (except new lines characters) between the fields.
 *
 * Fields enclosed with < > characters represent resources and have a string
 * in the the form of a URL. Fields enclosed with " " characters represent
 * literals or constants.
 */
class TRDFParser {
private:
  TSIn &Input;
  TChA Line;

public:
  /// Constructs the parsers with the specified input stream.
  TRDFParser (TSIn &InputStream);

  /*
   * Returns the next triple from the input stream.
   * Semantic of the return value is as follows:
   * -   1 : success (new triples has been set).
   * -   0 : end of file.
   * -  -1 : parse error (the input is not well-formatted).
   */
  int GetNextTriple (TStr &Subject, TStr &Predicate, TStr &Object);
};

TRDFParser::TRDFParser (TSIn &InputStream)
: Input(InputStream) {}

int TRDFParser::GetNextTriple (TStr &Subject, TStr &Predicate, TStr &Object)
{
  if (!Input.GetNextLn(Line) || Line.Empty() || Input.Eof()) {
    return 0;
  }

  int SubjectStartPos = Line.SearchCh('<');
  if (SubjectStartPos == -1) {
    return -1;
  }
  int SubjectEndPos = Line.SearchCh('>', SubjectStartPos + 1);
  if (SubjectEndPos == -1) {
    return -1;
  }
  Subject = Line.GetSubStr(SubjectStartPos + 1, SubjectEndPos - 1);
  Subject = TUrl::DecodeUrlStr(Subject);

  int PredicateStartPos = Line.SearchCh('<', SubjectEndPos + 1);
  if (PredicateStartPos == -1) {
    return -1;
  }
  int PredicateEndPos = Line.SearchCh('>', PredicateStartPos + 1);
  if (PredicateEndPos == -1) {
    return -1;
  }
  Predicate = Line.GetSubStr(PredicateStartPos + 1, PredicateEndPos - 1);
  Predicate = TUrl::DecodeUrlStr(Predicate);

  int ObjectStartPos = Line.SearchCh('"', PredicateEndPos + 1);
  //Subject is probably a resource
  if (ObjectStartPos == -1) {
    ObjectStartPos = Line.SearchCh('<', PredicateEndPos + 1);
    if (ObjectStartPos == -1) {
      return -1;
    }
    int ObjectEndPos = Line.SearchCh('>', ObjectStartPos + 1);
    if (ObjectEndPos == -1) {
      return -1;
    }
    Object = Line.GetSubStr(ObjectStartPos + 1, ObjectEndPos -1 );
    Object = TUrl::DecodeUrlStr(Object);
  //Subject is probably a literal
  } else {
    int ObjectEndPos = Line.SearchChBack('"');
    if (ObjectEndPos == -1) {
      return -1;
    }
    if (ObjectStartPos >= ObjectEndPos) {
      return -1;
    }
    Object = Line.GetSubStr(ObjectStartPos + 1, ObjectEndPos - 1);
  }

  return 1;
}

#endif
