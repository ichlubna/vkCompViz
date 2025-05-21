#include <string>
#include <iostream>
import arguments;

int main(int argc, char *argv[])
{
    std::string helpText =  "Testing help\n"
                            "--help, -h Prints this help\n"
                            "-e Just a check if the parameter exists\n"
                            "--lightsaber_power Floating point number\n"
                            "--nazghul_count Int number\n"
                            "--spell String\n";
    Arguments args(argc, argv);
    if(args.printHelpIfPresent(helpText))
        return 0;
    bool areYouHere = args["-e"]; 
    float groundHeight = args["--lightsaber_power"];
    int frightLevel = args["--nazghul_count"];
    //check that it is present if I don't want the default 0
    std::string youreAWizard = "I am not...";
    if(args["--spell"])
        youreAWizard = static_cast<std::string>(args["--spell"]);
    
    std::cout   << (areYouHere ? "I am here" : "I am not here") << std::endl
                << "I am " << groundHeight << " m heigh!" << std::endl
                << "I am scared " << frightLevel << " times!" << std::endl
                << youreAWizard << std::endl;
}
