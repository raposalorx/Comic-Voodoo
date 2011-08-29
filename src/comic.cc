#include "comic.h"

#include <pcrecpp.h>

#include <fstream>
#include "http.h"

#include "conversion.h"
#include "comic_globals.h"
#include "yaml-cpp/yaml.h"

using std::cout;
using std::endl;
using std::string;

void Comic::get_img(const char *mem, const string url)
{
    const string img = get_img_urls(mem);

    if(imgs.size() == 0 || (img!=imgs.back() && img!=""))
    {    // cut out the duplicate that happens with each respider and duplicates from reading the end_on url
        imgs.push_back(img);
        new_imgs.push(img);
        urls.push_back(url);
    }
    else
    {
        cout << "skipped" << endl
            << "imgs.size = " << imgs.size() << endl
            << "img = " << img << endl
            << "imgs.back = " << imgs.back() << endl;
    }

    //DEBUG
    printf("%s\n", img.c_str());
    fflush(stdout);
}

string Comic::get_img_urls(const string html)
{
    string img;
    string value;
    string found;
    string base;
    string swap = "!";
    pcrecpp::StringPiece input(html);

    while(swap != found)
    {    
        swap = found;
        img_regex.FindAndConsume(&input, &found);
        if(found == swap)
            break;

        if(found[0] == '/')
            base = base_url;
        else
        {
            base = last_url;
            base = base.substr(0, base.find_last_of('/')+1);
        }

        // not global
        if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
            value = base;
        else
            value = "";
        value += found;

        // sepparate images with ;
        img += value += ";";
    }

    return img;
}

string Comic::get_next(char *mem, string url)
{    
    const string html = mem;
    string found;
    string base;
    
    next_regex.PartialMatch(html, &found);
    
    if(found == "")
        return "";
    
    if(found[0] != '/')
    {
        base = url;
        base = base.substr(0, base.find_last_of('/')+1);
    }
    else
        base = base_url;

    // found url is the same as the current url, with or without the #
    if
    (
        (
            (found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.") 
            &&
            (
                (!strcmp(strcomb(2, base.c_str(), found.c_str()) , url.c_str()))
                || 
                (!strcmp(strcomb(2, base.c_str(), found.c_str()) , strcomb(2, url.c_str(), "#")))
            )
        )
        ||
        (
            (found.substr(0, 7) == "http://" || found.substr(0, 4) == "www.") 
            &&
            (
                (!strcmp(found.c_str() , url.c_str()))
                || 
                (!strcmp(found.c_str() , strcomb(2, url.c_str(), "#")))
            )
        )
    )
        return "";
    
    // found url is the end_on_url
    if
    ( 
        ( 
            (found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.") 
            && 
            (!strcmp(strcomb(2, base.c_str(), found.c_str()), end_on_url.c_str())) 
        )
        ||
        (
            (found.substr(0, 7) == "http://" || found.substr(0, 4) == "www.") 
            && 
            (!strcmp(found.c_str(), end_on_url.c_str()))
        )
    )
    {        
        // you want to download the img on end_on_url?
        if(read_end_url)
        {            
            string url_swap2 = url;
            
            // not global
            if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
                url = base;
            else
                url = "";

            url += found;
            
            HTTP page;
            get_http(page, url);
            if(page.mem == NULL)
                return "";
            
            // img on end_on_url
            get_img(page.mem, url);
            
            // keep the url before end_on_url
            url_swap = url_swap2;
            
            last_url = url_swap2;
            
            free(page.mem);
            page.mem=NULL;
            page.size = 0;
        }
        return "";
    }
    
    // not global
    if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
        url = base;
    else
        url = "";

    url += found;
    
    last_url = url;
    is_new_imgs = true;

    //DEBUG    
    printf("%s\n", url.c_str());
    fflush(stdout);
    
    return url;
}

void Comic::Spider()
{
    cout << "spidering " << name << endl;
    bool done = false;
    HTTP page;
    string next = last_url;
    
    get_http(page, next);
    while(!done && page.mem != NULL)
    {
        get_img(page.mem, next);
        
        next = get_next(page.mem, next);
        if(next == "")
            done = true;
        else
            get_http(page, next);
    }
    cout << download_imgs << endl; //DEBUG
    while(is_new_imgs && !new_imgs.empty() && download_imgs)
        download_img();
    saveImgFile();
    saveUrlFile();
    saveSettingsFile();
}

void Comic::download_img()
{
    const string folder = strcomb(2, getenv("HOME"), "/.comics");
    
    HTTP page;
    std::vector<string> img_urls;
    StringExplode(new_imgs.front(), ";", &img_urls);
    new_imgs.pop();
    unsigned int i = last_img;

    for(unsigned int j = 0; j < img_urls.size(); j++)
    {
        string n = itoa(i);
        // pad n with 0's till it is of length 4 ex: 0001
        n.insert(0, 4-n.size(), '0');

        // add a suffix based on the sub img counter ex: 0001b
        string suffix = "";
        if(j > 0)
            suffix.push_back('a'+j-1);
        if(chdir("/tmp/comics"))
            system("mkdir -p /tmp/comics");
        const string tmpfile = strcomb(7, "/tmp/comics/",  n.c_str(), suffix.c_str(), img_urls.at(j).substr(img_urls.at(j).find_last_of('.')).c_str());
        const string filename = strcomb(7, folder.c_str(), "/comics/", name.c_str(), "/",  n.c_str(), suffix.c_str(), ".png");

        if(!FileExists(filename))
        {
            get_http(page, img_urls.at(j));
            
            std::fstream fout(tmpfile.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
            fout.write((char *)page.mem, page.size);
            fout.close();

            system(strcomb(0, "convert ", tmpfile.c_str(), " \"", filename.c_str(), "\""));

            printf("%s -> %s\n", img_urls.at(j).c_str(), filename.c_str());
            fflush(stdout);

            free(page.mem);
            page.mem=NULL;
            page.size = 0;
        }
    }
    last_img++;
}

void Comic::saveImgFile()
{
    char* imgsFile = strcomb(4, folder.c_str(), "/comics/", name.c_str(), "/imgs.yaml");
    std::fstream comics_file(imgsFile, std::ios::out|std::ios::trunc);
    for(unsigned int i = 0; i < imgs.size(); i++)
    {
//        string s = strcomb(3, "- ", imgs.at(i).c_str(), "\n");
        comics_file.write("- ", strlen("- "));
        comics_file.write(imgs.at(i).c_str(), imgs.at(i).size());
        comics_file.write("\n", strlen("\n"));
    }
    comics_file.close();
    free(imgsFile);
}

void Comic::saveUrlFile()
{
    char* urlsFile = strcomb(4, folder.c_str(), "/comics/", name.c_str(), "/urls.yaml");
    std::fstream comics_file(urlsFile, std::ios::out|std::ios::trunc);
    for(unsigned int i = 0; i < urls.size(); i++)
    {
//        string s = strcomb(3, "- ", urls.at(i).c_str(), "\n");
        comics_file.write("- ", strlen("- "));
        comics_file.write(urls.at(i).c_str(), urls.at(i).size());
        comics_file.write("\n", strlen("\n"));
    }
    comics_file.close();
    free(urlsFile);
}

void Comic::saveSettingsFile()
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "last_url";
    out << YAML::Value << last_url;
    out << YAML::Key << "download_urls";
    out << YAML::Value << download_imgs;
    out << YAML::Key << "mark";
    out << YAML::Value << mark;
    out << YAML::EndMap;
    
    char* settingsFile = strcomb(4, folder.c_str(), "/comics/", name.c_str(), "/settings.yaml");
    std::fstream fout(settingsFile, std::ios::out|std::ios::trunc);
    free(settingsFile);
    // cout << "saving: " << strcomb(4, folder.c_str(), "/comics/", comic.name.c_str(), "/settings.yaml") << endl;
    fout.write(out.c_str(), out.size());
    fout.close();
}

void Comic::load()
{
    // load config files

    try 
    {
        char* configFile = strcomb(4, folder.c_str(), "/config/", name.c_str(), ".yaml");
        ifstream fin(configFile);
        YAML::Parser parser(fin);

        YAML::Node doc;    // already parsed
        parser.GetNextDocument(doc);
        if(const YAML::Node *pName = doc.FindValue("base_url")) // mandatory
            *pName >> base_url;
        if(const YAML::Node *pName = doc.FindValue("first_url")) // mandatory
            *pName >> first_url;
        else
            comics.at(i).base_url = "";
        if(const YAML::Node *pName = doc.FindValue("img_regex")) // mandatory
        {
            string img_regex;
            *pName >> img_regex;
            img_regex = img_regex;
        }
        else
            base_url = "";
        if(const YAML::Node *pName = doc.FindValue("next_regex")) // mandatory
        {
            string next_regex;
            *pName >> next_regex;
            next_regex = next_regex;
        }
        else
            base_url = "";
        if(const YAML::Node *pName = doc.FindValue("end_on_url"))
            *pName >> end_on_url;
        if(const YAML::Node *pName = doc.FindValue("read_end_url"))
            *pName >> read_end_url;
        free(configFile);
    } 
    catch(YAML::ParserException& e)
    {
        cout << name.c_str() << ".yaml" << e.what() << "\n";
    }


    // load settings, urls and imgs files
    char* settingsFile = strcomb(4, folder.c_str(), "/comics/", name.c_str(), "/settings.yaml");
    try 
    {
        ifstream fin(settingsFile);
        YAML::Parser parser(fin);

        YAML::Node doc;    // already parsed
        parser.GetNextDocument(doc);
        if(const YAML::Node *pName = doc.FindValue("last_url"))
            *pName >> last_url;
        if(const YAML::Node *pName = doc.FindValue("download_imgs"))
            *pName >> download_imgs;
        if(const YAML::Node *pName = doc.FindValue("mark"))
            *pName >> mark;
    } 
    catch(YAML::ParserException& e)
    {
        cout << settingsFile << e.what() << "\n";
    }
    free(settingsFile);
    char* imgsFile = strcomb(4, folder.c_str(), "/comics/", name.c_str(), "/imgs.yaml");
    try 
    {
        ifstream fin(imgsFile);
        YAML::Parser parser(fin);

        YAML::Node doc;    // already parsed
        parser.GetNextDocument(doc);
        for(unsigned ii=0;ii<doc.size();ii++) 
        {
            string url;
            doc[ii] >> url;
            imgs.push_back(url);
        }
    } 
    catch(YAML::ParserException& e)
    {
        cout << imgsFile << e.what() << "\n";mygetch();    
    }
    last_img = imgs.size();
    free(imgsFile);
    char* urlsFile = strcomb(4, folder.c_str(), "/comics/", name.c_str(), "/urls.yaml");
    try 
    {
        ifstream fin(urlsFile);
        YAML::Parser parser(fin);

        YAML::Node doc;    // already parsed
        parser.GetNextDocument(doc);
        for(unsigned ii=0;ii<doc.size();ii++) 
        {
            string url;
            doc[ii] >> url;
            urls.push_back(url);
        }
    } 
    catch(YAML::ParserException& e)
    {
        cout << urlsFile  << e.what() << "\n";mygetch();    
    }
    free(urlsFile);

    // set last_url if it wasn't already
    if(last_url == "")
        last_url = first_url;

}
