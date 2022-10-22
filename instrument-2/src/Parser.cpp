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

#include "Clause.h"
#include "Directive.h"
#include "Parser.h"

using namespace clang;

namespace instr {

// Collect all the keywords, directive names, clause names and enum names
// used by the instrumentation language in one place.
StringRef Parser::tokSentinel = "instrument";
StringRef Parser::tokFunction = "function";
StringRef Parser::tokLine = "line";
StringRef Parser::tokLoop = "loop";
StringRef Parser::tokRegion = "region";
StringRef Parser::tokName = "name";
StringRef Parser::tokFuncArgs = "args";
StringRef Parser::tokFuncStyle = "style";
StringRef Parser::tokFuncStyleDecl = "decl";
StringRef Parser::tokFuncStyleQual = "qual";
StringRef Parser::tokFuncStyleFull = "full";

Parser::Parser(Preprocessor& pp, InstrContext& instrContext)
    : pp(pp), diags(pp.getDiagnostics()), instrContext(instrContext),
      drParsers({
          {Parser::tokFunction.str(), &DrFunction::parse},
          {Parser::tokLine.str(), &DrLine::parse},
          {Parser::tokLoop.str(), &DrLoop::parse},
          {Parser::tokRegion.str(), &DrRegion::parse},
      }),
      clParsers({
          {Parser::tokName.str(), &ClName::parse},
          {Parser::tokFuncStyle.str(), &ClFuncStyle::parse},
          {Parser::tokFuncArgs.str(), &ClFuncArgs::parse},
      }),
      errMsgs(
          {{Parser::MissingDirectiveKind,
            "Expected one of function, line, loop or region "
            "after sentinel."},
           {Parser::UnknownDirectiveKind, "Unknown directive '%0'"},
           {Parser::UnknownClauseKind, "Unknown clause '%0'"},
           {Parser::InvalidClauseForDirective,
            "Invalid clause '%0' for directive '%1'"},
           {Parser::ExpectedToken, "Expected token '%0'"},
           {Parser::ExpectedIntLiteral, "Expected integer literal, not '%0'"},
           {Parser::ExpectedStringLiteral, "Expected string literal, not '%0'"},
           {Parser::ExpectedIdentifier, "Expected identifier, not '%0'"},
           {Parser::UnknownEnumValue, "Unknown enum value '%0'"},
           {Parser::UnexpectedEod, "Unexpected end of directive"}}) {
  ;
}

InstrContext& Parser::getInstrContext() const {
  return this->instrContext;
}

Preprocessor& Parser::getPreprocessor() const {
  return this->pp;
}

SourceManager& Parser::getSourceManager() const {
  return this->pp.getSourceManager();
}

DiagnosticsEngine& Parser::getDiagnostics() const {
  return this->diags;
}

DiagnosticBuilder Parser::report(Error err, const clang::SourceLocation& loc) {
  DiagnosticIDs& ids = *this->diags.getDiagnosticIDs().get();
  const auto id
      = ids.getCustomDiagID(DiagnosticIDs::Error, this->errMsgs.at(err));
  return this->diags.Report(loc, id);
}

bool Parser::error(Error err, const clang::SourceLocation& loc) {
  report(err, loc);
  return false;
}

bool Parser::isKnownClause(StringRef cl) const {
  return this->clParsers.find(cl.str()) != this->clParsers.end();
}

bool Parser::isKnownDirective(StringRef dr) const {
  return this->drParsers.find(dr.str()) != this->drParsers.end();
}

std::string Parser::spell(unsigned n) const {
  return this->pp.getSpelling(this->peek(n));
}

std::string Parser::spell(const Token& tok) const {
  return this->pp.getSpelling(tok);
}

SourceLocation Parser::loc(unsigned n) const {
  return this->peek(n).getLocation();
}

FullSourceLoc Parser::fullLoc(unsigned n) const {
  return FullSourceLoc(this->peek(n).getLocation(), this->getSourceManager());
}

bool Parser::eod() const {
  return this->is(tok::eod);
}

bool Parser::is(tok::TokenKind kind) const {
  return this->peek().is(kind);
}

Token& Parser::peek(unsigned n) {
  return this->toks.at(n);
}

const Token& Parser::peek(unsigned n) const {
  return this->toks.at(n);
}

Token Parser::consume() {
  Token tok = this->toks.front();
  this->toks.pop_front();
  return tok;
}

bool Parser::parseToken(tok::TokenKind kind) {
  if (this->eod())
    return this->error(Parser::UnexpectedEod, this->loc());

  if (not this->is(kind))
    return this->error(Parser::ExpectedToken, this->loc(), kind);

  this->consume();
  return true;
}

bool Parser::parseStringLiteralInto(std::string& out) {
  if (this->eod())
    return this->error(Parser::UnexpectedEod, this->loc());

  if (not this->is(tok::string_literal))
    return this->error(Parser::ExpectedStringLiteral, this->loc());

  out = this->spell(this->consume());
  return true;
}

bool Parser::parseIntLiteralInto(uint64_t& out) {
  if (this->eod())
    return this->error(Parser::UnexpectedEod, this->loc());

  if (not this->pp.parseSimpleIntegerLiteral(this->peek(), out))
    return this->error(Parser::ExpectedIntLiteral, this->loc());

  this->consume();
  return true;
}

bool Parser::parseIdentifierInto(std::string& out) {
  if (this->eod())
    return this->error(Parser::UnexpectedEod, this->loc());

  if (not this->is(tok::identifier))
    return this->error(Parser::ExpectedIdentifier, this->loc(), this->spell());

  out = this->spell(this->consume());
  return true;
}

bool Parser::parseIdentifierListInto(std::vector<std::string>& out) {
  if (this->eod())
    return this->error(Parser::ExpectedList, this->loc());

  // Empty list. The caller is responsible for consuming the closing ')'.
  if (this->is(tok::r_paren))
    return true;

  do {
    // Create an empty string at the back of the output list and parse the
    // identifier, if any, into it. It doesn't matter if the identifier was
    // not found because that will raise an error and the output is not
    // guaranteed to be anything valid in such a case.
    out.emplace_back();
    if (not this->parseIdentifierInto(out.back()))
      return false;

    if (this->eod())
      return this->error(Parser::UnexpectedEod, this->loc());

    // If there are more elements in the list, the next token must be a comma.
    // If not, the next token must be a list-terminating ')'. The terminating
    // ')' will be consumed by the caller. Any other token is an error.
    if (this->is(tok::comma))
      this->consume();
    else if (not this->is(tok::r_paren))
      return this->error(Parser::ExpectedToken, this->loc(), tok::comma);
  } while (not this->is(tok::r_paren));

  return true;
}

Clause* Parser::parseClause(const std::string& cl, const SourceLocation& loc) {
  return this->clParsers.at(cl)(*this, loc);
}

Clauses Parser::parseClauses() {
  // Each clause is of the form
  //
  //     <clause-name>
  //
  //           OR
  //
  //     <clause-name>(<clause-val>)
  //
  // where clause-val may be an integer or a string literal.
  //
  Clauses clauses;

  // The directive has already been consumed. Since clauses are optional, there
  // may be none.
  while (not this->eod()) {
    std::string cl = this->spell();
    SourceLocation loc = this->loc();

    if (not this->isKnownClause(cl)) {
      this->error(Error::UnknownClauseKind, this->loc(), cl);
      return std::vector<Clause*>();
    } else if (Clause* clause = this->parseClause(cl, loc)) {
      clauses.push_back(clause);
    }
  }

  return clauses;
}

Directive* Parser::parseDirective(const std::string& dr,
                                  const SourceLocation& loc) {
  return this->drParsers.at(dr)(*this, loc);
}

Directive* Parser::parseDirective(Token& tokSentinel) {
  // Lex the remaining tokens in the directive.
  this->lex();

  // Each annotation is of the form
  //
  //     #pragma instrument <kind> [<clause> ...]
  //
  // The parameter to the function tok will be the token corresponding to the
  // sentinel "demarcate". The first token in toks will be the sentinel token.
  // The second will be the directive and subsequent token, if any, will
  // constitute the clauses.
  //
  if (this->eod()) {
    this->error(
        Error::MissingDirectiveKind,
        tokSentinel.getLocation().getLocWithOffset(tokSentinel.getLength()));
    return nullptr;
  }

  std::string dr = this->spell();
  SourceLocation loc = this->loc();
  if (not this->isKnownDirective(dr)) {
    this->error(Error::UnknownDirectiveKind, loc, dr);
    return nullptr;
  } else {
    return this->parseDirective(dr, loc);
  }
}

void Parser::lex() {
  // lex all tokens until the end of the directive. This is usually till the
  // end of the line, unless continuation lines have been used.
  do {
    this->toks.emplace_back();
    this->getPreprocessor().Lex(this->toks.back());
  } while (not this->toks.back().is(tok::eod));
  this->toks.pop_back();
}

void Parser::prepareToParse() {
  this->toks.clear();
}

} // namespace instr
