#include "stdafx.h"
#include "HtmlUtils.h"

HtmlUtils::HtmlUtils()
{
}

HtmlUtils::~HtmlUtils()
{
}

void HtmlUtils::AddDocType(vector<string>& strings)
{
   strings.push_back(string("<!DOCTYPE html>"));
}

void HtmlUtils::AddStartTag(vector<string>& strings, const char* tag)
{
   if (tag == nullptr || *tag == '\0')
      return;

   string s = string("<") + string(tag) + string(">");
   strings.push_back(s);
}

void HtmlUtils::AddStartTag(vector<string>& strings, const char* tag, const char* attrib, const char* attribValue)
{
   if (tag == nullptr || *tag == '\0')
      return;

   if (attrib == nullptr || *attrib == '\0' || attribValue == nullptr || *attribValue == '\0')
   {
      AddStartTag(strings, tag);
   }

   string s = string("<") + string(tag) + string(" ") + string(attrib) + string("=") + string(attribValue) + string(">");
   strings.push_back(s);
}

void HtmlUtils::AddEndTag(vector<string>& strings, const char* tag)
{
   if (tag == nullptr || *tag == '\0')
      return;

   string s = string("</") + string(tag) + string(">");
   strings.push_back(s);
}

void HtmlUtils::AddTagWithContent(vector<string>& strings, const char* tag, const string& content)
{
   if (content.empty())
      return;

   AddTagWithContent(strings, tag, content.c_str());
}

void HtmlUtils::AddTagWithContent(vector<string>& strings, const char* tag, const char* content)
{
   if (tag == nullptr || *tag == '\0')
      return;

   if (content == nullptr || *content == '\0')
      return;

   string s = string("<") + string(tag) + string(">") + content + string("</") + string(tag) + string(">");
   strings.push_back(s);
}

