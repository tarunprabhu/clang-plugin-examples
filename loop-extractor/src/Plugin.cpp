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

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

#include <limits>
#include <memory>
#include <queue>

using namespace clang;

// Class that will be passed between the pragma handler and the visitor.
// The pragma handler will record the locations of the extract pragmas in the
// file. The visitor pragma will remove the pragmas in the order they were
// inserted.
class Pragmas {
private:
  std::string file_;
  std::queue<unsigned> lines;

public:
  static const unsigned invalid = std::numeric_limits<unsigned>::max();

public:
  void setFile(StringRef file) {
    this->file_ = file.str();
  }

  const std::string& file() const {
    return this->file_;
  }

  void push(unsigned line) {
    this->lines.push(line);
  }

  bool empty() {
    return this->lines.size() == 0;
  }

  unsigned peek() {
    return this->lines.front();
  }

  unsigned pop() {
    unsigned front = this->lines.front();
    this->lines.pop();
    return front;
  }

  unsigned findNearestAndPop(unsigned lno) {
    // There may be orphaned pragmas that are not associated with any loop
    // with another pragma being nearer the current loop.
    unsigned nearest = Pragmas::invalid;
    while ((not this->empty()) and (this->peek() < lno)) {
      if (nearest != Pragmas::invalid)
        llvm::errs() << "WARNING: Unassociated pragma on line " << nearest
                     << " of " << this->file() << "\n";
      nearest = this->pop();
    }

    return nearest;
  }
};

// Singleton instance of pragmas. There doesn't seem to be another reasonable
// way to pass data between the pragma handler and the consumer/visitor classes.
// It would be nice if there was a place in the CompilerInstance where
// something could be stashed since that is actually available everywhere,
// but if there is a place, I can't find it.
//
// To minimize the use of globals, this will only directly be referenced
// by the constructors of the ExtractPragmaHandler and ExtractConsumer
// which will maintain a local reference to this and do everything with
// that local reference.
static thread_local Pragmas gPragmas;

// The visitor class will visit all the AST nodes and is where the loops will
// be identified and associated with a pragma.
class ExtractVisitor : public RecursiveASTVisitor<ExtractVisitor> {
private:
  SourceManager& srcMgr;

  // This is the same as the pragmas member of ExtractConsumer. See the
  // comment associated with it in that class.
  Pragmas& pragmas;

protected:
  std::string loopType(Stmt* stmt) {
    switch (stmt->getStmtClass()) {
    case Stmt::ForStmtClass:
      return "for";
    case Stmt::WhileStmtClass:
      return "while";
    case Stmt::DoStmtClass:
      return "do";
    default:
      return "<unknown>";
    }
  }

  void associate(Stmt* stmt) {
    FullSourceLoc loc(stmt->getBeginLoc(), this->srcMgr);
    unsigned lno = loc.getLineNumber();
    unsigned plno = this->pragmas.findNearestAndPop(lno);
    if (plno != Pragmas::invalid)
      llvm::errs() << "Associating pragma at line " << plno << " with "
                   << loopType(stmt) << " loop at line " << lno << " of "
                   << loc.getFileEntry()->getName() << "\n";
  }

public:
  explicit ExtractVisitor(CompilerInstance& compiler, Pragmas& pragmas)
      : srcMgr(compiler.getSourceManager()), pragmas(pragmas) {
    ;
  }

  virtual ~ExtractVisitor() = default;

  // TODO: Should try to see what happens if a pragma is put inside a
  // template body which may be instantiated several times. I think the
  // idea here is to allow the visitor to traverse over those as well, but
  // that hasn't been tested and I am not sure if it works the way I think
  // it does.
  bool shouldVisitTemplateInstantiations() const {
    return true;
  }

  bool VisitForStmt(ForStmt* stmt) {
    this->associate(stmt);
    return true;
  }

  bool VisitDoStmt(DoStmt* stmt) {
    this->associate(stmt);
    return true;
  }

  bool VisitWhileStmt(WhileStmt* stmt) {
    this->associate(stmt);
    return true;
  }
};

// The consumer class does nothing, but merely calls the visitor class to
// traverse the entire translation unit.
class ExtractConsumer : public ASTConsumer {
private:
  // This is a reference to be the global singleton Pragmas object.
  // By keeping a local reference, we want to ensure that if we can ever get
  // rid of the need for a global singleton, most of the code would not need
  // to be changed.
  Pragmas& pragmas;
  ExtractVisitor visitor;

public:
  explicit ExtractConsumer(CompilerInstance& compiler, Pragmas& pragmas)
      : pragmas(pragmas), visitor(compiler, pragmas) {
    ;
  }

  virtual ~ExtractConsumer() = default;

  virtual void HandleTranslationUnit(ASTContext& context) {
    visitor.TraverseDecl(context.getTranslationUnitDecl());

    // This will be reached after the entire AST has been traversed, so
    // look for any unused pragmas here. These will be those that are after
    // the last loop.
    if (not pragmas.empty())
      llvm::errs() << "WARNING: Unassociated pragma on line " << pragmas.peek()
                   << " of " << pragmas.file() << "\n";
  }
};

// This is the main plugin class. It does nothing much beyond returning a
// specialized ASTConsumer object.
class ExtractPlugin : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance& compiler, StringRef file) override {
    gPragmas.setFile(file);
    return std::make_unique<ExtractConsumer>(compiler, gPragmas);
  }

  virtual bool ParseArgs(const CompilerInstance&,
                         const std::vector<std::string>& args) override {
    for (const std::string& arg : args)
      if (arg == "-help")
        llvm::errs() << "\nThis is an example plugin to show how a custom "
                     << "pragma can be used and associated with a statement. "
                     << "In this case, the pragma is associated with the "
                     << "nearest loop and will print the line number of the "
                     << "pragma and the line number of the loop with which it "
                     << "was associated."
                     << "\n\n";
    return true;
  }

  // This will be run before the main action. If the main action is codegen (in
  // clang-speak, codegen is understood to be LLVM-IR generation), this will be
  // run after LLVM-IR is generated. In this case, it doesn't actually matter
  // when it is run since we are only interested in demonstrating how a
  // pragma could be associated with an AST node, but if we wanted to do
  // something more sophisticated, it would matter when this plugin ran.
  //
  // By implementing this method, we are ensuring that the plugin is run
  // automatically.
  virtual ActionType getActionType() override {
    return PluginASTAction::ActionType::AddBeforeMainAction;
  }
};

// The pragma handler. This looks for pragmas with the extract sentinel
// and records their locations.
class ExtractPragmaHandler : public PragmaHandler {
private:
  // This is a local reference to the global singleton Pragmas object.
  // The idea is that if we can ever determine how not to use a global, most
  // of this code would not need to be changed.
  Pragmas& pragmas;

public:
  // Here "extract" is the sentinel of the pragma that will be matched.
  ExtractPragmaHandler() : PragmaHandler("extract"), pragmas(gPragmas) {
    ;
  }

  // This way, the pragma is only matched against the sentinel, but the
  // rest of the pragma is not examined. For something like OpenMP which has
  // a much richer pragma language, the rest of the line would need to be
  // parsed. That may be demonstrated in a different example plugin.
  void HandlePragma(Preprocessor& pp, PragmaIntroducer, Token& tok) {
    SourceManager& srcMgr = pp.getSourceManager();
    FullSourceLoc loc(tok.getLocation(), srcMgr);
    this->pragmas.push(loc.getLineNumber());
  }
};

static PragmaHandlerRegistry::Add<ExtractPragmaHandler>
    ExtractPragmaHandlerX("extract-pragma-handler",
                          "Pragma to annotate loops that will be extacted "
                          "and printed.");

static FrontendPluginRegistry::Add<ExtractPlugin>
    ExtractPluginX("loop-extractor",
                   "Associates an extract pragma with a loop.");
