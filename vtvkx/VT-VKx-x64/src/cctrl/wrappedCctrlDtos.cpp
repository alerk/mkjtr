/*
 * wrappedCctrlDtos.cpp
 *
 *  Created on: 08.03.2016
 *      Author: zelgerpe
 */

#include <string>
#include <sstream>
#include "wrappedCctrlDtos.h"
#include "util/repdef.h"

#include "util/what.h"
DEFINE_WHATINFO;


using namespace sitraffic;

static const char s_fszeichen[]=
    {
        ' ', //dunkel   dunkel  0
        '?', //roT1Hz   rot_blinken_start_dunkel 1Hz    1
        '?', //ROt1Hz   rot_blinken_start_hell 1Hz      2
        '.', //rot      rot     3
        'T', //geLB1Hz  gelb_blinken_start_dunkel 1Hz   4
        '?', //roTgeLB1Hz       rot_blinken_start_dunkel gelb_blinken_start_dunkel 1Hz  5
        '?', //ROtgeLB1Hz       rot_blinken_start_hell gelb_blinken_start_dunkel 1Hz    6
        '?', //rotgeLB1Hz       rot gelb_blinken_start_dunkel 1Hz       7
        '?', //GElb1Hz  gelb_blinken_start_hell 1Hz     8
        '?', //roTGElb1Hz       rot_blinken_start_dunkel gelb_blinken_start_hell 1Hz    9
        '?', //ROtGElb1Hz       rot_blinken_start_hell gelb_blinken_start_hell 1Hz      10
        '?', //rotGElb1Hz       rot gelb_blinken_start_hell 1Hz 11
        '/', //gelb     gelb    12
        '?', //roTgelb1Hz       rot_blinken_start_dunkel gelb 1Hz       13
        '?', //ROtgelb1Hz       rot_blinken_start_hell gelb 1Hz 14
        '=', //rotgelb  rot gelb        15
        '?', //grUEN1Hz gruen_blinken_start_dunkel 1Hz  16
        '?', //roTgrUEN1Hz      rot_blinken_start_dunkel gruen_blinken_start_dunkel 1Hz 17
        '?', //ROtgrUEN1Hz      rot_blinken_start_hell gruen_blinken_start_dunkel 1Hz   18
        '?', //rotgrUEN1Hz      rot gruen_blinken_start_dunkel 1Hz      19
        '?', //geLBgrUEN1Hz     gelb_blinken_start_dunkel gruen_blinken_start_dunkel 1Hz        20
        '?', //roTgeLBgrUEN1Hz  rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_dunkel 1Hz       21
        '?', //ROtgeLBgrUEN1Hz  rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_dunkel 1Hz 22
        '?', //rotgeLBgrUEN1Hz  rot gelb_blinken_start_dunkel gruen_blinken_start_dunkel 1Hz    23
        '?', //GElbgrUEN1Hz     gelb_blinken_start_hell gruen_blinken_start_dunkel 1Hz  24
        '?', //roTGElbgrUEN1Hz  rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_dunkel 1Hz 25
        '?', //ROtGElbgrUEN1Hz  rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_dunkel 1Hz   26
        '?', //rotGElbgrUEN1Hz  rot gelb_blinken_start_hell gruen_blinken_start_dunkel 1Hz      27
        '?', //gelbgrUEN1Hz     gelb gruen_blinken_start_dunkel 1Hz     28
        '?', //roTgelbgrUEN1Hz  rot_blinken_start_dunkel gelb gruen_blinken_start_dunkel 1Hz    29
        '?', //ROtgelbgrUEN1Hz  rot_blinken_start_hell gelb gruen_blinken_start_dunkel 1Hz      30
        '?', //rotgelbgrUEN1Hz  rot gelb gruen_blinken_start_dunkel 1Hz 31
        '?', //GRuen1Hz gruen_blinken_start_hell 1Hz    32
        '?', //roTGRuen1Hz      rot_blinken_start_dunkel gruen_blinken_start_hell 1Hz   33
        '?', //ROtGRuen1Hz      rot_blinken_start_hell gruen_blinken_start_hell 1Hz     34
        '?', //rotGRuen1Hz      rot gruen_blinken_start_hell 1Hz        35
        '?', //geLBGRuen1Hz     gelb_blinken_start_dunkel gruen_blinken_start_hell 1Hz  36
        '?', //roTgeLBGRuen1Hz  rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_hell 1Hz 37
        '?', //ROtgeLBGRuen1Hz  rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_hell 1Hz   38
        '?', //rotgeLBGRuen1Hz  rot gelb_blinken_start_dunkel gruen_blinken_start_hell 1Hz      39
        '?', //GElbGRuen1Hz     gelb_blinken_start_hell gruen_blinken_start_hell 1Hz    40
        '?', //roTGElbGRuen1Hz  rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_hell 1Hz   41
        '?', //ROtGElbGRuen1Hz  rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_hell 1Hz     42
        '?', //rotGElbGRuen1Hz  rot gelb_blinken_start_hell gruen_blinken_start_hell 1Hz        43
        '?', //gelbGRuen1Hz     gelb gruen_blinken_start_hell 1Hz       44
        '?', //roTgelbGRuen1Hz  rot_blinken_start_dunkel gelb gruen_blinken_start_hell 1Hz      45
        '?', //ROtgelbGRuen1Hz  rot_blinken_start_hell gelb gruen_blinken_start_hell 1Hz        46
        '?', //rotgelbGRuen1Hz  rot gelb gruen_blinken_start_hell 1Hz   47
        'I', //gruen    gruen   48
        '?', //roTgruen1Hz      rot_blinken_start_dunkel gruen 1Hz      49
        '?', //ROtgruen1Hz      rot_blinken_start_hell gruen 1Hz        50
        '?', //rotgruen rot gruen       51
        '?', //geLBgruen1Hz     gelb_blinken_start_dunkel gruen 1Hz     52
        '?', //roTgeLBgruen1Hz  rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen 1Hz    53
        '?', //ROtgeLBgruen1Hz  rot_blinken_start_hell gelb_blinken_start_dunkel gruen 1Hz      54
        '?', //rotgeLBgruen1Hz  rot gelb_blinken_start_dunkel gruen 1Hz 55
        '?', //GElbgruen1Hz     gelb_blinken_start_hell gruen 1Hz       56
        '?', //roTGElbgruen1Hz  rot_blinken_start_dunkel gelb_blinken_start_hell gruen 1Hz      57
        '?', //ROtGElbgruen1Hz  rot_blinken_start_hell gelb_blinken_start_hell gruen 1Hz        58
        '?', //rotGElbgruen1Hz  rot gelb_blinken_start_hell gruen 1Hz   59
        '?', //gelbgruen        gelb gruen      60
        '?', //roTgelbgruen1Hz  rot_blinken_start_dunkel gelb gruen 1Hz 61
        '?', //ROtgelbgruen1Hz  rot_blinken_start_hell gelb gruen 1Hz   62
        '?', //rotgelbgruen     rot gelb gruen  63
        '?', //dunkel2Hz        dunkel 2Hz      64
        '?', //roT2Hz   rot_blinken_start_dunkel 2Hz    65
        '?', //ROt2Hz   rot_blinken_start_hell 2Hz      66
        '?', //rot2Hz   rot 2Hz 67
        '?', //geLB2Hz  gelb_blinken_start_dunkel 2Hz   68
        '?', //roTgeLB2Hz       rot_blinken_start_dunkel gelb_blinken_start_dunkel 2Hz  69
        '?', //ROtgeLB2Hz       rot_blinken_start_hell gelb_blinken_start_dunkel 2Hz    70
        '?', //rotgeLB2Hz       rot gelb_blinken_start_dunkel 2Hz       71
        '?', //GElb2Hz  gelb_blinken_start_hell 2Hz     72
        '?', //roTGElb2Hz       rot_blinken_start_dunkel gelb_blinken_start_hell 2Hz    73
        '?', //ROtGElb2Hz       rot_blinken_start_hell gelb_blinken_start_hell 2Hz      74
        '?', //rotGElb2Hz       rot gelb_blinken_start_hell 2Hz 75
        '?', //gelb2Hz  gelb 2Hz        76
        '?', //roTgelb2Hz       rot_blinken_start_dunkel gelb 2Hz       77
        '?', //ROtgelb2Hz       rot_blinken_start_hell gelb 2Hz 78
        '?', //rotgelb2Hz       rot gelb 2Hz    79
        '?', //grUEN2Hz gruen_blinken_start_dunkel 2Hz  80
        '?', //roTgrUEN2Hz      rot_blinken_start_dunkel gruen_blinken_start_dunkel 2Hz 81
        '?', //ROtgrUEN2Hz      rot_blinken_start_hell gruen_blinken_start_dunkel 2Hz   82
        '?', //rotgrUEN2Hz      rot gruen_blinken_start_dunkel 2Hz      83
        '?', //geLBgrUEN2Hz     gelb_blinken_start_dunkel gruen_blinken_start_dunkel 2Hz        84
        '?', //roTgeLBgrUEN2Hz  rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_dunkel 2Hz       85
        '?', //ROtgeLBgrUEN2Hz  rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_dunkel 2Hz 86
        '?', //rotgeLBgrUEN2Hz  rot gelb_blinken_start_dunkel gruen_blinken_start_dunkel 2Hz    87
        '?', //GElbgrUEN2Hz     gelb_blinken_start_hell gruen_blinken_start_dunkel 2Hz  88
        '?', //roTGElbgrUEN2Hz  rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_dunkel 2Hz 89
        '?', //ROtGElbgrUEN2Hz  rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_dunkel 2Hz   90
        '?', //rotGElbgrUEN2Hz  rot gelb_blinken_start_hell gruen_blinken_start_dunkel 2Hz      91
        '?', //gelbgrUEN2Hz     gelb gruen_blinken_start_dunkel 2Hz     92
        '?', //roTgelbgrUEN2Hz  rot_blinken_start_dunkel gelb gruen_blinken_start_dunkel 2Hz    93
        '?', //ROtgelbgrUEN2Hz  rot_blinken_start_hell gelb gruen_blinken_start_dunkel 2Hz      94
        '?', //rotgelbgrUEN2Hz  rot gelb gruen_blinken_start_dunkel 2Hz 95
        '?', //GRuen2Hz gruen_blinken_start_hell 2Hz    96
        '?', //roTGRuen2Hz      rot_blinken_start_dunkel gruen_blinken_start_hell 2Hz   97
        '?', //ROtGRuen2Hz      rot_blinken_start_hell gruen_blinken_start_hell 2Hz     98
        '?', //rotGRuen2Hz      rot gruen_blinken_start_hell 2Hz        99
        '?', //geLBGRuen2Hz     gelb_blinken_start_dunkel gruen_blinken_start_hell 2Hz  100
        '?', //roTgeLBGRuen2Hz  rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_hell 2Hz 101
        '?', //ROtgeLBGRuen2Hz  rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_hell 2Hz   102
        '?', //rotgeLBGRuen2Hz  rot gelb_blinken_start_dunkel gruen_blinken_start_hell 2Hz      103
        '?', //GElbGRuen2Hz     gelb_blinken_start_hell gruen_blinken_start_hell 2Hz    104
        '?', //roTGElbGRuen2Hz  rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_hell 2Hz   105
        '?', //ROtGElbGRuen2Hz  rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_hell 2Hz     106
        '?', //rotGElbGRuen2Hz  rot gelb_blinken_start_hell gruen_blinken_start_hell 2Hz        107
        '?', //gelbGRuen2Hz     gelb gruen_blinken_start_hell 2Hz       108
        '?', //roTgelbGRuen2Hz  rot_blinken_start_dunkel gelb gruen_blinken_start_hell 2Hz      109
        '?', //ROtgelbGRuen2Hz  rot_blinken_start_hell gelb gruen_blinken_start_hell 2Hz        110
        '?', //rotgelbGRuen2Hz  rot gelb gruen_blinken_start_hell 2Hz   111
        '?', //gruen2Hz gruen 2Hz       112
        '?', //roTgruen2Hz      rot_blinken_start_dunkel gruen 2Hz      113
        '?', //ROtgruen2Hz      rot_blinken_start_hell gruen 2Hz        114
        '?', //rotgruen2Hz      rot gruen 2Hz   115
        '?', //geLBgruen2Hz     gelb_blinken_start_dunkel gruen 2Hz     116
        '?', //roTgeLBgruen2Hz  rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen 2Hz    117
        '?', //ROtgeLBgruen2Hz  rot_blinken_start_hell gelb_blinken_start_dunkel gruen 2Hz      118
        '?', //rotgeLBgruen2Hz  rot gelb_blinken_start_dunkel gruen 2Hz 119
        '?', //GElbgruen2Hz     gelb_blinken_start_hell gruen 2Hz       120
        '?', //roTGElbgruen2Hz  rot_blinken_start_dunkel gelb_blinken_start_hell gruen 2Hz      121
        '?', //ROtGElbgruen2Hz  rot_blinken_start_hell gelb_blinken_start_hell gruen 2Hz        122
        '?', //rotGElbgruen2Hz  rot gelb_blinken_start_hell gruen 2Hz   123
        '?', //gelbgruen2Hz     gelb gruen 2Hz  124
        '?', //roTgelbgruen2Hz  rot_blinken_start_dunkel gelb gruen 2Hz 125
        '?', //ROtgelbgruen2Hz  rot_blinken_start_hell gelb gruen 2Hz   126
        '?', //rotgelbgruen2Hz  rot gelb gruen 2Hz      127
        '?', //dunkel1R dunkel reserved_1       128
        '?', //roT1R    rot_blinken_start_dunkel reserved_1     129
        '?', //ROt1R    rot_blinken_start_hell reserved_1       130
        '?', //rot1R    rot reserved_1  131
        '?', //geLB1R   gelb_blinken_start_dunkel reserved_1    132
        '?', //roTgeLB1R        rot_blinken_start_dunkel gelb_blinken_start_dunkel reserved_1   133
        '?', //ROtgeLB1R        rot_blinken_start_hell gelb_blinken_start_dunkel reserved_1     134
        '?', //rotgeLB1R        rot gelb_blinken_start_dunkel reserved_1        135
        '?', //GElb1R   gelb_blinken_start_hell reserved_1      136
        '?', //roTGElb1R        rot_blinken_start_dunkel gelb_blinken_start_hell reserved_1     137
        '?', //ROtGElb1R        rot_blinken_start_hell gelb_blinken_start_hell reserved_1       138
        '?', //rotGElb1R        rot gelb_blinken_start_hell reserved_1  139
        '?', //gelb1R   gelb reserved_1 140
        '?', //roTgelb1R        rot_blinken_start_dunkel gelb reserved_1        141
        '?', //ROtgelb1R        rot_blinken_start_hell gelb reserved_1  142
        '?', //rotgelb1R        rot gelb reserved_1     143
        '?', //grUEN1R  gruen_blinken_start_dunkel reserved_1   144
        '?', //roTgrUEN1R       rot_blinken_start_dunkel gruen_blinken_start_dunkel reserved_1  145
        '?', //ROtgrUEN1R       rot_blinken_start_hell gruen_blinken_start_dunkel reserved_1    146
        '?', //rotgrUEN1R       rot gruen_blinken_start_dunkel reserved_1       147
        '?', //geLBgrUEN1R      gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_1 148
        '?', //roTgeLBgrUEN1R   rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_1        149
        '?', //ROtgeLBgrUEN1R   rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_1  150
        '?', //rotgeLBgrUEN1R   rot gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_1     151
        '?', //GElbgrUEN1R      gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_1   152
        '?', //roTGElbgrUEN1R   rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_1  153
        '?', //ROtGElbgrUEN1R   rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_1    154
        '?', //rotGElbgrUEN1R   rot gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_1       155
        '?', //gelbgrUEN1R      gelb gruen_blinken_start_dunkel reserved_1      156
        '?', //roTgelbgrUEN1R   rot_blinken_start_dunkel gelb gruen_blinken_start_dunkel reserved_1     157
        '?', //ROtgelbgrUEN1R   rot_blinken_start_hell gelb gruen_blinken_start_dunkel reserved_1       158
        '?', //rotgelbgrUEN1R   rot gelb gruen_blinken_start_dunkel reserved_1  159
        '?', //GRuen1R  gruen_blinken_start_hell reserved_1     160
        '?', //roTGRuen1R       rot_blinken_start_dunkel gruen_blinken_start_hell reserved_1    161
        '?', //ROtGRuen1R       rot_blinken_start_hell gruen_blinken_start_hell reserved_1      162
        '?', //rotGRuen1R       rot gruen_blinken_start_hell reserved_1 163
        '?', //geLBGRuen1R      gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_1   164
        '?', //roTgeLBGRuen1R   rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_1  165
        '?', //ROtgeLBGRuen1R   rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_1    166
        '?', //rotgeLBGRuen1R   rot gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_1       167
        '?', //GElbGRuen1R      gelb_blinken_start_hell gruen_blinken_start_hell reserved_1     168
        '?', //roTGElbGRuen1R   rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_hell reserved_1    169
        '?', //ROtGElbGRuen1R   rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_hell reserved_1      170
        '?', //rotGElbGRuen1R   rot gelb_blinken_start_hell gruen_blinken_start_hell reserved_1 171
        '?', //gelbGRuen1R      gelb gruen_blinken_start_hell reserved_1        172
        '?', //roTgelbGRuen1R   rot_blinken_start_dunkel gelb gruen_blinken_start_hell reserved_1       173
        '?', //ROtgelbGRuen1R   rot_blinken_start_hell gelb gruen_blinken_start_hell reserved_1 174
        '?', //rotgelbGRuen1R   rot gelb gruen_blinken_start_hell reserved_1    175
        '?', //gruen1R  gruen reserved_1        176
        '?', //roTgruen1R       rot_blinken_start_dunkel gruen reserved_1       177
        '?', //ROtgruen1R       rot_blinken_start_hell gruen reserved_1 178
        '?', //rotgruen1R       rot gruen reserved_1    179
        '?', //geLBgruen1R      gelb_blinken_start_dunkel gruen reserved_1      180
        '?', //roTgeLBgruen1R   rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen reserved_1     181
        '?', //ROtgeLBgruen1R   rot_blinken_start_hell gelb_blinken_start_dunkel gruen reserved_1       182
        '?', //rotgeLBgruen1R   rot gelb_blinken_start_dunkel gruen reserved_1  183
        '?', //GElbgruen1R      gelb_blinken_start_hell gruen reserved_1        184
        '?', //roTGElbgruen1R   rot_blinken_start_dunkel gelb_blinken_start_hell gruen reserved_1       185
        '?', //ROtGElbgruen1R   rot_blinken_start_hell gelb_blinken_start_hell gruen reserved_1 186
        '?', //rotGElbgruen1R   rot gelb_blinken_start_hell gruen reserved_1    187
        '?', //gelbgruen1R      gelb gruen reserved_1   188
        '?', //roTgelbgruen1R   rot_blinken_start_dunkel gelb gruen reserved_1  189
        '?', //ROtgelbgruen1R   rot_blinken_start_hell gelb gruen reserved_1    190
        '?', //rotgelbgruen1R   rot gelb gruen reserved_1       191
        '?', //dunkel2R dunkel reserved_2       192
        '?', //roT2R    rot_blinken_start_dunkel reserved_2     193
        '?', //ROt2R    rot_blinken_start_hell reserved_2       194
        '?', //rot2R    rot reserved_2  195
        '?', //geLB2R   gelb_blinken_start_dunkel reserved_2    196
        '?', //roTgeLB2R        rot_blinken_start_dunkel gelb_blinken_start_dunkel reserved_2   197
        '?', //ROtgeLB2R        rot_blinken_start_hell gelb_blinken_start_dunkel reserved_2     198
        '?', //rotgeLB2R        rot gelb_blinken_start_dunkel reserved_2        199
        '?', //GElb2R   gelb_blinken_start_hell reserved_2      200
        '?', //roTGElb2R        rot_blinken_start_dunkel gelb_blinken_start_hell reserved_2     201
        '?', //ROtGElb2R        rot_blinken_start_hell gelb_blinken_start_hell reserved_2       202
        '?', //rotGElb2R        rot gelb_blinken_start_hell reserved_2  203
        '?', //gelb2R   gelb reserved_2 204
        '?', //roTgelb2R        rot_blinken_start_dunkel gelb reserved_2        205
        '?', //ROtgelb2R        rot_blinken_start_hell gelb reserved_2  206
        '?', //rotgelb2R        rot gelb reserved_2     207
        '?', //grUEN2R  gruen_blinken_start_dunkel reserved_2   208
        '?', //roTgrUEN2R       rot_blinken_start_dunkel gruen_blinken_start_dunkel reserved_2  209
        '?', //ROtgrUEN2R       rot_blinken_start_hell gruen_blinken_start_dunkel reserved_2    210
        '?', //rotgrUEN2R       rot gruen_blinken_start_dunkel reserved_2       211
        '?', //geLBgrUEN2R      gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_2 212
        '?', //roTgeLBgrUEN2R   rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_2        213
        '?', //ROtgeLBgrUEN2R   rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_2  214
        '?', //rotgeLBgrUEN2R   rot gelb_blinken_start_dunkel gruen_blinken_start_dunkel reserved_2     215
        '?', //GElbgrUEN2R      gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_2   216
        '?', //roTGElbgrUEN2R   rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_2  217
        '?', //ROtGElbgrUEN2R   rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_2    218
        '?', //rotGElbgrUEN2R   rot gelb_blinken_start_hell gruen_blinken_start_dunkel reserved_2       219
        '?', //gelbgrUEN2R      gelb gruen_blinken_start_dunkel reserved_2      220
        '?', //roTgelbgrUEN2R   rot_blinken_start_dunkel gelb gruen_blinken_start_dunkel reserved_2     221
        '?', //ROtgelbgrUEN2R   rot_blinken_start_hell gelb gruen_blinken_start_dunkel reserved_2       222
        '?', //rotgelbgrUEN2R   rot gelb gruen_blinken_start_dunkel reserved_2  223
        '?', //GRuen2R  gruen_blinken_start_hell reserved_2     224
        '?', //roTGRuen2R       rot_blinken_start_dunkel gruen_blinken_start_hell reserved_2    225
        '?', //ROtGRuen2R       rot_blinken_start_hell gruen_blinken_start_hell reserved_2      226
        '?', //rotGRuen2R       rot gruen_blinken_start_hell reserved_2 227
        '?', //geLBGRuen2R      gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_2   228
        '?', //roTgeLBGRuen2R   rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_2  229
        '?', //ROtgeLBGRuen2R   rot_blinken_start_hell gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_2    230
        '?', //rotgeLBGRuen2R   rot gelb_blinken_start_dunkel gruen_blinken_start_hell reserved_2       231
        '?', //GElbGRuen2R      gelb_blinken_start_hell gruen_blinken_start_hell reserved_2     232
        '?', //roTGElbGRuen2R   rot_blinken_start_dunkel gelb_blinken_start_hell gruen_blinken_start_hell reserved_2    233
        '?', //ROtGElbGRuen2R   rot_blinken_start_hell gelb_blinken_start_hell gruen_blinken_start_hell reserved_2      234
        '?', //rotGElbGRuen2R   rot gelb_blinken_start_hell gruen_blinken_start_hell reserved_2 235
        '?', //gelbGRuen2R      gelb gruen_blinken_start_hell reserved_2        236
        '?', //roTgelbGRuen2R   rot_blinken_start_dunkel gelb gruen_blinken_start_hell reserved_2       237
        '?', //ROtgelbGRuen2R   rot_blinken_start_hell gelb gruen_blinken_start_hell reserved_2 238
        '?', //rotgelbGRuen2R   rot gelb gruen_blinken_start_hell reserved_2    239
        '?', //gruen2R  gruen reserved_2        240
        '?', //roTgruen2R       rot_blinken_start_dunkel gruen reserved_2       241
        '?', //ROtgruen2R       rot_blinken_start_hell gruen reserved_2 242
        '?', //rotgruen2R       rot gruen reserved_2    243
        '?', //geLBgruen2R      gelb_blinken_start_dunkel gruen reserved_2      244
        '?', //roTgeLBgruen2R   rot_blinken_start_dunkel gelb_blinken_start_dunkel gruen reserved_2     245
        '?', //ROtgeLBgruen2R   rot_blinken_start_hell gelb_blinken_start_dunkel gruen reserved_2       246
        '?', //rotgeLBgruen2R   rot gelb_blinken_start_dunkel gruen reserved_2  247
        '?', //GElbgruen2R      gelb_blinken_start_hell gruen reserved_2        248
        '?', //roTGElbgruen2R   rot_blinken_start_dunkel gelb_blinken_start_hell gruen reserved_2       249
        '?', //ROtGElbgruen2R   rot_blinken_start_hell gelb_blinken_start_hell gruen reserved_2 250
        '?', //rotGElbgruen2R   rot gelb_blinken_start_hell gruen reserved_2    251
        '?', //gelbgruen2R      gelb gruen reserved_2   252
        '?', //roTgelbgruen2R   rot_blinken_start_dunkel gelb gruen reserved_2  253
        '?', //ROtgelbgruen2R   rot_blinken_start_hell gelb gruen reserved_2    254
        '?' //rotgelbgruen2R   rot gelb gruen reserved_2       255
    };

static void ocit2char(std::ostringstream& out, int ocitCode)
{
  char ret = s_fszeichen[ocitCode & 0xff];
  if(ret!='?')
  {
    out << ret;
    return;
  }

  static char buf[3];
  sprintf(buf, "%02x", ocitCode);
  out << buf;
}

bool RequestedStatusWCctrl::isAtleastOnePiOn() const
  {
    if (signalProgram.value <= 0)
      return false;
    switch (intersectionState.value)
      {
      case RMNS_OFF_DEFAULT:
      case RMNS_OFF_FLASH_SECONDARY_DIRECTION:
      case RMNS_OFF_DARK:
      case RMNS_OFF_FLASH_ALL:
        return false;
      }
    for (TimeConstrainedTransactionTaggedIntValueCctrl tv : subIntersectionState)
      {
        if (tv.value == RNS_ON)
          return true;
      }
    return false;
  }

const TcTTVModOnOffStateCctrl * RequestedStatusWCctrl::getMod(RequestedStatusModNoCctrl mod) const
  {
    for(unsigned i=0; i< modState.modState_len; ++i)
      {
        const TcTTVModOnOffStateCctrl & moos(modState.modState_val[i]);
        if(moos.modType == mod)
          return &moos;
      }
    return NULL;
  }

/*
 * @return bitfield, 2^0 ModTD on, 2^1 mod public transport, 2^2 mod individual traffic
 */
unsigned RequestedStatusWCctrl::getModBits() const
  {
    static const RequestedStatusModNoCctrl mods[]={RSMN_IVState, RSMN_OEVState, RSMN_VAState };
    unsigned ret=0;
    for(RequestedStatusModNoCctrl mod: mods)
      {
        const TcTTVModOnOffStateCctrl * pmod=getMod(mod);
        if(!pmod)
          Report(RUNMODE_DEBUG, "RequestedStatusCCtrl_getModBits mod %d not found, assuming off", mod);
        ret <<= 1;
        if(pmod && pmod->value==RMS_ON)
          ret |= 1;
      }
    return ret;
  }

const TTVModOnOffStateCctrl * ActualStatusWCctrl::getMod(TypeOfModificationCctrl mod) const
  {
    for(unsigned i=0; i< modifications.modifications_len; ++i)
      {
        const TTVModOnOffStateCctrl & moos(modifications.modifications_val[i]);
        if(moos.modType == mod)
          return &moos;
      }
    return NULL;
  }

unsigned ActualStatusWCctrl::getModBits() const
  {
    static const TypeOfModificationCctrl mods[]={TOM_ModTDIndividualMkjtric, TOM_ModPublicTransport, TOM_ModTD };
    unsigned ret=0;
    for(TypeOfModificationCctrl mod: mods)
      {
        const TTVModOnOffStateCctrl * pmod=getMod(mod);
        ret <<= 1;
        if(!pmod)
          Report(RUNMODE_DEBUG, "ActualStatusCCtrl_getModBits mod %d not found, assuming off", mod);
        if(pmod && pmod->value==MOOS_ON)
          ret |= 1;
      }
    return ret;
  }
/** @return bitfield, 2^1 is set if partial intersection 1 is ON,
 *                    2^2 is set if partial intersection 2 is ON... */
unsigned ActualStatusWCctrl::getOnPiMask() const
  {
    unsigned ret=0;
    for(unsigned i=0; i< subIntersectionStates.subIntersectionStates_len; i++)
      {
        const TTVSubIntersectionDetailStateCctrl & sids(subIntersectionStates.subIntersectionStates_val[i]);
        if(sids.value==SDS_ON)
          {
            ret |= (1<< sids.subintersectionNo);
          }
      }
    return ret;
  }
//static
bool ActualStatusWCctrl::isPiOn(unsigned onPiMask, int subintersectionNumber)
  {
    return (1<< subintersectionNumber) & onPiMask;
  }

const SignalIndicationWCctrl * SignalIndicationsWCctrl::lookup(int sigNr) const
  {
    sgPatterns_t::const_iterator i = sgPatterns.find(sigNr);
    if (i != sgPatterns.end())
      return &(i->second);
    return 0;
  }

SignalIndicationsWCctrl & SignalIndicationsWCctrl::operator = (const SignalIndicationsCctrl & o)
  {
    baseTicktime = o.baseTicktime;
    baseUtcMinusTicktime = o.baseUtcMinusTicktime;
    tx = o.tx;
    sgPatterns.clear();
    for(unsigned i=0; i<o.sgPatterns.sgPatterns_len; i++)
      {
        SignalIndicationCctrl & si(o.sgPatterns.sgPatterns_val[i]);
        sgPatterns[si.sgNo] = si;
      }
    return *this;
  }

const char * SignalIndicationWCctrl::getStateTxt() const
  {
    if(!hasState())
      return "unknown";
    switch(getState())
      {
      case 0:
        /** blocked (gesperrt)*/
        return "red";
      case 1:
        /** clearance (freigegeben) */
        return "green";
      case 2:
        /** transition blocked-->clearance (Uebergang sperr-frei)*/
        return "redGreen";
      case 3:
        /** transition clearance-->blocked (Uebergang frei-sperr)*/
        return "greenRed";
      default:
        return "?";
      }
  }

std::string SignalIndicationsWCctrl::getDecodedSgPattern() const
{
   std::ostringstream out;
   for(unsigned i=0; i< sgPatterns.size(); i++)
   {
     const SignalIndicationWCctrl * si = lookup(i);
     if(si != 0)
       ocit2char(out, si->getOcitColor());
   }
  return out.str();
}

int64_t SignalIndicationsWCctrl::getBaseUtcMinusTicktime() const
{
    return baseUtcMinusTicktime;
}

//by Valentin
int LampStatusWCctrl::countLampStateFault(LampFaultStateCctrl* heads, int len) const
{
	int count = 0;
	for (int i = 0; i < len ; i++)
	{
		if (heads[i] == LFS_DEFECT || heads[i] == LFS_GROUP_DEFECT)
		{
			count++;
		}
	}
	return count;
}

//by Valentin
bool LampStatusWCctrl::isGreenHeadFault() const
{
	int len = greenHeads.greenHeads_len;
	LampFaultStateCctrl * headVals = greenHeads.greenHeads_val;
	if(len > 0)
	{
		int count = countLampStateFault(headVals, len);
		if(count > 0 )
			return true;
	}
	return false;
}

//by Valentin
bool LampStatusWCctrl::isRedHeadFault() const
{
	int len = redHeads.redHeads_len;
	LampFaultStateCctrl * headVals = redHeads.redHeads_val;
	if(len > 0)
	{
		int count = countLampStateFault(headVals, len);
		if(count > 0 )
			return true;
	}
	return false;
}

//by Valentin
bool LampStatusWCctrl::isAmberHeadFault() const
{
	int len = amberHeads.amberHeads_len;
	LampFaultStateCctrl * headVals = amberHeads.amberHeads_val;
	if(len > 0)
	{
		int count = countLampStateFault(headVals, len);
		if(count > 0 )
			return true;
	}
	return false;
}
