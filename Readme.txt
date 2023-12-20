Program overview:
This folder contains the files required to run the program. There should be the following folders in this folder: AutoTester, Gui, lib, IntegrationTesting, source, SPA and UnitTesting. There should also be a StartupSPASolution.sln file which will be the entry point to building the program. If any of the folders or files are missing, feel free to contact any of us from the report.

Solution requirements:
This program has been tested on the following version of the software and might not be able to function properly on other version.
- Microsoft Visual Studio Enterprise 2019: Version 16.7.1

Building the solution:
1. Open the StartupSPASolution.sln file in Microsoft Visual Studio.
2. Ensure that the Solution Configuration is Release and the Solutions Platform is Win32 in the top bar.
3. Right-click on AutoTester in the solution explorer and click Rebuild.
4. Once the build is completed, a new folder called Release will be created in Code04 with AutoTester.exe in it.

Running the test:
To run the provided tests, a simple run_all_tests_release.bat file is provided in the Tests04 folder. After the program has been build, and the AutoTester.exe is in the release folder, you can double click on the run_test_release.bat file to run the tests and get the outputs in Tests04/Iter1/output and Tests04/Iter2-3/output.

To run other tests, open command prompt/power shell and type in this command:
./Release/AutoTester.exe path_to_source_folder/source_file.txt path_to_query_folder/query_file.txt path_to_output_folder/output.xml

To enable extension extraction, toggle the `extractExtension` variable in TestWrapper.cpp and rebuild in RELEASE mode.
Note that this might take 10 - 20 mins to parse the programs due to the extraction.