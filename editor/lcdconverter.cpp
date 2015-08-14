#include "lcdconverter.h"

#include <QtCore>

LcdConverter::LcdConverter()
{
    for (ushort i = 0; i < 0x7b; i++)
        map_ks0066ru[i] = i;

    map_ks0066ru[0x0410] = 'A';
    map_ks0066ru[0x0412] = 'B';
    map_ks0066ru[0x0421] = 'C';
    map_ks0066ru[0x0415] = 'E';
    map_ks0066ru[0x041d] = 'H';
    map_ks0066ru[0x041a] = 'K';
    map_ks0066ru[0x041c] = 'M';
    map_ks0066ru[0x041e] = 'O';
    map_ks0066ru[0x0420] = 'P';
    map_ks0066ru[0x0422] = 'T';
    map_ks0066ru[0x0422] = 'X';

    map_ks0066ru[0x0430] = 'a';
    map_ks0066ru[0x042c] = 'b';
    map_ks0066ru[0x0441] = 'c';
    map_ks0066ru[0x0435] = 'e';
    map_ks0066ru[0x043e] = 'o';
    map_ks0066ru[0x0440] = 'p';
    map_ks0066ru[0x0445] = 'x';
    map_ks0066ru[0x0443] = 'y';

    map_ks0066ru[0x2469] = 0x7b; // ⑩
    map_ks0066ru[0x246b] = 0x7c; // ⑫
    map_ks0066ru[0x246e] = 0x7d; // ⑮
    map_ks0066ru[0x21b5] = 0x7e; // ↵
    map_ks0066ru[0x2750] = 0x7f; //

    map_ks0066ru[0x0411] = 0xa0; // Б
    map_ks0066ru[0x0413] = 0xa1; // Г
    map_ks0066ru[0x0401] = 0xa2; // Ё
    map_ks0066ru[0x0416] = 0xa3; // Ж
    map_ks0066ru[0x0417] = 0xa4; // З
    map_ks0066ru[0x0418] = 0xa5; // И
    map_ks0066ru[0x0419] = 0xa6; // Й
    map_ks0066ru[0x041b] = 0xa7; // Л
    map_ks0066ru[0x041f] = 0xa8; // П
    map_ks0066ru[0x0423] = 0xa9; // У
    map_ks0066ru[0x0424] = 0xaa; // Ф
    map_ks0066ru[0x0427] = 0xab; // Ч
    map_ks0066ru[0x0428] = 0xac; // Ш
    map_ks0066ru[0x042a] = 0xad; // Ъ
    map_ks0066ru[0x042b] = 0xae; // Ы
    map_ks0066ru[0x042d] = 0xaf; // Э

    map_ks0066ru[0x042e] = 0xb0; // Ю
    map_ks0066ru[0x042f] = 0xb1; // Я
    map_ks0066ru[0x0431] = 0xb2; // б
    map_ks0066ru[0x0432] = 0xb3; // в
    map_ks0066ru[0x0433] = 0xb4; // г
    map_ks0066ru[0x0451] = 0xb5; // ё
    map_ks0066ru[0x0436] = 0xb6; // ж
    map_ks0066ru[0x0437] = 0xb7; // з
    map_ks0066ru[0x0438] = 0xb8; // и
    map_ks0066ru[0x0439] = 0xb9; // й
    map_ks0066ru[0x043a] = 0xba; // к
    map_ks0066ru[0x043b] = 0xbb; // л
    map_ks0066ru[0x043c] = 0xbc; // м
    map_ks0066ru[0x043d] = 0xbd; // н
    map_ks0066ru[0x043f] = 0xbe; // п
    map_ks0066ru[0x0442] = 0xbf; // т

    map_ks0066ru[0x0447] = 0xc0; // ч
    map_ks0066ru[0x0448] = 0xc1; // ш
    map_ks0066ru[0x044a] = 0xc2; // ъ
    map_ks0066ru[0x044b] = 0xc3; // ы
    map_ks0066ru[0x044c] = 0xc4; // ь
    map_ks0066ru[0x044d] = 0xc5; // э
    map_ks0066ru[0x044e] = 0xc6; // ю
    map_ks0066ru[0x044f] = 0xc7; // я
    map_ks0066ru[0x00ab] = 0xc8; // «
    map_ks0066ru[0x00bb] = 0xc9; // »
    map_ks0066ru[0x201e] = 0xca; // “
    map_ks0066ru[0x201d] = 0xcb; // ”
    map_ks0066ru[0x2116] = 0xcc; // Ṇ
    map_ks0066ru[0x00bf] = 0xcd; // ¿
    map_ks0066ru[0x2a0d] = 0xce; // ⨍
    map_ks0066ru[0x00a3] = 0xcf; // £

    map_ks0066ru[0x02cc] = 0xd0; // ˌ
    map_ks0066ru[0x2577] = 0xd1; // ╷
    map_ks0066ru[0x2963] = 0xd2; // ⥣
    map_ks0066ru[0x2965] = 0xd3; // ⥥
    map_ks0066ru[0x215f] = 0xd4; // ⅟
    map_ks0066ru[0x00d7] = 0xd5; // ×
    map_ks0066ru[0x2044] = 0xd6; // ⁄
    map_ks0066ru[0x2160] = 0xd7; // Ⅰ
    map_ks0066ru[0x2161] = 0xd8; // Ⅱ
    map_ks0066ru[0x2191] = 0xd9; // ↑
    map_ks0066ru[0x2193] = 0xda; // ↓
    map_ks0066ru[0x21e4] = 0xdb; // ⇤
    map_ks0066ru[0x21e5] = 0xdc; // ⇥
    map_ks0066ru[0x2920] = 0xdd; // ⤠
    map_ks0066ru[0x2358] = 0xde; // ⍘
    map_ks0066ru[0x00b7] = 0xdf; // °

    map_ks0066ru[0x0414] = 0xe0; // Д
    map_ks0066ru[0x0426] = 0xe1; // Ц
    map_ks0066ru[0x0429] = 0xe2; // Щ
    map_ks0066ru[0x0434] = 0xe3; // д
    map_ks0066ru[0x0444] = 0xe4; // ф
    map_ks0066ru[0x0446] = 0xe5; // ц
    map_ks0066ru[0x0449] = 0xe6; // щ
    map_ks0066ru[0x2018] = 0xe7; // ‘
    map_ks0066ru[0x00a8] = 0xe8; // ¨
    map_ks0066ru[0x007e] = 0xe9; // ~
    map_ks0066ru[0x040e] = 0xea; // Ў
    map_ks0066ru[0x045e] = 0xeb; // ў
    map_ks0066ru[0x0404] = 0xec; // Є
    map_ks0066ru[0x0454] = 0xed; // є
    map_ks0066ru[0x0407] = 0xee; // Ї
    map_ks0066ru[0x2457] = 0xef; // ї

    map_ks0066ru[0x00bc] = 0xf0; // ¼
    map_ks0066ru[0x2153] = 0xf1; // ⅓
    map_ks0066ru[0x00bd] = 0xf2; // ½
    map_ks0066ru[0x00be] = 0xf3; // ¾
    map_ks0066ru[0x2630] = 0xf4; // ☰
    map_ks0066ru[0x2234] = 0xf5; // ♥
    map_ks0066ru[0x23ce] = 0xf6; // ⏎
    map_ks0066ru[0x2045] = 0xf7; // ⁅
    map_ks0066ru[0x253c] = 0xf8; // ┼
    map_ks0066ru[0x253d] = 0xf9; // ┽
    map_ks0066ru[0x2046] = 0xfa; // ⁆
    map_ks0066ru[0x2058] = 0xfb; // ⁘
    map_ks0066ru[0x2059] = 0xfc; // ⁙
    map_ks0066ru[0x00a7] = 0xfd; // §
    map_ks0066ru[0x00b6] = 0xfe; // ¶
    map_ks0066ru[0x2588] = 0xff; // █
}

QByteArray LcdConverter::encode(QString text)
{
    int i = 0;
    QByteArray ret;

    foreach (QChar qc, text) {
        if (map_ks0066ru.contains(qc)) {
            ret[i++] = map_ks0066ru[qc];
        } else {
            ret[i++] = 0xFF;
        }
    }

    return ret;
}

QString LcdConverter::decode(QByteArray ba)
{
    QString str = "";
    QBuffer buffer(&ba);
    char ch;

    buffer.open(QIODevice::ReadOnly);
    buffer.seek(0);

    while (buffer.pos() < buffer.size()) {
        buffer.getChar(&ch);
        str.append(map_ks0066ru.keys(ch).at(0));
    }

    buffer.close();

    return str;
}
