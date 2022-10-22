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

#include <clang/Lex/Preprocessor.h>

// Don't include Directive.h here because InstrContext.h includes it.
#include "InstrContext.h"

using namespace clang;

namespace instr {

Directive::Directive(InstrContext& instrContext,
                     const FullSourceLoc& loc,
                     Directive::Kind kind)
    : instrContext(instrContext), loc(loc), kind(kind) {
  this->instrContext.add(this);
}

Directive::Kind Directive::getKind() const {
  return this->kind;
}

const FullSourceLoc& Directive::getLoc() const {
  return this->loc;
}

const FileEntry* Directive::getFileEntry() const {
  return this->loc.getFileEntry();
}

unsigned Directive::getLineNumber() const {
  return this->loc.getLineNumber();
}

InstrContext& Directive::getInstrContext() {
  return this->instrContext;
}

// DrFunction

DrFunction::DrFunction(InstrContext& instrContext,
                       const FullSourceLoc& loc,
                       const std::vector<std::string>& argNames,
                       Style style)
    : Directive(instrContext, loc, Directive::Function), style(style),
      printArgs(PrintArgs::Some), argNames(argNames) {
  ;
}

DrFunction::DrFunction(InstrContext& instrContext,
                       const FullSourceLoc& loc,
                       PrintArgs printArgs,
                       Style style)
    : Directive(instrContext, loc, Directive::Function), style(style),
      printArgs(printArgs) {
  ;
}

StringRef DrFunction::spell() const {
  return Parser::tokFunction;
}

Style DrFunction::getStyle() const {
  return this->style;
}

bool DrFunction::shouldPrint(const std::string& arg) const {
  switch (this->printArgs) {
  case PrintArgs::All:
    return true;
  case PrintArgs::None:
    return false;
  default:
    for (const std::string& a : this->argNames)
      if (a == arg)
        return true;
    return false;
  }
}

bool DrFunction::shouldPrint(unsigned argNo) const {
  switch (this->printArgs) {
  case PrintArgs::All:
    return true;
  case PrintArgs::None:
    return false;
  default:
    return this->args.at(argNo);
  }
}

void DrFunction::setNumArgs(unsigned args) {
  this->args.resize(args, true);
}

void DrFunction::setArg(unsigned argNo) {
  this->args[argNo] = true;
}

void DrFunction::resetArg(unsigned argNo) {
  this->args[argNo] = false;
}

bool DrFunction::classof(const Directive* dr) {
  return dr->getKind() == Directive::Function;
}

DrFunction* DrFunction::parse(Parser& parser, const SourceLocation& loc) {
  // ClFuncArgs* clArgs = nullptr;
  // Style style = Style::Qual;
  // InstrContext& instrConetxt = parser.getInstrContext();
  // for (Clause* clause : parser.parseClauses())
  //   if (auto* clStyle = dyn_cast<ClFuncStyle>(clause))
  //     style = clStyle->getStyle();
  //   else if (auto* clFuncArgs = dyn_cast<ClFuncArgs>(clause))
  //     clArgs = clFuncArgs;
  //   else
  //     parser.error(Parser::InvalidClauseForDirective,
  //                  clause->spell(),
  //                  Parser::tokFunction);

  InstrContext& instrContext = parser.getInstrContext();
  SourceManager& srcMgr = parser.getSourceManager();
  return new DrFunction(instrContext, FullSourceLoc(loc, srcMgr));
}

// DrLine

DrLine::DrLine(InstrContext& instrContext,
               const FullSourceLoc& loc,
               const std::string& name)
    : Directive(instrContext, loc, Directive::Line), name(name) {
  ;
}

StringRef DrLine::spell() const {
  return Parser::tokLine;
}

const std::string& DrLine::getName() const {
  return this->name;
}

bool DrLine::classof(const Directive* dr) {
  return dr->getKind() == Directive::Line;
}

DrLine* DrLine::parse(Parser& parser, const SourceLocation& loc) {
  std::string name = "";
  for (Clause* clause : parser.parseClauses()) {
    if (auto* clName = dyn_cast<ClName>(clause)) {
      name = clName->getName();
    } else {
      parser.error(Parser::InvalidClauseForDirective, loc, name);
      return nullptr;
    }
  }

  InstrContext& instrContext = parser.getInstrContext();
  SourceManager& srcMgr = parser.getSourceManager();
  return new DrLine(instrContext, FullSourceLoc(loc, srcMgr), name);
}

// DrLoop

DrLoop::DrLoop(InstrContext& instrContext,
               const FullSourceLoc& loc,
               const std::string& name)
    : Directive(instrContext, loc, Directive::Loop), name(name) {
  ;
}

StringRef DrLoop::spell() const {
  return Parser::tokLoop;
}

const std::string& DrLoop::getName() const {
  return this->name;
}

bool DrLoop::classof(const Directive* dr) {
  return dr->getKind() == Directive::Loop;
}

DrLoop* DrLoop::parse(Parser& parser, const SourceLocation& loc) {
  std::string name = "";
  for (Clause* clause : parser.parseClauses()) {
    if (auto* clName = dyn_cast<ClName>(clause))
      name = clName->getName();
    else
      parser.error(Parser::InvalidClauseForDirective, loc, name);
  }

  InstrContext& instrContext = parser.getInstrContext();
  SourceManager& srcMgr = parser.getSourceManager();
  return new DrLoop(instrContext, FullSourceLoc(loc, srcMgr), name);
}

// DrRegion

DrRegion::DrRegion(InstrContext& instrContext,
                   const FullSourceLoc& loc,
                   const std::string& name)
    : Directive(instrContext, loc, Directive::Region), name(name) {
  ;
}

StringRef DrRegion::spell() const {
  return Parser::tokRegion;
}

const std::string& DrRegion::getName() const {
  return this->name;
}

bool DrRegion::classof(const Directive* dr) {
  return dr->getKind() == Directive::Region;
}

DrRegion* DrRegion::parse(Parser& parser, const SourceLocation& loc) {
  std::string name = "";
  for (Clause* clause : parser.parseClauses()) {
    if (auto* clName = dyn_cast<ClName>(clause))
      name = clName->getName();
    else
      parser.error(Parser::InvalidClauseForDirective, loc, name);
  }

  InstrContext& instrContext = parser.getInstrContext();
  SourceManager& srcMgr = parser.getSourceManager();
  return new DrRegion(instrContext, FullSourceLoc(loc, srcMgr), name);
}

} // namespace instr
