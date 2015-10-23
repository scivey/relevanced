#include <benchmark/benchmark.h>
#include <vector>
#include <string>
#include <memory>

#include <sstream>
#include "models/Document.h"
#include "testing/TestHelpers.h"
#include "document_processing_worker/DocumentProcessor.h"
#include "stemmer/ThreadSafeUtf8Stemmer.h"
#include "centroid_update_worker/DocumentAccumulator.h"
#include "stopwords/StopwordFilter.h"
#include "util/Clock.h"
using namespace std;
using namespace relevanced;
using namespace relevanced::models;
using namespace relevanced::document_processing_worker;
using namespace relevanced::centroid_update_worker;
using namespace relevanced::stemmer;
using namespace relevanced::stopwords;
using namespace relevanced::util;


vector<string> FOOTBALL {
  "Mourners gathered on the track that rings the Alto High School Yellowjackets' ",
  "football field in Texas on Saturday. They held hands and prayed for junior ",
  "Cam'ron Matthews, who died after a game under the Friday night lights the day before, ",
  "marking the sixth U.S. high school football death this season. It was not clear ",
  "from local media reports what exactly happened, but CNN affiliate KLTV in Tyler, ",
  "about an hour's drive from Alto, reported that the 6-foot junior told his teammates ",
  "during a huddle that he felt dizzy shortly before halftime.  He then collapsed, ",
  "and a helicopter transported him to a Tyler hospital, the station reported. ",
  "The game against Carlisle High School of Price, Texas, was postponed. Multiple ",
  "media outlets, including CNN affiliate KETK, reported that Matthews died Saturday "
  "after suffering a seizure during the game. A GoFundMe page set up to gather ",
  "donations for Matthews' parents and sister described the teen as \"a great kid, ",
  "athlete, strong in his faith & a friend to many.\" Alto Independent School District ",
  "Superintendent Kerry Birdwell grew teary as he described the young wide receiver, ",
  "quarterback and safety. He told CNN affiliate KTRE that counselors will be available ",
  "for students on Monday. \"When you have your own children and you want them to emulate ",
  "someone and to follow in their footsteps, Cam'ron was a perfect example of that,\" ",
  "Birdwell told the station. \"He was a great kid in the classroom, unbelievable ",
  "athlete, but more importantly just a great, great kid.\"Neither the Cherokee ",
  "County Sheriff's Office nor the nearest coroner, in Dallas County, had Matthews' ",
  "cause of death Sunday. The Dallas County coroner said it would forward its autopsy ",
  "results to Cherokee County, where Alto is located. In 2014, five of the country's 1.1 ",
  "million high school football players died of causes directly related to the sport, ",
  "such as head and spine injuries, according to a survey by the National Center for ",
  "Catastrophic Sports Injury Research at the University of North Carolina. Another ",
  "six players died of indirect causes: Three were heart-related, one was from heat ",
  "stroke and two were caused by hypernatremia (an electrolyte imbalance) and water ",
  "intoxication, the survey found. \"Certainly this is not going to be one of the low ",
  "years,\" Robert Cantu, medical director for the NCCSIR and a professor at the Boston ",
  "University School of Medicine, said during an interview earlier this month. The past ",
  "decade has seen an average of three fatalities each year directly attributable to high ",
  "school football, the survey said. In 2013, there were eight deaths directly linked to ",
  "high school football. Between 2005 and 2014, the deaths of 92 other high school ",
  "football players were indirectly related to the sport, according to the NCCSIR survey."
};

static void benchDocumentProcessing(benchmark::State &state) {
  ostringstream oss;
  for (auto &elem: FOOTBALL) {
    oss << elem;
  }
  string text = oss.str();
  shared_ptr<StemmerIf> stemPtr(new ThreadSafeUtf8Stemmer);
  shared_ptr<StopwordFilterIf> stopwordPtr(new StopwordFilter);
  shared_ptr<ClockIf> clockPtr(new Clock);
  DocumentProcessor processor(stemPtr, stopwordPtr, clockPtr);
  while (state.KeepRunning()) {
    Document doc("no-id", text);
    shared_ptr<Document> docPtr(&doc, NonDeleter<Document>());
    auto processed = processor.processNew(docPtr);
  }
}

BENCHMARK(benchDocumentProcessing);

static void benchDocumentAccumulator(benchmark::State &state) {
  ostringstream oss;
  for (auto &elem: FOOTBALL) {
    oss << elem;
  }
  string text = oss.str();
  shared_ptr<StemmerIf> stemPtr(new ThreadSafeUtf8Stemmer);
  shared_ptr<StopwordFilterIf> stopwordPtr(new StopwordFilter);
  shared_ptr<ClockIf> clockPtr(new Clock);
  DocumentProcessor processor(stemPtr, stopwordPtr, clockPtr);
  Document doc("no-id", text);
  shared_ptr<Document> docPtr(&doc, NonDeleter<Document>());
  auto processed = processor.processNew(docPtr);
  auto processedPtr = processed.get();
  while (state.KeepRunning()) {
    DocumentAccumulator accumulator;
    for (size_t i = 0; i < 500; i++) {
      accumulator.addDocument(processedPtr);
    }
    accumulator.getMagnitude();
    auto scores = accumulator.getScores();
  }
}
BENCHMARK(benchDocumentAccumulator);

int main(int argc, const char **argv) {
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
}
