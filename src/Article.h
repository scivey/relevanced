#pragma once
#include <string>
namespace {
  using namespace std;
}

class Article {
protected:
public:
  enum class Subject {
    MATH, POLITICS
  };
  string title_;
  string url_;
  string text_;
  Subject subject_;
  Article(const string &title, const string &url, Subject subject, const string &text):
    title_(title), url_(url), subject_(subject), text_(text) {}
};
