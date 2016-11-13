//
// Created by sun on 11/13/16.
//

#ifndef HTML_PARSER_HTMLPARSER_H
#define HTML_PARSER_HTMLPARSER_H

#include "myhashmap.h"
#include "myhashset.h"
#include "mystring.h"

class html_node;

class html_selector {
public:
    html_selector(myu32string tag = "") : m_tag(tag) {}
    template<typename CharT>
    html_selector(const CharT * tag) : m_tag(tag) {}
    html_selector(const html_selector &other)
            : m_tag(other.m_tag), m_classes(other.m_classes), m_attrs(other.m_attrs) {}
    html_selector &operator = (const html_selector &other) {
        m_tag = other.m_tag;
        m_classes = other.m_classes;
        m_attrs = other.m_attrs;
        return *this;
    }
    html_selector &id(myu32string x) {
        m_attrs.push_back(mymake_pair("id", x));
        return *this;
    }
    html_selector &class_(myu32string x) {
        m_classes.push_back(x);
        return *this;
    }
    html_selector &attr(myu32string k, myu32string v) {
        m_attrs.push_back(mymake_pair(k, v));
        return *this;
    }

    bool match(const html_node *node) const;
private:
    myu32string m_tag;
    myvector<myu32string> m_classes;
    myvector<mypair<myu32string, myu32string> > m_attrs;
};

template<typename PNode>
class html_result: public myhashset<PNode> {
public:
    html_result() {}
    html_result(const html_result &other) : myhashset<PNode>(other) {}
    html_result find_all(const html_selector &selector) const;
    myvector<myu32string> text() const;
    myvector<myu32string> html() const;
};

class html_node {
    friend class html_selector;
    friend std::size_t parse_node(const myu32string &html, std::size_t index, html_node &parent, html_node &node, int &open_num);
    friend std::ostream &operator << (std::ostream &out, const html_node &node);

public:
    html_node() : type(DOCUMENT), parent(NULL) {}
    html_node(const myu32string &html) : type(DOCUMENT), parent(NULL) {
        parse(html);
    }
    html_node(const html_node &other)
            : type(other.type), name(other.name), attrs(other.attrs),
              parent(other.parent), children(other.children),
              m_text(other.m_text) {}
    html_node &operator = (const html_node &other) {
        type     = other.type;
        name     = other.name;
        attrs    = other.attrs;
        parent   = other.parent;
        children = other.children;
        m_text   = other.m_text;
        return *this;
    }

    void parse(const myu32string &html);
    myu32string html() const;
    myu32string text() const;
    const html_node *find(const html_selector &selector) const;
    html_node *find(const html_selector &selector);
    html_result<const html_node *> find_all(const html_selector &selector) const;
    html_result<html_node *> find_all(const html_selector &selector);
private:
    enum {DOCUMENT, TAG, TEXT, COMMENT} type;
    // Tag
    myu32string name;
    myhashmap<myu32string, myu32string> attrs;
    myhashset<myu32string> classes;
    html_node *parent;
    myvector<html_node *> children;
    // Text and comment
    myu32string m_text;
};

template<typename PNode>
html_result<PNode> html_result<PNode>::find_all(const html_selector &selector) const {
    html_result result;
    for (typename myhashset<PNode>::const_iterator iter = this->cbegin(); iter != this->cend(); ++iter) {
        myhashset<PNode> temp_result = (*iter)->find_all(selector);
        result.insert(temp_result.cbegin(), temp_result.cend());
    }
    return result;
}

template<typename PNode>
myvector<myu32string> html_result<PNode>::text() const {
    myvector<myu32string> result;
    for (typename myhashset<PNode>::const_iterator iter = this->cbegin(); iter != this->cend(); ++iter)
        result.push_back((*iter)->text());
    return result;
}

template<typename PNode>
myvector<myu32string> html_result<PNode>::html() const {
    myvector<myu32string> result;
    for (typename myhashset<PNode>::const_iterator iter = this->cbegin(); iter != this->cend(); ++iter)
        result.push_back((*iter)->html());
    return result;
}

// Debug
std::ostream &operator << (std::ostream &out, const html_node &node);

typedef html_node html_dom;

#endif //HTML_PARSER_HTMLPARSER_H