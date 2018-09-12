#include <speed/speed.hpp>

#include "program.hpp"


int main(int argc, char* argv[])
{
    spdap::arg_parser ap("down_mangatown");
    ap.add_help_text("Options:");
    ap.add_key_value_arg({"--uri", "-uri"}, "The chapter index page uri.",
                         {spdap::avt_t::STRING}, 1, 1,
                         spdap::af_t::ALLWAYS_REQUIRED | spdap::af_t::DEFAULT_ARG_FLAGS);
    ap.add_key_value_arg({"--dest-path", "-d"}, "The destination path for the download.",
                         {spdap::avt_t::RWX_DIR}, 1, 1,
                         spdap::af_t::ALLWAYS_REQUIRED | spdap::af_t::DEFAULT_ARG_FLAGS);
    ap.add_key_value_arg({"--manga-name", "-nm"}, "The name of the manga to download.",
                         {spdap::avt_t::STRING}, 1, 1,
                         spdap::af_t::ALLWAYS_REQUIRED | spdap::af_t::DEFAULT_ARG_FLAGS);
    ap.add_key_value_arg({"--chapter-continue", "-cc"},
                         "The name of the chapter to continue the download.");
    ap.add_help_arg({"--help"}, "Display this help and exit.");
    ap.add_gplv3_version_arg({"--version"}, "Output version information and exit", "1.0.0", "2018",
                             "Killian Poulaud");
    ap.add_help_text("\nExample:\n  $ down_mangatown -uri "
                     "\"www.mangatown.com/manga/hope/\" -d \"./Hope\" -nm Hope");
    ap.parse_args((unsigned int)argc, argv);
    
    down_mangatown::program prog(ap.get_front_arg_value_as<std::filesystem::path>("--uri"),
                                 ap.get_front_arg_value_as<std::filesystem::path>("--dest-path"),
                                 ap.get_front_arg_value_as<std::string>("--manga-name"),
                                 ap.get_front_arg_value_as<std::string>("--chapter-continue", ""));
    
    return prog.execute();
}