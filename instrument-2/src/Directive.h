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

#ifndef CLANG_PLUGIN_EXAMPLES_INSTRUMENT_DIRECTIVE_H
#define CLANG_PLUGIN_EXAMPLES_INSTRUMENT_DIRECTIVE_H

// This is needed to enable LLVM-style RTTI. This lets us use cast<>,
// dyn_cast<> and isa<> with the annotation objects.
#include <llvm/Support/Casting.h>

#include "Instr.h"
#include "Parser.h"

namespace clang {
class SourceManager;
} // namespace clang

namespace instr {

// Forward declaration of the context class. Cannot include AnnContext.h because
// that would cause a circular dependence.
class InstrContext;

// Abstract base class for all annotations.
class Directive {
public:
  enum Kind {
    Function,
    Line,
    Loop,
    Region,
  };

private:
  // The instrContext owns a unique pointer to this object.
  InstrContext& instrContext;

  // The location of the directive kind in the pragma.
  clang::FullSourceLoc loc;

  const Kind kind;

protected:
  Directive() = delete;
  Directive(Directive&) = delete;
  Directive(Directive&&) = delete;
  Directive(InstrContext& instrContext,
            const clang::FullSourceLoc& loc,
            Directive::Kind kind);

public:
  virtual ~Directive() = default;
  virtual clang::StringRef spell() const = 0;

  Kind getKind() const;
  const clang::FullSourceLoc& getLoc() const;
  const clang::FileEntry* getFileEntry() const;
  unsigned getLineNumber() const;
  InstrContext& getInstrContext();
};

// #pragma instrument function
class DrFunction : public Directive {
public:
  enum PrintArgs {
    All,
    Some,
    None,
  };

private:
  Style style;

  // True if at least one of the arguments of the function should be printed
  // when the function is entered.
  PrintArgs printArgs;

  // The names of the arguments that should be printed when the function is
  // entered.
  std::vector<std::string> argNames;

  // The arguments of the function to be printed. When the object is created,
  // this will be empty, but once the directive is associated with a function,
  // this will be resized.
  std::vector<bool> args;

protected:
  // This should only be called with printArgs = PrintArgs::All or
  // PrintArgs::None.
  DrFunction(InstrContext& instrContext,
             const clang::FullSourceLoc& loc,
             PrintArgs printArgs = PrintArgs::All,
             Style style = Style::Qual);

  DrFunction(InstrContext& instrContext,
             const clang::FullSourceLoc& loc,
             const std::vector<std::string>& argNames,
             Style style = Style::Qual);

public:
  virtual ~DrFunction() = default;
  virtual clang::StringRef spell() const override;

  Style getStyle() const;

  // Check if the named argument should be printed. Will return true if either
  // the argument is present in the argNames member or if all the args should
  // be printed
  bool shouldPrint(const std::string& arg) const;
  bool shouldPrint(unsigned argNo) const;

  // Set the number of arguments in the function. This must be called once the
  // directive is associated with a function. When it is called, all of the
  // arguments are assumed to be printed, so the args member of the class
  // will be set to all true.
  void setNumArgs(unsigned args);

  // This should be called if the argument should be printed.
  void setArg(unsigned argNo);

  // This should be called if the argument should not be printed.
  void resetArg(unsigned argNo);

public:
  static DrFunction* parse(Parser& parser, const clang::SourceLocation& loc);
  static bool classof(const Directive* dr);
};

// #pragma instrument line
class DrLine : public Directive {
private:
  std::string name;

protected:
  DrLine(InstrContext& instrContext,
         const clang::FullSourceLoc& loc,
         const std::string& name = "");

public:
  virtual ~DrLine() = default;
  virtual clang::StringRef spell() const override;
  const std::string& getName() const;

public:
  static DrLine* parse(Parser& parser, const clang::SourceLocation& loc);
  static bool classof(const Directive* dr);
};

// #pragma instrument loop
class DrLoop : public Directive {
private:
  std::string name;

protected:
  DrLoop(InstrContext& instrContext,
         const clang::FullSourceLoc& loc,
         const std::string& name = "");

public:
  virtual ~DrLoop() = default;
  virtual clang::StringRef spell() const override;
  const std::string& getName() const;

public:
  static DrLoop* parse(Parser& parser, const clang::SourceLocation& loc);
  static bool classof(const Directive* dr);
};

// #pragma instrument region
class DrRegion : public Directive {
private:
  std::string name;

protected:
  DrRegion(InstrContext& instrContext,
           const clang::FullSourceLoc& loc,
           const std::string& name = "");

public:
  virtual ~DrRegion() = default;
  virtual clang::StringRef spell() const override;
  const std::string& getName() const;

public:
  static DrRegion* parse(Parser& parser, const clang::SourceLocation& loc);
  static bool classof(const Directive* dr);
};

} // namespace instr

#endif // CLANG_PLUGIN_EXAMPLES_INSTRUMENT_DIRECTIVE_H
