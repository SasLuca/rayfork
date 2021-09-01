#ifndef RAYFORK_FOUNDATION_ANNOTATIONS_H
#define RAYFORK_FOUNDATION_ANNOTATIONS_H

#define rf_in
#define rf_out
#define rf_inout
#define rf_outptr

#define rf_in_opt
#define rf_out_opt
#define rf_inout_opt
#define rf_outptr_opt

#define rf_buffer_size(size_member)
#define rf_field_range(begin, end)

#include "sal.h"
#if defined(rayfork_enable_sal_annotations)

#define rf_in _In_
#define rf_out _Out_
#define rf_inout _Inout_
#define rf_outptr _Outptr_

#define rf_in _In_opt_
#define rf_out _Out_opt_
#define rf_inout _Inout_opt_
#define rf_outptr _Outptr_opt_

#define rf_buffer_size(size_member)
#define rf_field_range(begin, end)
#endif // defined(rayfork_enable_sal_annotations)

#endif // RAYFORK_FOUNDATION_ANNOTATIONS_H