// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <vector>
#include <map>
#include "Tag.hpp"
#include "Transformer.hpp"
#include "Inclusion.hpp"
#include "ProgramNode.hpp"


namespace {

using namespace std;
using namespace SH;

struct TagCopyBase: public TransformerParent {
  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) { 
    Tag::cloneTo(*I);
    return false; 
  }
};
typedef DefaultTransformer<TagCopyBase> TagCopy;

struct 
SH_DLLEXPORT TagCsvDataBase: public StmtCsvData, public TransformerParent {
  vector<string> headers;
  map<string, int> headerIdx;

  bool handleStmt(BasicBlock::StmtList::iterator &I, CtrlGraphNode* node) { 
    const Statement& stmt = *I;
    Tag::TagList tags = stmt.get_all<Tag>();
    for(Tag::TagList::iterator T = tags.begin(); T != tags.end(); ++T) {
      string header = (*T)->toHeader();
      if(find(headers.begin(), headers.end(), header) == headers.end()) {
        headers.push_back(header);
        headerIdx[header] = headers.size() - 1;
      }
    }
    return false; 
  }

  std::string header() {
    ostringstream sout;
    for(size_t i = 0; i < headers.size(); ++i) {
      sout << "," << headers[i];
    }
    SH_DEBUG_PRINT("Tag header:" << sout.str());
    return sout.str();
  }

  std::string operator()(const Statement& stmt) {
    vector<string> result(headers.size(), "");

    Tag::TagList tags = stmt.get_all<Tag>();
    for(Tag::TagList::iterator T = tags.begin(); T != tags.end(); ++T) {
      result[headerIdx[(*T)->toHeader()]] = (*T)->toString();
    }
    ostringstream sout;
    for(size_t i = 0; i < result.size(); ++i) {
      sout << "," << result[i];
    }
    return sout.str();
  }
};
typedef DefaultTransformer<TagCsvDataBase> TagCsvData;



}

namespace SH {
Tag::TagStack Tag::tags;

void Tag::push(const Statement& stmt) {
  TagList stag = stmt.get_all<Tag>();
  for(TagList::iterator T = stag.begin(); T != stag.end(); ++T) {
    *T = dynamic_cast<Tag*>((*T)->clone());
  }
  tags.push(stag);
  if(tags.top().empty()) return;
  /*
  SH_DEBUG_PRINT("push tags from stmt " << stmt);
  for(TagList::iterator T = tags.top().begin(); T != tags.top().end(); ++T) {
    SH_DEBUG_PRINT("  " << (*T)->toString());
  }
  */
}

void Tag::push(const Tag* t) {
  TagList stag;
  stag.push_back(t);
  tags.push(stag);
}

void Tag::add(const Tag* t) {
  SH_DEBUG_ASSERT(!tags.top().empty());
  tags.top().push_back(t);
}

void Tag::dup() {
  tags.push(tags.top());
}

void Tag::pop() {
  /*
  if(!tags.top().empty()) { 
    SH_DEBUG_PRINT("pop tags");
  }
  */
  tags.pop();
}

void Tag::cloneTo(Statement& stmt) {
  if(tags.empty() || tags.top().empty()) return;
  //SH_DEBUG_PRINT("Adding tags to stmt " << stmt);
  for(TagList::iterator T = tags.top().begin(); T != tags.top().end(); ++T) {
    stmt.add_info((*T)->clone());
   // SH_DEBUG_PRINT("  " << (*T)->toString());
  }
}

void Tag::cloneToProgram(ProgramNodePtr p) {
  TagCopy tc;
  tc.transform(p);
}

StmtCsvDataPtr getTagCsvData(ProgramNodePtr p) {
  TagCsvData* tcd= new TagCsvData();
  tcd->transform(p);
  return tcd; 
}

} // namespace SH
