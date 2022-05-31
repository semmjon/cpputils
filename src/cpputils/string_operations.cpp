#include "string_operations.h"
#define strtk_no_tr1_or_boost
#include "datetime_utils.h"

namespace string_operations {

    py::object apply_match(const std::vector<std::string>& vec, std::string match){

        py::list result;

        for(auto value : vec){
            std::string::const_iterator start_iter = value.begin();
            std::string::const_iterator end_iter = value.end();
            std::string::const_iterator value_iter = start_iter;

            start_iter =
                    std::search(start_iter, end_iter,
                            // std::boyer_moore_horspool_searcher(
                                match.begin(),
                                match.end()
                            //)
                    );
            result.append(py::cast(start_iter != end_iter));
        }

        return result;

    }

    py::object extract_between(const std::string& data, std::string start, char end){

        py::list result;
        const int start_size = (int) start.size();

        std::string::const_iterator start_iter = data.begin();
        std::string::const_iterator end_iter = data.end();
        std::string::const_iterator value_iter = start_iter;

        while(true) {
            start_iter =
                    std::search(start_iter, end_iter,
                            // std::boyer_moore_horspool_searcher(
                                start.begin(),
                                start.end()
                            //)
                    );
            if(start_iter == end_iter){
                break;
            }
            start_iter += start_size;
            value_iter = std::find(start_iter + 1, end_iter, end);
            result.append(py::cast(std::string(start_iter, value_iter)));
        }

        return result;

    }

    template<class T_0, class T_1>
    static bool compare_type() {
        return (typeid(T_0) == typeid(T_1));
    }

    template<class T_in, class T_out>
    std::map<std::string, std::vector<T_out>> convert_to_map(const py::object& dictionary)
    {
        std::map<std::string, std::vector<T_out> > result;

        if(dictionary.is_none()){
            return result;
        }
        if(py::isinstance<py::dict>(dictionary)){
            for (std::pair<py::handle, py::handle> item : dictionary.cast<py::dict>())
            {
                if(item.first.is_none()){
                    item.first = py::str("None");
                }
                if(item.second.is_none()){
                    result[item.first.cast<std::string>()] = {};
                    continue;
                }
                if(py::isinstance<py::list>(item.second) &&
                        !compare_type<py::object, T_out>() &&
                        !compare_type<py::list, T_out>()) {
                    result[item.first.cast<std::string>()] = std::vector<T_out>();
                    for (auto item_value : item.second){
                        if(item_value.is_none()){
                            result[item.first.cast<std::string>()].push_back(py::str("None").cast<T_out>());
                            continue;
                        }
                        if(py::isinstance<T_in>(item_value)){
                            result[item.first.cast<std::string>()].push_back(item_value.cast<T_out>());
                        }
                    }
                    continue;
                }
//                item.second = py::str(item.second);
                if(py::isinstance<T_in>(item.second)){
                    result[item.first.cast<std::string>()] = {item.second.cast<T_out>()};
                }
            }
        }
        if(py::isinstance<py::list>(dictionary)){
            for (auto item : dictionary.cast<py::list>())
            {
                if(py::isinstance<T_in>(item)){
                    auto key_val = item.cast<std::string>();
                    result[key_val] = {item.cast<T_out>()};
                }
            }
        }
        if(py::isinstance<T_in>(dictionary)){
            result[dictionary.cast<py::str>()] = {dictionary.cast<T_out>()};
        }
        return result;
    }

    std::map<std::string, std::vector<std::string>> convert_to_map_str(const py::object& dictionary) {
        return convert_to_map<py::str, std::string>(dictionary);
    }

    std::map<std::string, std::vector<py::object>> convert_to_map_py(const py::object& dictionary) {
        return convert_to_map<py::object, py::object>(dictionary);
    }

    bool is_nan(std::string value){
        std::transform(value.begin(), value.end(), value.begin(), ::toupper);
        return std::find(NAN_STRINGS.begin(), NAN_STRINGS.end(), value) != NAN_STRINGS.end();
    }

    // get section index function
    std::array<int, 2> idx_between(
            std::string::const_iterator start_iter,
            std::string::const_iterator end_iter,
            const std::string& begin_pattern,
            const std::string& end_pattern,
            int skip = 0) {

        std::string::const_iterator skipped_iter = start_iter;
        std::advance(skipped_iter, skip);
        std::array<int, 2> idx{};

        // begin section index
        auto section_start_iter = std::search(skipped_iter, end_iter,
                // std::boyer_moore_horspool_searcher(
                                              begin_pattern.begin(), begin_pattern.end()
                //)
        );

        idx[0] = section_start_iter - start_iter;
        if(idx[0] < 0) return(empty_idx);

        // end section index;
        idx[1] = std::search(section_start_iter, end_iter,
                // std::boyer_moore_horspool_searcher(
                             end_pattern.begin(), end_pattern.end()
                //)
        ) - start_iter;

        if(idx[0] >= idx[1]) return(empty_idx);

        idx[1] += end_pattern.size();

        return (idx);
    }

    std::string trim(const std::string& str,
                     const std::string& whitespace)
    {
        const auto strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos)
            return ""; // no content

        const auto strEnd = str.find_last_not_of(whitespace);
        const auto strRange = strEnd - strBegin + 1;

        return str.substr(strBegin, strRange);
    }

    std::uint64_t parse8Chars(const char* string) noexcept
    {
        std::uint64_t chunk = 0;
        std::memcpy(&chunk, string, sizeof(chunk));

        // 1-byte mask trick (works on 4 pairs of single digits)
        std::uint64_t lower_digits = (chunk & 0x0f000f000f000f00) >> 8;
        std::uint64_t upper_digits = (chunk & 0x000f000f000f000f) * 10;
        chunk = lower_digits + upper_digits;

        // 2-byte mask trick (works on 2 pairs of two digits)
        lower_digits = (chunk & 0x00ff000000ff0000) >> 16;
        upper_digits = (chunk & 0x000000ff000000ff) * 100;
        chunk = lower_digits + upper_digits;

        // 4-byte mask trick (works on pair of four digits)
        lower_digits = (chunk & 0x0000ffff00000000) >> 32;
        upper_digits = (chunk & 0x000000000000ffff) * 10000;
        chunk = lower_digits + upper_digits;

        return chunk;
    }

    std::uint64_t parse64(std::string_view s) noexcept
    {
        std::uint64_t upper_digits = parse8Chars(s.data());
        std::uint64_t lower_digits = parse8Chars(s.data() + 8);
        return upper_digits * 100000000 + lower_digits;
    }

    dt_utils::datetime global_dt{};
    dt_utils::date_format00 date_format00(global_dt);
    dt_utils::date_format01 date_format01(global_dt);
    dt_utils::date_format02 date_format02(global_dt);
    dt_utils::date_format03 date_format03(global_dt);
    dt_utils::date_format04 date_format04(global_dt);
    dt_utils::date_format05 date_format05(global_dt);
    dt_utils::date_format06 date_format06(global_dt);
    dt_utils::date_format07 date_format07(global_dt);
    dt_utils::date_format08 date_format08(global_dt);
    dt_utils::date_format09 date_format09(global_dt);
    dt_utils::date_format10 date_format10(global_dt);
    dt_utils::date_format11 date_format11(global_dt);
    dt_utils::date_format12 date_format12(global_dt);
    dt_utils::date_format13 date_format13(global_dt);
    dt_utils::date_format14 date_format14(global_dt);
    dt_utils::date_format15 date_format15(global_dt);
    dt_utils::datetime_format00 datetime_format00(global_dt);
    dt_utils::datetime_format01 datetime_format01(global_dt);
    dt_utils::datetime_format02 datetime_format02(global_dt);
    dt_utils::datetime_format03 datetime_format03(global_dt);
    dt_utils::datetime_format04 datetime_format04(global_dt);
    dt_utils::datetime_format05 datetime_format05(global_dt);
    dt_utils::datetime_format06 datetime_format06(global_dt);
    dt_utils::datetime_format07 datetime_format07(global_dt);
    dt_utils::datetime_format08 datetime_format08(global_dt);
    dt_utils::datetime_format09 datetime_format09(global_dt);
    dt_utils::datetime_format10 datetime_format10(global_dt);
    dt_utils::datetime_format11 datetime_format11(global_dt);
    dt_utils::datetime_format12 datetime_format12(global_dt);
    dt_utils::datetime_format13 datetime_format13(global_dt);
    dt_utils::datetime_format14 datetime_format14(global_dt);
    dt_utils::datetime_format15 datetime_format15(global_dt);
    dt_utils::datetime_format16 datetime_format16(global_dt);
    dt_utils::datetime_format17 datetime_format17(global_dt);
    dt_utils::datetime_format18 datetime_format18(global_dt);
    dt_utils::datetime_format19 datetime_format19(global_dt);
    dt_utils::datetime_format20 datetime_format20(global_dt);
    dt_utils::datetime_format21 datetime_format21(global_dt);
    dt_utils::datetime_format22 datetime_format22(global_dt);
    dt_utils::datetime_format23 datetime_format23(global_dt);
    dt_utils::datetime_format24 datetime_format24(global_dt);
    dt_utils::datetime_format25 datetime_format25(global_dt);
    dt_utils::datetime_format26 datetime_format26(global_dt);
    dt_utils::datetime_format27 datetime_format27(global_dt);
    dt_utils::datetime_format28 datetime_format28(global_dt);
    dt_utils::datetime_format29 datetime_format29(global_dt);
    dt_utils::datetime_format30 datetime_format30(global_dt);
    dt_utils::datetime_format31 datetime_format31(global_dt);
    dt_utils::datetime_format32 datetime_format32(global_dt);
    dt_utils::datetime_format33 datetime_format33(global_dt);
    dt_utils::time_format0 time_format0(global_dt);
    dt_utils::time_format1 time_format1(global_dt);
    dt_utils::time_format2 time_format2(global_dt);
    dt_utils::time_format3 time_format3(global_dt);
    dt_utils::time_format4 time_format4(global_dt);
    dt_utils::time_format5 time_format5(global_dt);
    dt_utils::time_format6 time_format6(global_dt);
    dt_utils::time_format7 time_format7(global_dt);
    dt_utils::time_format8 time_format8(global_dt);

    py::object datetime = py::module::import("datetime").attr("datetime");
    py::object date = py::module::import("datetime").attr("date");
    py::object time = py::module::import("datetime").attr("time");
    py::object timedelta = py::module::import("datetime").attr("timedelta");
    py::object timezone = py::module::import("datetime").attr("timezone");
    py::object datetime_ms = py::module::import("cpputils._types").attr("datetime_ms");

    py::object get_global_datetime() {
        return global_dt.millisecond ?
               datetime_ms(
                       global_dt.year,
                       global_dt.month,
                       global_dt.day,
                       global_dt.hour,
                       global_dt.minute,
                       global_dt.second,
                       global_dt.millisecond,
                       timezone(timedelta(0, global_dt.tzd * 60))) :
               datetime(
                       global_dt.year,
                       global_dt.month,
                       global_dt.day,
                       global_dt.hour,
                       global_dt.minute,
                       global_dt.second,
                       global_dt.microsecond,
                       timezone(timedelta(0, global_dt.tzd * 60)));
    }

    py::object get_global_date() {
        return date(
                global_dt.year,
                global_dt.month,
                global_dt.day
        );
    }

    py::object get_global_time() {
        return time(
                global_dt.hour,
                global_dt.minute,
                global_dt.second,
                global_dt.millisecond ? global_dt.millisecond * 1000 : global_dt.microsecond,
                timezone(timedelta(0, global_dt.tzd * 60))
        );
    }

    py::object to_datetime(const std::string &value) {
        global_dt.clear();
        if (strtk::string_to_type_converter(value, datetime_format00)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format01)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format02)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format03)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format04)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format05)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format06)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format07)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format08)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format09)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format10)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format11)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format12)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format13)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format14)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format15)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format16)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format17)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format18)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format19)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format20)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format21)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format22)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format23)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format24)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format25)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format26)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format27)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format28)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format29)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format30)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format31)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format32)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, datetime_format33)) return get_global_datetime();
        if (strtk::string_to_type_converter(value, date_format00)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format01)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format02)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format03)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format04)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format05)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format06)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format07)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format08)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format09)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format10)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format11)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format12)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format13)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format14)) return get_global_date();
        if (strtk::string_to_type_converter(value, date_format15)) return get_global_date();
        if (strtk::string_to_type_converter(value, time_format0)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format1)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format2)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format3)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format4)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format5)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format6)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format7)) return get_global_time();
        if (strtk::string_to_type_converter(value, time_format8)) return get_global_time();

        return py::cast(value);
    };

    py::object uuid = py::module::import("uuid").attr("UUID");
    py::object to_integer = py::module::import("builtins").attr("int");
    py::object decimal = py::module::import("decimal").attr("Decimal");
    py::object eval_type(std::string value) {

        if (value.empty()) {
            return py::none();
        }

        int char_size = (int) value.size();
        char first_char = value[0];

        if (char_size <= 1) {
            if (std::isdigit(first_char))
                return (py::cast(std::stoi(&first_char)));
            return (py::cast(first_char));
        }

        // parse numeric
        if (std::regex_match(value, numeric_regex)) {

            if (value.find_first_of('.') != std::string::npos || value.back() == '.') {

                if (char_size > 18) {
                    return (decimal(value));
                }

                // parse double
                return (py::cast(std::stod(value)));

            }

            // parse numeric
            if (first_char == MINUS_CHAR) {
                value = value.erase(0, 1);
                uint64_t integer = parse64(value.c_str());
                if (integer < UINT_MAX) {
                    return (py::cast(-integer));
                }
                return (-to_integer(value));
            }

            uint64_t integer = parse64(value.c_str());
            if (integer < UINT_MAX) {
                return (py::cast(integer));
            }
            return (to_integer(value));

        }

        const char last_char = value.back();

        // remove quote
        if ((first_char == QUOTE_CHAR1 && last_char == QUOTE_CHAR1) ||
            (first_char == QUOTE_CHAR2 && last_char == QUOTE_CHAR2)) {
            value = value.erase(0, 1).erase(value.size() - 1);
        }

        char_size = (int) value.size();
        first_char = value[0];

        if (char_size == 1) {

            if (std::isdigit(first_char))
                return (py::cast(std::stoi(&first_char)));
            return (py::cast(first_char));

        }

        // boolean true or boolan false
        if (char_size < 6 && (std::toupper(first_char) == TRUE_CHAR || std::toupper(first_char) == FALSE_CHAR)) {
            if (std::regex_match(value, boolen_true_regex)) {
                return (py::cast(true));
            }

            if (std::regex_match(value, boolen_false_regex)) {
                return (py::cast(false));
            }
        }

        if (is_nan(value)) {
            return (py::cast<py::none>(Py_None));
        }

        if (char_size == 36 && std::regex_match(value, uuid_regex)) {
            return (uuid(value));
        }

        // datetime
        if (((char_size > 9) || (char_size == 8))) { // check date / datetime string
            return (to_datetime(value));
        }
        // normal string
        return (py::cast(value));
    };

    py::object eval_datetime(const std::string& value) {
        return to_datetime(value);
    }
}
