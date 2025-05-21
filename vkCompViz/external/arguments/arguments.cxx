module;

#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <string>
#include <exception>

export module arguments;

export class Arguments
{
    public:
    class Argument
    {
        public:
        Argument(){};
        Argument(std::string v) : value{v}, exists{true}{};

        operator std::string() const { return value; };
        operator int() { return std::stoi(value); };
        operator double() { return std::stod(value); };
        operator float() { return std::stof(value); };
        operator size_t() { return std::stoll(value); }
        operator bool() { return exists; }

        private:
        std::string value{"0"}; 
        bool exists{false};
    };

    using ArgList = std::unordered_map<std::string, Argument>;
    bool printHelpIfPresent(std::string helpText)
    {
        if(arguments["--help"] || arguments["-h"])
        {
            std::cout << helpText;
            return true;
        }
        return false;
    }

    Arguments(int argc, char** argv)
    {
        std::vector<std::string> argList(argv + 1, argv + argc);
        std::string lastParamName{""};
        bool lastAssigned{false};
        for (auto const& arg : argList)
        {
            bool isParamName{false};
            if(arg[0] == '-')
            {
                if(!isdigit(arg[1]))
                    isParamName = true;
                else if(arg[1] == '-' && !isdigit(arg[2]))
                    isParamName = true;
            }
            if(isParamName)
            {
                lastParamName = arg;
                arguments[lastParamName] = Argument("");
                lastAssigned = false;
            }
            else if(!lastAssigned)
            {
                arguments[lastParamName] = Argument(arg);
                lastAssigned = true;
            }
            else
            {
                throw std::invalid_argument("Unexpected argument: " + arg);
            }                
        }        
    }
   
    Argument operator[] (const std::string& key) { return arguments[key]; };

    private:
    ArgList arguments;
};
