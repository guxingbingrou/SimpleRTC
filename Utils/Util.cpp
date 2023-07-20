#include "Util.h"

Util::Util()
{

}

std::vector<std::string> Util::Split(std::string src, std::string delim)
{
    std::vector<std::string> split;

    std::string::size_type start_pos, end_pos;

    end_pos = src.find(delim);
    start_pos = 0;

    while(std::string::npos != end_pos){
        split.push_back(src.substr(start_pos, end_pos - start_pos));

        start_pos = end_pos + delim.size();

        end_pos = src.find(delim);
    }

    if(start_pos != src.size() -1){
        split.push_back(src.substr(start_pos));
    }

    return split;
}
