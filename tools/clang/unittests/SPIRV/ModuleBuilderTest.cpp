//===- unittests/SPIRV/ModuleBuilderTest.cpp ------ ModuleBuilder tests ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "spirv/1.0/spirv.hpp11"
#include "clang/SPIRV/ModuleBuilder.h"

#include "SPIRVTestUtils.h"

namespace {

using namespace clang::spirv;

using ::testing::ContainerEq;
using ::testing::ElementsAre;

TEST(ModuleBuilder, TakeModuleDirectlyCreatesHeader) {
  SPIRVContext context;
  ModuleBuilder builder(&context);

  EXPECT_THAT(builder.takeModule(),
              ElementsAre(spv::MagicNumber, spv::Version, 14u << 16, 1u, 0u));
}

TEST(ModuleBuilder, CreateFunction) {
  SPIRVContext context;
  ModuleBuilder builder(&context);

  const auto rType = context.takeNextId();
  const auto fType = context.takeNextId();
  const auto fId = context.getNextId();
  const auto resultId = builder.beginFunction(fType, rType);
  EXPECT_EQ(fId, resultId);
  EXPECT_TRUE(builder.endFunction());
  const auto result = builder.takeModule();

  auto expected = getModuleHeader(context.getNextId());
  appendVector(&expected,
               constructInst(spv::Op::OpFunction, {rType, fId, 0, fType}));
  appendVector(&expected, constructInst(spv::Op::OpFunctionEnd, {}));
  EXPECT_THAT(result, ContainerEq(expected));
}

TEST(ModuleBuilder, CreateBasicBlock) {
  SPIRVContext context;
  ModuleBuilder builder(&context);

  const auto rType = context.takeNextId();
  const auto fType = context.takeNextId();
  const auto fId = context.getNextId();
  EXPECT_NE(0, builder.beginFunction(fType, rType));
  const auto labelId = context.getNextId();
  const auto resultId = builder.bbCreate();
  EXPECT_EQ(labelId, resultId);
  EXPECT_TRUE(builder.bbReturn(resultId));
  EXPECT_TRUE(builder.endFunction());

  const auto result = builder.takeModule();

  auto expected = getModuleHeader(context.getNextId());
  appendVector(&expected,
               constructInst(spv::Op::OpFunction, {rType, fId, 0, fType}));
  appendVector(&expected, constructInst(spv::Op::OpLabel, {labelId}));
  appendVector(&expected, constructInst(spv::Op::OpReturn, {}));
  appendVector(&expected, constructInst(spv::Op::OpFunctionEnd, {}));

  EXPECT_THAT(result, ContainerEq(expected));
}

} // anonymous namespace
