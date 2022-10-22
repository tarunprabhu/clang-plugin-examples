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

#ifndef CLANG_PLUGIN_EXAMPLES_INSTRUMENT_PARSER_H
#define CLANG_PLUGIN_EXAMPLES_INSTRUMENT_PARSER_H

#include <clang/Basic/Diagnostic.h>

#include <llvm/Support/Compiler.h>

#include <deque>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "Instr.h"

namespace clang {
class DiagnosticBuilder;
class Preprocessor;
class SourceLocation;
class Token;
} // namespace clang

namespace instr {

// An instance of this class is associated with the pragma handler and is used
// to parse each instance of the pragma that is processed by the handler.
// The prepareToParse method must be called before a call to parse() otherwise,
class Parser {
public:
  enum Error {
    MissingDirectiveKind,
    UnknownDirectiveKind,
    UnknownClauseKind,
    InvalidClauseForDirective,
    ExpectedIdentifier,
    ExpectedIntLiteral,
    ExpectedList,
    ExpectedStringLiteral,
    ExpectedToken,
    UnknownEnumValue,
    UnexpectedEod,
  };

private:
  using Tokens = std::deque<clang::Token>;
  using FnParseDirective
      = std::function<Directive*(Parser&, const clang::SourceLocation&)>;
  using FnParseClause
      = std::function<Clause*(Parser&, const clang::SourceLocation&)>;

private:
  clang::Preprocessor& pp;
  clang::DiagnosticsEngine& diags;
  InstrContext& instrContext;

  // Map from the spellings of the directive kinds to the function that parses
  // that directive.
  const std::map<std::string, FnParseDirective> drParsers;

  // Map from the spelling of the clause to the function that parses that
  // clause.
  const std::map<std::string, FnParseClause> clParsers;

  // Map from error ids to messages
  const std::map<Error, std::string> errMsgs;

  // The tokens in the current pragma being parsed. When the pragma is first
  // lexed, the first token will be the one corresponding to the scope to be
  // instrumented.
  Tokens toks;

private:
  // Lex the remaining tokens in the directive. This will populate the list of
  // unconsumed tokens. The last directive in the list will always have kind
  // tok::eod.
  void lex();

  // Check if the end of the directive has been reached. This will be if the
  // token of kind eod is at the front of the list of unconsumed tokens.
  bool eod() const;

  // Check if the token at the front of the list is of the given kind.
  bool is(clang::tok::TokenKind kind) const;

  bool isKnownClause(clang::StringRef cl) const;
  bool isKnownDirective(clang::StringRef dr) const;

  // Peek (do not consume) the token at position n from the front of the list
  // of tokens. By default, this returns a reference to the token at the front
  // of the list of tokens. It is an error to call this with a position that is
  // out of bounds for the token list.
  clang::Token& peek(unsigned n = 0);

  // Parse the clause with the given kind.
  Clause* parseClause(const std::string& cl, const clang::SourceLocation& loc);

  // Parse the directive with the given kind.
  Directive* parseDirective(const std::string& dr,
                            const clang::SourceLocation& loc);

public:
  Parser(clang::Preprocessor& pp, InstrContext& instrContext);

  InstrContext& getInstrContext() const;
  clang::DiagnosticsEngine& getDiagnostics() const;
  clang::Preprocessor& getPreprocessor() const;
  clang::SourceManager& getSourceManager() const;

  // Generate an error report. One of the error() methods will supply any
  // necessary arguments to format the message.
  clang::DiagnosticBuilder report(Error err, const clang::SourceLocation& loc);

  // Raise the given error. Always returns false.
  bool error(Error err, const clang::SourceLocation& loc);

  // Raise an error with the given arguments. Always returns false.
  template <typename T1>
  bool error(Error err, const clang::SourceLocation& loc, T1 arg1) {
    report(err, loc) << arg1;
    return false;
  }

  // Raise an error with the given arguments. Always returns false.
  template <typename T1, typename T2>
  bool error(Error err, const clang::SourceLocation& loc, T1 arg1, T2 arg2) {
    report(err, loc) << arg1 << arg2;
    return false;
  }

  // Prepare the parser to parse a directive. This will clear any parsing
  // state.
  void prepareToParse();

  // Peek (do not consume) the token at position n from the front of the list
  // of tokens. By default, this returns a reference to the token at the front
  // of the list of tokens. It is an error to call this with a position that is
  // out of bounds for the token list.
  const clang::Token& peek(unsigned n = 0) const;

  // Get the spelling of the token at position n from the front of the list of
  // unconsumed tokens. By default, this returns the spelling of the token at
  // the front of the list of tokens. It is an error to call this with a
  // position that is out of bounds for the token list.
  std::string spell(unsigned n = 0) const;

  // Get the spelling of the given token.
  std::string spell(const clang::Token& tok) const;

  // Get the location of the token at position n from the front of the list of
  // consumed tokens. By default, this returns the location of the token at the
  // front of the list of tokens. It is an error to call this with a position
  // that is out of bounds for the token list.
  clang::SourceLocation loc(unsigned n = 0) const;

  // Get the full location of the token at position n from the front of the
  // list of consumed tokens. By default, this returns the full location of the
  // token at the front of the list of tokens. It is an error to call this with
  // a position that is out of bounds for the token list.
  clang::FullSourceLoc fullLoc(unsigned n = 0) const;

  // Consume the token at the front of the list of unconsumed tokens. This will
  // pop the token and return it.
  clang::Token consume();

  // Consume the token at the front of the list whose spelling matches the
  // given spelling. Raise a parse error if the spelling does not match.
  bool parseToken(clang::tok::TokenKind kind);

  // Consume the token at the front of the list if it is a string literal.
  // Raise a parse error if it is not. The literal will not be enclosed in
  // double quotes. The parsed string will be returned via the given out
  // parameter. Return true if the string literal could be parsed, false
  // otherwise.
  bool parseStringLiteralInto(std::string& sout);

  // Consume the token at the front of the list if it is a simple integer
  // literal. Raise a parse error if it is not. The parsed integer will be
  // returned via the given out parameter. Returns true if an integer literal
  // could be parsed, false otherwise.
  bool parseIntLiteralInto(uint64_t& iout);

  // Consume the token at the front of the list if it is an identifier. Raise
  // a parse error if it is not. This will not attempt to determine if the
  // identifier is known. The identifier is returned via the given out
  // parameter. Returns true if the identifier could be parsed, false otherwise.
  bool parseIdentifierInto(std::string& iout);

  // Parse a sequence of identifiers.
  bool parseIdentifierListInto(std::vector<std::string>& ids);

  // Parse the current pragma. The given token is the sentinel token whose
  // spelling is Parser::tokSentinel.
  Directive* parseDirective(clang::Token& tok);

  // Parse any clauses in the directive. The kind token of the directive
  // should been consumed before this method is called.
  Clauses parseClauses();

public:
  static clang::StringRef tokSentinel;
  static clang::StringRef tokFunction;
  static clang::StringRef tokLine;
  static clang::StringRef tokLoop;
  static clang::StringRef tokRegion;
  static clang::StringRef tokName;
  static clang::StringRef tokFuncArgs;
  static clang::StringRef tokFuncStyle;
  static clang::StringRef tokFuncStyleDecl;
  static clang::StringRef tokFuncStyleQual;
  static clang::StringRef tokFuncStyleFull;
};

} // namespace instr

#endif // CLANG_PLUGIN_EXAMPLES_INSTRUMENT_PARSER_H
