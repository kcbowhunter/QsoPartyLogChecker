#pragma once
class HtmlUtils
{
public:
   HtmlUtils();
   virtual ~HtmlUtils();

   static void AddDocType(vector<string>& strings);
   static void AddStartTag(vector<string>& strings, const char* tag);
   static void AddEndTag(vector<string>& strings, const char* tag);

   static void AddStartTag(vector<string>& strings, const char* tag, const char* attrib, const char* attribValue);

   static void AddTagWithContent(vector<string>& strings, const char* tag, const char* content);
   static void AddTagWithContent(vector<string>& strings, const char* tag, const string& content);
};

