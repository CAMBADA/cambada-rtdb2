#include "../RtDB2.h"
// System dependencies
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

// Structures directories
#include "../example_rtdb2_item.h"

#define RTDB2_GENERATOR_PATH    "/tmp/rtdb2_gen_storage"
#define RTDB2_BATCHS_PATH       "/tmp/rtdb2_gen_batchs"

using namespace boost::program_options;
using namespace std;

void insert_all_structures(RtDB2*);

int main(int argc , char** argv) {
    if (boost::filesystem::exists(RTDB2_GENERATOR_PATH)) {
        boost::filesystem::remove_all(RTDB2_GENERATOR_PATH);
    }

    std::string path;
    std::string dictionary_path;
    int iterations;
    std::stringstream menu_message;
    menu_message << "This script requires an RtDB in order to work, it can be created" << std::endl;
    menu_message << "or specified an existing RtDB path." << std::endl << std::endl;
    menu_message << "Usage: " << argv[0] << " [options]" << std::endl;
    menu_message << "Possible options";
    options_description menu(menu_message.str());
    menu.add_options()
            ("help", "produce this message")
            ("path", value<std::string>(&path)->default_value(RTDB2_GENERATOR_PATH), "specify a existing rtdb2 path")
            ("output", value<std::string>(&dictionary_path)->default_value("../config/zstd_dictionary.dic"), "specify a existing zstd dictionary path")
            ("train_size", value<int>(&iterations), "(required) specify the size (>0) of the dataset to train zstd");
    variables_map vm;
    store(parse_command_line(argc, argv, menu), vm);
    notify(vm);

    if (vm.count("help") || !vm.count("train_size") || iterations <= 0) {
        std::cout << menu << "\n";
        return 1;
    }

    bool new_storage = !boost::filesystem::exists(path);
    std::cout << "> Generating dictionary from " << path << "!" << std::endl;

    boost::shared_ptr<RtDB2> rtdb2 = boost::make_shared<RtDB2>(0, path);
    if (new_storage) {
        std::cout << "> Storage does not exist, inserting existing structures to it." << std::endl;
        insert_all_structures(rtdb2.get());
    }

    if (!boost::filesystem::exists("rtdb2_randomize_db")) {
        std::cout << "rtdb2_randomize_db does not exists! Check dictionary "
                "generator bash script inside rtdb2 libs!" << std::endl;
        return 1;
    }

    int batch_id;
    boost::filesystem::remove_all(RTDB2_BATCHS_PATH);
    boost::filesystem::create_directories(RTDB2_BATCHS_PATH);

    RtDB2BatchSettings batchSettings;
    batchSettings.compress = false;
    batchSettings.shared = true;

    int err;
    float progress = 0.0;
    int barWidth = 70;
    std::cout << "> Generating " << iterations << " batches in order to train zstd dictionary" << std::endl;

    for (batch_id = 0; batch_id < iterations; batch_id++) {
        progress = (float) batch_id / iterations;
        std::cout << "[";
        int pos = static_cast<int>(barWidth * progress);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout << std::flush;

        stringstream cmd_randomize;
        cmd_randomize << "./rtdb2_randomize_db ";
        cmd_randomize << path;
        const char* cmd_randomize_ = cmd_randomize.str().c_str();
        if ((err = system(cmd_randomize_)) != 0) {
            std::cout << "Failed to use rtdb2_randomize_db - Error code " << err << std::endl;
            return 2;
        }

        std::string batch_data;
        if (rtdb2->get_batch(batch_data, batchSettings, 0) != RTDB2_SUCCESS) {
            std::cout << "Failed to get batch of data from RtDB2" << std::endl;
            return 3;
        }

        stringstream output_file;
        output_file << RTDB2_BATCHS_PATH << "/batch" << batch_id;
        std::ofstream out(output_file.str().c_str());
        out << batch_data;
        out.close();
    }
    std::cout << "Done." << std::endl << "Training the dataset.." << std::flush;

    stringstream cmd_create_dict;
    cmd_create_dict << "zstd --train " << RTDB2_BATCHS_PATH << "/* -o " << dictionary_path;
    const char* cmd_create_dict_ = cmd_create_dict.str().c_str();

    if ((err = system(cmd_create_dict_)) != 0) {
        std::cout << "Failed to generate dictionary with the trained data" << std::endl;
        return 4;
    }

    boost::filesystem::remove_all(RTDB2_BATCHS_PATH);
    return 0;
}

void insert_all_structures(RtDB2* ptr_rtdb2) {
    ExampleItem example_item;
    ptr_rtdb2->put("EXAMPLE_ITEM", &example_item);
    ExampleItemMapped example_item_mapped;
    ptr_rtdb2->put("EXAMPLE_ITEM_MAPPED", &example_item_mapped);
    // Add more here...
}
