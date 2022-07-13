// pretty table metrics w/ benchmarking, unit conversions and a few format outputs { csv, tsv, markdown, ascii art }
// uses auto_table by Dmitry Ledentsov (public domain) and units class by Calum Grant (boost licensed).
// - rlyeh, zlib licensed

#pragma once
#include <cassert>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <numeric>
#include <float.h>
#include <iomanip>
#include "units.hpp"

#if defined(USE_OMP) || defined(_MSC_VER)
#include <omp.h>
#else
#include <chrono>
#endif

#define METRICS_VERSION "v1.0.0"

namespace metrics {

using namespace units::values;

template<typename FN>
double bench( const FN &fn ) {
    auto now = []() -> double {
#if defined(USE_OMP) || defined(_MSC_VER)
        static auto const epoch = omp_get_wtime();
        return omp_get_wtime() - epoch;
#else
        static auto const epoch = std::chrono::steady_clock::now(); // milli ms > micro us > nano ns
        return std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::steady_clock::now() - epoch ).count() / 1000000.0;
#endif
    };
    auto took = -now();
    return ( fn(), took + now() );
}

enum format {
    fmt_ascii,
    fmt_csv,
    fmt_tsv,
    fmt_markdown
};

template<format fmt>
class table {
    std::stringstream my_stream;
    typedef std::vector<std::string> row_t;
    typedef std::vector<row_t> rows_t;
    typedef std::map<size_t, size_t> column_widths_t;
    typedef std::map<size_t, size_t> column_align_t;
    column_widths_t column_widths;
    column_align_t column_aligns;
    rows_t rows;
    rows_t acc, avg, min, max;
    size_t header_every_nth_row;
    size_t horizontal_padding;

    char topleft, hline, topright, left, vline, right;
    char odd, even;
    unsigned parity;

public:
    table() : header_every_nth_row(0), horizontal_padding(0) {
        with_decimal_digits(2);

        odd = even = ' ';
        parity = 0;
        /****/ if( fmt == fmt_csv || fmt == fmt_tsv ) {
            topleft = ' ', topright = ' ', hline = ' ';
            left = ' ', vline = (fmt == fmt_csv ? ',' : '\t' ), right = ' ';
        } else if( fmt == fmt_markdown ) {
            topleft = ' ', hline = ' ', topright = ' ';
            left = '|', vline = '|', right = '|';
        } else {
            topleft = '+', hline = '-', topright = '+';
            left = '|', vline = '|', right = '|';
        }
    }

private:
    size_t width(std::string const& s) { return s.length(); }

    size_t combine_width(size_t one_width, size_t another_width) {
        return std::max(one_width, another_width);
    }

private:
    size_t get_total_width() {
        size_t sum = std::accumulate(column_widths.begin(), column_widths.end(), 0,
            [] (size_t i, const std::pair<const size_t, size_t>& x) {
                return i + x.second;
            } );
        return sum + column_widths.size() + 2 * column_widths.size() * horizontal_padding;
    }
    void print_horizontal_line(std::ostream& stream) {
        if( fmt == fmt_ascii ) {
        stream << topleft;
        auto sum = get_total_width() - 1;
        stream << std::string( sum, hline );
        stream << topright << "\n";
        }
    }

    std::string pad_column(std::string const& s, size_t column, bool is_right ) {
        size_t s_width = width(s);
        size_t column_width = column_widths[column];
        if (s_width > column_width) return s;

        if( is_right )
            return std::string(column_width - s_width + horizontal_padding, parity & 1 ? odd : even) + s +
            std::string(horizontal_padding, parity & 1 ? odd : even);
        else
            return std::string(horizontal_padding, parity & 1 ? odd : even) + s +
            std::string(column_width - s_width + horizontal_padding, parity & 1 ? odd : even);
    }

    void print_row(row_t const& row, std::ostream& stream, int /*color*/ = 7) {
        parity ++;

        size_t column_count = column_widths.size();

        char sep = left;
        for (size_t i = 0; i < column_count; i++) {
            stream << sep;
            stream << pad_column(row[i], i, column_aligns[i] > 0 );
            sep = vline;
        }

        stream << right << "\n";
    }

    void print_header(std::ostream& stream) {
        parity = 0;
        if (rows.size() > 0) {
            print_row(rows[0], stream, 10);
        }
        if( fmt == fmt_markdown ) {
            size_t column_count = column_widths.size();

            char sep = left;
            for (size_t i = 0; i < column_count; i++) {
                stream << sep;
                std::string fill( column_widths[i] + horizontal_padding * 2 , '-' );
                if( column_aligns[i] > 0 ) fill.back() = ':';
                else fill[0] = ':';
                stream << fill;
                sep = vline;
            }

            stream << right << "\n";
        }
    }

    void print_values( std::ostream& stream, rows_t &rows ) {
        size_t row_count = rows.size();

        if (row_count == 0) return;

        for (size_t row = 1; row < row_count - 1; row++) {
            if (row > 1 && header_every_nth_row &&
                (row - 1) % header_every_nth_row == 0) {
                print_horizontal_line(stream);
                print_header(stream);
                print_horizontal_line(stream);
            }

            print_row(rows[row], stream, 15);
        }

        if (rows[row_count - 1].size() > 0) print_row(rows[row_count - 1], stream, 15 );
    }

    void make_stats() {
        // make stats and indirectly adjust widths as well

        unsigned rows_n = rows.size() - 1; // exclude header
        acc = decltype(acc)();
        avg = decltype(avg)();
        min = decltype(min)();
        max = decltype(max)();

        if( rows_n + 1 < 1 ) { // empty and header-only tables apply here
            for( auto end = column_widths.size(), index = end - end; index < end; ++index ) {
                append( acc, 0 );
                append( avg, 0 );
                append( min, 0 );
                append( max, 0 );
            }
        } else {
            for( auto end = column_widths.size(), index = end - end; index < end; ++index ) {
                double vacc = 0, vmin = DBL_MAX, vmax = 0;
                for( auto &row : decltype(rows)(rows.begin()+1, rows.end()) ) {
                    double v = std::atof( row[index].c_str() );
                    vacc += v;
                    vmin = std::min( vmin, v );
                    vmax = std::max( vmax, v );
                }
                append(acc, vacc);
                append(avg, vacc / rows_n );
                append(min, vmin );
                append(max, vmax );
            }
        }
    }

    void print_tab(std::ostream &stream, const std::string &name) {
        if( fmt == fmt_ascii ) {
            stream << "+-" << std::string( name.size(), '-' ) << "+" << std::endl;
            stream << "| " << name << " \\" << std::string( get_total_width() - name.size() - 4, '-' ) << "+" << std::endl;
        }
    }

    void print_footer(std::ostream& stream) {
        print_horizontal_line(stream);
    }

public:
    table& add_column_left(std::string const& name, size_t min_width = 0) {
        size_t new_width = combine_width(width(name), min_width);
        column_widths[column_widths.size()] = new_width;
        column_aligns[column_widths.size() - 1] = 0;

        if (rows.size() < 1) rows.push_back(row_t());

        rows.front().push_back(name);
        return *this;
    }

    table& add_column_right(std::string const& name, size_t min_width = 0) {
        add_column_left( name, min_width );
        column_aligns[column_widths.size() - 1] = 1;
        return *this;
    }

    table& with_header_every_nth_row(size_t n) {
        if( fmt != fmt_markdown ) header_every_nth_row = n;
        return *this;
    }

    table& with_horizontal_padding(size_t n) {
        horizontal_padding = n;
        return *this;
    }

    table& with_odd_even_shadow(bool enable) {
        even = (enable ? '.' : ' ');
        return *this;
    }

    table& with_decimal_digits(int num) {
        my_stream.precision(num);
        my_stream << std::fixed;
        return *this;
    }

    template <typename TPar>
    table& append( rows_t &rows, TPar const& input) {
        assert( column_widths.size() > 0 && "no columns defined!" );

        if (rows.size() < 1) rows.push_back(row_t());

        if (rows.back().size() >= column_widths.size()) rows.push_back(row_t());

        my_stream << input;
        std::string entry(my_stream.str());
        size_t column = rows.back().size();
        size_t new_width = combine_width(width(entry), column_widths[column]);
        column_widths[column] = new_width;

        rows.back().push_back(entry);

        my_stream.str("");

        return *this;
    }

    template <typename TPar>
    table& operator<<(TPar const& input) {
        return append( rows, input );
    }

    table& operator<<(std::ostream &( *endl )(std::ostream &)) {
        if( *endl == static_cast<std::ostream &( * )(std::ostream&)>( std::endl ) ) {
            rows.push_back(row_t());
        }
        return *this;
    }

    void print(std::ostream& stream, bool with_stats = 1 ) {
        // gen stats && adjusts widths indirectly as well
        if( with_stats ) {
            make_stats();
            print_tab(stream, "metrics");
            print_header(stream);
            print_horizontal_line(stream);
        } else {
            print_horizontal_line(stream);
            print_header(stream);
            print_horizontal_line(stream);
        }
        print_values(stream, rows);
        if( with_stats ) {
        print_horizontal_line(stream);
        print_tab(stream, "stats-acc");
        print_values(stream, acc);
        print_tab(stream, "stats-avg");
        print_values(stream, avg);
        print_tab(stream, "stats-min");
        print_values(stream, min);
        print_tab(stream, "stats-max");
        print_values(stream, max);
        }
        print_footer(stream);
    }

    std::stringstream& get_stream() {
        return my_stream;
    }
};

typedef table<fmt_ascii> table_ascii;
typedef table<fmt_csv> table_csv;
typedef table<fmt_tsv> table_tsv;
typedef table<fmt_markdown> table_markdown;

}
