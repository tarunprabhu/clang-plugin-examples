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

#include "InstrContext.h"

#include <clang/Lex/Preprocessor.h>

using namespace clang;

namespace instr {

Clause::Clause(InstrContext& instrContext,
               const FullSourceLoc& loc,
               Clause::Kind kind)
    : instrContext(instrContext), loc(loc), kind(kind) {
  this->instrContext.add(this);
}

Clause::Kind Clause::getKind() const {
  return this->kind;
}

// ClName

ClName::ClName(InstrContext& instrContext,
               const FullSourceLoc& loc,
               const std::string& name)
    : Clause(instrContext, loc, Clause::Name), name(name) {
  ;
}

StringRef ClName::spell() const {
  return Parser::tokName;
}

const std::string& ClName::getName() const {
  return this->name;
}

bool ClName::classof(const Clause* cl) {
  return cl->getKind() == Clause::Name;
}

ClName* ClName::parse(Parser& parser, const clang::SourceLocation& loc) {
  std::string name = "";
  if (not parser.parseToken(tok::l_paren))
    return nullptr;
  if (not parser.parseStringLiteralInto(name))
    return nullptr;
  if (not parser.parseToken(tok::r_paren))
    return nullptr;

  InstrContext& instrContext = parser.getInstrContext();
  SourceManager& srcMgr = parser.getSourceManager();
  return new ClName(instrContext, FullSourceLoc(loc, srcMgr), name);
}

// ClFuncStyle

static const std::map<std::string, Style> styleEnums = {
    {Parser::tokFuncStyleDecl.str(), Style::Decl},
    {Parser::tokFuncStyleQual.str(), Style::Qual},
    {Parser::tokFuncStyleFull.str(), Style::Full},
};

ClFuncStyle::ClFuncStyle(InstrContext& instrContext,
                         const FullSourceLoc& loc,
                         Style style)
    : Clause(instrContext, loc, Clause::FuncStyle), style(style) {
  ;
}

StringRef ClFuncStyle::spell() const {
  return Parser::tokFuncStyle;
}

Style ClFuncStyle::getStyle() const {
  return this->style;
}

bool ClFuncStyle::classof(const Clause* cl) {
  return cl->getKind() == Clause::FuncStyle;
}

ClFuncStyle* ClFuncStyle::parse(Parser& parser, const SourceLocation& loc) {
  // Sanity check.
  if (parser.spell(parser.consume()) != Parser::tokFuncStyle)
    llvm_unreachable("Wrong parser called for clause");

  if (not parser.parseToken(tok::l_paren))
    return nullptr;

  Token tok = parser.consume();
  std::string style = parser.spell(tok);
  if (styleEnums.find(style) == styleEnums.end()) {
    parser.error(Parser::UnknownEnumValue, tok.getLocation(), style);
    return nullptr;
  }

  if (not parser.parseToken(tok::r_paren))
    return nullptr;

  InstrContext& instrContext = parser.getInstrContext();
  SourceManager& srcMgr = parser.getSourceManager();
  return new ClFuncStyle(
      instrContext, FullSourceLoc(loc, srcMgr), styleEnums.at(style));
}

// ClFuncArgs

ClFuncArgs::ClFuncArgs(InstrContext& instrContext,
                       const FullSourceLoc& loc,
                       const std::vector<std::string>& args)
    : Clause(instrContext, loc, Clause::FuncArgs), args(args) {
  ;
}

StringRef ClFuncArgs::spell() const {
  return Parser::tokFuncArgs;
}

ClFuncArgs::ConstIterator ClFuncArgs::begin() const {
  return this->args.begin();
}

ClFuncArgs::ConstIterator ClFuncArgs::end() const {
  return this->args.end();
}

bool ClFuncArgs::empty() const {
  return this->args.empty();
}

bool ClFuncArgs::hasArg(const std::string& arg) const {
  for (const std::string& a : this->args)
    if (a == arg)
      return true;
  return false;
}

bool ClFuncArgs::classof(const Clause* cl) {
  return cl->getKind() == Clause::FuncArgs;
}

ClFuncArgs* ClFuncArgs::parse(Parser& parser, const SourceLocation& loc) {
  std::vector<std::string> argNames;

  if (not parser.parseToken(tok::l_paren))
    return nullptr;
  if (not parser.parseIdentifierListInto(argNames))
    return nullptr;
  if (not parser.parseToken(tok::r_paren))
    return nullptr;

  InstrContext& instrContext = parser.getInstrContext();
  SourceManager& srcMgr = parser.getSourceManager();
  return new ClFuncArgs(instrContext, FullSourceLoc(loc, srcMgr), argNames);
}

} // namespace instr
