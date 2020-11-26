#include <iostream>
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

int main() {
    const char* json = "{\"project\":\"rapidjson\", \"stars\":10}";
    rapidjson::Document d;
    if(d.Parse(json).HasParseError())
        return 0;

    std::cout << d.IsArray() << std::endl;
    std::cout << d.IsObject() << std::endl;
//    d["project"].GetObject()["stars"].GetArray();

    d.HasMember("project");
    d["stars"].IsNumber();

    rapidjson::Value::MemberIterator project = d.FindMember("project");
    if(project != d.MemberEnd())
        std::cout << project->value.GetString() << std::endl;

    rapidjson::Value &s = d["stars"];
    s.SetInt(s.GetInt()+1);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    std::cout << buffer.GetString() << std::endl;
    return 0;
}
