#include <speed/speed.hpp>
#include <speed/speed_alias.hpp>

#include "program.hpp"

namespace dmt = down_mangatown;


int main(int argc, char* argv[])
{
    spd::ap::arg_parser ap("down_mangatown", "Download manga scans using mangatown.com");
    ap.add_key_value_arg({"--manga-name", "-n"},
                         "Allows to specify the name of the manga to download. By default the manga"
                         " name is taken from the uri.");
    ap.add_key_value_arg({"--dest-path", "-d"},
                         "Allows to specify the destination path for the download. By default the "
                         "destination path is the current directory.",
                         {spd::ap::avt_t::RWX_DIR});
    ap.add_key_value_arg({"--chapter-continue", "-c"},
                         "Allows to specify the name of the chapter to continue a suspended "
                         "download.");
    ap.add_help_arg({"--help"}, "Display this help and exit.");
    ap.add_gplv3_version_arg({"--version"}, "Output version information and exit", "1.0.0", "2018",
                             "Killian Poulaud");
    ap.add_keyless_arg("URI", "URI", "", {spd::ap::avt_t::STRING}, 1, 1,
                       spd::ap::af_t::DEFAULT_KEYLESS_ARG_FLAGS, {".+"});
    ap.add_help_text("");
    ap.add_help_text("Examples:");
    ap.add_help_text(R"(  $ down_mangatown "www.mangatown.com/manga/hope/")");
    ap.add_help_text(R"(  $ down_mangatown "www.mangatown.com/manga/hope/" -n Hope)");
    ap.add_help_text(R"(  $ down_mangatown "www.mangatown.com/manga/hope/" -n Hope -d "./Hope")");
    ap.add_help_text(R"(  $ down_mangatown "www.mangatown.com/manga/hope/" -n Hope -d "./Hope" )"
                     "-c 72");
    ap.parse_args((unsigned int)argc, argv);
    
    dmt::program prog(ap.get_front_arg_value_as<std::filesystem::path>("URI"),
                      ap.get_front_arg_value_as<std::filesystem::path>("--dest-path", "./"),
                      ap.get_front_arg_value_as<std::string>("--manga-name", ""),
                      ap.get_front_arg_value_as<std::string>("--chapter-continue", ""));
    
    return prog.execute();
}