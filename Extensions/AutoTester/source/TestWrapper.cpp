#include "TestWrapper.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool TestWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
    // create any objects here as instance variables of this class
    // as well as any initialization required for your spa program
    this->pkb = PKB();
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
    // call your parser to do the parsing
    // ...rest of your code...

    // Read file into string
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();

    bool extractExtension = true;
    Parser parser = Parser(&pkb);
    DesignExtractor dex = DesignExtractor(&pkb, extractExtension);

    try {
        parser.parse(buffer.str());
        pkb.sortAllDataStructure();
        dex.extractDesign();
    } catch (std::exception const &ex) {
        std::cout << ex.what() << std::endl;
        exit(1);
    }
    pkb.sortAllDataStructure();
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results) {
  // call your evaluator to evaluate the query here
  // ...code to evaluate query...

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
    QueryManager::parseAndEvaluateQuery(query, results, &pkb);
}
