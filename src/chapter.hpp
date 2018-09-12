//
// Created by Killian on 05/09/18.
//

#ifndef DOWN_MANGATOWN_CHAPTER_HPP
#define DOWN_MANGATOWN_CHAPTER_HPP

#include <filesystem>


namespace down_mangatown {


class chapter
{
public:
    chapter(std::filesystem::path uri, std::string nme)
            : uri_(std::move(uri))
            , nme_(std::move(nme))
    {
    }
    
    inline const std::filesystem::path& get_uri() const noexcept
    {
        return uri_;
    }
    
    inline const std::string& get_name() const noexcept
    {
        return nme_;
    }

private:
    std::filesystem::path uri_;
    
    std::string nme_;
};


}


#endif
