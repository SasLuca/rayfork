#ifndef RAYFORK_FOUNDATION_CSV_H
#define RAYFORK_FOUNDATION_CSV_H

typedef struct rf_csv_iter
{
    rf_str src;
    rf_str row_separator;
    rf_str col_separator;
    rf_bool valid;
} rf_csv_iter;

rf_int rf_csv_col_count_ex(rf_str src, rf_str separator)
{
    rf_int result = 0;

    while (rf_str_valid(src))
    {
        rf_str_pop_first_split(&src, separator);
        result++;
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

void test()
{
    rf_str csv_str = rf_cstr("...");

    for (rf_csv_iter csv = rf_csv_make_iter(csv); rf_csv_valid(&csv); rf_csv_advance_row(&csv))
    {
        for (rf_str element = rf_csv_next_col_element(csv); rf_csv_col_valid(&csv); rf_csv_next_element())
        {
        }
    }
}

#endif // RAYFORK_FOUNDATION_CSV_H