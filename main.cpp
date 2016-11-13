#include <iostream>
#include <fstream>

#include "mycodecvt.h"
#include "mydir.h"
#include "htmlparser.h"

using namespace std;

int main()
{
    myhashset<myu32string> dictionary;
    size_t max_key_length = 0;
    // Load dictionary
    ifstream dict_file("dictionary.dic");
    mystring line;
    while (getline(dict_file, line)) {
        if (line.empty())
            continue;
        myu32string data = utf8_to_utf32(line);
        if (data.size() > max_key_length)
            max_key_length = data.size();
        dictionary.insert(data);
    }

    for (auto filename:  mydir("input")) {
        ifstream html_file(("input/" + filename).c_str());
        ofstream info_file(("output/" + filename.substr(0, filename.find(".")) + ".info").c_str());
        ofstream word_file(("output/" + filename.substr(0, filename.find(".")) + ".txt").c_str());
        // Build dom tree
        html_dom dom(input_utf8_to_utf32(html_file));
        // Parse information
        myu32string headline, question, author, content;
        headline = dom.find("h1")->text();
        question = dom.find("h2")->text();
        author = dom.find(html_selector("span").class_("author"))->text();
        if (!author.empty() && author.back() == L'\uff0c')
            author.pop_back();
        for (auto i : dom.find(html_selector("div").class_("content"))->find_all("p").text()) {
            content += i;
            content.push_back('\n');
        }
        // Output information
        info_file << utf32_to_utf8(headline) << '\n'
              << utf32_to_utf8(question) << '\n'
              << utf32_to_utf8(author) << '\n'
              << utf32_to_utf8(content);
        // Split words and output
        size_t start = 0;
        while (start != content.size()) {
            size_t length = content.size() - start;
            if (length > max_key_length)
                length = max_key_length;
            while (length > 1 && dictionary.find(content.substr(start, length)) == dictionary.end())
                --length;
            word_file << utf32_to_utf8(content.substr(start, length)) << '\n';
            start += length;
        }
    }
    return 0;
}