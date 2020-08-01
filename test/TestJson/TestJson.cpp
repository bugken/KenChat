#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>

using namespace std;

string func_json1()
{
    json js;
    js["msg_type"] = 2;
    js["msg_from"] = "Client";
    js["msg_to"] = "Server";
    js["msg_from_ip"] = "127.0.0.1";
    js["msg_to_ip"] = "127.0.0.1";
    js["msg_to_body"]["client1"] = "Hi, client1";
    js["msg_to_body"]["client2"] = "Hi, client2";
    js["msg_to_body"]["client3"] = "Hi, client3";

    string strSendBuff = js.dump();

    return strSendBuff.c_str();
}

string func_json2()
{
    json js;
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    js["vector"] = vec;
    string strSendBuff = js.dump();

    return strSendBuff.c_str();
}

string func_json3()
{
    json js;
    map<int, string> m;
    m.insert({1, "1"});
    m.insert(pair<int, string>(2, "2"));
    m.insert(pair<int, string>(3, "3"));
    m.insert(pair<int, string>(4, "4"));
    m.insert(pair<int, string>(5, "5"));

    js["map"] = m;
    string strSendBuff = js.dump();

    return strSendBuff.c_str();
}


int main()
{
    cout << "......This Is Test Begin!......" << endl;
    string strRevBuff = func_json2();
    json js = json::parse(strRevBuff);
    cout << js["vector"] << endl;
    vector<int> vec = js["vector"];
    for (auto &v : vec)
        cout << v;
    cout<<endl;
    cout << "......This Is Test End!......" << endl;

    return 0;
}