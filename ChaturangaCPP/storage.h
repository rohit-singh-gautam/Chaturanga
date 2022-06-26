#pragma once
#include <climits>

typedef char position_storage_type;
typedef char row_storage_type;
typedef char col_storage_type;
typedef char player_storate_type;
typedef char mohra_storate_type;
typedef unsigned char piece_storate_type;
typedef short int board_score_type;
typedef short int niti_depth_type;
typedef unsigned long long int board_hash_type;
typedef unsigned char bitmohra_internal_t;

typedef unsigned char promotion_compressed;

#define board_score_type_max ((board_score_type)(SHRT_MAX))
#define board_score_type_min ((board_score_type)(-(SHRT_MAX)))