//===--- utils/unittest/UnitTestMain/TestMain.cpp - unittest driver -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "llvm/Support/Signals.h"

#include "SPIRVTestOptions.h"

#if defined(_WIN32)
#include <windows.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif
#endif

const char *TestMainArgv0;

int main(int argc, char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(true /* Disable crash reporting */);

  // Initialize both gmock and gtest.
  testing::InitGoogleMock(&argc, argv);

  for (int i = 1; i < argc; ++i) {
    if (std::string("--spirv-test-root") == argv[i]) {
      // Allow the user set the root directory for test input files.
      if (i + 1 < argc) {
        clang::spirv::testOptions::inputDataDir = argv[i + 1];
        i++;
      } else {
        fprintf(stderr, "Error: --spirv-test-root requires an argument\n");
        return 1;
      }
    }
  }

  // Make it easy for a test to re-execute itself by saving argv[0].
  TestMainArgv0 = argv[0];

#if defined(_WIN32)
  // Disable all of the possible ways Windows conspires to make automated
  // testing impossible.
  ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#if defined(_MSC_VER)
  ::_set_error_mode(_OUT_TO_STDERR);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
#endif

  return RUN_ALL_TESTS();
}
