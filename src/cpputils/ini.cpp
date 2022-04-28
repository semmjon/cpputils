#include "ini.h"

#include <utility>

namespace ini{

    // Parser Config (User-Input)
    struct ParserConfig {
        map<std::string, std::vector<std::string> >files;
        map<std::string, std::vector<std::string> >sections;
        map<std::string, std::vector<std::string> >keys;
        map<std::string, std::vector<py::object> >defaults;
        py::dict envir; // MAIN ENVIRONMENT for configs
        ParserConfig(
                map<std::string, std::vector<std::string> >t_files,
                map<std::string, std::vector<std::string> >t_sections,
                map<std::string, std::vector<std::string> >t_keys,
                map<std::string, std::vector<py::object> >t_defaults,
                py::dict t_envir) :
                files(std::move(t_files)),
                sections(std::move(t_sections)),
                keys(std::move(t_keys)),
                defaults(std::move(t_defaults)),
                envir(std::move(t_envir)){}

    };

    // Input File Data
    struct FileData{
        py::dict file_envir;    // File ENVIRONMENT for configs
        std::string contents; // content read into
        FileData(
                py::dict t_file_envir,
                std::string t_contents) :
                file_envir(std::move(t_file_envir)),
                contents(std::move(t_contents)) {}
    };

    // Section Meta Data
    struct SectionData{
        py::dict section_envir; // Section ENVIRONMENT for configs
        std::array<int, 2> section_cursor; // section id / idx-begin idx-end
        FileData m_FileData;
        SectionData(
                py::dict t_section_envir,
                std::array<int, 2> t_section_cursor,
                FileData t_FileData) :
                section_envir(std::move(t_section_envir)),
                section_cursor(t_section_cursor),
                m_FileData(std::move(t_FileData)) {}
    };

    // Parser Process Data
    struct ParserData{
        std::function<void(FileData data, ParserData m_ParserData)> ParseSections;
        std::function<void(SectionData data, ParserData m_ParserData)> ParseKeys;
        ParserConfig m_ParserConfig;
        ParserData(
                std::function<void(FileData data, ParserData m_ParserData)> t_ParseSections,
                std::function<void(SectionData data, ParserData m_ParserData)> t_ParseKeys,
                ParserConfig t_ParserConfig) :
                ParseSections(std::move(t_ParseSections)),
                ParseKeys(std::move(t_ParseKeys)),
                m_ParserConfig(std::move(t_ParserConfig)) {}
    };

    inline void ParseAllKeys(SectionData t_SectionData, const ParserData& t_ParserData) {

        std::string::const_iterator start_iter = t_SectionData.m_FileData.contents.begin();
        std::string::const_iterator end_iter = t_SectionData.m_FileData.contents.begin();

        std::advance(start_iter, t_SectionData.section_cursor[0]);
        std::advance(end_iter, t_SectionData.section_cursor[1]);
        start_iter = std::find(start_iter, end_iter, NEWLINE); // next line after section header

        std::string::const_iterator next_line_iter = std::find(start_iter, end_iter, NEWLINE);;
        std::string::const_iterator next_value_iter;

        // // split name and value for each line if not empty or comment
        while (true) {

            start_iter = next_line_iter + 1; // skip line

            next_line_iter = std::find(start_iter, end_iter, NEWLINE);
            next_line_iter = std::find(start_iter, next_line_iter, COMMENT_CHAR);
            next_value_iter = std::find(start_iter, next_line_iter, NEWVALUE);

            if (next_line_iter == end_iter) {
                break;
            }

            if(start_iter == next_value_iter)
                continue;

            std::string value = string_operations::trim(
                    std::string(next_value_iter, next_line_iter).erase(0, 1));
            if(!value.empty())
                t_SectionData.section_envir[py::cast(string_operations::trim(
                        std::string(start_iter, next_value_iter)))] =
                        string_operations::eval_type(value);
        }
    }

    inline void ParseDefinedKeys(SectionData t_SectionData, const ParserData& t_ParserData) {

        for (const auto& item : t_ParserData.m_ParserConfig.keys) {

            for (auto item_value : item.second) {

                std::string::const_iterator start_iter = t_SectionData.m_FileData.contents.begin();
                std::string::const_iterator end_iter = t_SectionData.m_FileData.contents.begin();

                std::advance(start_iter, t_SectionData.section_cursor[0]);
                std::advance(end_iter, t_SectionData.section_cursor[1]);

                std::string::const_iterator line_iter;
                std::string::const_iterator value_iter = std::find(start_iter, end_iter, NEWLINE); // next line after section header

                while(true){

                    // get value
                    start_iter =
                            std::search(value_iter, end_iter,
                                    // std::boyer_moore_horspool_searcher(
                                        item_value.begin(),
                                        item_value.end()
                                    //)
                            );

                    if (start_iter == end_iter) {
                        char* env_value = std::getenv(item_value.c_str());
                        if(env_value==nullptr){
                            std::string upper_env = item_value;
                            std::transform(upper_env.begin(),
                                           upper_env.end(), upper_env.begin(),
                                           ::toupper);
                            env_value = std::getenv(upper_env.c_str());
                        }
                        if(env_value!=nullptr){
                            t_SectionData.section_envir[py::cast(item.first)] = string_operations::eval_type(env_value);
                            break;
                        }
                        if(!t_ParserData.m_ParserConfig.defaults.empty()) {
                            auto default_value = t_ParserData.m_ParserConfig.defaults.find(item.first);
                            if(default_value!=t_ParserData.m_ParserConfig.defaults.end()){
                                if(!default_value->second.empty()){
                                    t_SectionData.section_envir[py::cast(item.first)] = default_value->second[0];
                                    break;
                                }
                                t_SectionData.section_envir[py::cast(item.first)] = py::none();
                            }
                        }
                        break;
                    }

                    line_iter = std::find(start_iter + 1, end_iter, NEWLINE);
                    value_iter = std::find(start_iter + 1, line_iter, NEWVALUE);

                    // get total key name if only prefix
                    while(true){
                        if( std::string(start_iter-1, start_iter) == &NEWLINE) break;
                        start_iter--;
                    }

                    std::string value = string_operations::trim(std::string(value_iter, line_iter).erase(0, 1));

                    if(!value.empty()){
                        t_SectionData.section_envir[py::cast(item.first)] = string_operations::eval_type(value);
                        break;
                    }

                    start_iter+=1;
                }
            }
        }
    }

    inline void ParseSectionsDefault(FileData t_FileData, const ParserData& t_ParserData, py::dict section_envir) {
        std::array<int, 2> section_cursor{};
        t_FileData.contents.insert(0, 1, NEWLINE);
        section_cursor[0] = 0;
        section_cursor[1] = (int) t_FileData.contents.size();
        // parse all keys in all sections for config without section
        t_ParserData.ParseKeys(
                SectionData(
                        std::move(section_envir), // Section Environment
                        section_cursor, // Cursor for content
                        t_FileData // Parent ini data
                ), t_ParserData);
    }

    // parse all sections
    inline void ParseAllSections(FileData t_FileData, const ParserData& t_ParserData) {

        std::array<int, 2> section_cursor{};
        section_cursor[1] =
                (int) t_FileData.contents.find_first_of(SECTION_OPEN_CHAR, 0);

        if(section_cursor[1] == std::string::npos) {
            py::dict section_envir = t_FileData.file_envir;
            ParseSectionsDefault(t_FileData, t_ParserData, section_envir);
            return;
        }

        section_cursor[1] += 1;

        // split name and value for each line if not empty or comment
        while (true) {

            section_cursor[0] = (int) t_FileData.contents.find_first_of(
                    SECTION_CLOSE_CHAR, section_cursor[1]);

            std::string section_name = t_FileData.contents.substr(
                    section_cursor[1],
                    section_cursor[0] - section_cursor[1]);

            section_cursor[1] =
                    (int) t_FileData.contents.find_first_of(SECTION_OPEN_CHAR,
                                                            section_cursor[0]) + 1;

            // std::ptrdiff_t section_name_idx = std::find(config.sections.begin(),
            // config.sections.end(), section_name) - config.sections.begin();
            // if(section_name_idx >= config.sections.size()) continue;

            t_FileData.file_envir[py::cast(section_name)] = py::dict();

            if (!section_cursor[1]){
                section_cursor[1] = (int) t_FileData.contents.size() - 1;
                t_ParserData.ParseKeys(
                        SectionData(
                                t_FileData.file_envir[py::cast(section_name)], // Section Environment
                                section_cursor, // Cursor for content
                                t_FileData // Parent ini data
                        ), t_ParserData);
                break;
            }

            t_ParserData.ParseKeys(
                    SectionData(
                            t_FileData.file_envir[py::cast(section_name)], // Section Environment
                            section_cursor, // Cursor for content
                            t_FileData // Parent ini data
                    ), t_ParserData);
        }

    }

    inline void ParseDefinedSections(FileData t_FileData, const ParserData& t_ParserData) {

        for (const auto& item : t_ParserData.m_ParserConfig.sections) {

            if(t_ParserData.m_ParserConfig.sections.size() == 1 && string_operations::is_nan(item.first)){
                py::dict section_envir = t_FileData.file_envir;
                ParseSectionsDefault(t_FileData, t_ParserData, section_envir);
                continue;
            }

            for(const auto& item_value : item.second){

                t_FileData.file_envir[py::cast(item.first)] =
                        py::dict();

                // config.begin_pattern is section name + pattern
                std::array<int, 2> section_cursor = string_operations::idx_between(
                        t_FileData.contents.begin(), t_FileData.contents.end(),
                        SECTION_OPEN_CHAR + item_value + SECTION_CLOSE_CHAR,
                        BEGIN_PATTERN, 0);

                if (section_cursor == string_operations::empty_idx) {
                    // handling if section not found or has no section

                    if(string_operations::is_nan(item_value)) {
                        py::dict section_envir = t_FileData.file_envir[py::cast(item.first)];
                        ParseSectionsDefault(t_FileData, t_ParserData, section_envir);
                    }

                    continue;
                }

                section_cursor[0] += (int) item_value.size() + 1;
                section_cursor[1] -= (int) BEGIN_PATTERN.size() - 1;

                // parse all keys
                t_ParserData.ParseKeys(
                        SectionData(
                                t_FileData.file_envir[py::cast(item.first)], // Section Environment
                                section_cursor, // Cursor for content
                                t_FileData // Parent ini data
                        ), t_ParserData);
            }
        }
    }

    inline void ParseAllFiles(const ParserData& t_ParserData){

        for (const auto& item : t_ParserData.m_ParserConfig.files) {

            py::dict file_envir = py::dict();

            if(t_ParserData.m_ParserConfig.files.size() == 1 && string_operations::is_nan(item.first)){
                file_envir = t_ParserData.m_ParserConfig.envir;
            } else {
                t_ParserData.m_ParserConfig.envir[py::cast(item.first)] = file_envir;
            }

            if(item.second.empty()){
                FileData m_FileData(file_envir,"");
                t_ParserData.ParseSections(m_FileData, t_ParserData);
            }

            for(std::string item_value : item.second) {
                item_value = string_operations::path_exanduser(item_value);
                if(!system_operations::file_exists(item_value)){
//                    py::module logger = py::module::import("logging");
//                    logger.attr("getLogger")("cpputils.ini_load").attr("warning")("skipping file '" + item_value + "', because not exists!");
                    continue;
                }

                FileData m_FileData(file_envir,
                                    system_operations::read_file(item_value, true));
                t_ParserData.ParseSections(m_FileData, t_ParserData);
            }
        }

        // load defaults if no file exists / providing
        if(t_ParserData.m_ParserConfig.envir.empty() &&  !t_ParserData.m_ParserConfig.defaults.empty()){
//            py::module logger = py::module::import("logging");
//            logger.attr("getLogger")("cpputils.ini_load").attr("warning")("0 files found, loading default values only.");
            py::dict file_envir = t_ParserData.m_ParserConfig.envir;
            FileData m_FileData(file_envir,"");
            t_ParserData.ParseSections(m_FileData, t_ParserData);
        }
    }

// This is a simple (lightweight) C++ function to parse ini file into python dict
// Environment(s).
    py::dict ini_load(const map<std::string, std::vector<std::string> >& files,
                      const map<std::string, std::vector<std::string> >& sections,
                      const map<std::string, std::vector<std::string> >& keys,
                      const map<std::string, std::vector<py::object> >& defaults
    ) {

        py::dict envir;

        // Controller
        switch (!sections.empty() * 1 + !keys.empty() * 2) {
            case 0:
            {
                ParseAllFiles(ParserData(
                        ParseAllSections,
                        ParseAllKeys,
                        ParserConfig(
                                files, // convert all StringVector to c++ vector of strings (keys + names)
                                {}, // sections && section_names
                                {}, // keys && key_names
                                defaults,
                                envir
                        )));
            } break;
            case 1:
            {
                ParseAllFiles(ParserData(
                        ParseDefinedSections,
                        ParseAllKeys,
                        ParserConfig(
                                files, // convert all StringVector to c++ vector of strings (keys + names)
                                sections, // sections
                                {}, // keys && key_names
                                defaults,
                                envir
                        )));
            } break;
            case 2:
            {
                ParseAllFiles(ParserData(
                        ParseAllSections,
                        ParseDefinedKeys,
                        ParserConfig(
                                files, // convert all StringVector to c++ vector of strings (keys + names)
                                {}, // sections && section_names
                                keys,
                                defaults,
                                envir
                        )));
            } break;
            case 3:
            {
                ParseAllFiles(ParserData(
                        ParseDefinedSections,
                        ParseDefinedKeys,
                        ParserConfig(
                                files, // convert all StringVector to c++ vector of strings (keys + names)
                                sections, // sections
                                keys,
                                defaults,
                                envir
                        )));
            } break;
        }

        return envir;
    }

}
