// Copyright (c) The HLSL2GLSLFork Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.


#ifndef _PARSER_HELPER_INCLUDED_
#define _PARSER_HELPER_INCLUDED_

#include "SymbolTable.h"
#include "localintermediate.h"


//
// The following are extra variables needed during parsing, grouped together so
// they can be passed to the parser without needing a global.
//
struct TParseContext
{
   TParseContext(TSymbolTable& symt, TIntermediate& interm, EShLanguage L, TInfoSink& is) : 
   intermediate(interm), symbolTable(symt), infoSink(is), language(L), treeRoot(0),
   recoveredFromError(false), numErrors(0), lexAfterType(false), loopNestingLevel(0), 
   inTypeParen(false)
   {
   }
   TIntermediate& intermediate; // to hold and build a parse tree
   TSymbolTable& symbolTable;   // symbol table that goes with the language currently being parsed
   TInfoSink& infoSink;
   EShLanguage language;        // vertex or fragment language (future: pack or unpack)
   TIntermNode* treeRoot;       // root of parse tree being created
   bool recoveredFromError;     // true if a parse error has occurred, but we continue to parse
   int numErrors;
   bool lexAfterType;           // true if we've recognized a type, so can only be looking for an identifier
   int loopNestingLevel;        // 0 if outside all loops
   bool inTypeParen;            // true if in parentheses, looking only for an identifier
   const TType* currentFunctionType;  // the return type of the function that's currently being parsed
   bool functionReturnsValue;   // true if a non-void function has a return

   void C_DECL error(TSourceLoc, const char *szReason, const char *szToken, 
                     const char *szExtraInfoFormat, ...);
   bool reservedErrorCheck(int line, const TString& identifier);
   void recover();

   TQualifier getDefaultQualifier() const { return symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary; }

   bool parseVectorFields(const TString&, int vecSize, TVectorFields&, int line);
   bool parseMatrixFields(const TString&, int matSize, TVectorFields&, int line);
   void assignError(int line, const char* op, TString left, TString right);
   void unaryOpError(int line, char* op, TString operand);
   void binaryOpError(int line, char* op, TString left, TString right);
   bool lValueErrorCheck(int line, char* op, TIntermTyped*);
   bool constErrorCheck(TIntermTyped* node);
   bool integerErrorCheck(TIntermTyped* node, char* token);
   bool globalErrorCheck(int line, bool global, char* token);
   bool constructorErrorCheck(int line, TIntermNode*, TFunction&, TOperator, TType*);
   bool arraySizeErrorCheck(int line, TIntermTyped* expr, int& size);
   bool arrayQualifierErrorCheck(int line, TPublicType type);
   bool arrayTypeErrorCheck(int line, TPublicType type);
   bool arrayErrorCheck(int line, TString& identifier, TPublicType type, TVariable*& variable);
   bool arrayErrorCheck(int line, TString& identifier, const TTypeInfo *info, TPublicType type, TVariable*& variable);
   bool insertBuiltInArrayAtGlobalLevel();
   bool voidErrorCheck(int, const TString&, const TPublicType&);
   bool boolErrorCheck(int, const TIntermTyped*);
   bool boolErrorCheck(int, const TPublicType&);
   bool boolOrVectorErrorCheck(int line, const TIntermTyped* type);
   bool samplerErrorCheck(int line, const TPublicType& pType, const char* reason);
   bool structQualifierErrorCheck(int line, const TPublicType& pType);
   bool parameterSamplerErrorCheck(int line, TQualifier qualifier, const TType& type);
   bool containsSampler(TType& type);
   bool nonInitConstErrorCheck(int line, TString& identifier, TPublicType& type);
   bool nonInitErrorCheck(int line, TString& identifier, const TTypeInfo *info, TPublicType& type);
   bool nonInitErrorCheck(int line, TString& identifier, TPublicType& type);
   bool paramErrorCheck(int line, TQualifier qualifier, TQualifier paramQualifier, TType* type);
   const TFunction* findFunction(int line, TFunction* pfnCall, bool *builtIn = 0);
   bool executeInitializer(TSourceLoc line, TString& identifier, const TTypeInfo *info, TPublicType& pType, 
                           TIntermTyped* initializer, TIntermNode*& intermNode, TVariable* variable = 0);
   bool executeInitializer(TSourceLoc line, TString& identifier, TPublicType& pType, 
                           TIntermTyped* initializer, TIntermNode*& intermNode, TVariable* variable = 0);
   bool areAllChildConst(TIntermAggregate* aggrNode);
   TIntermTyped* addConstructor(TIntermNode*, const TType*, TOperator, TFunction*, TSourceLoc);
   TIntermTyped* foldConstConstructor(TIntermAggregate* aggrNode, const TType& type);
   TIntermTyped* constructArray(TIntermAggregate*, const TType*, TOperator, TSourceLoc);
   TIntermTyped* constructStruct(TIntermNode*, TType*, int, TSourceLoc, bool subset);
   TIntermTyped* constructBuiltIn(const TType*, TOperator, TIntermNode*, TSourceLoc, bool subset);
   TIntermTyped* addConstVectorNode(TVectorFields&, TIntermTyped*, TSourceLoc);
   TIntermTyped* addConstMatrixNode(int , TIntermTyped*, TSourceLoc);
   TIntermTyped* addConstArrayNode(int index, TIntermTyped* node, TSourceLoc line);
   TIntermTyped* addConstStruct(TString& , TIntermTyped*, TSourceLoc);
   TIntermTyped* addAssign(TOperator op, TIntermTyped* left, TIntermTyped* right, TSourceLoc);
   TIntermAggregate* mergeAggregates( TIntermAggregate *left, TIntermAggregate *right);
   bool arraySetMaxSize(TIntermSymbol*, TType*, int, bool, TSourceLoc);
   TOperator getConstructorOp( const TType&);
   TIntermNode* promoteFunctionArguments( TIntermNode *node, const TFunction* func);
   TString HashErrMsg; 
   bool AfterEOF;
};

int PaParseString(char* source, TParseContext&);
void PaReservedWord();
int PaIdentOrType(TString& id, TParseContext&, TSymbol*&);
int PaParseComment(int &lineno, TParseContext&);
void setInitialState();

typedef TParseContext* TParseContextPointer;
extern TParseContextPointer& GetGlobalParseContext();
#define GlobalParseContext GetGlobalParseContext()

typedef struct TThreadParseContextRec
{
   TParseContext *lpGlobalParseContext;
} TThreadParseContext;

#endif // _PARSER_HELPER_INCLUDED_

