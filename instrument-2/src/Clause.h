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

#ifndef CLANG_PLUGIN_EXAMPLES_INSTRUMENT_CLAUSE_H
#define CLANG_PLUGIN_EXAMPLES_INSTRUMENT_CLAUSE_H

#include <llvm/Support/Casting.h>

#include "Instr.h"
#include "Parser.h"

namespace instr {

class InstrContext;

class Clause {
public:
  enum Kind {
    Name,
    FuncStyle,
    FuncArgs,
  };

private:
  InstrContext& instrContext;
  clang::FullSourceLoc loc;
  const Clause::Kind kind;

protected:
  Clause(InstrContext& instrContext,
         const clang::FullSourceLoc& loc,
         Clause::Kind kind);

public:
  Kind getKind() const;
  virtual ~Clause() = default;
  virtual clang::StringRef spell() const = 0;
};

// Clause name(<string-literal>)
//
// Example:
//
//     #pragma instrument region name("area-51")
//
class ClName : public Clause {
private:
  std::string name;

protected:
  ClName(InstrContext& instrContext,
         const clang::FullSourceLoc& loc,
         const std::string& name);

public:
  virtual ~ClName() = default;
  virtual clang::StringRef spell() const override;
  const std::string& getName() const;

public:
  static ClName* parse(Parser& parser, const clang::SourceLocation& loc);
  static bool classof(const Clause* clause);
};

// Clause style(decl | qual | full)
//
// Example:
//
//     #pragma instrument function style(qual)
//
class ClFuncStyle : public Clause {
private:
  Style style;

protected:
  ClFuncStyle(InstrContext& instrContext,
              const clang::FullSourceLoc& loc,
              Style style);

public:
  virtual ~ClFuncStyle() = default;
  virtual clang::StringRef spell() const override;
  Style getStyle() const;

public:
  static ClFuncStyle* parse(Parser& parser, const clang::SourceLocation& loc);
  static bool classof(const Clause* clause);
};

// Clause args(<possibly-empty-list-of-identifiers>)
//
// Example:
//
//     #pragma instrument function args(arg1, arg3)
//     #pragma instrument function args()
//
class ClFuncArgs : public Clause {
private:
  std::vector<std::string> args;

public:
  using ConstIterator = decltype(args)::const_iterator;

protected:
  ClFuncArgs(InstrContext& instrContext,
             const clang::FullSourceLoc& loc,
             const std::vector<std::string>& args);

public:
  virtual ~ClFuncArgs() = default;
  virtual clang::StringRef spell() const override;
  ConstIterator begin() const;
  ConstIterator end() const;
  bool empty() const;
  bool hasArg(const std::string& arg) const;

public:
  static ClFuncArgs* parse(Parser& parser, const clang::SourceLocation& loc);
  static bool classof(const Clause* clause);
};

} // namespace instr

#endif // CLANG_PLUGIN_EXAMPLES_INSTRUMENT_CLAUSE_H
