/*
  Copyright  2022  Tarun Prabhu

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "Visitor.h"

#include <clang/AST/ParentMapContext.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include <cctype>
#include <limits>
#include <sstream>

using namespace clang;

Visitor::Visitor(CompilerInstance& ci, Pragmas& pragmas, Rewriter& rewriter)
    : ci(ci), astContext(ci.getASTContext()), srcMgr(ci.getSourceManager()),
      pragmas(pragmas), rewriter(rewriter) {
  ;
}

bool Visitor::shouldDemarcate(Stmt* stmt) {
  FullSourceLoc loc(stmt->getBeginLoc(), this->srcMgr);
  const FileEntry* file = loc.getFileEntry();
  unsigned lno = loc.getLineNumber();
  unsigned plno = this->pragmas.findNearestAndPop(file, lno);
  return plno != Pragmas::invalid;
}

// Check at which column of the line the loop begins.
static unsigned getIndent(StringRef buf, unsigned off) {
  // This will only be called on loops that have been tagged with a pragma.
  // By definition (barring utterly perverse usage such as including a file
  // that contains the pragma on the same line as the loop), we are always
  // guaranteed to find a newline somewhere before the loop.
  unsigned i = 0;
  for (i = 0; buf[off - i] != '\n'; i++) {
    // If there was something else on the line that was not a whitespace (for
    // instance a comment), there is no indent and the sentinel functions
    // should be inserted on the same line.
    if (not std::isspace(buf[off - i - 1]))
      return 0;
  }
  return i;
}

static std::string indent(unsigned spaces, const std::string& s) {
  std::stringstream ss;
  for (unsigned i = 0; i < spaces; i++)
    ss << " ";
  ss << s;
  return ss.str();
}

void Visitor::demarcate(Stmt* stmt) {
  SourceLocation beg = stmt->getBeginLoc();
  SourceLocation end = stmt->getEndLoc();
  SourceManager& srcMgr = this->ci.getSourceManager();
  StringRef buf = srcMgr.getBufferData(srcMgr.getFileID(end));
  std::string enterLoop = "{ __enterLoop(); ";
  std::string exitLoop = " __exitLoop(); }";

  // Currently there is a bug where even if there is some other statement
  // between the loop and the pragma, the pragma is associated with the loop
  // when it really shouldn't. Assuming that this is fixed, the pragma should
  // be on the line immediately above the loop, or at the very least with only
  // comments separating the pragma from the loop.
  // tab will be 0 if there is anything other than whitespace between the
  // loop and the pragma (this includes comments).
  unsigned tab = getIndent(buf, srcMgr.getFileOffset(beg));
  if (tab) {
    enterLoop = "{ __enterLoop();\n";
    exitLoop = "\n" + indent(tab - 1, "__exitLoop(); }");
  }

  // When using the endLoc of a statement, it points to the column that
  // contains the end of the innermost statement in the AST. This doesn't
  // necessarily correspond to the end of the statement in the source.
  // For instance, in this loop,
  //
  //     for (k = 0; k < n; k++)
  //         c[i][j] += a[i][k] * b[k][j];
  //                                   ^
  // getEndLoc() for the ForStmt will return the location marked with the caret.
  // This is because that is the innermost child of the for loop in the AST.
  // To get the actual location at which to insert the sentinel, we may need
  // to move to the first semicolon after endLoc.
  //
  // If the for loop has braces, the innermost child will be the CompoundStmt
  // and the endLoc will be the location of the closing brace which is what
  // we need.
  //
  unsigned off = srcMgr.getFileOffset(end);
  unsigned i = 0;
  for (i = 0; off + i < buf.size(); i++) {
    if (buf[off + i] == '}')
      break;
    if (buf[off + i] == ';')
      break;
  }

  // We call rewriter.InsertText with the third parameter set to false.
  // That parameter is the InsertAfter flag. Since it is set to false, move
  // the location to the right by one to stop it from inserting the text before
  // the location.
  SourceLocation loc = end.getLocWithOffset(i + 1);

  this->rewriter.InsertText(beg, enterLoop, false, true);
  this->rewriter.InsertText(loc, exitLoop, false, false);
}

void Visitor::maybeDemarcate(Stmt* stmt) {
  if (this->shouldDemarcate(stmt))
    this->demarcate(stmt);
}

// TODO: Should try to see what happens if a pragma is put inside a
// template body which may be instantiated several times. I think the
// idea here is to allow the visitor to traverse over those as well, but
// that hasn't been tested and I am not sure if it works the way I think
// it does.
bool Visitor::shouldVisitTemplateInstantiations() const {
  return true;
}

bool Visitor::VisitForStmt(ForStmt* stmt) {
  this->maybeDemarcate(stmt);

  return true;
}

bool Visitor::VisitDoStmt(DoStmt* stmt) {
  this->maybeDemarcate(stmt);

  return true;
}

bool Visitor::VisitWhileStmt(WhileStmt* stmt) {
  this->maybeDemarcate(stmt);

  return true;
}
