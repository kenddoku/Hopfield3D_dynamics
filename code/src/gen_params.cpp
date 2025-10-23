/*  
    This program generates wparams_config-XXXXXXX.txt files where 'XXXXXXX' is the identification number of a specific 
    configuration of the following parameters:

    1st row: nu (the order of fractional differential equations)
    2nd row: x0_, y0_, z0_ (the initial values of neurons)
    3rd row: w11, w12, w13
    4th row: w21, w22, w23
    5th row: w31, w32, w33
    ^-- these three rows hold the weights of a given Hopfield network 
    6th row: n_iter (the number of time-evol iterations)
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace fs = std::filesystem;

// v``` !!! ALWAYS keep it at 'false' state here for safety !!!
bool FULL_OVERWRITE = false; // Parameter which determines whether to overwrite all wparams_config-XXXXXXX.txt files at once
bool FULL_KEEP = false; // Parameter which determines whether to KEEP all pre-existing wparams_config-XXXXXXX.txt files (non-existing files still get created)

// Fetching the environment variable $PROJECT which is a path to the whole project
// Is represented as char* i.e. C-style string, but I instantly pass it as an argument to 
// save it as a fs::path type variable
fs::path PROJECT = fs::path( std::getenv("PROJECT") ); 

enum class StringCode
{
    yes,
    no,
    ALL,
    KEEP,
    unknown
};

StringCode hashString(const std::string& str)
{
    if(str == "yes") return StringCode::yes;
    else if(str == "no") return StringCode::no;
    else if(str == "ALL") return StringCode::ALL;
    else if (str == "KEEP") return StringCode::KEEP;
    return StringCode::unknown;
}

enum class ControlParamNameCode
{
    nu,
    x0_, y0_, z0_,
    w11, w12, w13,
    w21, w22, w23,
    w31, w32, w33,
    unknown
};

ControlParamNameCode hashControlParamName(const std::string& str)
{
    if(str == "nu") return ControlParamNameCode::nu;

    else if(str == "x0") return ControlParamNameCode::x0_;
    else if(str == "y0") return ControlParamNameCode::y0_;
    else if(str == "z0") return ControlParamNameCode::z0_;

    else if(str == "w11") return ControlParamNameCode::w11;
    else if(str == "w12") return ControlParamNameCode::w12;
    else if(str == "w13") return ControlParamNameCode::w13;

    else if(str == "w21") return ControlParamNameCode::w21;
    else if(str == "w22") return ControlParamNameCode::w22;
    else if(str == "w23") return ControlParamNameCode::w23;

    else if(str == "w31") return ControlParamNameCode::w31;
    else if(str == "w32") return ControlParamNameCode::w32;
    else if(str == "w33") return ControlParamNameCode::w33;

    return ControlParamNameCode::unknown;
}

struct Params 
{
    double nu;
    double x0_; double y0_; double z0_;
    double w11; double w12; double w13;
    double w21; double w22; double w23;
    double w31; double w32; double w33;

    int n_iter; // Does not count as a control parameter
};

void createFile(const std::string& filename, Params& wp, double* CONTROL_PARAM_PTR, const double p)
{
    *CONTROL_PARAM_PTR = p;

    std::ofstream file(filename);
    
    file << std::fixed << std::setprecision(3) << wp.nu << '\n';
    file << std::fixed << std::setprecision(3) << wp.x0_ << " " << wp.y0_ << " " << wp.z0_ << '\n';
    file << std::fixed << std::setprecision(3) << wp.w11 << " " << wp.w12 << " " << wp.w13 << '\n';
    file << std::fixed << std::setprecision(3) << wp.w21 << " " << wp.w22 << " " << wp.w23 << '\n';
    file << std::fixed << std::setprecision(3) << wp.w31 << " " << wp.w32 << " " << wp.w33 << '\n';
    file << wp.n_iter;

    file.close();
}

// CURRENTLY UNUSED FUNCTION 
// Function converting upper-case string to lower-case strings (does not affect numbers)
// std::string toLowerCase(const std::string& input)
// {
//     std::string result = input;  // copy input
//     std::transform(result.begin(), result.end(), result.begin(),
//                    [](unsigned char c){ return std::tolower(c); });
//     return result;
// }

// Checks if any wparams_config-XXXXXXX.txt files exist. If not, it sets last_config_id to "-1" (i.e. "no files detected")
void checkLastConfigID(const fs::path& PARAMS_DIR)
{
    fs::path wparams_dir = PARAMS_DIR / "wparams";
    std::vector<std::string> param_names {
        "nu",
        "x0", "y0", "z0",
        "w11", "w12", "w13",
        "w21", "w22", "w23",
        "w31", "w32", "w33"
    };
    
    bool do_files_exist = false;

    for(std::string param_name : param_names)
    {
        fs::path current_dir = wparams_dir / param_name;
        if(!fs::is_empty(current_dir))
        {
            do_files_exist = true;
            break;
        }
    }

    if(do_files_exist == false)
    {
        fs::path last_config_id_path = PARAMS_DIR / "configs" / "last_config_id.txt"; // file holding IDs (from-to) of latest checked configurations
        std::ofstream last_config_id_file(last_config_id_path);

        last_config_id_file << "-1\n"; // last_config_id = -1 means that wparams_config-XXXXXXX.txt files don't exist
        last_config_id_file << "-1";
        last_config_id_file.close();

        fs::path config_id_list_path = PARAMS_DIR / "configs" / "config_id_list.txt"; // file holding all sets of configuration IDs that were created
        std::ofstream config_id_list_file(config_id_list_path, std::ios::trunc); // removing file's content (it does not exist, it gets created)
        config_id_list_file.close();
    }
}

void saveConfigFile(const fs::path& PARAMS_DIR, const int last_config_id, const int N_CONFIG_SETS, const std::string& CONTROL_PARAM_NAME)
{
    std::ostringstream oss;
    oss << "configs/" << CONTROL_PARAM_NAME << "/config-" << std::setw(7) << std::setfill('0') << (last_config_id + 1)
        << "-" << std::setw(7) << std::setfill('0') << (last_config_id + N_CONFIG_SETS) << ".sh";
    fs::path source = PROJECT / "CONFIG.sh";
    fs::path destination = PARAMS_DIR / oss.str();

    try {
        fs::copy_file(source, destination);
        std::cout << "CONFIG file saved successfully.\n";
    } catch(fs::filesystem_error& e) {
        std::cerr << "ERROR while saving CONFIG file: " << e.what() << '\n';
    }
}

int main(int argc, char* argv[])
{
    const int scaling_factor = 1000; // Scaling factor by which control parameter is multiplied to safely cast it to an int
    fs::path PARAMS_DIR = argv[1]; // The directory where folders with parameters are stored
    checkLastConfigID(PARAMS_DIR); // Ensures that last_config_id is set to -1 if no wparams_config-XXXXXXX.txt files exist

    const int N_CONFIG_SETS = std::stoi(argv[2]);
    // Casting control parameter to an int to avoid double type arithmetic errors while iterating
    std::string CONTROL_PARAM_NAME = argv[3];
    int CONTROL_PARAM_MIN = static_cast<int>( std::round( std::stod(argv[4])*scaling_factor ) );
    int CONTROL_PARAM_MAX = static_cast<int>( std::round( std::stod(argv[5])*scaling_factor ) );
    int CONTROL_PARAM_STEP = static_cast<int>( std::round( std::stod(argv[6])*scaling_factor ) );

    Params wp; // Creating a struct object holding all parameters

    wp.nu = std::stod(argv[7]);

    wp.x0_ = std::stod(argv[8]); wp.y0_ = std::stod(argv[9]); wp.z0_ = std::stod(argv[10]);

    wp.w11 = std::stod(argv[11]); wp.w12 = std::stod(argv[12]); wp.w13 = std::stod(argv[13]);
    wp.w21 = std::stod(argv[14]); wp.w22 = std::stod(argv[15]); wp.w23 = std::stod(argv[16]);
    wp.w31 = std::stod(argv[17]); wp.w32 = std::stod(argv[18]); wp.w33 = std::stod(argv[19]);

    wp.n_iter = std::stoi(argv[20]);

    double* CONTROL_PARAM_PTR; // Pointer to a control parameter. It makes it possible to change the value of the parameter specified in CONFIG.sh file

    switch(hashControlParamName(CONTROL_PARAM_NAME))
    {
        case ControlParamNameCode::nu:
            CONTROL_PARAM_PTR = &wp.nu;
            break;

        case ControlParamNameCode::x0_:
            CONTROL_PARAM_PTR = &wp.x0_;
            break;
        case ControlParamNameCode::y0_:
            CONTROL_PARAM_PTR = &wp.y0_;
            break;
        case ControlParamNameCode::z0_:
            CONTROL_PARAM_PTR = &wp.z0_;
            break;

        case ControlParamNameCode::w11:
            CONTROL_PARAM_PTR = &wp.w11;
            break;
        case ControlParamNameCode::w12:
            CONTROL_PARAM_PTR = &wp.w12;
            break;
        case ControlParamNameCode::w13:
            CONTROL_PARAM_PTR = &wp.w13;
            break;
        case ControlParamNameCode::w21:
            CONTROL_PARAM_PTR = &wp.w21;
            break;
        case ControlParamNameCode::w22:
            CONTROL_PARAM_PTR = &wp.w22;
            break;
        case ControlParamNameCode::w23:
            CONTROL_PARAM_PTR = &wp.w23;
            break;
        case ControlParamNameCode::w31:
            CONTROL_PARAM_PTR = &wp.w31;
            break;
        case ControlParamNameCode::w32:
            CONTROL_PARAM_PTR = &wp.w32;
            break;
        case ControlParamNameCode::w33:
            CONTROL_PARAM_PTR = &wp.w33;
            break;
        
        case ControlParamNameCode::unknown:
            std::cerr << "\033[31mWRONG 'CONTROL_PARAM_NAME': \033[0" << CONTROL_PARAM_NAME << std::endl;
            std::cerr << "\033[31mFAILED TO CREATE PARAMETER FILES\033[0" << std::endl;
            return 1;
    }

    if(FULL_OVERWRITE == true)
    {
        FULL_OVERWRITE = false;
        std::cout << "FULL_OVERWRITE was set 'true' manually. This might cause accidental deletion of all already existing wparams_config-XXXXXXX.txt files.\n";
        std::cout << "Please consider changing it back to 'false'.\n";
        std::cout << "NO FILE WAS OVERWRITTEN YET.\n\n";
    }

    // Looking up the last config ID (second line in the .txt file) to create proper wparams_config-XXXXXXX.txt names
    int last_config_id;
    fs::path last_config_id_path = PARAMS_DIR / "configs/last_config_id.txt";
    if(fs::exists(last_config_id_path))
    {
        std::ifstream last_config_id_file(last_config_id_path);
        if(!last_config_id_file.is_open())
        {
            std::cerr << "ERROR opening " << last_config_id_path << '\n';
            return 1;
        }
        
        std::string line;
        std::getline(last_config_id_file, line); // Skipping the first line as it is the old 'last_config_id'
        std::getline(last_config_id_file, line);
        last_config_id = std::stoi(line);
        last_config_id_file.close();
    }
    else
    {
        std::cerr << "File " << last_config_id_path << " does not exist!" << '\n';
        return 1;
    }

    // Saving the CONFIG.sh file with which the wparams_config-XXXXXXX.txt files are generated
    saveConfigFile(PARAMS_DIR, last_config_id, N_CONFIG_SETS, CONTROL_PARAM_NAME);

    // Saving the old and the new value of last_config_id to the last_config_id.txt file
    if(fs::exists(last_config_id_path))
    {
        std::ofstream last_config_id_file(last_config_id_path);
        last_config_id_file << ( last_config_id + 1 ) << '\n';
        last_config_id_file << ( last_config_id + N_CONFIG_SETS );
        last_config_id_file.close();
    }

    fs::path config_id_list_path = PARAMS_DIR / "configs" / "config_id_list.txt";
    if(fs::exists(config_id_list_path))
    {
        std::ofstream config_id_list_file(config_id_list_path, std::ios::app); // opening config_id_list_file to append the latest config IDs
        config_id_list_file << ( last_config_id + 1 ) << " " << ( last_config_id + N_CONFIG_SETS ) << '\n';
        config_id_list_file.close();
    }

    for(int k=0; k<N_CONFIG_SETS; ++k)
    {
        last_config_id++;

        double p = (double)(CONTROL_PARAM_MIN + k*CONTROL_PARAM_STEP) / scaling_factor; // This does not care about the chosen control parameter's value set manually in CONFIG.sh, it sets the control parameter to CONTROL_PARAM_MIN and increment it based on CONTROL_PARAM_STEP

        std::ostringstream oss;
        oss << "wparams_config-" << std::setw(7) << std::setfill('0') << last_config_id << ".txt"; // Path to a file holding parameters
        fs::path filePath = PARAMS_DIR / "wparams" / CONTROL_PARAM_NAME / oss.str();

        if(fs::exists(filePath) && FULL_OVERWRITE == false)
        {
            if(FULL_KEEP == true) continue; // This line ensures that all pre-existing files would not get overwritten if the user specified it

            std::cout << "File " << filePath << " already exists.\n";
            std::cout << "Do you want to OVERWRITE it? Type 'yes/no' (type 'ALL/KEEP' to OVERWRITE/KEEP all already existing wparams_config-XXXXXXX.txt files).\n";
            
            while(true)
            {
                std::string to_overwrite;
                std::cin >> to_overwrite;

                switch(hashString(to_overwrite))
                {
                    case StringCode::yes:
                        createFile(filePath, wp, CONTROL_PARAM_PTR, p);
                        break;
                    case StringCode::no:
                        break;
                    case StringCode::ALL:
                        std::cout << "ARE YOU SURE YOU WANT TO OVERWRITE ALL EXISTING wparams_config-XXXXXXX.txt FILES?\n";
                        std::cout << "Type 'yes/no' to confirm:\n";

                        while(true)
                        {
                            std::string confirm;
                            std::cin >> confirm;

                            switch(hashString(confirm))
                            {
                                case StringCode::yes:
                                    FULL_OVERWRITE = true;
                                    createFile(filePath, wp, CONTROL_PARAM_PTR, p);
                                    std::cout << "ALL pre-existing files will be overwritten\n";
                                    break;
                                case StringCode::no:
                                    std::cout << "Overwriting of ALL files was canceled. Your files are safe.\n";
                                    break;
                                case StringCode::ALL:
                                    std::cout << "Invalid input. Please type 'yes/no' to continue.\n";
                                    continue;
                                case StringCode::KEEP:
                                    std::cout << "Invalid input. Please type 'yes/no' to continue.\n";
                                    continue;
                                case StringCode::unknown:
                                    std::cout << "Invalid input. Please type 'yes/no' to continue.\n";
                                    continue;
                            }
                            break;
                        }
                        break;
                    case StringCode::KEEP:
                        FULL_KEEP = true;
                        break;
                    case StringCode::unknown:
                        std::cout << "Invalid input. Please type 'yes/no' or 'ALL' to continue.\n";
                        continue;
                }
                break;
            }
        }
        else if(fs::exists(filePath) == false || FULL_OVERWRITE == true)
            createFile(filePath, wp, CONTROL_PARAM_PTR, p);
    }
    
    return 0;
}
