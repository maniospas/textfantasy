#include "unit.h"
#include "agent.h"

static std::string json_escape(std::string s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;   // backslash
            case '\"': out += "\\\""; break;   // double quote
            case '\n': out += "\\n";  break;   // newline
            case '\r': break;                  // drop CR if present
            default:   out += c;      break;
        }
    }
    return out;
}

std::string requested = "";
std::string cached = "---";

std::string request_engine(Unit& u) {
    // If we already asked for this unit, just give whatever is cached
    if (cached=="...") // waiting for reply
        return cached; 
    if (cached=="---")
        requested = ""; // retry in this case
    else if (requested == std::to_string((long long)&u)+std::to_string(u.mood)+"_"+std::to_string(u.health))
        return cached;

    // mark this unit as requested and immediately give placeholder
    requested = std::to_string((long long)&u)+std::to_string(u.mood)+"_"+std::to_string(u.health);
    cached = "...";

    // kick off the slow network call in a detached background thread
    std::string request = "A ";
    request += u.name;
    request += "\n";
    if(u.mood) {
        if(u.mood<0)
            request += "is " +std::to_string(-u.mood)+"0% angry";
        else
            request += "is " +std::to_string(u.mood)+"0% friendly";
    }
    else 
        request += " is there";
    if(u.max_health)
        request += ". It is healthy "+std::to_string(u.health)+" out of "+std::to_string(u.max_health);
    request += ". Describe in at most six words how it acts or looks in front of me (refer to me as you). The genre is fantasy but not everything is important. ";
    if (!u.items.empty() || u.mood) {
        request += " Ensure you alude to at as many of: health, mood, ";
        for (auto& item : u.items) {
            request += ",";
            request += item.name;
        }
    }
    std::string safe_request = json_escape(request);
    std::thread([safe_request] {
        const int token_limit = 25;
        std::string cmd =
            "curl -s -X POST http://localhost:11434/api/generate "
            "-H \"Content-Type: application/json\" "
            "-d '{\"model\":\"llama3.2\","
            //"-d '{\"model\":\"qwen:0.5b\","
            "\"prompt\":\"" + safe_request + "\","
            "\"num_predict\":" + std::to_string(token_limit) + ","
            "\"stream\": false}'";

        std::array<char, 4096> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (pipe) {
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
                result += buffer.data();
        }

        // update the global cache once finished
        if (result.find("error") != std::string::npos) {
            cached = "---";
            return;
        }
        if (result.find(""))
        if (result.find("cannot") != std::string::npos) {
            cached = "---";
            return;
        }
        if (result.find("/") != std::string::npos) {
            cached = "---";
            return;
        }
        if (result.find("\\") != std::string::npos) {
            cached = "---";
            return;
        }
        if (result.find("can't") != std::string::npos) {
            cached = "---";
            return;
        }
        if (result.find("can not") != std::string::npos) {
            cached = "---";
            return;
        }
        std::regex re("\"response\"\\s*:\\s*\"(.*?)\"");
        std::smatch m;
        if (std::regex_search(result, m, re))
            result = m[1].str();
        auto nl = result.find('\n');
        if (nl != std::string::npos)
            result = result.substr(0, nl);
        if (result.size()>=50 || !result.size() || result.at(result.size()-1)!='.') { // generated too long description and it wasn't captured
            cached = "---";
            return;
        }
        cached = result;
    }).detach(); // background thread
    return cached;
}