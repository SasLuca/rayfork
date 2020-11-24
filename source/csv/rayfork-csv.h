#ifndef RAYFORK_CSV_H
#define RAYFORK_CSV_H

#include "rayfork-core.h"
#include "rayfork-str.h"

typedef struct rf_csv_iter
{
    rf_str src;
    rf_int offset;
    rf_int count;
} rf_csv_iter;

rf_int rf_csv_col_count_ex(rf_str src, rf_str separator)
{
    rf_int result = 0;

    while (src.valid && src.data[0] != '\n')
    {
        result += rf_str_match_prefix(src, separator);
        src = rf_str_nextb(src);
    }

    return result;
}

rf_int rf_csv_col_count(rf_str csv)
{
    rf_int result = rf_csv_col_count_ex(csv, rf_cstr(","));
    return result;
}

rf_csv_iter rf_csv_make_iter_ex(rf_str csv)
{
    rf_csv_iter iter = {0};
    return iter;
}

/*void test()
{
    char* csv_str = "...";
    int col_count = rf_csv_col_count(csv_str);

    for (rf_csv_iter csv_it = rf_csv_make_iter(csv); csv_it.valid; rf_csv_iter_next_row(&csv_it))
    {
        for (rf_str )
    }

    for (rf_csv_row r = rf_csv_first_row(csv_str); r.valid; r = rf_csv_next_row(r))
    {
        for (rf_csv_col c = rf_csv_first_col(r); c.valid; c = rf_csv_next_col(c))
        {

        }
    }
}*/

#endif // RAYFORK_CSV_H