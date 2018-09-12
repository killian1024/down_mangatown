//
// Created by Killian on 05/09/18.
//

#ifndef DOWN_MANGATOWN_PROGRAM_HPP
#define DOWN_MANGATOWN_PROGRAM_HPP

#include <filesystem>
#include <list>

#include <libxml/HTMLparser.h>

#include <speed/speed.hpp>

#include "chapter.hpp"


namespace down_mangatown {


class program
{
public:
    program(
            std::filesystem::path bse_uri,
            std::filesystem::path dest_pth,
            std::string manga_nme,
            std::string chap_cont
    );
    
    ~program();
    
    int execute();

private:
    bool download_charpters_data(std::list<chapter>* chap_lst) const;
    
    bool download_chapter(const chapter& chap) const;
    
    bool download_page(
            const std::string& chap_nme,
            const std::filesystem::path& chap_pth,
            const std::filesystem::path& pag_uri,
            std::filesystem::path* nxt_pag_uri
    ) const;
    
    bool http_get(
            const std::filesystem::path& uri,
            const std::filesystem::path& dest_pth
    ) const noexcept;
    
    xmlNode* search_tag(const char* tag_nme, xmlNode* html_nde) const noexcept;
    
    xmlNode* search_tag(
            const char* tag_nme,
            xmlNode* html_nde,
            const char* attr_nme,
            const char* val_nme
    ) const noexcept;
    
    xmlNode* next_absolute_tag(xmlNode *html_nde) const noexcept;
    
    xmlNode* previous_tag(xmlNode* html_nde) const noexcept;
    
private:
    std::filesystem::path bse_uri_;
    
    std::filesystem::path dest_pth_;
    
    std::string manga_nme_;
    
    std::string chap_cont_;
};


}


#endif
