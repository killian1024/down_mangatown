//
// Created by Killian on 05/09/18.
//

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "program.hpp"


namespace down_mangatown {


program::program(
        std::filesystem::path bse_uri,
        std::filesystem::path dest_pth,
        std::string manga_nme,
        std::string chap_cont
)
        : bse_uri_(std::move(bse_uri))
        , dest_pth_(std::move(dest_pth))
        , manga_nme_(std::move(manga_nme))
        , chap_cont_(std::move(chap_cont))
{
    if (manga_nme_.empty())
    {
        for (std::filesystem::path nme_buildr = bse_uri_;
             !nme_buildr.empty();
             nme_buildr = nme_buildr.parent_path())
        {
            if (!nme_buildr.filename().empty())
            {
                manga_nme_ = nme_buildr.filename();
                break;
            }
        }
        
        if (manga_nme_.empty())
        {
            manga_nme_ = "Unknown";
        }
    }
}


program::~program()
{
    std::cout << spdios::set_default_text;
}


int program::execute()
{
    std::list<chapter> chap_lst;
    bool cont = !chap_cont_.empty();
    
    std::cout << spdios::set_light_blue_text << "Downloading chapters data...";
    if (!download_charpters_data(&chap_lst))
    {
        std::cout << spdios::set_light_red_text << "[fail]" << spdios::newl;
        return -1;
    }
    std::cout << spdios::set_light_green_text << "[ok]" << spdios::newl;
    
    for (auto& x : chap_lst)
    {
        if (cont)
        {
            if (x.get_name() != chap_cont_)
            {
                continue;
            }
            
            cont = false;
        }
        
        std::cout << spdios::set_light_blue_text
                  << "Downloading chapter " << x.get_name()
                  << spdios::newl;
        if (!download_chapter(x))
        {
            return -1;
        }
    }
    
    return 0;
}


bool program::download_charpters_data(std::list<chapter>* chap_lst) const
{
    std::filesystem::path dest_pth;
    htmlDocPtr html_parsr;
    xmlNode* root_nde;
    xmlNode* cur_nde;
    xmlNode* dat_nde;
    const char* chap_raw_uri;
    std::filesystem::path chap_uri;
    std::string chap_nme;
    std::regex chap_nme_rgx(R"([0-9]+)");
    std::smatch chap_nme_match;
    std::string chap_nme_match_suffix;
    std::size_t chap_nr;
    std::size_t max_digs = 0;
    std::stringstream sstream;
    sstream.fill('0');
    
    // Download the index page.
    dest_pth = spdsys::get_tmp_path();
    dest_pth /= "down-mangatown-index";
    dest_pth += std::to_string(spdsys::get_pid());
    if (!http_get(bse_uri_, dest_pth))
    {
        return false;
    }
    
    // Initialize html parser.
    html_parsr = htmlReadFile(dest_pth.c_str(), nullptr, HTML_PARSE_NOBLANKS |
                              HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    if (html_parsr == nullptr)
    {
        remove(dest_pth.c_str());
        return false;
    }
    root_nde = xmlDocGetRootElement(html_parsr);
    if (root_nde == nullptr)
    {
        xmlFreeDoc(html_parsr);
        remove(dest_pth.c_str());
        return false;
    }
    
    // Iterate all chapters data in the index page.
    for (cur_nde = search_tag("ul", root_nde, "class", "chapter_list");
         (cur_nde = search_tag("li", cur_nde)) != nullptr;
         cur_nde = cur_nde->next)
    {
        if ((dat_nde = search_tag("a", cur_nde)) == nullptr)
        {
            continue;
        }
    
        chap_raw_uri = (const char*)xmlGetProp(dat_nde, (const xmlChar*)"href");
        if (chap_raw_uri != nullptr && chap_raw_uri[0] == '/')
        {
            chap_raw_uri += 2;
        }
        chap_uri = chap_raw_uri;
        
        chap_nme = (const char*)xmlNodeGetContent(dat_nde);
    
        // Get the chapter number.
        while (std::regex_search(chap_nme, chap_nme_match, chap_nme_rgx))
        {
            chap_nme_match_suffix = chap_nme_match.suffix().str();
            if (!chap_nme_match_suffix.empty() &&
                (chap_nme_match_suffix[0] != ' ' && chap_nme_match_suffix[0] != '.'))
            {
                chap_nme = chap_nme_match_suffix;
                continue;
            }
            
            chap_nr = spdcast::type_cast<std::size_t>(chap_nme_match.str());
            
            if (spdscalars::get_n_digits(chap_nr) > max_digs)
            {
                max_digs = spdscalars::get_n_digits(chap_nr);
            }
            
            sstream.str("");
            sstream << std::setw(max_digs) << chap_nr;
            
            chap_nme = sstream.str();
            chap_nme += chap_nme_match.suffix().str();
            chap_nme.erase(remove_if(chap_nme.begin(), chap_nme.end(), isspace), chap_nme.end());
    
            chap_lst->push_front(chapter(std::move(chap_uri), std::move(chap_nme)));
            
            break;
        }
    }
    
    xmlFreeDoc(html_parsr);
    remove(dest_pth.c_str());
    
    return !chap_lst->empty();
}


bool program::download_chapter(const chapter& chap) const
{
    std::filesystem::path pag_uri = chap.get_uri();
    std::filesystem::path chap_pth = dest_pth_;
    
    chap_pth /= "Chapter ";
    chap_pth += chap.get_name();
    
    if (!std::filesystem::exists(chap_pth))
    {
        std::cout << spdios::set_light_blue_text << "Making directory " << chap_pth << "...";
        if (!spdsys::mkdir(chap_pth.c_str()))
        {
            std::cout << spdios::set_light_red_text << "[fail]" << spdios::newl;
            return false;
        }
        std::cout << spdios::set_light_green_text << "[ok]" << spdios::newl;
    }
    
    while (!pag_uri.empty())
    {
        std::cout << spdios::set_light_blue_text << "Downloading page in " << pag_uri << "...";
        if (!download_page(chap.get_name(), chap_pth, pag_uri, &pag_uri))
        {
            std::cout << spdios::set_light_red_text << "[fail]" << spdios::newl;
            return false;
        }
        std::cout << spdios::set_light_green_text << "[ok]" << spdios::newl;
    }
    
    return true;
}


bool program::download_page(
        const std::string& chap_nme,
        const std::filesystem::path& chap_pth,
        const std::filesystem::path& pag_uri,
        std::filesystem::path* nxt_pag_uri
) const
{
    std::filesystem::path page_dest_pth;
    htmlDocPtr html_parsr;
    xmlNode* root_nde;
    xmlNode* cur_nde;
    std::filesystem::path img_uri;
    std::filesystem::path img_dest_pth;
    const char* raw_nxt_pag_uri;
    bool scs = true;
    
    // Download the page.
    page_dest_pth = spdsys::get_tmp_path();
    page_dest_pth /= "down-mangatown-page";
    page_dest_pth += std::to_string(spdsys::get_pid());
    if (!http_get(pag_uri, page_dest_pth))
    {
        return false;
    }
    
    // Initialize html parser.
    html_parsr = htmlReadFile(page_dest_pth.c_str(), nullptr, HTML_PARSE_NOBLANKS |
                              HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    if (html_parsr == nullptr)
    {
        scs = false;
        goto remove;
    }
    root_nde = xmlDocGetRootElement(html_parsr);
    if (root_nde == nullptr)
    {
        scs = false;
        goto xmlFreeDoc;
    }
    
    // Find the image.
    if ((cur_nde = search_tag("img", root_nde, "id", "image")) == nullptr)
    {
        scs = false;
        goto xmlFreeDoc;
    }
    img_uri = (const char*)xmlGetProp(cur_nde, (const xmlChar*)"src");
    
    // Build image destination path.
    img_dest_pth = chap_pth;
    img_dest_pth /= manga_nme_;
    img_dest_pth += " - Chapter ";
    img_dest_pth += chap_nme;
    img_dest_pth += " - Page ";
    if ((cur_nde = search_tag("option", root_nde, "selected", "selected")) == nullptr)
    {
        scs = false;
        goto xmlFreeDoc;
    }
    img_dest_pth += (const char*)xmlNodeGetContent(cur_nde);
    
    // Download the image.
    if (!http_get(img_uri, img_dest_pth))
    {
        scs = false;
        goto xmlFreeDoc;
    }
    
    // Find the next page uri.
    if ((cur_nde = search_tag("a", root_nde, "onclick", "return next_page();")) == nullptr)
    {
        scs = false;
        goto xmlFreeDoc;
    }
    raw_nxt_pag_uri = (const char*)xmlGetProp(cur_nde, (const xmlChar*)"href");
    if (raw_nxt_pag_uri != nullptr && raw_nxt_pag_uri[0] == '/')
    {
        raw_nxt_pag_uri += 2;
    }
    if (spdstr::strcmp(raw_nxt_pag_uri, "javascript:void(0);") != 0)
    {
        *nxt_pag_uri = raw_nxt_pag_uri;
    }
    else
    {
        nxt_pag_uri->clear();
    }
    
xmlFreeDoc:
    xmlFreeDoc(html_parsr);
remove:
    remove(page_dest_pth.c_str());
    
    return scs;
}


bool program::http_get(
        const std::filesystem::path& uri,
        const std::filesystem::path& dest_pth
) const noexcept
{
    int ret_val;
    std::string cmd = "wget -q -O \"";
    cmd += dest_pth;
    cmd += "\" \"";
    cmd += uri;
    cmd += "\"";
    cmd.erase(remove_if(cmd.begin(), cmd.end(), [](int ch) { return ch == '\0'; }), cmd.end());
    
    if (spdsys::execute_command(cmd.c_str(), &ret_val))
    {
        return ret_val == 0;
    }
    
    return false;
}


xmlNode* program::search_tag(const char* tag_nme, xmlNode* html_nde) const noexcept
{
    for (xmlNode* cur_nde = html_nde; cur_nde != nullptr; )
    {
        if (cur_nde->type == XML_ELEMENT_NODE)
        {
            if (strcmp((const char*)cur_nde->name, tag_nme) == 0)
            {
                return cur_nde;
            }
        }
        
        if (cur_nde->children != nullptr)
        {
            cur_nde = cur_nde->children;
        }
        else if (cur_nde->next != nullptr)
        {
            cur_nde = cur_nde->next;
        }
        else if (cur_nde->parent != nullptr)
        {
            for (cur_nde = cur_nde->parent; cur_nde != nullptr; cur_nde = cur_nde->parent)
            {
                if (cur_nde->next != nullptr)
                {
                    cur_nde = cur_nde->next;
                }
            }
        }
        else
        {
            cur_nde = nullptr;
        }
    }
    
    return nullptr;
}


xmlNode* program::search_tag(
        const char* tag_nme,
        xmlNode* html_nde,
        const char* attr_nme,
        const char* val_nme
) const noexcept
{
    for (xmlNode* cur_nde = html_nde; cur_nde != nullptr; )
    {
        if (cur_nde->type == XML_ELEMENT_NODE)
        {
            if (strcmp((const char*)cur_nde->name, tag_nme) == 0 &&
                spdstr::strcmp((const char*)xmlGetProp(cur_nde, (const xmlChar*)attr_nme),
                               val_nme) == 0)
            {
                return cur_nde;
            }
        }
        
        if (cur_nde->children != nullptr)
        {
            cur_nde = cur_nde->children;
        }
        else if (cur_nde->next != nullptr)
        {
            cur_nde = cur_nde->next;
        }
        else if (cur_nde->parent != nullptr)
        {
            for (cur_nde = cur_nde->parent; cur_nde != nullptr; cur_nde = cur_nde->parent)
            {
                if (cur_nde->next != nullptr)
                {
                    cur_nde = cur_nde->next;
                    break;
                }
            }
        }
        else
        {
            cur_nde = nullptr;
        }
    }
    
    return nullptr;
}


xmlNode* program::next_absolute_tag(xmlNode* html_nde) const noexcept
{
    xmlNode* cur_nde;
    
    if (html_nde != nullptr)
    {
        if (html_nde->children != nullptr)
        {
            return html_nde->children;
        }
        else if (html_nde->next != nullptr)
        {
            return html_nde->next;
        }
        else if (html_nde->parent != nullptr)
        {
            for (cur_nde = html_nde->parent; cur_nde != nullptr; cur_nde = cur_nde->parent)
            {
                if (cur_nde->next != nullptr)
                {
                    return cur_nde->next;
                }
            }
        }
    }
    
    return nullptr;
}


xmlNode* program::previous_tag(xmlNode* html_nde) const noexcept
{
    xmlNode* cur_nde;
    
    if (html_nde != nullptr)
    {
        if (html_nde->prev != nullptr)
        {
            return html_nde->prev;
        }
        else if (html_nde->parent != nullptr)
        {
            return html_nde->parent;
        }
    }
    
    return nullptr;
}


}
