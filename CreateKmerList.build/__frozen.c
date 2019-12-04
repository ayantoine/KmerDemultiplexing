// This provides the frozen (compiled bytecode) files that are included if
// any.
#include <Python.h>

#include "nuitka/constants_blob.h"

// Blob from which modules are unstreamed.
#define stream_data constant_bin

// These modules should be loaded as bytecode. They may e.g. have to be loadable
// during "Py_Initialize" already, or for irrelevance, they are only included
// in this un-optimized form. These are not compiled by Nuitka, and therefore
// are not accelerated at all, merely bundled with the binary or module, so
// that CPython library can start out finding them.

struct frozen_desc {
    char const *name;
    ssize_t start;
    int size;
};

void copyFrozenModulesTo( struct _frozen *destination )
{
    struct frozen_desc frozen_modules[] = {
        { "base64", 6368888, 11253 },
        { "codecs", 6380141, 36628 },
        { "copy_reg", 6416769, 5137 },
        { "encodings", 6421906, -4362 },
        { "encodings.aliases", 6426268, 8760 },
        { "encodings.ascii", 6435028, 2253 },
        { "encodings.base64_codec", 6437281, 3829 },
        { "encodings.big5", 6441110, 1748 },
        { "encodings.big5hkscs", 6442858, 1788 },
        { "encodings.bz2_codec", 6444646, 4721 },
        { "encodings.charmap", 6449367, 3465 },
        { "encodings.cp037", 6452832, 2829 },
        { "encodings.cp1006", 6455661, 2915 },
        { "encodings.cp1026", 6458576, 2843 },
        { "encodings.cp1140", 6461419, 2829 },
        { "encodings.cp1250", 6464248, 2866 },
        { "encodings.cp1251", 6467114, 2863 },
        { "encodings.cp1252", 6469977, 2866 },
        { "encodings.cp1253", 6472843, 2879 },
        { "encodings.cp1254", 6475722, 2868 },
        { "encodings.cp1255", 6478590, 2887 },
        { "encodings.cp1256", 6481477, 2865 },
        { "encodings.cp1257", 6484342, 2873 },
        { "encodings.cp1258", 6487215, 2871 },
        { "encodings.cp424", 6490086, 2859 },
        { "encodings.cp437", 6492945, 8064 },
        { "encodings.cp500", 6501009, 2829 },
        { "encodings.cp720", 6503838, 2926 },
        { "encodings.cp737", 6506764, 8292 },
        { "encodings.cp775", 6515056, 8078 },
        { "encodings.cp850", 6523134, 7811 },
        { "encodings.cp852", 6530945, 8080 },
        { "encodings.cp855", 6539025, 8261 },
        { "encodings.cp856", 6547286, 2891 },
        { "encodings.cp857", 6550177, 7801 },
        { "encodings.cp858", 6557978, 7781 },
        { "encodings.cp860", 6565759, 8047 },
        { "encodings.cp861", 6573806, 8058 },
        { "encodings.cp862", 6581864, 8193 },
        { "encodings.cp863", 6590057, 8058 },
        { "encodings.cp864", 6598115, 8189 },
        { "encodings.cp865", 6606304, 8058 },
        { "encodings.cp866", 6614362, 8293 },
        { "encodings.cp869", 6622655, 8105 },
        { "encodings.cp874", 6630760, 2957 },
        { "encodings.cp875", 6633717, 2826 },
        { "encodings.cp932", 6636543, 1756 },
        { "encodings.cp949", 6638299, 1756 },
        { "encodings.cp950", 6640055, 1756 },
        { "encodings.euc_jis_2004", 6641811, 1812 },
        { "encodings.euc_jisx0213", 6643623, 1812 },
        { "encodings.euc_jp", 6645435, 1764 },
        { "encodings.euc_kr", 6647199, 1764 },
        { "encodings.gb18030", 6648963, 1772 },
        { "encodings.gb2312", 6650735, 1764 },
        { "encodings.gbk", 6652499, 1740 },
        { "encodings.hex_codec", 6654239, 3781 },
        { "encodings.hp_roman8", 6658020, 4112 },
        { "encodings.hz", 6662132, 1732 },
        { "encodings.idna", 6663864, 6368 },
        { "encodings.iso2022_jp", 6670232, 1801 },
        { "encodings.iso2022_jp_1", 6672033, 1817 },
        { "encodings.iso2022_jp_2", 6673850, 1817 },
        { "encodings.iso2022_jp_2004", 6675667, 1841 },
        { "encodings.iso2022_jp_3", 6677508, 1817 },
        { "encodings.iso2022_jp_ext", 6679325, 1833 },
        { "encodings.iso2022_kr", 6681158, 1801 },
        { "encodings.iso8859_1", 6682959, 2868 },
        { "encodings.iso8859_10", 6685827, 2883 },
        { "encodings.iso8859_11", 6688710, 2977 },
        { "encodings.iso8859_13", 6691687, 2886 },
        { "encodings.iso8859_14", 6694573, 2904 },
        { "encodings.iso8859_15", 6697477, 2883 },
        { "encodings.iso8859_16", 6700360, 2885 },
        { "encodings.iso8859_2", 6703245, 2868 },
        { "encodings.iso8859_3", 6706113, 2875 },
        { "encodings.iso8859_4", 6708988, 2868 },
        { "encodings.iso8859_5", 6711856, 2869 },
        { "encodings.iso8859_6", 6714725, 2913 },
        { "encodings.iso8859_7", 6717638, 2876 },
        { "encodings.iso8859_8", 6720514, 2907 },
        { "encodings.iso8859_9", 6723421, 2868 },
        { "encodings.johab", 6726289, 1756 },
        { "encodings.koi8_r", 6728045, 2890 },
        { "encodings.koi8_u", 6730935, 2876 },
        { "encodings.latin_1", 6733811, 2283 },
        { "encodings.mac_arabic", 6736094, 8014 },
        { "encodings.mac_centeuro", 6744108, 2937 },
        { "encodings.mac_croatian", 6747045, 2945 },
        { "encodings.mac_cyrillic", 6749990, 2935 },
        { "encodings.mac_farsi", 6752925, 2849 },
        { "encodings.mac_greek", 6755774, 2889 },
        { "encodings.mac_iceland", 6758663, 2928 },
        { "encodings.mac_latin2", 6761591, 4907 },
        { "encodings.mac_roman", 6766498, 2906 },
        { "encodings.mac_romanian", 6769404, 2946 },
        { "encodings.mac_turkish", 6772350, 2929 },
        { "encodings.palmos", 6775279, 3066 },
        { "encodings.ptcp154", 6778345, 4890 },
        { "encodings.punycode", 6783235, 7942 },
        { "encodings.quopri_codec", 6791177, 3647 },
        { "encodings.raw_unicode_escape", 6794824, 2202 },
        { "encodings.rot_13", 6797026, 3656 },
        { "encodings.shift_jis", 6800682, 1788 },
        { "encodings.shift_jis_2004", 6802470, 1828 },
        { "encodings.shift_jisx0213", 6804298, 1828 },
        { "encodings.string_escape", 6806126, 2061 },
        { "encodings.tis_620", 6808187, 2938 },
        { "encodings.undefined", 6811125, 2589 },
        { "encodings.unicode_escape", 6813714, 2150 },
        { "encodings.unicode_internal", 6815864, 2176 },
        { "encodings.utf_16", 6818040, 5160 },
        { "encodings.utf_16_be", 6823200, 1990 },
        { "encodings.utf_16_le", 6825190, 1990 },
        { "encodings.utf_32", 6827180, 5724 },
        { "encodings.utf_32_be", 6832904, 1883 },
        { "encodings.utf_32_le", 6834787, 1883 },
        { "encodings.utf_7", 6836670, 1883 },
        { "encodings.utf_8", 6838553, 1942 },
        { "encodings.utf_8_sig", 6840495, 4977 },
        { "encodings.uu_codec", 6845472, 4909 },
        { "encodings.zlib_codec", 6850381, 4641 },
        { "functools", 6855022, 6561 },
        { "locale", 6861583, 56524 },
        { "quopri", 6918107, 6544 },
        { "re", 6924651, 13363 },
        { "sre_compile", 6938014, 12522 },
        { "sre_constants", 6950536, 6177 },
        { "sre_parse", 6956713, 21076 },
        { "string", 6977789, 20349 },
        { "stringprep", 6998138, 14439 },
        { "struct", 7012577, 229 },
        { "types", 7012806, 2703 },
        { NULL, 0, 0 }
    };

    struct frozen_desc *current = frozen_modules;

    for(;;)
    {
        destination->name = (char *)current->name;
        destination->code = (unsigned char *)&constant_bin[ current->start ];
        destination->size = current->size;

        if (destination->name == NULL) break;

        current += 1;
        destination += 1;
    };
}
